#include "MemberApi.h"
#include "../ApiClient.h"

MemberApi::MemberApi(ApiClient *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
{
}

QNetworkReply* MemberApi::list(const QString &tenantId, const QUrlQuery &params)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/members").arg(tenantId);
    return m_client->get(endpoint, params);
}

QNetworkReply* MemberApi::getByCardNo(const QString &tenantId, const QString &cardNo)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/members/by-card/%2")
        .arg(tenantId, cardNo);
    return m_client->get(endpoint);
}

QNetworkReply* MemberApi::getByPhone(const QString &tenantId, const QString &phone)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/members/by-phone/%2")
        .arg(tenantId, phone);
    return m_client->get(endpoint);
}

QNetworkReply* MemberApi::create(const QString &tenantId, const QJsonObject &memberJson)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/members").arg(tenantId);
    return m_client->post(endpoint, memberJson);
}

QNetworkReply* MemberApi::update(int cloudId, const QJsonObject &memberJson)
{
    QString endpoint = QStringLiteral("/api/v1/members/%1").arg(cloudId);
    return m_client->put(endpoint, memberJson);
}

QNetworkReply* MemberApi::remove(int cloudId)
{
    QString endpoint = QStringLiteral("/api/v1/members/%1").arg(cloudId);
    return m_client->del(endpoint);
}
