#include "OrderRepo.h"
#include "core/database/DatabaseManager.h"
#include "core/network/OfflineQueue.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QRandomGenerator>

OrderRepo::OrderRepo(DatabaseManager *db, OfflineQueue *queue, QObject *parent)
    : QObject(parent)
    , m_db(db)
    , m_queue(queue)
{
}

QString OrderRepo::saveOrder(const Order &order)
{
    QSqlQuery query = m_db->execute(
        "INSERT INTO orders (tenant_id, store_id, order_no, member_id, total_amount, "
        "discount_amount, final_amount, payment_method, status, cashier_id, shift_id, "
        "remark, sync_status, created_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
        {order.tenantId, order.storeId, order.orderNo, order.memberId,
         order.totalAmount, order.discountAmount, order.finalAmount,
         order.paymentMethod, order.status, order.cashierId, order.shiftId,
         order.remark, order.syncStatus, QDateTime::currentDateTime()});

    if (query.lastError().isValid()) {
        return QString();
    }

    int orderId = query.lastInsertId().toInt();

    // Insert order items
    for (const OrderItem &item : order.items) {
        m_db->execute(
            "INSERT INTO order_items (tenant_id, order_id, product_id, barcode, "
            "product_name, unit_price, quantity, discount_rate, subtotal) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)",
            {item.tenantId, orderId, item.productId, item.barcode,
             item.productName, item.unitPrice, item.quantity,
             item.discountRate, item.subtotal});
    }

    // Insert payments
    for (const Payment &payment : order.payments) {
        m_db->execute(
            "INSERT INTO payments (tenant_id, order_id, method, amount, "
            "reference_no, status, created_at) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)",
            {payment.tenantId, orderId, payment.method, payment.amount,
             payment.referenceNo, payment.status, QDateTime::currentDateTime()});
    }

    // Push to offline queue for sync
    m_queue->push("INSERT", "orders", orderId, order.toJson());

    return order.orderNo;
}

bool OrderRepo::updateOrderStatus(int orderId, int status)
{
    m_db->execute("UPDATE orders SET status = ? WHERE id = ?", {status, orderId});
    m_queue->push("UPDATE", "orders", orderId, {{"status", status}});
    return true;
}

bool OrderRepo::updateOrderCloudId(int orderId, const QString &cloudId)
{
    m_db->execute("UPDATE orders SET cloud_id = ? WHERE id = ?", {cloudId, orderId});
    return true;
}

Order OrderRepo::findByOrderNo(const QString &orderNo)
{
    QSqlQuery query = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, order_no, member_id, total_amount, "
        "discount_amount, final_amount, payment_method, status, cashier_id, shift_id, "
        "remark, sync_status, created_at FROM orders WHERE order_no = ?",
        {orderNo});

    if (query.next()) {
        Order order;
        order.id = query.value(0).toInt();
        order.cloudId = query.value(1).toString();
        order.tenantId = query.value(2).toString();
        order.storeId = query.value(3).toString();
        order.orderNo = query.value(4).toString();
        order.memberId = query.value(5).toInt();
        order.totalAmount = query.value(6).toDouble();
        order.discountAmount = query.value(7).toDouble();
        order.finalAmount = query.value(8).toDouble();
        order.paymentMethod = query.value(9).toString();
        order.status = query.value(10).toInt();
        order.cashierId = query.value(11).toInt();
        order.shiftId = query.value(12).toInt();
        order.remark = query.value(13).toString();
        order.syncStatus = query.value(14).toInt();
        order.createdAt = query.value(15).toDateTime();
        return order;
    }

    return Order();
}

Order OrderRepo::findById(int id)
{
    QSqlQuery query = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, order_no, member_id, total_amount, "
        "discount_amount, final_amount, payment_method, status, cashier_id, shift_id, "
        "remark, sync_status, created_at FROM orders WHERE id = ?",
        {id});

    if (query.next()) {
        Order order;
        order.id = query.value(0).toInt();
        order.cloudId = query.value(1).toString();
        order.tenantId = query.value(2).toString();
        order.storeId = query.value(3).toString();
        order.orderNo = query.value(4).toString();
        order.memberId = query.value(5).toInt();
        order.totalAmount = query.value(6).toDouble();
        order.discountAmount = query.value(7).toDouble();
        order.finalAmount = query.value(8).toDouble();
        order.paymentMethod = query.value(9).toString();
        order.status = query.value(10).toInt();
        order.cashierId = query.value(11).toInt();
        order.shiftId = query.value(12).toInt();
        order.remark = query.value(13).toString();
        order.syncStatus = query.value(14).toInt();
        order.createdAt = query.value(15).toDateTime();
        return order;
    }

    return Order();
}

