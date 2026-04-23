#pragma once
#include <QObject>
#include <QJsonObject>
#include "cashier/viewmodel/models/PaymentRecordModel.h"

class MemberService;
class ApiClient;

struct PaymentRequest {
    QString orderNo;
    QString method;
    double amount = 0;
    QString memberId;
};

struct PaymentResult {
    bool success = false;
    QString transactionId;
    QString error;
    QJsonObject extra;
};

class PaymentService : public QObject {
    Q_OBJECT
public:
    explicit PaymentService(MemberService *memberService, QObject *parent = nullptr);

    PaymentResult pay(const PaymentRequest &request);
    bool isPaymentMethodAvailable(const QString &method, bool isOnline) const;

    PaymentResult payWithMemberBalance(const QString &orderNo, const QString &memberId, double amount);
    QJsonObject prepareMobilePayment(const QString &orderNo, const QString &method, double amount);
    QString checkMobilePaymentStatus(const QString &transactionId);

signals:
    void paymentCompleted(const PaymentResult &result);
    void paymentFailed(const QString &error);

private:
    MemberService *m_memberService;
};
