#include "OfflineQueue.h"
#include "database/DatabaseManager.h"
#include <QJsonDocument>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

OfflineQueue::OfflineQueue(QObject *parent)
    : QObject(parent)
{
}

void OfflineQueue::setDatabase(DatabaseManager *db)
{
    m_db = db;
}

void OfflineQueue::push(const QString &operation, const QString &tableName,
                         int recordId, const QJsonObject &payload)
{
    if (!m_db) return;

    QString sql = QStringLiteral(
        "INSERT INTO offline_queue (operation, table_name, record_id, payload, retry_count, created_at) "
        "VALUES (?, ?, ?, ?, 0, datetime('now'))"
    );

    QVariantList params;
    params << operation << tableName << recordId
           << QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));

    QSqlQuery query = m_db->execute(sql, params);
    if (query.lastError().type() == QSqlError::NoError) {
        emit taskAdded();
    }
}

QList<OfflineTask> OfflineQueue::getPending()
{
    QList<OfflineTask> tasks;

    if (!m_db) return tasks;

    QString sql = QStringLiteral(
        "SELECT id, operation, table_name, record_id, payload, retry_count, "
        "last_error, created_at FROM offline_queue "
        "ORDER BY created_at ASC"
    );

    QSqlQuery query = m_db->execute(sql);
    while (query.next()) {
        OfflineTask task;
        task.id = query.value(0).toLongLong();
        task.operation = query.value(1).toString();
        task.tableName = query.value(2).toString();
        task.recordId = query.value(3).toInt();
        task.payload = QJsonDocument::fromJson(query.value(4).toString().toUtf8()).object();
        task.retryCount = query.value(5).toInt();
        task.lastError = query.value(6).toString();
        task.createdAt = QDateTime::fromString(query.value(7).toString(), Qt::ISODate);
        tasks.append(task);
    }

    return tasks;
}

void OfflineQueue::markCompleted(qlonglong taskId)
{
    if (!m_db) return;

    QString sql = QStringLiteral("DELETE FROM offline_queue WHERE id = ?");
    m_db->execute(sql, {taskId});
    emit taskCompleted(taskId);
}

void OfflineQueue::markFailed(qlonglong taskId, const QString &error)
{
    if (!m_db) return;

    QString sql = QStringLiteral(
        "UPDATE offline_queue SET last_error = ?, retry_count = retry_count + 1 "
        "WHERE id = ?"
    );
    m_db->execute(sql, {error, taskId});
    emit taskFailed(taskId, error);
}

void OfflineQueue::retry(qlonglong taskId)
{
    if (!m_db) return;

    // Reset last_error to allow re-processing; retry_count already incremented
    QString sql = QStringLiteral(
        "UPDATE offline_queue SET last_error = '' WHERE id = ?"
    );
    m_db->execute(sql, {taskId});
}

void OfflineQueue::retryAll()
{
    if (!m_db) return;

    // Clear all error states so the sync manager will re-process them
    QString sql = QStringLiteral(
        "UPDATE offline_queue SET last_error = '' WHERE last_error != ''"
    );
    m_db->execute(sql);
}

void OfflineQueue::cleanup(int daysToKeep)
{
    if (!m_db) return;

    QString sql = QStringLiteral(
        "DELETE FROM offline_queue "
        "WHERE created_at < datetime('now', '-%1 days')"
    ).arg(daysToKeep);

    m_db->execute(sql);
}
