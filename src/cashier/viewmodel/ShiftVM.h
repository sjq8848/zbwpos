#pragma once
#include <QObject>
#include "service/ShiftService.h"

class ShiftVM : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap currentShift READ currentShift NOTIFY shiftChanged)
    Q_PROPERTY(QVariantList shiftOrders READ shiftOrders NOTIFY ordersChanged)
    Q_PROPERTY(double totalSales READ totalSales NOTIFY ordersChanged)
    Q_PROPERTY(double cashInDrawer READ cashInDrawer NOTIFY shiftChanged)

public:
    explicit ShiftVM(ShiftService *service, QObject *parent = nullptr);

    QVariantMap currentShift() const;
    QVariantList shiftOrders() const;
    double totalSales() const;
    double cashInDrawer() const;

    Q_INVOKABLE bool startShift(double startCash);
    Q_INVOKABLE bool endShift(double endCash);
    Q_INVOKABLE void refresh();

    void setTenantStore(const QString &tenantId, const QString &storeId);
    void setCashier(int cashierId);

signals:
    void shiftChanged();
    void ordersChanged();

private:
    ShiftService *m_service;
    Shift m_currentShift;
    QList<Order> m_shiftOrders;
    QString m_tenantId;
    QString m_storeId;
    int m_cashierId = 0;
};
