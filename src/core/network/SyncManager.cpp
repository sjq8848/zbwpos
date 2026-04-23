#include "SyncManager.h"
#include "ApiClient.h"
#include "OfflineQueue.h"
#include "database/DatabaseManager.h"
#include "network/models/ApiResponse.h"
#include <QJsonDocument>
#include <QNetworkReply>

SyncManager::SyncManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_autoSyncTimer, &QTimer::timeout,
            this, &SyncManager::onAutoSyncTick);
}

void SyncManager::setDatabase(DatabaseManager *db)
{
    m_db = db;
}

void SyncManager::setApiClient(ApiClient *client)
{
    m_api = client;
}

void SyncManager::setOfflineQueue(OfflineQueue *queue)
{
    m_queue = queue;
}

void SyncManager::syncAll()
{
    if (m_status == SyncInProgress) return;

    m_status = SyncInProgress;
    emit syncStarted();

    if (!m_queue) {
        m_status = SyncFailed;
        emit syncError(QStringLiteral("OfflineQueue not set"));
        return;
    }

    QList<OfflineTask> tasks = m_queue->getPending();
    int total = tasks.size();

    if (total == 0) {
        m_status = SyncSuccess;
        m_lastSyncTime = QDateTime::currentDateTime();
        emit syncCompleted(0, 0);
        return;
    }

    int successCount = 0;
    int failedCount = 0;
    int current = 0;

    for (const OfflineTask &task : tasks) {
        current++;
        emit syncProgress(current, total);

        // Process each task by calling the appropriate API
        QNetworkReply *reply = processTask(task);

        if (!reply) {
            // No matching API endpoint or network unavailable
            m_queue->markFailed(task.id, QStringLiteral("No matching API handler"));
            failedCount++;
            continue;
        }

        // For synchronous-style processing in this iteration,
        // we use a local event loop to wait for each reply.
        // In production, this would be fully async.
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QByteArray responseData = reply->readAll();
        reply->deleteLater();

        QJsonObject body = QJsonDocument::fromJson(responseData).object();
        ApiResponse response = ApiResponse::fromNetworkReply(httpCode, body);

        if (response.success) {
            m_queue->markCompleted(task.id);
            successCount++;
        } else if (httpCode == 409) {
            // Conflict detected - apply auto-resolution rules
            ConflictStrategy strategy = autoResolveStrategy(task);
            if (strategy == ManualMerge) {
                m_status = SyncConflict;
                emit conflictDetected(task.tableName, task.recordId);
                failedCount++;
            } else {
                resolveConflict(task.tableName, task.recordId, strategy);
                m_queue->markCompleted(task.id);
                successCount++;
            }
        } else {
            m_queue->markFailed(task.id, response.error);
            failedCount++;
        }
    }

    m_lastSyncTime = QDateTime::currentDateTime();

    if (failedCount == 0) {
        m_status = SyncSuccess;
    } else if (successCount == 0) {
        m_status = SyncFailed;
    } else {
        m_status = SyncSuccess; // partial success counts as success
    }

    emit syncCompleted(successCount, failedCount);
}

void SyncManager::syncProducts()
{
    syncTable(QStringLiteral("products"));
}

void SyncManager::syncMembers()
{
    syncTable(QStringLiteral("members"));
}

void SyncManager::syncOrders()
{
    syncTable(QStringLiteral("orders"));
}

SyncManager::SyncStatus SyncManager::status() const
{
    return m_status;
}

int SyncManager::pendingCount() const
{
    if (!m_queue) return 0;
    return m_queue->getPending().size();
}

QDateTime SyncManager::lastSyncTime() const
{
    return m_lastSyncTime;
}

void SyncManager::resolveConflict(const QString &tableName, int recordId,
                                   ConflictStrategy strategy)
{
    if (!m_db || !m_queue) return;

    // Apply the chosen conflict resolution strategy
    switch (strategy) {
    case UseLocal:
        // Local data wins - will be pushed on next sync
        // Mark the task as completed; the local version is authoritative
        break;
    case UseRemote:
        // Remote data wins - update local DB from remote
        // The remote version was already accepted by the server
        break;
    case ManualMerge:
        // User must resolve manually - emit conflictDetected is handled by caller
        emit conflictDetected(tableName, recordId);
        return;
    }

    // For auto-resolved conflicts, find and mark the task as completed
    QList<OfflineTask> tasks = m_queue->getPending();
    for (const OfflineTask &task : tasks) {
        if (task.tableName == tableName && task.recordId == recordId) {
            m_queue->markCompleted(task.id);
            break;
        }
    }
}

void SyncManager::startAutoSync(int intervalMs)
{
    m_autoSyncTimer.start(intervalMs);
}

void SyncManager::stopAutoSync()
{
    m_autoSyncTimer.stop();
}

void SyncManager::onAutoSyncTick()
{
    syncAll();
}

