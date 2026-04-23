#include "Migration.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

static const char *DDL_SCHEMA_VERSION =
    "CREATE TABLE IF NOT EXISTS schema_version ("
    "    version INTEGER PRIMARY KEY,"
    "    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP"
    ");";

static const char *DDL_TENANT_CONFIG =
    "CREATE TABLE IF NOT EXISTS tenant_config ("
    "    id INTEGER PRIMARY KEY,"
    "    tenant_id TEXT NOT NULL,"
    "    tenant_name TEXT,"
    "    store_id TEXT NOT NULL,"
    "    store_name TEXT,"
    "    config_json TEXT,"
    "    bind_time DATETIME"
    ");";

static const char *DDL_PRODUCTS =
    "CREATE TABLE IF NOT EXISTS products ("
    "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    cloud_id TEXT,"
    "    tenant_id TEXT NOT NULL,"
    "    store_id TEXT NOT NULL,"
    "    barcode TEXT NOT NULL,"
    "    sku TEXT,"
    "    name TEXT NOT NULL,"
    "    category_id INTEGER,"
    "    unit TEXT DEFAULT '件',"
    "    price DECIMAL(10,2) NOT NULL,"
    "    cost_price DECIMAL(10,2),"
    "    stock_qty DECIMAL(10,3) DEFAULT 0,"
    "    min_stock DECIMAL(10,3) DEFAULT 0,"
    "    status INTEGER DEFAULT 1,"
    "    image_url TEXT,"
    "    sync_status INTEGER DEFAULT 0,"
    "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "    UNIQUE(tenant_id, store_id, barcode)"
    ");";

static const char *DDL_CATEGORIES =
    "CREATE TABLE IF NOT EXISTS categories ("
    "    id INTEGER PRIMARY KEY,"
    "    cloud_id TEXT,"
    "    tenant_id TEXT NOT NULL,"
    "    name TEXT NOT NULL,"
    "    parent_id INTEGER,"
    "    sort_order INTEGER DEFAULT 0,"
    "    sync_status INTEGER DEFAULT 0,"
    "    UNIQUE(tenant_id, name, parent_id)"
    ");";

static const char *DDL_MEMBERS =
    "CREATE TABLE IF NOT EXISTS members ("
    "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    cloud_id TEXT,"
    "    tenant_id TEXT NOT NULL,"
    "    card_no TEXT,"
    "    name TEXT,"
    "    phone TEXT,"
    "    gender INTEGER DEFAULT 0,"
    "    birthday DATE,"
    "    level_id INTEGER,"
    "    points INTEGER DEFAULT 0,"
    "    balance DECIMAL(10,2) DEFAULT 0,"
    "    total_spent DECIMAL(10,2) DEFAULT 0,"
    "    status INTEGER DEFAULT 1,"
    "    sync_status INTEGER DEFAULT 0,"
    "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "    UNIQUE(tenant_id, card_no)"
    ");";

static const char *DDL_MEMBER_LEVELS =
    "CREATE TABLE IF NOT EXISTS member_levels ("
    "    id INTEGER PRIMARY KEY,"
    "    cloud_id TEXT,"
    "    tenant_id TEXT NOT NULL,"
    "    name TEXT NOT NULL,"
    "    discount DECIMAL(3,2) DEFAULT 1.00,"
    "    min_points INTEGER DEFAULT 0"
    ");";

static const char *DDL_ORDERS =
    "CREATE TABLE IF NOT EXISTS orders ("
    "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    cloud_id TEXT,"
    "    tenant_id TEXT NOT NULL,"
    "    store_id TEXT NOT NULL,"
    "    order_no TEXT UNIQUE NOT NULL,"
    "    member_id INTEGER,"
    "    total_amount DECIMAL(10,2) NOT NULL,"
    "    discount_amount DECIMAL(10,2) DEFAULT 0,"
    "    final_amount DECIMAL(10,2) NOT NULL,"
    "    payment_method TEXT NOT NULL,"
    "    status INTEGER DEFAULT 1,"
    "    cashier_id INTEGER,"
    "    shift_id INTEGER,"
    "    remark TEXT,"
    "    sync_status INTEGER DEFAULT 0,"
    "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
    ");";

