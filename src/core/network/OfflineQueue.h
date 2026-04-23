#pragma once
#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QDateTime>

struct OfflineTask {
    qlonglong id = 0;
    QString tenantId;
    QString storeId;
    QString operation; // INSERT, UPDATE, DELETE
    QString tableName;
    int recordId = 0;
    QJsonObject payload;
    int retryCount = 0;
    QString lastError;
    QDateTime createdAt;
};

class OfflineQueue : public QObject {
    Q_OBJECT
public:
    explicit OfflineQueue(QObject *parent = nullptr);

    void setDatabase(class DatabaseManager *db);

    void push(const QString &operation, const QString &tableName,
              int recordId, const QJsonObject &payload);
    QList<OfflineTask> getPending();
    void markCompleted(qlonglong taskId);
    void markFailed(qlonglong taskId, const QString &error);
    void retry(qlonglong taskId);
    void retryAll();
    void cleanup(int daysToKeep = 7);

signals:
    void taskAdded();
    void taskCompleted(qlonglong taskId);
    void taskFailed(qlonglong taskId, const QString &error);

private:
    DatabaseManager *m_db = nullptr;
};
