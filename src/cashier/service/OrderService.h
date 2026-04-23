#pragma once
#include <QObject>
#include "repository/OrderRepo.h"
#include "cashier/viewmodel/models/CartItemModel.h"

class OrderRepo;
class PaymentService;
class MemberService;

struct OrderRequest {
    QString tenantId;
    QString storeId;
    int cashierId = 0;
    int shiftId = 0;
    int memberId = 0;
    QList<CartItem> items;
    double totalAmount = 0;
    double discountAmount = 0;
    double finalAmount = 0;
    QString remark;
};

struct OrderResult {
    bool success = false;
    QString orderNo;
    int orderId = 0;
    QString error;
};

class OrderService : public QObject {
    Q_OBJECT
public:
    explicit OrderService(OrderRepo *repo, QObject *parent = nullptr);

    OrderResult createOrder(const OrderRequest &request);
    bool completeOrder(const QString &orderNo, const QList<PaymentRecord> &payments);
    bool suspendOrder(const QString &orderNo);
    bool resumeOrder(const QString &orderNo);
    OrderResult refundOrder(const QString &orderNo, const QString &reason);

    Order getOrder(const QString &orderNo);
    QList<Order> getTodayOrders(const QString &tenantId, const QString &storeId);
    QList<Order> getSuspendedOrders(const QString &tenantId, const QString &storeId);

signals:
    void orderCreated(const Order &order);
    void orderCompleted(const Order &order);

private:
    OrderRepo *m_repo;
};
