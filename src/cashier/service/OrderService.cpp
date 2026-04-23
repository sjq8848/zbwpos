#include "OrderService.h"
#include "cashier/viewmodel/models/PaymentRecordModel.h"

OrderService::OrderService(OrderRepo *repo, QObject *parent)
    : QObject(parent), m_repo(repo)
{
}

OrderResult OrderService::createOrder(const OrderRequest &request) {
    OrderResult result;
    if (request.items.isEmpty()) {
        result.error = "Cannot create empty order";
        return result;
    }

    Order order;
    order.tenantId = request.tenantId;
    order.storeId = request.storeId;
    order.cashierId = request.cashierId;
    order.shiftId = request.shiftId;
    order.memberId = request.memberId;
    order.totalAmount = request.totalAmount;
    order.discountAmount = request.discountAmount;
    order.finalAmount = request.finalAmount;
    order.remark = request.remark;
    order.status = 1; // normal

    for (const auto &item : request.items) {
        OrderItem oi;
        oi.tenantId = request.tenantId;
        oi.barcode = item.barcode;
        oi.productName = item.name;
        oi.productId = item.productId;
        oi.unitPrice = item.unitPrice;
        oi.quantity = item.quantity;
        oi.discountRate = item.discountRate;
        oi.subtotal = item.subtotal;
        order.items.append(oi);
    }

    QString orderNo = m_repo->saveOrder(order);
    if (orderNo.isEmpty()) {
        result.error = "Failed to save order";
        return result;
    }

    order.orderNo = orderNo;
    result.success = true;
    result.orderNo = orderNo;
    emit orderCreated(order);
    return result;
}

bool OrderService::completeOrder(const QString &orderNo, const QList<PaymentRecord> &payments) {
    Order order = m_repo->findByOrderNo(orderNo);
    if (order.id == 0) return false;

    for (const auto &rec : payments) {
        Payment p;
        p.tenantId = order.tenantId;
        p.orderId = order.id;
        p.method = rec.method;
        p.amount = rec.amount;
        p.referenceNo = rec.referenceNo;
        p.status = 1;
        order.payments.append(p);
    }

    order.status = 1; // completed
    emit orderCompleted(order);
    return true;
}

bool OrderService::suspendOrder(const QString &orderNo) {
    return m_repo->updateOrderStatus(m_repo->findByOrderNo(orderNo).id, 3); // suspended
}

bool OrderService::resumeOrder(const QString &orderNo) {
    return m_repo->updateOrderStatus(m_repo->findByOrderNo(orderNo).id, 1); // normal
}

OrderResult OrderService::refundOrder(const QString &orderNo, const QString &reason) {
    OrderResult result;
    Order order = m_repo->findByOrderNo(orderNo);
    if (order.id == 0) {
        result.error = "Order not found";
        return result;
    }

    m_repo->updateOrderStatus(order.id, 2); // refunded
    result.success = true;
    result.orderNo = orderNo;
    return result;
}

Order OrderService::getOrder(const QString &orderNo) {
    return m_repo->findByOrderNo(orderNo);
}

QList<Order> OrderService::getTodayOrders(const QString &tenantId, const QString &storeId) {
    return m_repo->findTodayOrders(tenantId, storeId);
}

QList<Order> OrderService::getSuspendedOrders(const QString &tenantId, const QString &storeId) {
    return m_repo->findSuspended(tenantId, storeId);
}
