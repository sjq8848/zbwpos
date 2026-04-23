#include <QtTest>
#include "core/database/DatabaseManager.h"
#include "core/network/OfflineQueue.h"
#include "product/repository/ProductRepo.h"

class TestSyncFlow : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testOfflineQueuePush();
    void testOfflineQueuePending();
    void testOfflineQueueMarkCompleted();
    void testOfflineQueueRetry();
    void testSyncStatusTracking();
    void cleanupTestCase();

private:
    DatabaseManager *m_db = nullptr;
    OfflineQueue *m_queue = nullptr;
};

void TestSyncFlow::initTestCase() {
    m_db = new DatabaseManager(this);
    QVERIFY(m_db->open(":memory:"));
    QVERIFY(m_db->runMigrations());

    m_queue = new OfflineQueue(this);
    m_queue->setDatabase(m_db);
}

void TestSyncFlow::testOfflineQueuePush() {
    m_queue->push("INSERT", "products", 1, QJsonObject{{"name", "test"}});
    QCOMPARE(m_queue->getPending().size(), 1);
}

void TestSyncFlow::testOfflineQueuePending() {
    auto tasks = m_queue->getPending();
    QVERIFY(!tasks.isEmpty());
    QCOMPARE(tasks.first().operation, QString("INSERT"));
    QCOMPARE(tasks.first().tableName, QString("products"));
}

void TestSyncFlow::testOfflineQueueMarkCompleted() {
    auto tasks = m_queue->getPending();
    QVERIFY(!tasks.isEmpty());
    m_queue->markCompleted(tasks.first().id);
    QCOMPARE(m_queue->getPending().size(), 0);
}

void TestSyncFlow::testOfflineQueueRetry() {
    m_queue->push("UPDATE", "members", 2, QJsonObject{{"name", "test"}});
    auto tasks = m_queue->getPending();
    m_queue->markFailed(tasks.first().id, "Network error");

    // Retry
    m_queue->retry(tasks.first().id);
    auto pending = m_queue->getPending();
    QVERIFY(!pending.isEmpty());
}

void TestSyncFlow::testSyncStatusTracking() {
    ProductRepo repo(m_db, this);
    Product p;
    p.tenantId = "t001";
    p.storeId = "s001";
    p.barcode = "SYNC001";
    p.name = "Sync Test Product";
    p.price = 5.00;

    repo.save(p);

    // Product should be findable locally
    Product found = repo.findByBarcode("t001", "s001", "SYNC001");
    QCOMPARE(found.name, QString("Sync Test Product"));
}

void TestSyncFlow::cleanupTestCase() {
    m_db->close();
}

QTEST_MAIN(TestSyncFlow)
#include "test_sync_flow.moc"
