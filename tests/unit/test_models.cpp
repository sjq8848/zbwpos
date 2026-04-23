#include <QtTest>
#include <QJsonObject>
#include "core/database/Models.h"

class TestModels : public QObject {
    Q_OBJECT

private slots:
    void testProductDefaults();
    void testProductToJson();
    void testProductFromJson();
    void testCategoryDefaults();
    void testMemberDefaults();
    void testOrderDefaults();
    void testCashierDefaults();
    void testShiftDefaults();
};

void TestModels::testProductDefaults() {
    Product p;
    QCOMPARE(p.id, 0);
    QVERIFY(p.cloudId.isEmpty());
    QVERIFY(p.tenantId.isEmpty());
    QVERIFY(p.storeId.isEmpty());
    QVERIFY(p.barcode.isEmpty());
    QVERIFY(p.sku.isEmpty());
    QVERIFY(p.name.isEmpty());
    QCOMPARE(p.categoryId, 0);
    QCOMPARE(p.unit, QStringLiteral("件"));
    QCOMPARE(p.price, 0.0);
    QCOMPARE(p.costPrice, 0.0);
    QCOMPARE(p.stockQty, 0.0);
    QCOMPARE(p.minStock, 0.0);
    QCOMPARE(p.status, 1);
    QVERIFY(p.imageUrl.isEmpty());
    QCOMPARE(p.syncStatus, 0);
}

void TestModels::testProductToJson() {
    Product p;
    p.cloudId = "cloud123";
    p.tenantId = "tenant1";
    p.storeId = "store1";
    p.barcode = "6901234567890";
    p.sku = "SKU001";
    p.name = "Test Product";
    p.categoryId = 5;
    p.unit = "kg";
    p.price = 99.99;
    p.costPrice = 50.0;
    p.stockQty = 100.5;
    p.minStock = 10.0;
    p.status = 1;
    p.imageUrl = "http://example.com/image.png";
    p.syncStatus = 0;

    QJsonObject json = p.toJson();
    QCOMPARE(json["cloud_id"].toString(), QString("cloud123"));
    QCOMPARE(json["tenant_id"].toString(), QString("tenant1"));
    QCOMPARE(json["store_id"].toString(), QString("store1"));
    QCOMPARE(json["barcode"].toString(), QString("6901234567890"));
    QCOMPARE(json["sku"].toString(), QString("SKU001"));
    QCOMPARE(json["name"].toString(), QString("Test Product"));
    QCOMPARE(json["category_id"].toInt(), 5);
    QCOMPARE(json["unit"].toString(), QString("kg"));
    QCOMPARE(json["price"].toDouble(), 99.99);
    QCOMPARE(json["cost_price"].toDouble(), 50.0);
    QCOMPARE(json["stock_qty"].toDouble(), 100.5);
    QCOMPARE(json["min_stock"].toDouble(), 10.0);
    QCOMPARE(json["status"].toInt(), 1);
    QCOMPARE(json["image_url"].toString(), QString("http://example.com/image.png"));
    QCOMPARE(json["sync_status"].toInt(), 0);
}

void TestModels::testProductFromJson() {
    QJsonObject json;
    json["cloud_id"] = "cloud456";
    json["tenant_id"] = "tenant2";
    json["store_id"] = "store2";
    json["barcode"] = "6909876543210";
    json["sku"] = "SKU002";
    json["name"] = "Another Product";
    json["category_id"] = 3;
    json["unit"] = "个";
    json["price"] = 199.99;
    json["cost_price"] = 100.0;
    json["stock_qty"] = 50.0;
    json["min_stock"] = 5.0;
    json["status"] = 0;
    json["image_url"] = "http://example.com/image2.png";
    json["sync_status"] = 1;

    Product p = Product::fromJson(json);
    QCOMPARE(p.cloudId, QString("cloud456"));
    QCOMPARE(p.tenantId, QString("tenant2"));
    QCOMPARE(p.storeId, QString("store2"));
    QCOMPARE(p.barcode, QString("6909876543210"));
    QCOMPARE(p.sku, QString("SKU002"));
    QCOMPARE(p.name, QString("Another Product"));
    QCOMPARE(p.categoryId, 3);
    QCOMPARE(p.unit, QString("个"));
    QCOMPARE(p.price, 199.99);
    QCOMPARE(p.costPrice, 100.0);
    QCOMPARE(p.stockQty, 50.0);
    QCOMPARE(p.minStock, 5.0);
    QCOMPARE(p.status, 0);
    QCOMPARE(p.imageUrl, QString("http://example.com/image2.png"));
    QCOMPARE(p.syncStatus, 1);
}

