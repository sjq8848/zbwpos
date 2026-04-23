#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>

struct AuthResult {
    bool success = false;
    QString token;
    QString refreshToken;
    QString error;
    QJsonObject userInfo;  // tenant_id, store_id, user info
};

class AuthManager : public QObject {
    Q_OBJECT
public:
    explicit AuthManager(QObject *parent = nullptr);

    bool hasToken() const;
    QString currentToken() const;
    QString currentRefreshToken() const;
    bool isTokenValid() const;
    bool isTokenExpiringSoon() const; // within 5 minutes

    void setTokens(const QString &accessToken, const QString &refreshToken);
    void clearTokens();

    QJsonObject decodeJwtPayload(const QString &token) const;

signals:
    void tokenExpired();
    void tokenRefreshed();

private:
    QString m_accessToken;
    QString m_refreshToken;
};
