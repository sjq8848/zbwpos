#pragma once
#include <QObject>
#include "core/database/Models.h"

class DatabaseManager;
class OfflineQueue;

class OrderRepo : public QObject {
    Q_OBJECT
public:
    explicit OrderRepo(DatabaseManager *db, OfflineQueue *queue, QObject *parent = nullptr);

    QString saveOrder(const Order &order);  // returns order_no
    bool updateOrderStatus(int orderId, int status);
    bool updateOrderCloudId(int orderId, const QString &cloudId);

    Order findByOrderNo(const QString &orderNo);
    Order findById(int id);
    QList<Order> findByDateRange(const QString &tenantId, const QString &storeId,
                                  const QDate &from, const QDate &to);
    QList<Order> findTodayOrders(const QString &tenantId, const QString &storeId);
    QList<Order> findSuspended(const QString &tenantId, const QString &storeId);
    QList<Order> getPendingUpload(const QString &tenantId, const QString &storeId);
    void markSynced(int orderId, const QString &cloudId);

private:
    QString generateOrderNo();
    DatabaseManager *m_db;
    OfflineQueue *m_queue;
};