static const char *DDL_ORDER_ITEMS =
    "CREATE TABLE IF NOT EXISTS order_items ("
    "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    tenant_id TEXT NOT NULL,"
    "    order_id INTEGER NOT NULL,"
    "    product_id INTEGER NOT NULL,"
    "    barcode TEXT NOT NULL,"
    "    product_name TEXT NOT NULL,"
    "    unit_price DECIMAL(10,2) NOT NULL,"
    "    quantity DECIMAL(10,3) NOT NULL,"
    "    discount_rate DECIMAL(3,2) DEFAULT 1.00,"
    "    subtotal DECIMAL(10,2) NOT NULL,"
    "    FOREIGN KEY (order_id) REFERENCES orders(id)"
    ");";

static const char *DDL_PAYMENTS =
    "CREATE TABLE IF NOT EXISTS payments ("
    "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    tenant_id TEXT NOT NULL,"
    "    order_id INTEGER NOT NULL,"
    "    method TEXT NOT NULL,"
    "    amount DECIMAL(10,2) NOT NULL,"
    "    reference_no TEXT,"
    "    status INTEGER DEFAULT 1,"
    "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "    FOREIGN KEY (order_id) REFERENCES orders(id)"
    ");";

static const char *DDL_OFFLINE_QUEUE =
    "CREATE TABLE IF NOT EXISTS offline_queue ("
    "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    tenant_id TEXT NOT NULL,"
    "    store_id TEXT NOT NULL,"
    "    operation TEXT NOT NULL,"
    "    table_name TEXT NOT NULL,"
    "    record_id INTEGER NOT NULL,"
    "    payload TEXT,"
    "    retry_count INTEGER DEFAULT 0,"
    "    last_error TEXT,"
    "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
    ");";

static const char *DDL_CASHIERS =
    "CREATE TABLE IF NOT EXISTS cashiers ("
    "    id INTEGER PRIMARY KEY,"
    "    cloud_id TEXT,"
    "    tenant_id TEXT NOT NULL,"
    "    username TEXT NOT NULL,"
    "    password_hash TEXT NOT NULL,"
    "    name TEXT,"
    "    role TEXT DEFAULT 'cashier',"
    "    status INTEGER DEFAULT 1,"
    "    sync_status INTEGER DEFAULT 0,"
    "    UNIQUE(tenant_id, username)"
    ");";

static const char *DDL_SHIFTS =
    "CREATE TABLE IF NOT EXISTS shifts ("
    "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    cloud_id TEXT,"
    "    tenant_id TEXT NOT NULL,"
    "    store_id TEXT NOT NULL,"
    "    cashier_id INTEGER NOT NULL,"
    "    start_time DATETIME NOT NULL,"
    "    end_time DATETIME,"
    "    start_cash DECIMAL(10,2),"
    "    end_cash DECIMAL(10,2),"
    "    status INTEGER DEFAULT 1,"
    "    sync_status INTEGER DEFAULT 0"
    ");";

static const char *ALL_DDL[] = {
    DDL_SCHEMA_VERSION,
    DDL_TENANT_CONFIG,
    DDL_PRODUCTS,
    DDL_CATEGORIES,
    DDL_MEMBERS,
    DDL_MEMBER_LEVELS,
    DDL_ORDERS,
    DDL_ORDER_ITEMS,
    DDL_PAYMENTS,
    DDL_OFFLINE_QUEUE,
    DDL_CASHIERS,
    DDL_SHIFTS
};

static const int DDL_COUNT = sizeof(ALL_DDL) / sizeof(ALL_DDL[0]);

bool Migration::run(QSqlDatabase &db) {
    // Check if schema_version table already exists
    QSqlQuery checkQuery(db);
    checkQuery.prepare(
        "SELECT name FROM sqlite_master WHERE type='table' AND name='schema_version'");
    if (checkQuery.exec() && checkQuery.next()) {
        // schema_version exists, migrations already applied
        return true;
    }

    if (!db.transaction()) {
        qWarning() << "Failed to start transaction:" << db.lastError().text();
        return false;
    }

    QSqlQuery q(db);
    for (int i = 0; i < DDL_COUNT; ++i) {
        if (!q.exec(QString::fromUtf8(ALL_DDL[i]))) {
            qWarning() << "Migration DDL failed:" << q.lastError().text();
            db.rollback();
            return false;
        }
    }

    // Insert initial schema version
    q.prepare("INSERT INTO schema_version (version) VALUES (1)");
    if (!q.exec()) {
        qWarning() << "Failed to insert schema version:" << q.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qWarning() << "Failed to commit transaction:" << db.lastError().text();
        return false;
    }

    qDebug() << "Database migration completed: schema version 1";
    return true;
}
