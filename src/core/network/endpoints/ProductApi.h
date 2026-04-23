#pragma once
#include <QObject>
#include <QNetworkReply>
#include <QJsonObject>
#include <QUrlQuery>

class ApiClient;

class ProductApi : public QObject {
    Q_OBJECT
public:
    explicit ProductApi(ApiClient *client, QObject *parent = nullptr);

    QNetworkReply* list(const QString &tenantId, const QString &storeId,
                        const QUrlQuery &params = QUrlQuery());
    QNetworkReply* getByBarcode(const QString &tenantId, const QString &storeId,
                                const QString &barcode);
    QNetworkReply* create(const QString &tenantId, const QString &storeId,
                          const QJsonObject &productJson);
    QNetworkReply* update(int cloudId, const QJsonObject &productJson);
    QNetworkReply* remove(int cloudId);

private:
    ApiClient *m_client;
};
