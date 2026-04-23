#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    void setBaseUrl(const QString &url);
    void setAuthToken(const QString &token);
    void clearAuthToken();

    QNetworkReply* get(const QString &endpoint, const QUrlQuery &params = {});
    QNetworkReply* post(const QString &endpoint, const QJsonObject &body);
    QNetworkReply* put(const QString &endpoint, const QJsonObject &body);
    QNetworkReply* del(const QString &endpoint);

signals:
    void unauthorized();
    void networkError(int statusCode);
    void rateLimited();

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkRequest createRequest(const QString &endpoint);
    void handleCommonErrors(QNetworkReply *reply);

    QNetworkAccessManager *m_manager;
    QString m_baseUrl;
    QString m_authToken;
};
