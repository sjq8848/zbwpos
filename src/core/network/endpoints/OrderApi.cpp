#include "OrderApi.h"
#include "../ApiClient.h"

OrderApi::OrderApi(ApiClient *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
{
}

QNetworkReply* OrderApi::list(const QString &tenantId, const QString &storeId,
                               const QUrlQuery &params)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/stores/%2/orders")
        .arg(tenantId, storeId);
    return m_client->get(endpoint, params);
}

QNetworkReply* OrderApi::create(const QString &tenantId, const QString &storeId,
                                 const QJsonObject &orderJson)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/stores/%2/orders")
        .arg(tenantId, storeId);
    return m_client->post(endpoint, orderJson);
}

QNetworkReply* OrderApi::updateStatus(int cloudId, const QString &status)
{
    QString endpoint = QStringLiteral("/api/v1/orders/%1/status").arg(cloudId);
    QJsonObject body;
    body[QStringLiteral("status")] = status;
    return m_client->put(endpoint, body);
}

QNetworkReply* OrderApi::getById(int cloudId)
{
    QString endpoint = QStringLiteral("/api/v1/orders/%1").arg(cloudId);
    return m_client->get(endpoint);
}
