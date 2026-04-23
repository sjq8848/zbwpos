#include "ApiClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QSslConfiguration>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &ApiClient::onReplyFinished);
}

void ApiClient::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
    if (!m_baseUrl.endsWith(QLatin1Char('/')))
        m_baseUrl += QLatin1Char('/');
}

void ApiClient::setAuthToken(const QString &token)
{
    m_authToken = token;
}

void ApiClient::clearAuthToken()
{
    m_authToken.clear();
}

QNetworkReply* ApiClient::get(const QString &endpoint, const QUrlQuery &params)
{
    QUrl url(m_baseUrl + endpoint);
    if (!params.isEmpty())
        url.setQuery(params);

    QNetworkRequest request = createRequest(endpoint);
    request.setUrl(url);
    return m_manager->get(request);
}

QNetworkReply* ApiClient::post(const QString &endpoint, const QJsonObject &body)
{
    QNetworkRequest request = createRequest(endpoint);
    QJsonDocument doc(body);
    return m_manager->post(request, doc.toJson(QJsonDocument::Compact));
}

QNetworkReply* ApiClient::put(const QString &endpoint, const QJsonObject &body)
{
    QNetworkRequest request = createRequest(endpoint);
    QJsonDocument doc(body);
    return m_manager->put(request, doc.toJson(QJsonDocument::Compact));
}

QNetworkReply* ApiClient::del(const QString &endpoint)
{
    QNetworkRequest request = createRequest(endpoint);
    return m_manager->deleteResource(request);
}

void ApiClient::onReplyFinished(QNetworkReply *reply)
{
    handleCommonErrors(reply);
    reply->deleteLater();
}

QNetworkRequest ApiClient::createRequest(const QString &endpoint)
{
    QUrl url(m_baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization",
                             QStringLiteral("Bearer %1").arg(m_authToken).toUtf8());
    }

    return request;
}

void ApiClient::handleCommonErrors(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
        return;

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    switch (statusCode) {
    case 401:
        emit unauthorized();
        break;
    case 429:
        emit rateLimited();
        break;
    default:
        if (statusCode > 0)
            emit networkError(statusCode);
        else
            emit networkError(0); // network-level failure, no HTTP status
        break;
    }
}
