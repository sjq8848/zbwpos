#include "CashierVM.h"
#include "cashier/service/PaymentService.h"

CashierVM::CashierVM(ProductService *productService, MemberService *memberService,
                       OrderService *orderService, PaymentService *paymentService,
                       ShiftService *shiftService, QObject *parent)
    : QObject(parent)
    , m_cart(new CartVM(this))
    , m_payment(new PaymentVM(this))
    , m_shift(new ShiftVM(shiftService, this))
    , m_productService(productService)
    , m_memberService(memberService)
    , m_orderService(orderService)
    , m_paymentService(paymentService)
{
    connect(m_payment, &PaymentVM::paymentCompleted,
            this, &CashierVM::onPaymentCompleted);
}

CartVM* CashierVM::cart() const { return m_cart; }
PaymentVM* CashierVM::payment() const { return m_payment; }

QVariantMap CashierVM::currentMember() const {
    QVariantMap map;
    if (m_currentMember.id > 0) {
        map["id"] = m_currentMember.id;
        map["name"] = m_currentMember.name;
        map["cardNo"] = m_currentMember.cardNo;
        map["phone"] = m_currentMember.phone;
        map["points"] = m_currentMember.points;
        map["balance"] = m_currentMember.balance;
    }
    return map;
}

bool CashierVM::isProcessing() const { return m_isProcessing; }
QString CashierVM::statusMessage() const { return m_statusMessage; }

void CashierVM::setTenantStore(const QString &tenantId, const QString &storeId) {
    m_tenantId = tenantId;
    m_storeId = storeId;
    m_shift->setProperty("tenantId", tenantId);
}

void CashierVM::setCashier(int cashierId) {
    m_cashierId = cashierId;
    m_shift->setProperty("cashierId", cashierId);
}

void CashierVM::scanBarcode(const QString &barcode) {
    inputBarcode(barcode);
}

void CashierVM::inputBarcode(const QString &barcode) {
    if (m_tenantId.isEmpty() || m_storeId.isEmpty()) {
        m_statusMessage = "System not configured";
        emit statusChanged();
        return;
    }

    Product p = m_productService->findByBarcode(m_tenantId, m_storeId, barcode);
    if (p.id == 0) {
        m_statusMessage = "Product not found: " + barcode;
        emit statusChanged();
        return;
    }

    m_cart->addItem(p.barcode, p.name, p.price, p.id);
    m_statusMessage = "Added: " + p.name;
    emit statusChanged();
}

void CashierVM::selectMember(const QString &keyword) {
    Member m;
    if (keyword.length() > 8) {
        m = m_memberService->findByPhone(m_tenantId, keyword);
    } else {
        m = m_memberService->findByCardNo(m_tenantId, keyword);
    }

    if (m.id == 0) {
        m_statusMessage = "Member not found";
        emit statusChanged();
        return;
    }

    m_currentMember = m;
    m_cart->setMemberId(QString::number(m.id));
    double rate = m_memberService->calculateDiscount(m, m_cart->subtotal());
    m_cart->setMemberDiscount(rate);

    m_statusMessage = "Member: " + m.name;
    emit memberChanged();
    emit statusChanged();
}

void CashierVM::clearMember() {
    m_currentMember = Member();
    m_cart->setMemberId("");
    m_cart->setMemberDiscount(1.0);
    emit memberChanged();
}

void CashierVM::suspendOrder() {
    if (m_cart->itemCount() == 0) return;

    OrderRequest req;
    req.tenantId = m_tenantId;
    req.storeId = m_storeId;
    req.cashierId = m_cashierId;
    // req.shiftId = m_shift->currentShift().id;
    req.remark = "Suspended";

    auto result = m_orderService->createOrder(req);
    if (result.success) {
        m_orderService->suspendOrder(result.orderNo);
        m_cart->clear();
        m_statusMessage = "Order suspended";
        emit statusChanged();
    }
}

void CashierVM::resumeOrder(const QString &orderNo) {
    Order order = m_orderService->getOrder(orderNo);
    if (order.id == 0) return;

    m_cart->clear();
    for (const auto &item : order.items) {
        m_cart->addItem(item.barcode, item.productName, item.unitPrice, item.productId, item.quantity);
    }

    m_orderService->resumeOrder(orderNo);
    m_statusMessage = "Order resumed";
    emit statusChanged();
}

void CashierVM::startPayment() {
    if (m_cart->itemCount() == 0) {
        m_statusMessage = "Cart is empty";
        emit statusChanged();
        return;
    }

    m_payment->setAmountDue(m_cart->total());
    m_payment->reset();
    emit showPaymentDialog();
}

void CashierVM::clearAll() {
    m_cart->clear();
    clearMember();
    m_statusMessage = "";
    emit statusChanged();
}

void CashierVM::onPaymentCompleted(const QList<PaymentRecord> &records, double change) {
    m_isProcessing = true;
    emit processingChanged();

    OrderRequest req;
    req.tenantId = m_tenantId;
    req.storeId = m_storeId;
    req.cashierId = m_cashierId;
    req.memberId = m_currentMember.id;
    req.totalAmount = m_cart->subtotal();
    req.discountAmount = m_cart->subtotal() - m_cart->total();
    req.finalAmount = m_cart->total();

    for (int i = 0; i < m_cart->items()->rowCount(); ++i) {
        auto item = m_cart->items()->getItem(i);
        req.items.append(item);
    }

    auto result = m_orderService->createOrder(req);
    if (result.success) {
        m_orderService->completeOrder(result.orderNo, records);
        m_statusMessage = "Order completed: " + result.orderNo;
        emit orderCompleted(result.orderNo);
    } else {
        m_statusMessage = "Error: " + result.error;
    }

    m_cart->clear();
    clearMember();
    m_isProcessing = false;
    emit processingChanged();
    emit statusChanged();
}
