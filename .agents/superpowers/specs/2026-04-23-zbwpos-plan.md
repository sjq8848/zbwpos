# ZBW POS 收银端 Implementation Plan

> **For agentic workers:** REQUIRED: Use superpowers:subagent-driven-development (if subagents available) or superpowers:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 构建基于Qt5.15/Qt6双版本的SaaS零售POS跨平台收银客户端，支持离线优先+云同步。

**Architecture:** MVVM + 服务层，按业务模块组织代码（cashier/product/member/core）。ViewModel桥接QML与C++，Service封装业务逻辑，Repository双源模式（本地SQLite + 云端REST API）。

**Tech Stack:** Qt 5.15 / Qt 6.x, C++17, QML, SQLite, REST API + JWT, ESC/POS协议, qmake

**Design Spec:** `docs/superpowers/specs/2026-04-23-zbwpos-design.md`

---

## Chunk 1: Project Setup & Build System

搭建项目骨架和Qt5/Qt6双版本构建系统，确保空项目可以在两套Qt版本下编译运行。

### Task 1: Create project directory structure and qmake files

**Files:**
- Create: `zbwpos.pro`
- Create: `common.pri`
- Create: `qt5_compat.pri`
- Create: `qt6_compat.pri`
- Create: `src/src.pro`
- Create: `src/app/app.pro`
- Create: `src/core/core.pri`
- Create: `src/cashier/cashier.pri`
- Create: `src/product/product.pri`
- Create: `src/member/member.pri`
- Create: `tests/tests.pro`
- Create: `android/android.pri`
- Create: `resources/qml.qrc`

- [ ] **Step 1: Create top-level zbwpos.pro**
  ```qmake
  # zbwpos.pro
  lessThan(QT_MAJOR_VERSION, 5) | greaterThan(QT_MAJOR_VERSION, 6) {
      error("Unsupported Qt version. Use Qt 5.15 or Qt 6.x")
  }

  include(common.pri)

  equals(QT_MAJOR_VERSION, 5) {
      include(qt5_compat.pri)
      message("Building with Qt 5.x")
  }
  equals(QT_MAJOR_VERSION, 6) {
      include(qt6_compat.pri)
      message("Building with Qt 6.x")
  }

  TEMPLATE = subdirs
  CONFIG += ordered

  SUBDIRS += \
      src \
      tests
  ```

- [ ] **Step 2: Create common.pri**
  ```qmake
  # common.pri
  CONFIG += c++17

  QT += core gui widgets sql network

  DEFINES += \
      QT_DEPRECATED_WARNINGS \
      QT_NO_CAST_TO_ASCII \
      QT_STRICT_ITERATORS

  INCLUDEPATH += $$PWD/src

  # Output directory
  CONFIG(debug, debug|release) {
      DESTDIR = $$PWD/build/debug
  } else {
      DESTDIR = $$PWD/build/release
  }
  ```

- [ ] **Step 3: Create qt5_compat.pri**
  ```qmake
  # qt5_compat.pri
  QT += qml quick quickcontrols2 quickwidgets
  DEFINES += USE_QT5
  ```

- [ ] **Step 4: Create qt6_compat.pri**
  ```qmake
  # qt6_compat.pri
  QT += qml quick quickcontrols2
  DEFINES += USE_QT6
  equals(QT_MAJOR_VERSION, 6): CONFIG += qtquickcompiler
  ```

- [ ] **Step 5: Create src/src.pro**
  ```qmake
  # src/src.pro
  TEMPLATE = subdirs
  CONFIG += ordered

  SUBDIRS += \
      app
  ```

- [ ] **Step 6: Create src/app/app.pro**
  ```qmake
  # src/app/app.pro
  include(../../common.pri)
  equals(QT_MAJOR_VERSION, 5): include(../../qt5_compat.pri)
  equals(QT_MAJOR_VERSION, 6): include(../../qt6_compat.pri)
  include(../core/core.pri)
  include(../cashier/cashier.pri)
  include(../product/product.pri)
  include(../member/member.pri)

  TARGET = zbwpos
  TEMPLATE = app

  SOURCES += \
      main.cpp

  RESOURCES += \
      $$PWD/../../resources/qml.qrc
  ```

- [ ] **Step 7: Create empty .pri files for each module**
  ```qmake
  # src/core/core.pri
  HEADERS += 
  SOURCES += 
  ```

  Similarly for cashier.pri, product.pri, member.pri (same empty template).

- [ ] **Step 8: Create minimal qml.qrc**
  ```xml
  <!-- resources/qml.qrc -->
  <RCC>
      <qresource prefix="/">
          <file>main.qml</file>
      </qresource>
  </RCC>
  ```

- [ ] **Step 9: Create minimal main.qml**
  ```qml
  // resources/main.qml
  import QtQuick 2.15
  import QtQuick.Controls 2.15
  import QtQuick.Layouts 1.15

  ApplicationWindow {
      visible: true
      width: 1280
      height: 800
      title: "ZBW POS"
      
      Label {
          anchors.centerIn: parent
          text: "ZBW POS - Ready"
          font.pixelSize: 32
      }
  }
  ```

- [ ] **Step 10: Create tests/tests.pro**
  ```qmake
  # tests/tests.pro
  include(../common.pri)
  QT += testlib

  TEMPLATE = app
  TARGET = zbwpos_tests
  ```

- [ ] **Step 11: Create android/android.pri**
  ```qmake
  # android/android.pri
  android {
      QT += androidextras
      ANDROID_PACKAGE_SOURCE_DIR = $$PWD

      equals(QT_MAJOR_VERSION, 5) {
          ANDROID_MIN_SDK_VERSION = 21
          ANDROID_TARGET_SDK_VERSION = 28
      }
      equals(QT_MAJOR_VERSION, 6) {
          ANDROID_MIN_SDK_VERSION = 23
          ANDROID_TARGET_SDK_VERSION = 33
      }

      DEFINES += ZBW_PLATFORM_ANDROID
  }
  !android {
      DEFINES += ZBW_PLATFORM_DESKTOP
  }
  ```

- [ ] **Step 12: Commit**
  ```bash
  git init
  git add zbwpos.pro common.pri qt5_compat.pri qt6_compat.pri src/ tests/ android/ resources/
  git commit -m "feat: project skeleton with Qt5/Qt6 dual build system"
  ```

---

### Task 2: QtCompat layer and minimal main.cpp

**Files:**
- Create: `src/core/QtCompat.h`
- Create: `src/core/Common.h`
- Create: `src/app/main.cpp`

- [ ] **Step 1: Write QtCompat.h**
  ```cpp
  // src/core/QtCompat.h
  #pragma once
  #include <QtGlobal>

  #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      #define ZBW_QT6 1
      #define ZBW_QT5 0
  #else
      #define ZBW_QT6 0
      #define ZBW_QT5 1
  #endif
  ```

