#pragma once
#include <QObject>
#include <QList>
#include "core/database/Models.h"

class DatabaseManager;
class OfflineQueue;

struct ShiftSummary {
    int orderCount = 0;
    double totalSales = 0;
    double cashSales = 0;
    double cardSales = 0;
    double mobileSales = 0;
    double startCash = 0;
    double endCash = 0;
};

class ShiftService : public QObject {
    Q_OBJECT
public:
    explicit ShiftService(DatabaseManager *db, OfflineQueue *queue, QObject *parent = nullptr);

    Shift startShift(const QString &tenantId, const QString &storeId, int cashierId, double startCash);
    ShiftSummary endShift(int shiftId, double endCash);
    Shift getCurrentShift(const QString &tenantId, const QString &storeId, int cashierId);
    QList<Order> getShiftOrders(int shiftId);
    bool hasOpenShift(const QString &tenantId, const QString &storeId, int cashierId);

signals:
    void shiftStarted(const Shift &shift);
    void shiftEnded(int shiftId, const ShiftSummary &summary);

private:
    DatabaseManager *m_db;
    OfflineQueue *m_queue;
};
