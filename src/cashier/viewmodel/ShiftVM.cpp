#include "ShiftVM.h"

ShiftVM::ShiftVM(ShiftService *service, QObject *parent)
    : QObject(parent)
    , m_service(service)
{
}

QVariantMap ShiftVM::currentShift() const
{
    QVariantMap map;
    if (m_currentShift.id > 0) {
        map["id"] = m_currentShift.id;
        map["tenantId"] = m_currentShift.tenantId;
        map["storeId"] = m_currentShift.storeId;
        map["cashierId"] = m_currentShift.cashierId;
        map["startTime"] = m_currentShift.startTime;
        map["endTime"] = m_currentShift.endTime;
        map["startCash"] = m_currentShift.startCash;
        map["endCash"] = m_currentShift.endCash;
        map["status"] = m_currentShift.status;
    }
    return map;
}

QVariantList ShiftVM::shiftOrders() const
{
    QVariantList list;
    for (const Order &order : m_shiftOrders) {
        QVariantMap map;
        map["orderNo"] = order.orderNo;
        map["totalAmount"] = order.totalAmount;
        map["finalAmount"] = order.finalAmount;
        map["paymentMethod"] = order.paymentMethod;
        map["createdAt"] = order.createdAt;
        map["status"] = order.status;
        list.append(map);
    }
    return list;
}

double ShiftVM::totalSales() const
{
    double total = 0;
    for (const Order &order : m_shiftOrders) {
        if (order.status == ZBW::ORDER_NORMAL) {
            total += order.finalAmount;
        }
    }
    return total;
}

double ShiftVM::cashInDrawer() const
{
    if (m_currentShift.id == 0)
        return 0;
    return m_currentShift.startCash; // + cash payments - cash refunds
}

bool ShiftVM::startShift(double startCash)
{
    if (m_tenantId.isEmpty() || m_storeId.isEmpty() || m_cashierId == 0)
        return false;

    m_currentShift = m_service->startShift(m_tenantId, m_storeId, m_cashierId, startCash);
    if (m_currentShift.id > 0) {
        emit shiftChanged();
        return true;
    }
    return false;
}

bool ShiftVM::endShift(double endCash)
{
    if (m_currentShift.id == 0)
        return false;

    ShiftSummary summary = m_service->endShift(m_currentShift.id, endCash);
    if (summary.orderCount >= 0) {
        m_currentShift = Shift();
        m_shiftOrders.clear();
        emit shiftChanged();
        emit ordersChanged();
        return true;
    }
    return false;
}

void ShiftVM::refresh()
{
    if (m_tenantId.isEmpty() || m_storeId.isEmpty() || m_cashierId == 0)
        return;

    m_currentShift = m_service->getCurrentShift(m_tenantId, m_storeId, m_cashierId);
    if (m_currentShift.id > 0) {
        m_shiftOrders = m_service->getShiftOrders(m_currentShift.id);
    } else {
        m_shiftOrders.clear();
    }
    emit shiftChanged();
    emit ordersChanged();
}

void ShiftVM::setTenantStore(const QString &tenantId, const QString &storeId)
{
    m_tenantId = tenantId;
    m_storeId = storeId;
}

void ShiftVM::setCashier(int cashierId)
{
    m_cashierId = cashierId;
}