- [ ] **Step 2: Write Common.h**
  ```cpp
  // src/core/Common.h
  #pragma once
  #include <QString>
  #include <QDateTime>
  #include <QVariant>
  #include "QtCompat.h"

  namespace ZBW {

  // Sync status constants
  constexpr int SYNC_OK = 0;
  constexpr int SYNC_PENDING = 1;
  constexpr int SYNC_CONFLICT = 2;

  // Order status
  constexpr int ORDER_NORMAL = 1;
  constexpr int ORDER_REFUND = 2;
  constexpr int ORDER_CANCEL = 3;

  // Payment methods
  const QString PAY_CASH = "cash";
  const QString PAY_CARD = "card";
  const QString PAY_WECHAT = "wechat";
  const QString PAY_ALIPAY = "alipay";
  const QString PAY_MEMBER_BALANCE = "member_balance";

  }
  ```

- [ ] **Step 3: Write minimal main.cpp**
  ```cpp
  // src/app/main.cpp
  #include <QApplication>
  #include <QQmlApplicationEngine>
  #include <QQuickStyle>
  #include "core/QtCompat.h"

  int main(int argc, char *argv[])
  {
      QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
      
      QApplication app(argc, argv);
      app.setOrganizationName("ZBW");
      app.setApplicationName("ZBW POS");
      
      QQuickStyle::setStyle("Material");
      
      QQmlApplicationEngine engine;
      engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
      
      if (engine.rootObjects().isEmpty())
          return -1;
      
      return app.exec();
  }
  ```

- [ ] **Step 4: Update core.pri to include new files**
  ```qmake
  # src/core/core.pri
  HEADERS += \
      $$PWD/core/QtCompat.h \
      $$PWD/core/Common.h
  SOURCES += 
  ```

- [ ] **Step 5: Build with Qt5.15 to verify**
  Run: `cd D:/Claude_Projects/zbwpos && <qt5-qmake-path> zbwpos.pro && make` (or `nmake`/`mingw32-make`)
  Expected: Build succeeds, empty window with "ZBW POS - Ready" label

- [ ] **Step 6: Build with Qt6.x to verify**
  Run: `<qt6-qmake-path> zbwpos.pro && make`
  Expected: Build succeeds, same window

- [ ] **Step 7: Commit**
  ```bash
  git add src/core/QtCompat.h src/core/Common.h src/app/main.cpp src/core/core.pri
  git commit -m "feat: QtCompat layer and minimal main entry point"
  ```

---

## Chunk 2: Core Database Layer

SQLite数据库管理、Migration、基础数据模型。这是所有业务模块的地基。

### Task 3: DatabaseManager and Migration

**Files:**
- Create: `src/core/database/DatabaseManager.h`
- Create: `src/core/database/DatabaseManager.cpp`
- Create: `src/core/database/Migration.h`
- Create: `src/core/database/Migration.cpp`
- Test: `tests/unit/test_database.cpp`

- [ ] **Step 1: Write failing test for DatabaseManager**
  ```cpp
  // tests/unit/test_database.cpp
  #include <QtTest>
  #include "core/database/DatabaseManager.h"

  class TestDatabase : public QObject {
      Q_OBJECT
  private slots:
      void testOpenDatabase();
      void testMigrationCreatesTables();
      void testTenantConfig();
  };

  void TestDatabase::testOpenDatabase() {
      DatabaseManager db;
      QVERIFY(db.open(":memory:"));
      QVERIFY(db.isOpen());
      db.close();
  }

  void TestDatabase::testMigrationCreatesTables() {
      DatabaseManager db;
      db.open(":memory:");
      QVERIFY(db.runMigrations());
      
      QSqlQuery q = db.execute("SELECT name FROM sqlite_master WHERE type='table'");
      QStringList tables;
      while (q.next()) tables << q.value(0).toString();
      
      QVERIFY(tables.contains("tenant_config"));
      QVERIFY(tables.contains("products"));
      QVERIFY(tables.contains("categories"));
      QVERIFY(tables.contains("members"));
      QVERIFY(tables.contains("orders"));
      QVERIFY(tables.contains("order_items"));
      QVERIFY(tables.contains("payments"));
      QVERIFY(tables.contains("offline_queue"));
      QVERIFY(tables.contains("cashiers"));
      QVERIFY(tables.contains("shifts"));
      QVERIFY(tables.contains("member_levels"));
  }

  void TestDatabase::testTenantConfig() {
      DatabaseManager db;
      db.open(":memory:");
      db.runMigrations();
      
      QSqlQuery q = db.execute(
          "INSERT INTO tenant_config (tenant_id, tenant_name, store_id, store_name, bind_time) "
          "VALUES (?, ?, ?, ?, datetime('now'))",
          {"t001", "Test Corp", "s001", "Main Store"}
      );
      QVERIFY(q.isActive());
      
      QSqlQuery sel = db.execute("SELECT tenant_name FROM tenant_config WHERE tenant_id = ?", {"t001"});
      QVERIFY(sel.next());
      QCOMPARE(sel.value(0).toString(), QString("Test Corp"));
  }

  QTEST_MAIN(TestDatabase)
  #include "test_database.moc"
  ```

- [ ] **Step 2: Run test to verify it fails**
  Run: build and run test
  Expected: FAIL - DatabaseManager class not found

- [ ] **Step 3: Write DatabaseManager.h**
  ```cpp
  // src/core/database/DatabaseManager.h
  #pragma once
  #include <QObject>
  #include <QSqlDatabase>
  #include <QSqlQuery>
  #include <QSqlError>
  #include <QStringList>
  #include <QVariantList>

  class DatabaseManager : public QObject {
      Q_OBJECT
  public:
      explicit DatabaseManager(QObject *parent = nullptr);
      ~DatabaseManager();

      bool open(const QString &dbPath);
      void close();
      bool isOpen() const;
      bool runMigrations();

      QSqlQuery execute(const QString &sql, const QVariantList &params = {});
      QSqlDatabase database() const;

  private:
      QSqlDatabase m_db;
  };
  ```

- [ ] **Step 4: Write DatabaseManager.cpp**
  ```cpp
  // src/core/database/DatabaseManager.cpp
  #include "DatabaseManager.h"
  #include "Migration.h"
  #include <QSqlQuery>
  #include <QSqlError>
  #include <QDebug>

  DatabaseManager::DatabaseManager(QObject *parent)
      : QObject(parent)
  {
  }

  DatabaseManager::~DatabaseManager() {
      close();
  }

  bool DatabaseManager::open(const QString &dbPath) {
      m_db = QSqlDatabase::addDatabase("QSQLITE", "zbwpos_" + QString::number(reinterpret_cast<quintptr>(this)));
      m_db.setDatabaseName(dbPath);
      if (!m_db.open()) {
          qWarning() << "Failed to open database:" << m_db.lastError().text();
          return false;
      }
      m_db.exec("PRAGMA journal_mode=WAL");
      m_db.exec("PRAGMA foreign_keys=ON");
      return true;
  }

  void DatabaseManager::close() {
      if (m_db.isOpen()) m_db.close();
  }

  bool DatabaseManager::isOpen() const {
      return m_db.isOpen();
  }

  bool DatabaseManager::runMigrations() {
      return Migration::run(m_db);
  }

  QSqlQuery DatabaseManager::execute(const QString &sql, const QVariantList &params) {
      QSqlQuery q(m_db);
      if (!q.prepare(sql)) {
          qWarning() << "SQL prepare error:" << q.lastError().text() << sql;
          return q;
      }
      for (int i = 0; i < params.size(); ++i)
          q.bindValue(i, params[i]);
      if (!q.exec()) {
          qWarning() << "SQL exec error:" << q.lastError().text() << sql;
      }
      return q;
  }

  QSqlDatabase DatabaseManager::database() const {
      return m_db;
  }
  ```

