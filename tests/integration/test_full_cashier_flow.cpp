#include <QtTest>
#include "core/database/DatabaseManager.h"
#include "core/network/OfflineQueue.h"
#include "product/repository/ProductRepo.h"
#include "product/service/ProductService.h"
#include "member/repository/MemberRepo.h"
#include "member/service/MemberService.h"
#include "cashier/repository/OrderRepo.h"
#include "cashier/service/OrderService.h"
#include "cashier/service/PaymentService.h"
#include "cashier/service/ShiftService.h"
#include "cashier/viewmodel/CashierVM.h"

class TestFullCashierFlow : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testScanAndCheckout();
    void testMemberDiscount();
    void cleanupTestCase();

private:
    DatabaseManager *m_db = nullptr;
    OfflineQueue *m_queue = nullptr;
    ProductRepo *m_productRepo = nullptr;
    ProductService *m_productService = nullptr;
    MemberRepo *m_memberRepo = nullptr;
    MemberService *m_memberService = nullptr;
    OrderRepo *m_orderRepo = nullptr;
    OrderService *m_orderService = nullptr;
    PaymentService *m_paymentService = nullptr;
    ShiftService *m_shiftService = nullptr;
};

void TestFullCashierFlow::initTestCase() {
    m_db = new DatabaseManager(this);
    QVERIFY(m_db->open(":memory:"));
    QVERIFY(m_db->runMigrations());

    m_queue = new OfflineQueue(this);
    m_queue->setDatabase(m_db);

    m_productRepo = new ProductRepo(m_db, this);
    m_productService = new ProductService(m_productRepo, m_queue, this);

    m_memberRepo = new MemberRepo(m_db, this);
    m_memberService = new MemberService(m_memberRepo, m_queue, this);

    m_orderRepo = new OrderRepo(m_db, m_queue, this);
    m_orderService = new OrderService(m_orderRepo, this);

    m_paymentService = new PaymentService(m_memberService, this);
    m_shiftService = new ShiftService(m_db, m_queue, this);

    // Seed test data
    Product p;
    p.tenantId = "t001";
    p.storeId = "s001";
    p.barcode = "6901234567890";
    p.name = "Test Product";
    p.price = 10.00;
    m_productRepo->save(p);

    Member m;
    m.tenantId = "t001";
    m.cardNo = "M001";
    m.name = "Test Member";
    m.phone = "13800138000";
    m.balance = 100.00;
    m.points = 500;
    m_memberRepo->save(m);
}

void TestFullCashierFlow::testScanAndCheckout() {
    // Find product by barcode
    Product p = m_productService->findByBarcode("t001", "s001", "6901234567890");
    QVERIFY(p.id > 0);
    QCOMPARE(p.name, QString("Test Product"));
    QCOMPARE(p.price, 10.00);

    // Create order
    OrderRequest req;
    req.tenantId = "t001";
    req.storeId = "s001";
    req.cashierId = 1;

    CartItem item;
    item.barcode = p.barcode;
    item.name = p.name;
    item.unitPrice = p.price;
    item.quantity = 2;
    item.subtotal = 20.00;
    item.productId = p.id;
    req.items.append(item);
    req.totalAmount = 20.00;
    req.finalAmount = 20.00;

    auto result = m_orderService->createOrder(req);
    QVERIFY(result.success);
    QVERIFY(!result.orderNo.isEmpty());

    // Verify order in DB
    Order order = m_orderService->getOrder(result.orderNo);
    QCOMPARE(order.finalAmount, 20.00);
    QCOMPARE(order.status, 1);
}

void TestFullCashierFlow::testMemberDiscount() {
    Member m = m_memberService->findByCardNo("t001", "M001");
    QVERIFY(m.id > 0);

    double discounted = m_memberService->calculateDiscount(m, 100.00);
    QVERIFY(discounted <= 100.00);
    QVERIFY(discounted > 0);
}

void TestFullCashierFlow::cleanupTestCase() {
    m_db->close();
}

QTEST_MAIN(TestFullCashierFlow)
#include "test_full_cashier_flow.moc"
