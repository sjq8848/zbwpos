#pragma once
#include <QObject>
#include "CartVM.h"
#include "PaymentVM.h"
#include "ShiftVM.h"
#include "product/service/ProductService.h"
#include "member/service/MemberService.h"
#include "cashier/service/OrderService.h"

class CashierVM : public QObject {
    Q_OBJECT
    Q_PROPERTY(CartVM* cart READ cart CONSTANT)
    Q_PROPERTY(PaymentVM* payment READ payment CONSTANT)
    Q_PROPERTY(QVariantMap currentMember READ currentMember NOTIFY memberChanged)
    Q_PROPERTY(bool isProcessing READ isProcessing NOTIFY processingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusChanged)

public:
    explicit CashierVM(ProductService *productService, MemberService *memberService,
                       OrderService *orderService, PaymentService *paymentService,
                       ShiftService *shiftService, QObject *parent = nullptr);

    CartVM* cart() const;
    PaymentVM* payment() const;
    QVariantMap currentMember() const;
    bool isProcessing() const;
    QString statusMessage() const;

    void setTenantStore(const QString &tenantId, const QString &storeId);
    void setCashier(int cashierId);

    Q_INVOKABLE void scanBarcode(const QString &barcode);
    Q_INVOKABLE void inputBarcode(const QString &barcode);
    Q_INVOKABLE void selectMember(const QString &keyword);
    Q_INVOKABLE void clearMember();
    Q_INVOKABLE void suspendOrder();
    Q_INVOKABLE void resumeOrder(const QString &orderNo);
    Q_INVOKABLE void startPayment();
    Q_INVOKABLE void clearAll();

signals:
    void memberChanged();
    void processingChanged();
    void statusChanged();
    void showPaymentDialog();
    void orderCompleted(const QString &orderNo);

private slots:
    void onPaymentCompleted(const QList<PaymentRecord> &records, double change);

private:
    CartVM *m_cart;
    PaymentVM *m_payment;
    ShiftVM *m_shift;
    ProductService *m_productService;
    MemberService *m_memberService;
    OrderService *m_orderService;
    PaymentService *m_paymentService;
    Member m_currentMember;
    bool m_isProcessing = false;
    QString m_statusMessage;
    QString m_tenantId;
    QString m_storeId;
    int m_cashierId = 0;
};