- [ ] **Step 5: Write Migration.h**
  ```cpp
  // src/core/database/Migration.h
  #pragma once
  #include <QSqlDatabase>

  class Migration {
  public:
      static bool run(QSqlDatabase &db);
  };
  ```

- [ ] **Step 6: Write Migration.cpp** — all CREATE TABLE statements from spec section 4
  ```cpp
  // src/core/database/Migration.cpp
  #include "Migration.h"
  #include <QSqlQuery>
  #include <QSqlError>
  #include <QVersionNumber>
  #include <QDebug>

  static const char *SQL_V1 = R"(
  CREATE TABLE IF NOT EXISTS schema_version (
      version INTEGER PRIMARY KEY,
      applied_at DATETIME DEFAULT CURRENT_TIMESTAMP
  );

  CREATE TABLE IF NOT EXISTS tenant_config (
      id              INTEGER PRIMARY KEY,
      tenant_id       TEXT NOT NULL,
      tenant_name     TEXT,
      store_id        TEXT NOT NULL,
      store_name      TEXT,
      config_json     TEXT,
      bind_time       DATETIME
  );

  CREATE TABLE IF NOT EXISTS products (
      id              INTEGER PRIMARY KEY AUTOINCREMENT,
      cloud_id        TEXT,
      tenant_id       TEXT NOT NULL,
      store_id        TEXT NOT NULL,
      barcode         TEXT NOT NULL,
      sku             TEXT,
      name            TEXT NOT NULL,
      category_id     INTEGER,
      unit            TEXT DEFAULT '件',
      price           DECIMAL(10,2) NOT NULL,
      cost_price      DECIMAL(10,2),
      stock_qty       DECIMAL(10,3) DEFAULT 0,
      min_stock       DECIMAL(10,3) DEFAULT 0,
      status          INTEGER DEFAULT 1,
      image_url       TEXT,
      sync_status     INTEGER DEFAULT 0,
      created_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
      updated_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
      UNIQUE(tenant_id, store_id, barcode)
  );

  CREATE TABLE IF NOT EXISTS categories (
      id              INTEGER PRIMARY KEY,
      cloud_id        TEXT,
      tenant_id       TEXT NOT NULL,
      name            TEXT NOT NULL,
      parent_id       INTEGER,
      sort_order      INTEGER DEFAULT 0,
      sync_status     INTEGER DEFAULT 0,
      UNIQUE(tenant_id, name, parent_id)
  );

  CREATE TABLE IF NOT EXISTS members (
      id              INTEGER PRIMARY KEY AUTOINCREMENT,
      cloud_id        TEXT,
      tenant_id       TEXT NOT NULL,
      card_no         TEXT,
      name            TEXT,
      phone           TEXT,
      gender          INTEGER DEFAULT 0,
      birthday        DATE,
      level_id        INTEGER,
      points          INTEGER DEFAULT 0,
      balance         DECIMAL(10,2) DEFAULT 0,
      total_spent     DECIMAL(10,2) DEFAULT 0,
      status          INTEGER DEFAULT 1,
      sync_status     INTEGER DEFAULT 0,
      created_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
      updated_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
      UNIQUE(tenant_id, card_no)
  );

  CREATE TABLE IF NOT EXISTS member_levels (
      id              INTEGER PRIMARY KEY,
      cloud_id        TEXT,
      tenant_id       TEXT NOT NULL,
      name            TEXT NOT NULL,
      discount        DECIMAL(3,2) DEFAULT 1.00,
      min_points      INTEGER DEFAULT 0
  );

  CREATE TABLE IF NOT EXISTS orders (
      id              INTEGER PRIMARY KEY AUTOINCREMENT,
      cloud_id        TEXT,
      tenant_id       TEXT NOT NULL,
      store_id        TEXT NOT NULL,
      order_no        TEXT UNIQUE NOT NULL,
      member_id       INTEGER,
      total_amount    DECIMAL(10,2) NOT NULL,
      discount_amount DECIMAL(10,2) DEFAULT 0,
      final_amount    DECIMAL(10,2) NOT NULL,
      payment_method  TEXT NOT NULL,
      status          INTEGER DEFAULT 1,
      cashier_id      INTEGER,
      shift_id        INTEGER,
      remark          TEXT,
      sync_status     INTEGER DEFAULT 0,
      created_at      DATETIME DEFAULT CURRENT_TIMESTAMP
  );

  CREATE TABLE IF NOT EXISTS order_items (
      id              INTEGER PRIMARY KEY AUTOINCREMENT,
      tenant_id       TEXT NOT NULL,
      order_id        INTEGER NOT NULL,
      product_id      INTEGER NOT NULL,
      barcode         TEXT NOT NULL,
      product_name    TEXT NOT NULL,
      unit_price      DECIMAL(10,2) NOT NULL,
      quantity        DECIMAL(10,3) NOT NULL,
      discount_rate   DECIMAL(3,2) DEFAULT 1.00,
      subtotal        DECIMAL(10,2) NOT NULL,
      FOREIGN KEY (order_id) REFERENCES orders(id)
  );

  CREATE TABLE IF NOT EXISTS payments (
      id              INTEGER PRIMARY KEY AUTOINCREMENT,
      tenant_id       TEXT NOT NULL,
      order_id        INTEGER NOT NULL,
      method          TEXT NOT NULL,
      amount          DECIMAL(10,2) NOT NULL,
      reference_no    TEXT,
      status          INTEGER DEFAULT 1,
      created_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
      FOREIGN KEY (order_id) REFERENCES orders(id)
  );

  CREATE TABLE IF NOT EXISTS offline_queue (
      id              INTEGER PRIMARY KEY AUTOINCREMENT,
      tenant_id       TEXT NOT NULL,
      store_id        TEXT NOT NULL,
      operation       TEXT NOT NULL,
      table_name      TEXT NOT NULL,
      record_id       INTEGER NOT NULL,
      payload         TEXT,
      retry_count     INTEGER DEFAULT 0,
      last_error      TEXT,
      created_at      DATETIME DEFAULT CURRENT_TIMESTAMP
  );

  CREATE TABLE IF NOT EXISTS cashiers (
      id              INTEGER PRIMARY KEY,
      cloud_id        TEXT,
      tenant_id       TEXT NOT NULL,
      username        TEXT NOT NULL,
      password_hash   TEXT NOT NULL,
      name            TEXT,
      role            TEXT DEFAULT 'cashier',
      status          INTEGER DEFAULT 1,
      sync_status     INTEGER DEFAULT 0,
      UNIQUE(tenant_id, username)
  );

  CREATE TABLE IF NOT EXISTS shifts (
      id              INTEGER PRIMARY KEY AUTOINCREMENT,
      cloud_id        TEXT,
      tenant_id       TEXT NOT NULL,
      store_id        TEXT NOT NULL,
      cashier_id      INTEGER NOT NULL,
      start_time      DATETIME NOT NULL,
      end_time        DATETIME,
      start_cash      DECIMAL(10,2),
      end_cash        DECIMAL(10,2),
      status          INTEGER DEFAULT 1,
      sync_status     INTEGER DEFAULT 0
  );
  )";

  bool Migration::run(QSqlDatabase &db) {
      QSqlQuery q(db);
      
      // Check if schema_version table exists
      q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='schema_version'");
      if (!q.next()) {
          // Fresh database, run V1
          if (!db.transaction()) return false;
          if (!q.exec(QString::fromUtf8(SQL_V1))) {
              db.rollback();
              qWarning() << "Migration V1 failed:" << q.lastError().text();
              return false;
          }
          q.exec("INSERT INTO schema_version (version) VALUES (1)");
          db.commit();
          qDebug() << "Migration V1 applied";
      }
      
      // Future migrations: check schema_version and apply newer ones
      return true;
  }
  ```

