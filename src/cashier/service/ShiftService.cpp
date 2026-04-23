#include "ShiftService.h"
#include "core/database/DatabaseManager.h"
#include "core/network/OfflineQueue.h"
#include <QDateTime>

ShiftService::ShiftService(DatabaseManager *db, OfflineQueue *queue, QObject *parent)
    : QObject(parent), m_db(db), m_queue(queue)
{
}

Shift ShiftService::startShift(const QString &tenantId, const QString &storeId,
                                int cashierId, double startCash) {
    Shift shift;
    shift.tenantId = tenantId;
    shift.storeId = storeId;
    shift.cashierId = cashierId;
    shift.startCash = startCash;
    shift.startTime = QDateTime::currentDateTime();
    shift.status = 1; // open

    auto q = m_db->execute(
        "INSERT INTO shifts (tenant_id, store_id, cashier_id, start_time, start_cash, status) "
        "VALUES (?, ?, ?, ?, ?, 1)",
        {tenantId, storeId, cashierId, shift.startCash.toString(Qt::ISODate), startCash}
    );

    if (q.lastInsertId().isValid()) {
        shift.id = q.lastInsertId().toInt();
        m_queue->push("INSERT", "shifts", shift.id, QJsonObject{});
        emit shiftStarted(shift);
    }
    return shift;
}

ShiftSummary ShiftService::endShift(int shiftId, double endCash) {
    ShiftSummary summary;
    summary.endCash = endCash;

    auto q = m_db->execute(
        "SELECT tenant_id, start_cash FROM shifts WHERE id = ?",
        {shiftId}
    );
    if (q.next()) {
        // Calculate sales from orders in this shift
        auto orderQ = m_db->execute(
            "SELECT COUNT(*), COALESCE(SUM(final_amount), 0) FROM orders "
            "WHERE shift_id = ? AND status = 1",
            {shiftId}
        );
        if (orderQ.next()) {
            summary.orderCount = orderQ.value(0).toInt();
            summary.totalSales = orderQ.value(1).toDouble();
        }

        m_db->execute(
            "UPDATE shifts SET end_time = ?, end_cash = ?, status = 2 WHERE id = ?",
            {QDateTime::currentDateTime().toString(Qt::ISODate), endCash, shiftId}
        );
        m_queue->push("UPDATE", "shifts", shiftId, QJsonObject{});

        emit shiftEnded(shiftId, summary);
    }

    return summary;
}

Shift ShiftService::getCurrentShift(const QString &tenantId, const QString &storeId, int cashierId) {
    auto q = m_db->execute(
        "SELECT id, start_time, start_cash FROM shifts "
        "WHERE tenant_id = ? AND store_id = ? AND cashier_id = ? AND status = 1 "
        "ORDER BY start_time DESC LIMIT 1",
        {tenantId, storeId, cashierId}
    );

    Shift shift;
    shift.tenantId = tenantId;
    shift.storeId = storeId;
    shift.cashierId = cashierId;

    if (q.next()) {
        shift.id = q.value(0).toInt();
        shift.startTime = QDateTime::fromString(q.value(1).toString(), Qt::ISODate);
        shift.startCash = q.value(2).toDouble();
        shift.status = 1;
    }
    return shift;
}

QList<Order> ShiftService::getShiftOrders(int shiftId) {
    QList<Order> orders;
    auto q = m_db->execute(
        "SELECT id, order_no, final_amount, payment_method, created_at FROM orders "
        "WHERE shift_id = ? ORDER BY created_at DESC",
        {shiftId}
    );

    while (q.next()) {
        Order o;
        o.id = q.value(0).toInt();
        o.orderNo = q.value(1).toString();
        o.finalAmount = q.value(2).toDouble();
        o.paymentMethod = q.value(3).toString();
        o.createdAt = QDateTime::fromString(q.value(4).toString(), Qt::ISODate);
        orders.append(o);
    }
    return orders;
}

bool ShiftService::hasOpenShift(const QString &tenantId, const QString &storeId, int cashierId) {
    auto q = m_db->execute(
        "SELECT COUNT(*) FROM shifts "
        "WHERE tenant_id = ? AND store_id = ? AND cashier_id = ? AND status = 1",
        {tenantId, storeId, cashierId}
    );
    if (q.next()) return q.value(0).toInt() > 0;
    return false;
}
