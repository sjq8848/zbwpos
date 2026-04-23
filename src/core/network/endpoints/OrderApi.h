#pragma once
#include <QObject>
#include <QNetworkReply>
#include <QJsonObject>
#include <QUrlQuery>

class ApiClient;

class OrderApi : public QObject {
    Q_OBJECT
public:
    explicit OrderApi(ApiClient *client, QObject *parent = nullptr);

    QNetworkReply* list(const QString &tenantId, const QString &storeId,
                        const QUrlQuery &params = QUrlQuery());
    QNetworkReply* create(const QString &tenantId, const QString &storeId,
                          const QJsonObject &orderJson);
    QNetworkReply* updateStatus(int cloudId, const QString &status);
    QNetworkReply* getById(int cloudId);

private:
    ApiClient *m_client;
};
