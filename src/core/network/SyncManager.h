#pragma once
#include <QObject>
#include <QTimer>
#include "OfflineQueue.h"

class QNetworkReply;

class SyncManager : public QObject {
    Q_OBJECT
public:
    enum SyncStatus { SyncIdle, SyncInProgress, SyncSuccess, SyncFailed, SyncConflict };
    Q_ENUM(SyncStatus)

    enum ConflictStrategy { UseLocal, UseRemote, ManualMerge };
    Q_ENUM(ConflictStrategy)

    explicit SyncManager(QObject *parent = nullptr);

    void setDatabase(class DatabaseManager *db);
    void setApiClient(class ApiClient *client);
    void setOfflineQueue(class OfflineQueue *queue);

    void syncAll();
    void syncProducts();
    void syncMembers();
    void syncOrders();

    SyncStatus status() const;
    int pendingCount() const;
    QDateTime lastSyncTime() const;

    void resolveConflict(const QString &tableName, int recordId, ConflictStrategy strategy);

    void startAutoSync(int intervalMs = 60000); // default 1 minute
    void stopAutoSync();

signals:
    void syncStarted();
    void syncProgress(int current, int total);
    void syncCompleted(int success, int failed);
    void syncError(const QString &error);
    void conflictDetected(const QString &tableName, int recordId);

private slots:
    void onAutoSyncTick();

private:
    QNetworkReply* processTask(const OfflineTask &task);
    ConflictStrategy autoResolveStrategy(const OfflineTask &task) const;
    void syncTable(const QString &tableName);

    DatabaseManager *m_db = nullptr;
    ApiClient *m_api = nullptr;
    OfflineQueue *m_queue = nullptr;
    SyncStatus m_status = SyncIdle;
    QDateTime m_lastSyncTime;
    QTimer m_autoSyncTimer;
};
