#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>

class DatabaseManager;
class ApiClient;
class AuthManager;
class OfflineQueue;
class SyncManager;
class HardwareManager;
class BarcodeFilter;
class AppSettings;

class ProductRepo;
class CategoryRepo;
class MemberRepo;
class OrderRepo;

class ProductService;
class CategoryService;
class MemberService;
class OrderService;
class PaymentService;
class ShiftService;

class ProductVM;
class MemberVM;
class CashierVM;

class Application : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loginStateChanged)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY onlineStateChanged)
    Q_PROPERTY(QString userName READ userName NOTIFY loginStateChanged)

public:
    explicit Application(QObject *parent = nullptr);
    ~Application();

    bool initialize(QQmlApplicationEngine *engine);
    void shutdown();

    bool isLoggedIn() const;
    bool isOnline() const;
    QString userName() const;

    Q_INVOKABLE bool login(const QString &serverUrl, const QString &username, const QString &password);
    Q_INVOKABLE void logout();

signals:
    void loginStateChanged();
    void onlineStateChanged();

private:
    void createServices();
    void createViewModels();
    void registerQmlTypes(QQmlApplicationEngine *engine);
    void installBarcodeFilter();

    // Infrastructure
    DatabaseManager *m_db = nullptr;
    ApiClient *m_apiClient = nullptr;
    AuthManager *m_authManager = nullptr;
    OfflineQueue *m_offlineQueue = nullptr;
    SyncManager *m_syncManager = nullptr;
    HardwareManager *m_hardwareManager = nullptr;
    BarcodeFilter *m_barcodeFilter = nullptr;
    AppSettings *m_settings = nullptr;

    // Repositories
    ProductRepo *m_productRepo = nullptr;
    CategoryRepo *m_categoryRepo = nullptr;
    MemberRepo *m_memberRepo = nullptr;
    OrderRepo *m_orderRepo = nullptr;

    // Services
    ProductService *m_productService = nullptr;
    CategoryService *m_categoryService = nullptr;
    MemberService *m_memberService = nullptr;
    OrderService *m_orderService = nullptr;
    PaymentService *m_paymentService = nullptr;
    ShiftService *m_shiftService = nullptr;

    // ViewModels
    ProductVM *m_productVM = nullptr;
    MemberVM *m_memberVM = nullptr;
    CashierVM *m_cashierVM = nullptr;

    // State
    bool m_isLoggedIn = false;
    bool m_isOnline = true;
    QString m_userName;
    QString m_tenantId;
    QString m_storeId;
};
