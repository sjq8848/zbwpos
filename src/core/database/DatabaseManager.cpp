#include "DatabaseManager.h"
#include "Migration.h"
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager() { close(); }

bool DatabaseManager::open(const QString &dbPath) {
    m_db = QSqlDatabase::addDatabase("QSQLITE",
        "zbwpos_" + QString::number(reinterpret_cast<quintptr>(this)));
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    m_db.exec("PRAGMA journal_mode=WAL");
    m_db.exec("PRAGMA foreign_keys=ON");
    return true;
}

void DatabaseManager::close() {
    if (m_db.isOpen()) m_db.close();
}

bool DatabaseManager::isOpen() const {
    return m_db.isOpen();
}

bool DatabaseManager::runMigrations() {
    return Migration::run(m_db);
}

QSqlQuery DatabaseManager::execute(const QString &sql, const QVariantList &params) {
    QSqlQuery q(m_db);
    if (!q.prepare(sql)) {
        qWarning() << "SQL prepare error:" << q.lastError().text() << sql;
        return q;
    }
    for (int i = 0; i < params.size(); ++i)
        q.bindValue(i, params[i]);
    if (!q.exec()) {
        qWarning() << "SQL exec error:" << q.lastError().text() << sql;
    }
    return q;
}

QSqlDatabase DatabaseManager::database() const { return m_db; }