QList<Order> OrderRepo::findByDateRange(const QString &tenantId, const QString &storeId,
                                          const QDate &from, const QDate &to)
{
    QList<Order> result;
    QSqlQuery query = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, order_no, member_id, total_amount, "
        "discount_amount, final_amount, payment_method, status, cashier_id, shift_id, "
        "remark, sync_status, created_at FROM orders "
        "WHERE tenant_id = ? AND store_id = ? AND date(created_at) BETWEEN ? AND ? "
        "ORDER BY created_at DESC",
        {tenantId, storeId, from.toString(Qt::ISODate), to.toString(Qt::ISODate)});

    while (query.next()) {
        Order order;
        order.id = query.value(0).toInt();
        order.cloudId = query.value(1).toString();
        order.tenantId = query.value(2).toString();
        order.storeId = query.value(3).toString();
        order.orderNo = query.value(4).toString();
        order.memberId = query.value(5).toInt();
        order.totalAmount = query.value(6).toDouble();
        order.discountAmount = query.value(7).toDouble();
        order.finalAmount = query.value(8).toDouble();
        order.paymentMethod = query.value(9).toString();
        order.status = query.value(10).toInt();
        order.cashierId = query.value(11).toInt();
        order.shiftId = query.value(12).toInt();
        order.remark = query.value(13).toString();
        order.syncStatus = query.value(14).toInt();
        order.createdAt = query.value(15).toDateTime();
        result.append(order);
    }

    return result;
}

QList<Order> OrderRepo::findTodayOrders(const QString &tenantId, const QString &storeId)
{
    QDate today = QDate::currentDate();
    return findByDateRange(tenantId, storeId, today, today);
}

QList<Order> OrderRepo::findSuspended(const QString &tenantId, const QString &storeId)
{
    QList<Order> result;
    QSqlQuery query = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, order_no, member_id, total_amount, "
        "discount_amount, final_amount, payment_method, status, cashier_id, shift_id, "
        "remark, sync_status, created_at FROM orders "
        "WHERE tenant_id = ? AND store_id = ? AND status = 0 "
        "ORDER BY created_at DESC",
        {tenantId, storeId});

    while (query.next()) {
        Order order;
        order.id = query.value(0).toInt();
        order.cloudId = query.value(1).toString();
        order.tenantId = query.value(2).toString();
        order.storeId = query.value(3).toString();
        order.orderNo = query.value(4).toString();
        order.memberId = query.value(5).toInt();
        order.totalAmount = query.value(6).toDouble();
        order.discountAmount = query.value(7).toDouble();
        order.finalAmount = query.value(8).toDouble();
        order.paymentMethod = query.value(9).toString();
        order.status = query.value(10).toInt();
        order.cashierId = query.value(11).toInt();
        order.shiftId = query.value(12).toInt();
        order.remark = query.value(13).toString();
        order.syncStatus = query.value(14).toInt();
        order.createdAt = query.value(15).toDateTime();
        result.append(order);
    }

    return result;
}

QList<Order> OrderRepo::getPendingUpload(const QString &tenantId, const QString &storeId)
{
    QList<Order> result;
    QSqlQuery query = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, order_no, member_id, total_amount, "
        "discount_amount, final_amount, payment_method, status, cashier_id, shift_id, "
        "remark, sync_status, created_at FROM orders "
        "WHERE tenant_id = ? AND store_id = ? AND sync_status = ? "
        "ORDER BY created_at ASC",
        {tenantId, storeId, ZBW::SYNC_PENDING});

    while (query.next()) {
        Order order;
        order.id = query.value(0).toInt();
        order.cloudId = query.value(1).toString();
        order.tenantId = query.value(2).toString();
        order.storeId = query.value(3).toString();
        order.orderNo = query.value(4).toString();
        order.memberId = query.value(5).toInt();
        order.totalAmount = query.value(6).toDouble();
        order.discountAmount = query.value(7).toDouble();
        order.finalAmount = query.value(8).toDouble();
        order.paymentMethod = query.value(9).toString();
        order.status = query.value(10).toInt();
        order.cashierId = query.value(11).toInt();
        order.shiftId = query.value(12).toInt();
        order.remark = query.value(13).toString();
        order.syncStatus = query.value(14).toInt();
        order.createdAt = query.value(15).toDateTime();
        result.append(order);
    }

    return result;
}

void OrderRepo::markSynced(int orderId, const QString &cloudId)
{
    m_db->execute("UPDATE orders SET sync_status = ?, cloud_id = ? WHERE id = ?",
                  {ZBW::SYNC_OK, cloudId, orderId});
}

QString OrderRepo::generateOrderNo()
{
    QDateTime now = QDateTime::currentDateTime();
    QString datePart = now.toString("yyyyMMdd");
    QString timePart = now.toString("HHmmss");
    int random = QRandomGenerator::global()->bounded(1000, 9999);
    return QStringLiteral("ORD%1%2%3").arg(datePart, timePart).arg(random);
}
