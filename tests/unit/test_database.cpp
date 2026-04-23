#include <QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "core/database/DatabaseManager.h"

class TestDatabase : public QObject {
    Q_OBJECT

private slots:
    void testOpenDatabase();
    void testMigrationCreatesTables();
};

void TestDatabase::testOpenDatabase() {
    DatabaseManager db;
    QVERIFY(db.open(":memory:"));
    QVERIFY(db.isOpen());
    db.close();
    QVERIFY(!db.isOpen());
}

void TestDatabase::testMigrationCreatesTables() {
    DatabaseManager db;
    QVERIFY(db.open(":memory:"));
    QVERIFY(db.runMigrations());

    QSqlQuery q(db.database());
    q.prepare("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
    QVERIFY(q.exec());

    QStringList tables;
    while (q.next())
        tables << q.value(0).toString();

    QCOMPARE(tables.count(), 12);

    QVERIFY(tables.contains("cashiers"));
    QVERIFY(tables.contains("categories"));
    QVERIFY(tables.contains("members"));
    QVERIFY(tables.contains("member_levels"));
    QVERIFY(tables.contains("offline_queue"));
    QVERIFY(tables.contains("order_items"));
    QVERIFY(tables.contains("orders"));
    QVERIFY(tables.contains("payments"));
    QVERIFY(tables.contains("products"));
    QVERIFY(tables.contains("schema_version"));
    QVERIFY(tables.contains("shifts"));
    QVERIFY(tables.contains("tenant_config"));
}

QTEST_MAIN(TestDatabase)
#include "test_database.moc"
