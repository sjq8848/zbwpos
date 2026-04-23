#include "Application.h"
#include "core/database/DatabaseManager.h"
#include "core/network/ApiClient.h"
#include "core/network/AuthManager.h"
#include "core/network/OfflineQueue.h"
#include "core/network/SyncManager.h"
#include "core/network/endpoints/AuthApi.h"
#include "core/hardware/HardwareManager.h"
#include "core/hardware/BarcodeFilter.h"
#include "core/settings/AppSettings.h"
#include "product/repository/ProductRepo.h"
#include "product/repository/CategoryRepo.h"
#include "member/repository/MemberRepo.h"
#include "cashier/repository/OrderRepo.h"
#include "product/service/ProductService.h"
#include "product/service/CategoryService.h"
#include "member/service/MemberService.h"
#include "cashier/service/OrderService.h"
#include "cashier/service/PaymentService.h"
#include "cashier/service/ShiftService.h"
#include "product/viewmodel/ProductVM.h"
#include "member/viewmodel/MemberVM.h"
#include "cashier/viewmodel/CashierVM.h"
#include <QStandardPaths>
#include <QDir>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>

Application::Application(QObject *parent)
    : QObject(parent)
{
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize(QQmlApplicationEngine *engine)
{
    // Create settings first
    m_settings = new AppSettings(this);

    // Create infrastructure
    m_db = new DatabaseManager(this);
    m_apiClient = new ApiClient(this);
    m_authManager = new AuthManager(this);
    m_offlineQueue = new OfflineQueue(this);
    m_syncManager = new SyncManager(this);
    m_hardwareManager = HardwareManager::instance();
    m_barcodeFilter = new BarcodeFilter(this);

    // Initialize database
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    QString dbPath = dataPath + "/zbwpos.db";
    if (!m_db->open(dbPath)) {
        qWarning() << "Failed to open database:" << dbPath;
        return false;
    }

    // Run migrations
    if (!m_db->runMigrations()) {
        qWarning() << "Failed to run migrations";
        return false;
    }

    // Set up offline queue
    m_offlineQueue->setDatabase(m_db);

    // Set up API client
    QString savedServerUrl = m_settings->serverUrl();
    if (!savedServerUrl.isEmpty()) {
        m_apiClient->setBaseUrl(savedServerUrl);
    }

    // Set up sync manager
    m_syncManager->setDatabase(m_db);
    m_syncManager->setApiClient(m_apiClient);
    m_syncManager->setOfflineQueue(m_offlineQueue);
    m_syncManager->startAutoSync(60000); // 1 minute interval

    // Initialize hardware
    m_hardwareManager->initialize();

    // Create services and view models
    createServices();
    createViewModels();

    // Register QML types
    registerQmlTypes(engine);

    // Install barcode filter for global barcode handling
    installBarcodeFilter();

    return true;
}

void Application::shutdown()
{
    if (m_hardwareManager) {
        m_hardwareManager->shutdown();
    }
    if (m_settings) {
        m_settings->sync();
    }
    if (m_syncManager) {
        m_syncManager->stopAutoSync();
    }
    if (m_db && m_db->isOpen()) {
        m_db->close();
    }
}

bool Application::isLoggedIn() const
{
    return m_isLoggedIn;
}

bool Application::isOnline() const
{
    return m_isOnline;
}

QString Application::userName() const
{
    return m_userName;
}

bool Application::login(const QString &serverUrl, const QString &username, const QString &password)
{
    m_apiClient->setBaseUrl(serverUrl);
    m_settings->setServerUrl(serverUrl);

    // Use AuthApi for login
    class AuthApi *authApi = new class AuthApi(m_apiClient, this);
    QNetworkReply *reply = authApi->login(username, password);

    // Synchronous wait for simplicity (in production, use async)
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    bool success = false;
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = QJsonDocument::fromJson(reply->readAll()).object();
        QString token = response["data"].toObject()["accessToken"].toString();
        QString refreshToken = response["data"].toObject()["refreshToken"].toString();
        QJsonObject userInfo = response["data"].toObject()["user"].toObject();

        if (!token.isEmpty()) {
            m_authManager->setTokens(token, refreshToken);
            m_apiClient->setAuthToken(token);

            // Extract tenant and store info
            m_tenantId = userInfo["tenantId"].toString();
            m_storeId = userInfo["storeId"].toString();
            m_userName = userInfo["name"].toString();
            int cashierId = userInfo["id"].toInt();

            m_settings->setTenantId(m_tenantId);
            m_settings->setStoreId(m_storeId);
            m_settings->setLastCashierId(cashierId);

            // Propagate to ViewModels
            if (m_productVM) {
                m_productVM->setTenantStore(m_tenantId, m_storeId);
            }
            if (m_memberVM) {
                m_memberVM->setTenantStore(m_tenantId);
            }
            if (m_cashierVM) {
                m_cashierVM->setTenantStore(m_tenantId, m_storeId);
                m_cashierVM->setCashier(cashierId);
            }

            m_isLoggedIn = true;
            emit loginStateChanged();
            success = true;
        }
    } else {
        qWarning() << "Login failed:" << reply->errorString();
    }

    reply->deleteLater();
    authApi->deleteLater();
    return success;
}

void Application::logout()
{
    m_authManager->clearTokens();
    m_apiClient->clearAuthToken();
    m_isLoggedIn = false;
    m_userName.clear();
    m_tenantId.clear();
    m_storeId.clear();
    emit loginStateChanged();
}