- [ ] **Step 7: Update core.pri**
  ```qmake
  # src/core/core.pri
  HEADERS += \
      $$PWD/QtCompat.h \
      $$PWD/Common.h \
      $$PWD/database/DatabaseManager.h \
      $$PWD/database/Migration.h

  SOURCES += \
      $$PWD/database/DatabaseManager.cpp \
      $$PWD/database/Migration.cpp
  ```

- [ ] **Step 8: Run test to verify it passes**
  Run: build and run test_database
  Expected: All 3 tests PASS

- [ ] **Step 9: Commit**
  ```bash
  git add src/core/database/ tests/unit/test_database.cpp src/core/core.pri
  git commit -m "feat: DatabaseManager with SQLite migration system"
  ```

---

### Task 4: Data model structs (Product, Member, Order, etc.)

**Files:**
- Create: `src/core/database/Models.h`
- Test: `tests/unit/test_models.cpp`

- [ ] **Step 1: Write failing test for data models**
  ```cpp
  // tests/unit/test_models.cpp
  #include <QtTest>
  #include "core/database/Models.h"

  class TestModels : public QObject {
      Q_OBJECT
  private slots:
      void testProductDefaults();
      void testProductToJson();
      void testProductFromJson();
      void testOrderStruct();
      void testMemberStruct();
  };

  void TestModels::testProductDefaults() {
      Product p;
      QVERIFY(p.id == 0);
      QVERIFY(p.syncStatus == 0);
      QVERIFY(p.status == 1);
      QCOMPARE(p.unit, QString("件"));
  }

  void TestModels::testProductToJson() {
      Product p;
      p.cloudId = "c001";
      p.tenantId = "t001";
      p.storeId = "s001";
      p.barcode = "6901234567890";
      p.name = "Test Product";
      p.price = 9.90;
      
      QJsonObject json = p.toJson();
      QCOMPARE(json["barcode"].toString(), QString("6901234567890"));
      QCOMPARE(json["name"].toString(), QString("Test Product"));
      QCOMPARE(json["price"].toDouble(), 9.90);
  }

  void TestModels::testProductFromJson() {
      QJsonObject json;
      json["cloud_id"] = "c001";
      json["barcode"] = "6901234567890";
      json["name"] = "From JSON";
      json["price"] = 15.50;
      
      Product p = Product::fromJson(json);
      QCOMPARE(p.cloudId, QString("c001"));
      QCOMPARE(p.barcode, QString("6901234567890"));
      QCOMPARE(p.price, 15.50);
  }

  void TestModels::testOrderStruct() {
      Order o;
      QVERIFY(o.id == 0);
      QVERIFY(o.status == 1);
      QVERIFY(o.syncStatus == 0);
  }

  void TestModels::testMemberStruct() {
      Member m;
      QVERIFY(m.id == 0);
      QVERIFY(m.points == 0);
      QVERIFY(m.balance == 0.0);
      QVERIFY(m.status == 1);
  }

  QTEST_MAIN(TestModels)
  #include "test_models.moc"
  ```

- [ ] **Step 2: Run test to verify it fails**
  Expected: FAIL - Models.h not found

