#pragma once
#include <QObject>
#include <QNetworkReply>
#include <QJsonObject>
#include <QUrlQuery>

class ApiClient;

class MemberApi : public QObject {
    Q_OBJECT
public:
    explicit MemberApi(ApiClient *client, QObject *parent = nullptr);

    QNetworkReply* list(const QString &tenantId, const QUrlQuery &params = QUrlQuery());
    QNetworkReply* getByCardNo(const QString &tenantId, const QString &cardNo);
    QNetworkReply* getByPhone(const QString &tenantId, const QString &phone);
    QNetworkReply* create(const QString &tenantId, const QJsonObject &memberJson);
    QNetworkReply* update(int cloudId, const QJsonObject &memberJson);
    QNetworkReply* remove(int cloudId);

private:
    ApiClient *m_client;
};
