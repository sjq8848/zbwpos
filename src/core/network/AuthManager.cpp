#include "AuthManager.h"
#include <QSettings>
#include <QByteArray>
#include <QJsonDocument>
#include <QDateTime>

AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
{
    // Restore persisted tokens on startup
    QSettings settings;
    m_accessToken = settings.value(QStringLiteral("auth/accessToken")).toString();
    m_refreshToken = settings.value(QStringLiteral("auth/refreshToken")).toString();
}

bool AuthManager::hasToken() const
{
    return !m_accessToken.isEmpty();
}

QString AuthManager::currentToken() const
{
    return m_accessToken;
}

QString AuthManager::currentRefreshToken() const
{
    return m_refreshToken;
}

bool AuthManager::isTokenValid() const
{
    if (m_accessToken.isEmpty())
        return false;

    QJsonObject payload = decodeJwtPayload(m_accessToken);
    if (payload.isEmpty())
        return false;

    qlonglong exp = payload[QStringLiteral("exp")].toVariant().toLongLong();
    // exp is seconds since epoch
    qlonglong now = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
    return now < exp;
}

bool AuthManager::isTokenExpiringSoon() const
{
    if (m_accessToken.isEmpty())
        return true;

    QJsonObject payload = decodeJwtPayload(m_accessToken);
    if (payload.isEmpty())
        return true;

    qlonglong exp = payload[QStringLiteral("exp")].toVariant().toLongLong();
    qlonglong now = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
    // Within 5 minutes (300 seconds) of expiration
    return (exp - now) < 300;
}

void AuthManager::setTokens(const QString &accessToken, const QString &refreshToken)
{
    m_accessToken = accessToken;
    m_refreshToken = refreshToken;

    QSettings settings;
    settings.setValue(QStringLiteral("auth/accessToken"), m_accessToken);
    settings.setValue(QStringLiteral("auth/refreshToken"), m_refreshToken);

    emit tokenRefreshed();
}

void AuthManager::clearTokens()
{
    m_accessToken.clear();
    m_refreshToken.clear();

    QSettings settings;
    settings.remove(QStringLiteral("auth/accessToken"));
    settings.remove(QStringLiteral("auth/refreshToken"));

    emit tokenExpired();
}

QJsonObject AuthManager::decodeJwtPayload(const QString &token) const
{
    // JWT format: header.payload.signature
    QStringList parts = token.split(QLatin1Char('.'));
    if (parts.size() != 3)
        return QJsonObject();

    // The payload is the middle segment (index 1)
    QByteArray payloadData = parts[1].toUtf8();

    // Base64url decode: replace URL-safe chars, add padding
    payloadData.replace('-', '+');
    payloadData.replace('_', '/');

    int padding = (4 - (payloadData.size() % 4)) % 4;
    payloadData.append(padding, '=');

    QByteArray decoded = QByteArray::fromBase64(payloadData);
    if (decoded.isEmpty())
        return QJsonObject();

    QJsonDocument doc = QJsonDocument::fromJson(decoded);
    if (doc.isNull() || !doc.isObject())
        return QJsonObject();

    return doc.object();
}