void TestModels::testCategoryDefaults() {
    Category c;
    QCOMPARE(c.id, 0);
    QVERIFY(c.cloudId.isEmpty());
    QVERIFY(c.tenantId.isEmpty());
    QVERIFY(c.name.isEmpty());
    QCOMPARE(c.parentId, 0);
    QCOMPARE(c.sortOrder, 0);
    QCOMPARE(c.syncStatus, 0);
}

void TestModels::testMemberDefaults() {
    Member m;
    QCOMPARE(m.id, 0);
    QVERIFY(m.cloudId.isEmpty());
    QVERIFY(m.tenantId.isEmpty());
    QVERIFY(m.cardNo.isEmpty());
    QVERIFY(m.name.isEmpty());
    QVERIFY(m.phone.isEmpty());
    QCOMPARE(m.gender, 0);
    QVERIFY(!m.birthday.isValid());
    QCOMPARE(m.levelId, 0);
    QCOMPARE(m.points, 0);
    QCOMPARE(m.balance, 0.0);
    QCOMPARE(m.totalSpent, 0.0);
    QCOMPARE(m.status, 1);
    QCOMPARE(m.syncStatus, 0);
}

void TestModels::testOrderDefaults() {
    Order o;
    QCOMPARE(o.id, 0);
    QVERIFY(o.cloudId.isEmpty());
    QVERIFY(o.tenantId.isEmpty());
    QVERIFY(o.storeId.isEmpty());
    QVERIFY(o.orderNo.isEmpty());
    QCOMPARE(o.memberId, 0);
    QCOMPARE(o.totalAmount, 0.0);
    QCOMPARE(o.discountAmount, 0.0);
    QCOMPARE(o.finalAmount, 0.0);
    QVERIFY(o.paymentMethod.isEmpty());
    QCOMPARE(o.status, 1);
    QCOMPARE(o.cashierId, 0);
    QCOMPARE(o.shiftId, 0);
    QVERIFY(o.remark.isEmpty());
    QCOMPARE(o.syncStatus, 0);
    QVERIFY(o.items.isEmpty());
    QVERIFY(o.payments.isEmpty());
}

void TestModels::testCashierDefaults() {
    Cashier c;
    QCOMPARE(c.id, 0);
    QVERIFY(c.cloudId.isEmpty());
    QVERIFY(c.tenantId.isEmpty());
    QVERIFY(c.username.isEmpty());
    QVERIFY(c.passwordHash.isEmpty());
    QVERIFY(c.name.isEmpty());
    QCOMPARE(c.role, QString("cashier"));
    QCOMPARE(c.status, 1);
    QCOMPARE(c.syncStatus, 0);
}

void TestModels::testShiftDefaults() {
    Shift s;
    QCOMPARE(s.id, 0);
    QVERIFY(s.cloudId.isEmpty());
    QVERIFY(s.tenantId.isEmpty());
    QVERIFY(s.storeId.isEmpty());
    QCOMPARE(s.cashierId, 0);
    QVERIFY(!s.startTime.isValid());
    QVERIFY(!s.endTime.isValid());
    QCOMPARE(s.startCash, 0.0);
    QCOMPARE(s.endCash, 0.0);
    QCOMPARE(s.status, 1);
    QCOMPARE(s.syncStatus, 0);
}

QTEST_MAIN(TestModels)
#include "test_models.moc"
