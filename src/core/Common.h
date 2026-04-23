#pragma once
#include <QString>
#include <QDateTime>
#include <QVariant>
#include "QtCompat.h"

namespace ZBW {

constexpr int SYNC_OK = 0;
constexpr int SYNC_PENDING = 1;
constexpr int SYNC_CONFLICT = 2;

constexpr int ORDER_NORMAL = 1;
constexpr int ORDER_REFUND = 2;
constexpr int ORDER_CANCEL = 3;

inline const QString PAY_CASH = QStringLiteral("cash");
inline const QString PAY_CARD = QStringLiteral("card");
inline const QString PAY_WECHAT = QStringLiteral("wechat");
inline const QString PAY_ALIPAY = QStringLiteral("alipay");
inline const QString PAY_MEMBER_BALANCE = QStringLiteral("member_balance");

} // namespace ZBW