QNetworkReply* SyncManager::processTask(const OfflineTask &task)
{
    if (!m_api) return nullptr;

    // Build endpoint path based on table name and operation
    QString endpoint;
    QJsonObject body = task.payload;

    if (task.tableName == QStringLiteral("products")) {
        if (task.operation == QStringLiteral("INSERT")) {
            endpoint = QStringLiteral("/api/v1/products");
            return m_api->post(endpoint, body);
        } else if (task.operation == QStringLiteral("UPDATE")) {
            int cloudId = body[QStringLiteral("cloud_id")].toInt();
            endpoint = QStringLiteral("/api/v1/products/%1").arg(cloudId);
            return m_api->put(endpoint, body);
        } else if (task.operation == QStringLiteral("DELETE")) {
            int cloudId = body[QStringLiteral("cloud_id")].toInt();
            endpoint = QStringLiteral("/api/v1/products/%1").arg(cloudId);
            return m_api->del(endpoint);
        }
    } else if (task.tableName == QStringLiteral("members")) {
        if (task.operation == QStringLiteral("INSERT")) {
            endpoint = QStringLiteral("/api/v1/members");
            return m_api->post(endpoint, body);
        } else if (task.operation == QStringLiteral("UPDATE")) {
            int cloudId = body[QStringLiteral("cloud_id")].toInt();
            endpoint = QStringLiteral("/api/v1/members/%1").arg(cloudId);
            return m_api->put(endpoint, body);
        } else if (task.operation == QStringLiteral("DELETE")) {
            int cloudId = body[QStringLiteral("cloud_id")].toInt();
            endpoint = QStringLiteral("/api/v1/members/%1").arg(cloudId);
            return m_api->del(endpoint);
        }
    } else if (task.tableName == QStringLiteral("orders")) {
        if (task.operation == QStringLiteral("INSERT")) {
            endpoint = QStringLiteral("/api/v1/orders");
            return m_api->post(endpoint, body);
        } else if (task.operation == QStringLiteral("UPDATE")) {
            int cloudId = body[QStringLiteral("cloud_id")].toInt();
            endpoint = QStringLiteral("/api/v1/orders/%1").arg(cloudId);
            return m_api->put(endpoint, body);
        }
    }

    return nullptr;
}

SyncManager::ConflictStrategy SyncManager::autoResolveStrategy(const OfflineTask &task) const
{
    // Auto-resolution rules:
    // 1. New record with existing cloud_id -> UseRemote
    //    (the server already has this record, remote version is authoritative)
    if (task.operation == QStringLiteral("INSERT") &&
        task.payload.contains(QStringLiteral("cloud_id")) &&
        task.payload[QStringLiteral("cloud_id")].toInt() > 0) {
        return UseRemote;
    }

    // 2. Local delete + remote update -> UseRemote
    //    (remote has newer data, don't delete it)
    if (task.operation == QStringLiteral("DELETE")) {
        return UseRemote;
    }

    // 3. Orders -> always UseLocal
    //    (POS is the source of truth for transactions)
    if (task.tableName == QStringLiteral("orders")) {
        return UseLocal;
    }

    // 4. All other conflicts require manual merge
    return ManualMerge;
}

void SyncManager::syncTable(const QString &tableName)
{
    if (m_status == SyncInProgress) return;

    if (!m_queue) return;

    QList<OfflineTask> allTasks = m_queue->getPending();
    QList<OfflineTask> filtered;
    for (const OfflineTask &task : allTasks) {
        if (task.tableName == tableName)
            filtered.append(task);
    }

    if (filtered.isEmpty()) return;

    m_status = SyncInProgress;
    emit syncStarted();

    int total = filtered.size();
    int successCount = 0;
    int failedCount = 0;
    int current = 0;

    for (const OfflineTask &task : filtered) {
        current++;
        emit syncProgress(current, total);

        QNetworkReply *reply = processTask(task);
        if (!reply) {
            m_queue->markFailed(task.id, QStringLiteral("No matching API handler"));
            failedCount++;
            continue;
        }

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QByteArray responseData = reply->readAll();
        reply->deleteLater();

        QJsonObject body = QJsonDocument::fromJson(responseData).object();
        ApiResponse response = ApiResponse::fromNetworkReply(httpCode, body);

        if (response.success) {
            m_queue->markCompleted(task.id);
            successCount++;
        } else if (httpCode == 409) {
            ConflictStrategy strategy = autoResolveStrategy(task);
            if (strategy == ManualMerge) {
                m_status = SyncConflict;
                emit conflictDetected(task.tableName, task.recordId);
                failedCount++;
            } else {
                resolveConflict(task.tableName, task.recordId, strategy);
                successCount++;
            }
        } else {
            m_queue->markFailed(task.id, response.error);
            failedCount++;
        }
    }

    m_lastSyncTime = QDateTime::currentDateTime();
    m_status = (failedCount == 0) ? SyncSuccess : SyncFailed;
    emit syncCompleted(successCount, failedCount);
}
