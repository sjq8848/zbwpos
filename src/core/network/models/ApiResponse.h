#pragma once
#include <QJsonObject>
#include <QString>

struct ApiResponse {
    bool success = false;
    int statusCode = 0;
    QJsonObject data;
    QString error;
    QString message;

    static ApiResponse fromNetworkReply(int httpCode, const QJsonObject &body) {
        ApiResponse r;
        r.statusCode = httpCode;
        r.success = (httpCode >= 200 && httpCode < 300);
        r.data = body;
        r.message = body["message"].toString();
        if (!r.success) r.error = body["error"].toString();
        return r;
    }
};