- [ ] **Step 3: Write Models.h** — all data structures from spec section 4 with JSON serialization
  ```cpp
  // src/core/database/Models.h
  #pragma once
  #include <QString>
  #include <QDateTime>
  #include <QJsonObject>
  #include <QJsonArray>
  #include <QList>
  #include <QVariant>

  struct Product {
      int id = 0;
      QString cloudId;
      QString tenantId;
      QString storeId;
      QString barcode;
      QString sku;
      QString name;
      int categoryId = 0;
      QString unit = QStringLiteral("件");
      double price = 0.0;
      double costPrice = 0.0;
      double stockQty = 0.0;
      double minStock = 0.0;
      int status = 1;
      QString imageUrl;
      int syncStatus = 0;
      QDateTime createdAt;
      QDateTime updatedAt;

      QJsonObject toJson() const {
          return {
              {"cloud_id", cloudId},
              {"tenant_id", tenantId},
              {"store_id", storeId},
              {"barcode", barcode},
              {"sku", sku},
              {"name", name},
              {"category_id", categoryId},
              {"unit", unit},
              {"price", price},
              {"cost_price", costPrice},
              {"stock_qty", stockQty},
              {"status", status},
              {"image_url", imageUrl}
          };
      }

      static Product fromJson(const QJsonObject &json) {
          Product p;
          p.cloudId = json["cloud_id"].toString();
          p.tenantId = json["tenant_id"].toString();
          p.storeId = json["store_id"].toString();
          p.barcode = json["barcode"].toString();
          p.sku = json["sku"].toString();
          p.name = json["name"].toString();
          p.categoryId = json["category_id"].toInt();
          p.unit = json["unit"].toString("件");
          p.price = json["price"].toDouble();
          p.costPrice = json["cost_price"].toDouble();
          p.stockQty = json["stock_qty"].toDouble();
          p.status = json["status"].toInt(1);
          p.imageUrl = json["image_url"].toString();
          return p;
      }
  };

  struct Category {
      int id = 0;
      QString cloudId;
      QString tenantId;
      QString name;
      int parentId = 0;
      int sortOrder = 0;
      int syncStatus = 0;
  };

  struct Member {
      int id = 0;
      QString cloudId;
      QString tenantId;
      QString cardNo;
      QString name;
      QString phone;
      int gender = 0;
      QDate birthday;
      int levelId = 0;
      int points = 0;
      double balance = 0.0;
      double totalSpent = 0.0;
      int status = 1;
      int syncStatus = 0;
      QDateTime createdAt;
      QDateTime updatedAt;

      QJsonObject toJson() const {
          return {
              {"cloud_id", cloudId},
              {"tenant_id", tenantId},
              {"card_no", cardNo},
              {"name", name},
              {"phone", phone},
              {"points", points},
              {"balance", balance}
          };
      }

      static Member fromJson(const QJsonObject &json) {
          Member m;
          m.cloudId = json["cloud_id"].toString();
          m.tenantId = json["tenant_id"].toString();
          m.cardNo = json["card_no"].toString();
          m.name = json["name"].toString();
          m.phone = json["phone"].toString();
          m.points = json["points"].toInt();
          m.balance = json["balance"].toDouble();
          return m;
      }
  };

  struct MemberLevel {
      int id = 0;
      QString cloudId;
      QString tenantId;
      QString name;
      double discount = 1.00;
      int minPoints = 0;
  };

  struct OrderItem {
      int id = 0;
      QString tenantId;
      int orderId = 0;
      int productId = 0;
      QString barcode;
      QString productName;
      double unitPrice = 0.0;
      double quantity = 0.0;
      double discountRate = 1.00;
      double subtotal = 0.0;
  };

  struct Payment {
      int id = 0;
      QString tenantId;
      int orderId = 0;
      QString method;
      double amount = 0.0;
      QString referenceNo;
      int status = 1;
      QDateTime createdAt;
  };

  struct Order {
      int id = 0;
      QString cloudId;
      QString tenantId;
      QString storeId;
      QString orderNo;
      int memberId = 0;
      double totalAmount = 0.0;
      double discountAmount = 0.0;
      double finalAmount = 0.0;
      QString paymentMethod;
      int status = 1;
      int cashierId = 0;
      int shiftId = 0;
      QString remark;
      int syncStatus = 0;
      QDateTime createdAt;
      QList<OrderItem> items;
      QList<Payment> payments;
  };

  struct Cashier {
      int id = 0;
      QString cloudId;
      QString tenantId;
      QString username;
      QString passwordHash;
      QString name;
      QString role = QStringLiteral("cashier");
      int status = 1;
      int syncStatus = 0;
  };

  struct Shift {
      int id = 0;
      QString cloudId;
      QString tenantId;
      QString storeId;
      int cashierId = 0;
      QDateTime startTime;
      QDateTime endTime;
      double startCash = 0.0;
      double endCash = 0.0;
      int status = 1;
      int syncStatus = 0;
  };
  ```

- [ ] **Step 4: Update core.pri to include Models.h**
  Add `$$PWD/database/Models.h` to HEADERS

- [ ] **Step 5: Run test to verify it passes**
  Expected: All 5 tests PASS

- [ ] **Step 6: Commit**
  ```bash
  git add src/core/database/Models.h tests/unit/test_models.cpp src/core/core.pri
  git commit -m "feat: data model structs with JSON serialization"
  ```

---

## Chunk 3: Core Network Layer

REST API客户端、JWT认证管理、离线队列。

### Task 5: ApiClient (REST client base)

**Files:**
- Create: `src/core/network/ApiClient.h`
- Create: `src/core/network/ApiClient.cpp`
- Create: `src/core/network/models/ApiResponse.h`
- Test: `tests/unit/test_apiclient.cpp`

- [ ] **Step 1: Write failing test for ApiClient**
  Test that ApiClient can be constructed, auth token can be set, and that it emits signals on network events. Use QSignalSpy.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write ApiResponse.h**
  ```cpp
  // src/core/network/models/ApiResponse.h
  #pragma once
  #include <QJsonObject>
  #include <QString>

  struct ApiResponse {
      bool success = false;
      int statusCode = 0;
      QJsonObject data;
      QString error;
      QString message;

      static ApiResponse fromNetworkReply(int httpCode, const QJsonObject &body) {
          ApiResponse r;
          r.statusCode = httpCode;
          r.success = (httpCode >= 200 && httpCode < 300);
          r.data = body;
          r.message = body["message"].toString();
          if (!r.success) r.error = body["error"].toString();
          return r;
      }
  };
  ```

- [ ] **Step 4: Write ApiClient.h/cpp**
  Key methods: get/post/put/del, setAuthToken, signals for unauthorized/networkError/rateLimited.
  Use QNetworkAccessManager internally. All requests add Authorization header with Bearer token.

- [ ] **Step 5: Run test to verify it passes**

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: REST API client with JWT auth headers"
  ```

---

### Task 6: AuthManager (JWT management)

**Files:**
- Create: `src/core/network/AuthManager.h`
- Create: `src/core/network/AuthManager.cpp`
- Test: `tests/unit/test_authmanager.cpp`

- [ ] **Step 1: Write failing test**
  Test: token validity check, isTokenExpiringSoon, login flow mock.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write AuthManager.h/cpp**
  JWT decode (base64 payload to get exp claim), token refresh, secure storage via QSettings (placeholder for qtkeychain integration). Key methods: login, logout, isTokenValid, isTokenExpiringSoon, currentToken, refreshToken.

- [ ] **Step 4: Run test to verify it passes**

- [ ] **Step 5: Commit**
  ```bash
  git commit -m "feat: JWT auth manager with token validation"
  ```

---

### Task 7: OfflineQueue

**Files:**
- Create: `src/core/network/OfflineQueue.h`
- Create: `src/core/network/OfflineQueue.cpp`
- Test: `tests/unit/test_offlinequeue.cpp`

- [ ] **Step 1: Write failing test**
  Test push, getPending, markCompleted, markFailed, retry, cleanup.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write OfflineQueue.h/cpp**
  Uses DatabaseManager to persist queue items in offline_queue table. Key methods match spec section 3.

- [ ] **Step 4: Run test to verify it passes**

- [ ] **Step 5: Commit**
  ```bash
  git commit -m "feat: offline queue for deferred sync operations"
  ```

---

### Task 8: SyncManager

**Files:**
- Create: `src/core/network/SyncManager.h`
- Create: `src/core/network/SyncManager.cpp`
- Test: `tests/unit/test_syncmanager.cpp`

- [ ] **Step 1: Write failing test**
  Test sync status, pending count, conflict resolution with auto-rules from spec section 11.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write SyncManager.h/cpp**
  Background thread, processes OfflineQueue items, calls API endpoints, handles conflicts with auto-rules (UseRemote for new record conflicts, UseLocal for orders, ManualMerge for products/members). Signals: syncStarted/Progress/Completed/Error/ConflictDetected.

- [ ] **Step 4: Run test to verify it passes**

- [ ] **Step 5: Commit**
  ```bash
  git commit -m "feat: sync manager with conflict resolution"
  ```

---

### Task 9: API endpoint classes

**Files:**
- Create: `src/core/network/endpoints/AuthApi.h/.cpp`
- Create: `src/core/network/endpoints/ProductApi.h/.cpp`
- Create: `src/core/network/endpoints/MemberApi.h/.cpp`
- Create: `src/core/network/endpoints/OrderApi.h/.cpp`
- Test: `tests/unit/test_api_endpoints.cpp`

- [ ] **Step 1: Write failing test**
  Test each API class constructs correct URLs and request bodies.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write endpoint classes**
  Each wraps ApiClient with domain-specific methods. AuthApi: login/refresh/logout. ProductApi: list/getByBarcode/create/update. MemberApi: list/getByCardNo/getByPhone. OrderApi: create/updateStatus/list.

- [ ] **Step 4: Run test to verify it passes**

- [ ] **Step 5: Update core.pri with all network files**

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: REST API endpoint classes for auth/product/member/order"
  ```

