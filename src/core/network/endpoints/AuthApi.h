#pragma once
#include <QObject>
#include <QNetworkReply>
#include <QJsonObject>

class ApiClient;

class AuthApi : public QObject {
    Q_OBJECT
public:
    explicit AuthApi(ApiClient *client, QObject *parent = nullptr);

    QNetworkReply* login(const QString &username, const QString &password);
    QNetworkReply* refreshToken(const QString &token);
    QNetworkReply* logout();

private:
    ApiClient *m_client;
};