bool Application::enterDemoMode()
{
    // Use hardcoded demo values
    m_tenantId = "demo_tenant";
    m_storeId = "demo_store";
    m_userName = "演示收银员";
    m_isOnline = false; // Offline mode

    m_settings->setTenantId(m_tenantId);
    m_settings->setStoreId(m_storeId);
    m_settings->setLastCashierId(1);

    // Propagate to ViewModels
    if (m_productVM) {
        m_productVM->setTenantStore(m_tenantId, m_storeId);
    }
    if (m_memberVM) {
        m_memberVM->setTenantStore(m_tenantId);
    }
    if (m_cashierVM) {
        m_cashierVM->setTenantStore(m_tenantId, m_storeId);
        m_cashierVM->setCashier(1);
    }

    // Seed demo data
    seedDemoData();

    m_isLoggedIn = true;
    emit loginStateChanged();
    emit onlineStateChanged();

    qDebug() << "Demo mode activated - tenant:" << m_tenantId << "store:" << m_storeId;
    return true;
}

void Application::seedDemoData()
{
    // Seed categories
    m_db->execute(
        "INSERT OR IGNORE INTO categories (id, tenant_id, name, sort_order) VALUES "
        "(1, ?, '饮料', 1), "
        "(2, ?, '零食', 2), "
        "(3, ?, '日用品', 3)",
        {m_tenantId, m_tenantId, m_tenantId}
    );

    // Seed products
    QList<QVariantList> products = {
        {"6901234567890", "可口可乐500ml", 3.00, 1},
        {"6901234567891", "百事可乐500ml", 3.00, 1},
        {"6901234567892", "农夫山泉550ml", 2.00, 1},
        {"6901234567893", "乐事薯片原味", 8.50, 2},
        {"6901234567894", "好丽友派", 6.50, 2},
        {"6901234567895", "舒肤佳香皂", 5.00, 3},
        {"6901234567896", "清扬洗发水", 35.00, 3},
        {"6901234567897", "高露洁牙膏", 12.00, 3},
        {"6901234567898", "红牛功能饮料", 6.00, 1},
        {"6901234567899", "可口可乐330ml听装", 2.50, 1}
    };

    for (const auto &p : products) {
        m_db->execute(
            "INSERT OR IGNORE INTO products (tenant_id, store_id, barcode, name, price, category_id, unit, status) "
            "VALUES (?, ?, ?, ?, ?, ?, '件', 1)",
            {m_tenantId, m_storeId, p[0], p[1], p[2], p[3]}
        );
    }

    // Seed members
    QList<QVariantList> members = {
        {"M001", "张三", "13800138001", 100, 500.00},
        {"M002", "李四", "13800138002", 250, 1200.00},
        {"M003", "王五", "13800138003", 50, 80.00}
    };

    for (const auto &m : members) {
        m_db->execute(
            "INSERT OR IGNORE INTO members (tenant_id, card_no, name, phone, points, balance, status) "
            "VALUES (?, ?, ?, ?, ?, ?, 1)",
            {m_tenantId, m[0], m[1], m[2], m[3], m[4]}
        );
    }

    // Seed cashier
    m_db->execute(
        "INSERT OR IGNORE INTO cashiers (id, tenant_id, username, password_hash, name, role, status) "
        "VALUES (1, ?, 'admin', 'demo', '演示收银员', 'admin', 1)",
        {m_tenantId}
    );

    qDebug() << "Demo data seeded - 3 categories, 10 products, 3 members";
}

void Application::createServices()
{
    // Repositories
    m_productRepo = new ProductRepo(m_db, this);
    m_categoryRepo = new CategoryRepo(m_db, this);
    m_memberRepo = new MemberRepo(m_db, this);
    m_orderRepo = new OrderRepo(m_db, m_offlineQueue, this);

    // Services
    m_productService = new ProductService(m_productRepo, m_offlineQueue, this);
    m_categoryService = new CategoryService(m_categoryRepo, this);
    m_memberService = new MemberService(m_memberRepo, m_offlineQueue, this);
    m_orderService = new OrderService(m_orderRepo, this);
    m_paymentService = new PaymentService(m_memberService, this);
    m_shiftService = new ShiftService(m_db, m_offlineQueue, this);
}

void Application::createViewModels()
{
    m_productVM = new ProductVM(m_productService, this);
    m_memberVM = new MemberVM(m_memberService, this);
    m_cashierVM = new CashierVM(m_productService, m_memberService,
                                  m_orderService, m_paymentService,
                                  m_shiftService, this);
}

void Application::registerQmlTypes(QQmlApplicationEngine *engine)
{
    QQmlContext *ctx = engine->rootContext();

    // Register Application itself as "App"
    ctx->setContextProperty("App", this);

    // Register ViewModels
    ctx->setContextProperty("ProductVM", m_productVM);
    ctx->setContextProperty("MemberVM", m_memberVM);
    ctx->setContextProperty("CashierVM", m_cashierVM);

    // Register Settings
    ctx->setContextProperty("Settings", m_settings);
}

void Application::installBarcodeFilter()
{
    if (m_barcodeFilter) {
        m_barcodeFilter->setActive(true);
        // The barcode filter will be installed on QQuickWindow after QML loads
    }
}