---

## Chunk 4: Hardware Abstraction Layer

### Task 10: Hardware device interfaces

**Files:**
- Create: `src/core/hardware/IHardwareDevice.h`
- Create: `src/core/hardware/IBarcodeScanner.h`
- Create: `src/core/hardware/IPrinter.h`
- Create: `src/core/hardware/ICashDrawer.h`
- Create: `src/core/hardware/ICustomerDisplay.h`

- [ ] **Step 1: Write all interface headers** as specified in design section 5.
  IHardwareDevice: open/close/isConnected/deviceName/lastError.
  IBarcodeScanner: startScanning/stopScanning, signal barcodeRead.
  IPrinter: printReceipt/printTestPage/cutPaper/openCashDrawer/printerStatus.
  ICashDrawer: open/isOpen.
  ICustomerDisplay: showWelcome/showItem/showTotal/showPayment/clear.

- [ ] **Step 2: Commit**
  ```bash
  git commit -m "feat: hardware device interface definitions"
  ```

---

### Task 11: HardwareManager, DeviceFactory, and platform implementations

**Files:**
- Create: `src/core/hardware/HardwareManager.h/.cpp`
- Create: `src/core/hardware/DeviceFactory.h/.cpp`
- Create: `src/core/hardware/BarcodeFilter.h/.cpp` (global keyboard event filter for scanner)
- Create: `src/core/hardware/platform/PrinterEscPos.h/.cpp`
- Test: `tests/unit/test_hardware.cpp`

- [ ] **Step 1: Write failing test for HardwareManager**
  Test device registration, BarcodeFilter detection logic.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write HardwareManager** — device lifecycle, registration by type.
- [ ] **Step 4: Write DeviceFactory** — creates platform-specific implementations.
- [ ] **Step 5: Write BarcodeFilter** — detects rapid keystroke sequences ending with Enter (barcode scanner simulation). Emits barcodeRead signal.
- [ ] **Step 6: Write PrinterEscPos** — ESC/POS command builder over serial/USB port.
- [ ] **Step 7: Run test to verify it passes**

- [ ] **Step 8: Update core.pri with hardware files**

- [ ] **Step 9: Commit**
  ```bash
  git commit -m "feat: hardware manager with barcode filter and ESC/POS printer"
  ```

---

## Chunk 5: Product Module

商品是收银台的基础依赖，先实现商品模块。

### Task 12: ProductRepo

**Files:**
- Create: `src/product/repository/ProductRepo.h/.cpp`
- Create: `src/product/repository/CategoryRepo.h/.cpp`
- Test: `tests/unit/test_productrepo.cpp`

- [ ] **Step 1: Write failing test**
  Test: save product, findByBarcode, search, getByCategory, syncStatus tracking.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write ProductRepo** — implements BaseRepo pattern, uses DatabaseManager for local, ProductApi for remote. Key methods: findByBarcode, search(keyword), getByCategory, syncFromCloud.

- [ ] **Step 4: Write CategoryRepo** — similar pattern for categories.

- [ ] **Step 5: Run test to verify it passes**

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: product and category repository with dual-source pattern"
  ```

---

### Task 13: ProductService

**Files:**
- Create: `src/product/service/ProductService.h/.cpp`
- Create: `src/product/service/CategoryService.h/.cpp`
- Test: `tests/unit/test_productservice.cpp`

- [ ] **Step 1: Write failing test**
  Test: findByBarcode returns product, search returns filtered list, stockLowWarning signal.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write ProductService** — delegates to ProductRepo, adds business logic (stock warning, price update validation). CategoryService similarly wraps CategoryRepo.

- [ ] **Step 4: Run test to verify it passes**

- [ ] **Step 5: Commit**
  ```bash
  git commit -m "feat: product and category service layer"
  ```

---

### Task 14: ProductVM and ProductManagePage QML

**Files:**
- Create: `src/product/viewmodel/ProductVM.h/.cpp`
- Create: `src/product/qml/ProductManagePage.qml`
- Test: `tests/unit/test_productvm.cpp`

- [ ] **Step 1: Write failing test**
  Test Q_PROPERTY reads, Q_INVOKABLE search method, signal emissions.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write ProductVM** — exposes product list, search, category filter to QML via Q_PROPERTY/Q_INVOKABLE.

- [ ] **Step 4: Write ProductManagePage.qml** — basic table/grid layout for product management (Widgets-style).

- [ ] **Step 5: Run test to verify it passes**

- [ ] **Step 6: Update product.pri**

- [ ] **Step 7: Commit**
  ```bash
  git commit -m "feat: product viewmodel and management page"
  ```

---

## Chunk 6: Member Module

### Task 15: MemberRepo + MemberService + MemberVM

**Files:**
- Create: `src/member/repository/MemberRepo.h/.cpp`
- Create: `src/member/service/MemberService.h/.cpp`
- Create: `src/member/viewmodel/MemberVM.h/.cpp`
- Create: `src/member/qml/MemberPage.qml`
- Test: `tests/unit/test_membermodule.cpp`

- [ ] **Step 1: Write failing test** for MemberRepo (findByCardNo, findByPhone, updateBalance, updatePoints)
- [ ] **Step 2: Run test to verify it fails**
- [ ] **Step 3: Write MemberRepo** — dual-source, findByCardNo, findByPhone, balance/points operations
- [ ] **Step 4: Write MemberService** — business logic for discount calculation, points/balance operations, signals
- [ ] **Step 5: Write MemberVM** — Q_PROPERTY for currentMember, searchResults; Q_INVOKABLE for findByCardNo, findByPhone
- [ ] **Step 6: Write MemberPage.qml** — basic member search and info display
- [ ] **Step 7: Run test to verify it passes**
- [ ] **Step 8: Update member.pri**
- [ ] **Step 9: Commit**
  ```bash
  git commit -m "feat: member module - repo, service, viewmodel, page"
  ```

---

## Chunk 7: Cashier Module (Core POS)

收银台是系统的核心功能，包含购物车、支付、订单。

### Task 16: CartItemModel and CartVM

**Files:**
- Create: `src/cashier/viewmodel/models/CartItemModel.h/.cpp`
- Create: `src/cashier/viewmodel/CartVM.h/.cpp`
- Test: `tests/unit/test_cartvm.cpp`

- [ ] **Step 1: Write failing test**
  Test: addItem increases itemCount, subtotal calculates correctly, removeItem works, applyDiscount updates total, clear resets cart.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write CartItemModel** — QAbstractListModel with roles: ProductName, UnitPrice, Quantity, Subtotal, Barcode. Supports add/remove/update rows.

- [ ] **Step 4: Write CartVM** — manages CartItemModel, calculates subtotal/discount/total, emits cartChanged signal. Q_PROPERTY: items, itemCount, subtotal, discount, total, memberId.

- [ ] **Step 5: Run test to verify it passes**

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: cart viewmodel with item model and calculation"
  ```

