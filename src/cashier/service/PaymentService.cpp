#include "PaymentService.h"
#include "member/service/MemberService.h"

PaymentService::PaymentService(MemberService *memberService, QObject *parent)
    : QObject(parent), m_memberService(memberService)
{
}

PaymentResult PaymentService::pay(const PaymentRequest &request) {
    PaymentResult result;
    result.transactionId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    if (request.method == "member_balance") {
        return payWithMemberBalance(request.orderNo, request.memberId, request.amount);
    }

    // Cash, card, wechat, alipay - for now mark success
    // Real implementation would integrate with payment gateways
    result.success = true;
    emit paymentCompleted(result);
    return result;
}

bool PaymentService::isPaymentMethodAvailable(const QString &method, bool isOnline) const {
    if (method == "wechat" || method == "alipay") {
        return isOnline;
    }
    return true; // cash, card, member_balance available offline
}

PaymentResult PaymentService::payWithMemberBalance(const QString &orderNo,
                                                    const QString &memberId,
                                                    double amount) {
    PaymentResult result;
    result.transactionId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    // Try to deduct balance - MemberService will check if sufficient
    bool ok = m_memberService->deductBalance(memberId.toInt(), amount);
    if (!ok) {
        result.error = "Insufficient balance or member not found";
        emit paymentFailed(result.error);
        return result;
    }

    result.success = true;
    result.extra["memberId"] = memberId;
    emit paymentCompleted(result);
    return result;
}

QJsonObject PaymentService::prepareMobilePayment(const QString &orderNo,
                                                  const QString &method,
                                                  double amount) {
    // Mock implementation - real would call cloud API
    QJsonObject result;
    result["qrCodeUrl"] = QString("https://pay.example.com/%1/%2")
        .arg(method, QUuid::createUuid().toString(QUuid::WithoutBraces));
    result["transactionId"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    result["expireTime"] = 120; // seconds
    return result;
}

QString PaymentService::checkMobilePaymentStatus(const QString &transactionId) {
    // Mock implementation - real would poll cloud API
    Q_UNUSED(transactionId);
    return "pending"; // or "success", "failed", "expired"
}
