#include "AuthApi.h"
#include "../ApiClient.h"
#include <QJsonObject>

AuthApi::AuthApi(ApiClient *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
{
}

QNetworkReply* AuthApi::login(const QString &username, const QString &password)
{
    QJsonObject body;
    body[QStringLiteral("username")] = username;
    body[QStringLiteral("password")] = password;

    return m_client->post(QStringLiteral("/api/v1/auth/login"), body);
}

QNetworkReply* AuthApi::refreshToken(const QString &token)
{
    QJsonObject body;
    body[QStringLiteral("refresh_token")] = token;

    return m_client->post(QStringLiteral("/api/v1/auth/refresh"), body);
}

QNetworkReply* AuthApi::logout()
{
    return m_client->post(QStringLiteral("/api/v1/auth/logout"), QJsonObject());
}