---

### Task 17: PaymentVM (with split payment state machine)

**Files:**
- Create: `src/cashier/viewmodel/PaymentVM.h/.cpp`
- Create: `src/cashier/viewmodel/models/PaymentRecordModel.h/.cpp`
- Test: `tests/unit/test_paymentvm.cpp`

- [ ] **Step 1: Write failing test**
  Test: setMethod changes state, addPartialPayment records payment and reduces amountDue, canComplete is true when amountPaid >= amountDue, complete transitions to Completed state, cancel resets.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write PaymentRecordModel** — QAbstractListModel for partial payment records (method, amount).

- [ ] **Step 4: Write PaymentVM** — state machine (Idle→SelectingMethod→InputtingAmount→PartialPaid→Completed). Tracks amountDue, amountPaid, change, paymentRecords list. addPartialPayment adds a partial payment and reduces amountDue. canComplete = (amountPaid >= original amountDue).

- [ ] **Step 5: Run test to verify it passes**

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: payment viewmodel with split payment state machine"
  ```

---

### Task 18: OrderRepo + OrderService + PaymentService

**Files:**
- Create: `src/cashier/repository/OrderRepo.h/.cpp`
- Create: `src/cashier/service/OrderService.h/.cpp`
- Create: `src/cashier/service/PaymentService.h/.cpp`
- Test: `tests/unit/test_orderservice.cpp`

- [ ] **Step 1: Write failing test**
  Test: createOrder saves to DB with correct amounts, completeOrder updates status, suspendOrder/resumeOrder, payment with cash calculates change, mobile payment flow mock.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write OrderRepo** — saveOrder (inserts order + order_items + payments in transaction), findByOrderNo, findTodayOrders, findSuspended, getPendingUpload, markSynced.

- [ ] **Step 4: Write OrderService** — createOrder from cart data, completeOrder after payment, suspendOrder/resumeOrder, refundOrder.

- [ ] **Step 5: Write PaymentService** — pay with method validation, payWithMemberBalance (calls MemberService.deductBalance), prepareMobilePayment (returns QR code URL), checkMobilePaymentStatus (polling).

- [ ] **Step 6: Run test to verify it passes**

- [ ] **Step 7: Commit**
  ```bash
  git commit -m "feat: order repo, order service, payment service"
  ```

---

### Task 19: ShiftService + ShiftVM

**Files:**
- Create: `src/cashier/service/ShiftService.h/.cpp`
- Create: `src/cashier/viewmodel/ShiftVM.h/.cpp` (move to models dir if needed)
- Test: `tests/unit/test_shiftservice.cpp`

- [ ] **Step 1: Write failing test**
  Test: startShift creates record, endShift calculates summary, getCurrentShift.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write ShiftService** — startShift, endShift, getCurrentShift, getShiftOrders.

- [ ] **Step 4: Write ShiftVM** — Q_PROPERTY: currentShift, shiftOrders, totalSales, cashInDrawer.

- [ ] **Step 5: Run test to verify it passes**

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: shift service and viewmodel"
  ```

---

### Task 20: CashierVM (收银台主控制器)

**Files:**
- Create: `src/cashier/viewmodel/CashierVM.h/.cpp`
- Create: `src/cashier/viewmodel/ProductSearchVM.h/.cpp`
- Test: `tests/unit/test_cashiervm.cpp`

- [ ] **Step 1: Write failing test**
  Test: scanBarcode adds product to cart, selectMember applies discount, suspendOrder/resumeOrder, startPayment emits showPaymentDialog.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Write ProductSearchVM** — search results model, keyword property, delegates to ProductService.

- [ ] **Step 4: Write CashierVM** — orchestrates CartVM, PaymentVM, ProductSearchVM, MemberVM. scanBarcode calls ProductService.findByBarcode then CartVM.addItem. selectMember loads member and applies discount. startPayment creates order via OrderService and emits showPaymentDialog.

- [ ] **Step 5: Run test to verify it passes**

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: cashier viewmodel - main POS controller"
  ```

---

## Chunk 8: QML收银台界面

### Task 21: QML styles and base components

**Files:**
- Create: `src/cashier/qml/styles/AppColors.qml`
- Create: `src/cashier/qml/styles/AppFonts.qml`
- Create: `src/cashier/qml/styles/AppTheme.qml`
- Create: `src/cashier/qml/components/TopStatusBar.qml`
- Create: `src/cashier/qml/components/CategoryTabBar.qml`
- Create: `src/cashier/qml/components/ProductCard.qml`
- Create: `src/cashier/qml/components/NumberPad.qml`
- Create: `src/cashier/qml/components/BarcodeInput.qml`

- [ ] **Step 1: Write style files** — AppColors (POS-friendly color palette), AppFonts (touch-optimized sizes), AppTheme (spacing, radius constants).

- [ ] **Step 2: Write TopStatusBar** — shows store name, cashier name, shift info, online status, settings button. Binds to Application context properties.

- [ ] **Step 3: Write CategoryTabBar** — horizontal scrollable tab bar, emits categorySelected(int categoryId).

- [ ] **Step 4: Write ProductCard** — displays product image, name, price. Clickable. Emits clicked signal.

- [ ] **Step 5: Write NumberPad** — grid of digit buttons + backspace, for manual amount input.

- [ ] **Step 6: Write BarcodeInput** — text field with "scan or type" placeholder, shows scan status animation. Emits barcodeEntered(string).

- [ ] **Step 7: Update qml.qrc with all new QML files**

- [ ] **Step 8: Commit**
  ```bash
  git commit -m "feat: QML style system and base cashier components"
  ```

---

### Task 22: Cart panel and product grid

**Files:**
- Create: `src/cashier/qml/components/ProductGrid.qml`
- Create: `src/cashier/qml/components/CartItemDelegate.qml`
- Create: `src/cashier/qml/components/CartPanel.qml`
- Create: `src/cashier/qml/components/MemberInfo.qml`
- Create: `src/cashier/qml/components/ActionButtonBar.qml`

- [ ] **Step 1: Write ProductGrid** — GridView of ProductCard items, filtered by selected category. Binds to ProductVM.searchResults.

- [ ] **Step 2: Write CartItemDelegate** — single cart row: product name, unit price, quantity controls (+/-), subtotal, remove button.

- [ ] **Step 3: Write CartPanel** — right panel: ListView of CartItemDelegate, member info bar, totals, suspend/clear/checkout buttons. Binds to CartVM.

- [ ] **Step 4: Write MemberInfo** — compact member info display: name, phone, points, discount rate.

- [ ] **Step 5: Write ActionButtonBar** — bottom bar with member query, product search, refund, shift, more buttons.

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: cart panel, product grid, and action bar QML components"
  ```

