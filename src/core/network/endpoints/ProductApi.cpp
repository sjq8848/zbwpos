#include "ProductApi.h"
#include "../ApiClient.h"

ProductApi::ProductApi(ApiClient *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
{
}

QNetworkReply* ProductApi::list(const QString &tenantId, const QString &storeId,
                                 const QUrlQuery &params)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/stores/%2/products")
        .arg(tenantId, storeId);
    return m_client->get(endpoint, params);
}

QNetworkReply* ProductApi::getByBarcode(const QString &tenantId, const QString &storeId,
                                         const QString &barcode)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/stores/%2/products/by-barcode/%3")
        .arg(tenantId, storeId, barcode);
    return m_client->get(endpoint);
}

QNetworkReply* ProductApi::create(const QString &tenantId, const QString &storeId,
                                   const QJsonObject &productJson)
{
    QString endpoint = QStringLiteral("/api/v1/tenants/%1/stores/%2/products")
        .arg(tenantId, storeId);
    return m_client->post(endpoint, productJson);
}

QNetworkReply* ProductApi::update(int cloudId, const QJsonObject &productJson)
{
    QString endpoint = QStringLiteral("/api/v1/products/%1").arg(cloudId);
    return m_client->put(endpoint, productJson);
}

QNetworkReply* ProductApi::remove(int cloudId)
{
    QString endpoint = QStringLiteral("/api/v1/products/%1").arg(cloudId);
    return m_client->del(endpoint);
}