---

### Task 23: Dialogs

**Files:**
- Create: `src/cashier/qml/dialogs/PaymentDialog.qml`
- Create: `src/cashier/qml/dialogs/MemberSearchDialog.qml`
- Create: `src/cashier/qml/dialogs/ProductSearchDialog.qml`
- Create: `src/cashier/qml/dialogs/SuspendOrderDialog.qml`
- Create: `src/cashier/qml/dialogs/RefundDialog.qml`
- Create: `src/cashier/qml/dialogs/ShiftDialog.qml`

- [ ] **Step 1: Write PaymentDialog** — modal dialog with amount due display, payment method selector (cash/wechat/alipay/card/balance), quick cash buttons, amount input, change display, partial payment records, confirm/cancel buttons. Binds to PaymentVM. Handles split payment flow.

- [ ] **Step 2: Write MemberSearchDialog** — search by phone/card, results list, select member.

- [ ] **Step 3: Write ProductSearchDialog** — keyword search, results list, add to cart.

- [ ] **Step 4: Write SuspendOrderDialog** — list of suspended orders, select to resume.

- [ ] **Step 5: Write RefundDialog** — order number input, refund items selection, confirm.

- [ ] **Step 6: Write ShiftDialog** — start/end shift, cash count, shift summary.

- [ ] **Step 7: Update qml.qrc**

- [ ] **Step 8: Commit**
  ```bash
  git commit -m "feat: all cashier dialog QML components"
  ```

---

### Task 24: CashierPage (main cashier screen assembly)

**Files:**
- Create: `src/cashier/qml/CashierPage.qml`

- [ ] **Step 1: Write CashierPage** — assembles all components from Task 21-23 per the layout in spec section 9. TopStatusBar at top, ProductGrid + BarcodeInput on left, CartPanel on right, ActionButtonBar at bottom. Dialog instances for payment/member/product/suspend/refund/shift. Connections to CashierVM signals.

- [ ] **Step 2: Update main.qml to load CashierPage**

- [ ] **Step 3: Verify UI renders correctly** — run app, check layout matches wireframe.

- [ ] **Step 4: Commit**
  ```bash
  git commit -m "feat: cashier main page with full layout"
  ```

---

## Chunk 9: Application Entry & Integration

### Task 25: Application class and NavigationManager

**Files:**
- Create: `src/app/Application.h/.cpp`
- Create: `src/app/MainWindow.h/.cpp`
- Create: `src/app/NavigationManager.h/.cpp`
- Create: `src/app/qml/main.qml` (update)
- Create: `src/app/qml/AppNavigation.qml`

- [ ] **Step 1: Write Application** — initializes DatabaseManager, creates all Service instances, registers ViewModels as QML context properties, installs BarcodeFilter as global event filter, manages app lifecycle.

- [ ] **Step 2: Write MainWindow** — QQuickWidget or QML-based window, hosts main QML content.

- [ ] **Step 3: Write NavigationManager** — manages page switching between CashierPage, ProductManagePage, MemberPage. Exposed to QML as context property.

- [ ] **Step 4: Write AppNavigation.qml** — navigation wrapper, loads pages based on NavigationManager state.

- [ ] **Step 5: Update main.qml** — loads AppNavigation, sets up Material theme and window properties.

- [ ] **Step 6: Update main.cpp** — create Application instance, wire up services and QML engine.

- [ ] **Step 7: Build and verify app launches with cashier page**

- [ ] **Step 8: Commit**
  ```bash
  git commit -m "feat: application entry point with service wiring and navigation"
  ```

---

### Task 26: AppSettings and login flow

**Files:**
- Create: `src/core/settings/AppSettings.h/.cpp`
- Create: `src/app/qml/LoginPage.qml`

- [ ] **Step 1: Write AppSettings** — wraps QSettings, stores server URL, tenant_id, store_id, last cashier, theme preference. Uses QSettings with organization/domain scope.

- [ ] **Step 2: Write LoginPage.qml** — server URL input, username/password fields, login button. Binds to AuthService. On success, navigates to cashier page.

- [ ] **Step 3: Wire login flow in Application** — on startup, check if token is valid. If not, show LoginPage. On login success, initialize tenant config, load initial data, navigate to CashierPage.

- [ ] **Step 4: Update qml.qrc**

- [ ] **Step 5: Build and test login flow end-to-end**

- [ ] **Step 6: Commit**
  ```bash
  git commit -m "feat: settings and login flow"
  ```

---

## Chunk 10: Integration Testing & Polish

### Task 27: Integration tests

**Files:**
- Create: `tests/integration/test_full_cashier_flow.cpp`
- Create: `tests/integration/test_sync_flow.cpp`

- [ ] **Step 1: Write full cashier flow test** — scan product → add to cart → select member → start payment → pay cash → complete order → verify DB records.

- [ ] **Step 2: Write sync flow test** — create order offline → queue entry exists → simulate network → sync succeeds → order marked as synced.

- [ ] **Step 3: Run all tests and verify pass**

- [ ] **Step 4: Commit**
  ```bash
  git commit -m "test: integration tests for cashier and sync flows"
  ```

---

### Task 28: Receipt printing integration

**Files:**
- Modify: `src/core/hardware/platform/PrinterEscPos.cpp` (enhance)
- Modify: `src/cashier/service/OrderService.cpp` (add print call)

- [ ] **Step 1: Enhance PrinterEscPos** — build full ESC/POS receipt format: header (store name), items table, totals, payment info, footer. Support 58mm and 80mm widths.

- [ ] **Step 2: Integrate print into OrderService** — after completeOrder, call HardwareManager.printer()->printReceipt().

- [ ] **Step 3: Test with virtual serial port or log output**

- [ ] **Step 4: Commit**
  ```bash
  git commit -m "feat: receipt printing integration on order completion"
  ```

---

### Task 29: Final build verification on both Qt versions

- [ ] **Step 1: Clean build with Qt 5.15 on Windows**
  Run: `<qt5-qmake> && make clean && make`
  Expected: Build succeeds

- [ ] **Step 2: Clean build with Qt 6.x**
  Run: `<qt6-qmake> && make clean && make`
  Expected: Build succeeds

- [ ] **Step 3: Run all tests on both Qt versions**

- [ ] **Step 4: Fix any Qt5/Qt6 compatibility issues found**

- [ ] **Step 5: Final commit**
  ```bash
  git commit -m "chore: dual Qt version build verification complete"
  ```
