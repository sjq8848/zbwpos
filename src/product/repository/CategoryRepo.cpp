#include "CategoryRepo.h"
#include "core/database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

CategoryRepo::CategoryRepo(DatabaseManager *db, QObject *parent)
    : QObject(parent), m_db(db) {}

Category CategoryRepo::findById(int id) {
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, name, parent_id, sort_order, sync_status "
        "FROM categories WHERE id=?",
        {id});
    if (q.next()) return mapFromQuery(q);
    return Category();
}

QList<Category> CategoryRepo::findAll(const QString &tenantId) {
    QList<Category> result;
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, name, parent_id, sort_order, sync_status "
        "FROM categories WHERE tenant_id=? ORDER BY sort_order, name",
        {tenantId});
    while (q.next()) result.append(mapFromQuery(q));
    return result;
}

QList<Category> CategoryRepo::getChildren(const QString &tenantId, int parentId) {
    QList<Category> result;
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, name, parent_id, sort_order, sync_status "
        "FROM categories WHERE tenant_id=? AND parent_id=? ORDER BY sort_order, name",
        {tenantId, parentId});
    while (q.next()) result.append(mapFromQuery(q));
    return result;
}

bool CategoryRepo::save(const Category &category) {
    if (category.id > 0) {
        QSqlQuery q = m_db->execute(
            "INSERT OR REPLACE INTO categories "
            "(id, cloud_id, tenant_id, name, parent_id, sort_order, sync_status) "
            "VALUES (?, ?, ?, ?, ?, ?, 1)",
            {category.id, category.cloudId, category.tenantId, category.name,
             category.parentId, category.sortOrder});
        if (q.lastError().isValid()) {
            qWarning() << "CategoryRepo::save failed:" << q.lastError().text();
            return false;
        }
        return true;
    } else {
        QSqlQuery q = m_db->execute(
            "INSERT OR REPLACE INTO categories "
            "(cloud_id, tenant_id, name, parent_id, sort_order, sync_status) "
            "VALUES (?, ?, ?, ?, ?, 1)",
            {category.cloudId, category.tenantId, category.name,
             category.parentId, category.sortOrder});
        if (q.lastError().isValid()) {
            qWarning() << "CategoryRepo::save failed:" << q.lastError().text();
            return false;
        }
        return true;
    }
}

bool CategoryRepo::remove(int id) {
    QSqlQuery q = m_db->execute("DELETE FROM categories WHERE id=?", {id});
    return q.numRowsAffected() > 0;
}

Category CategoryRepo::mapFromQuery(QSqlQuery &q) {
    Category c;
    c.id = q.value(0).toInt();
    c.cloudId = q.value(1).toString();
    c.tenantId = q.value(2).toString();
    c.name = q.value(3).toString();
    c.parentId = q.value(4).toInt();
    c.sortOrder = q.value(5).toInt();
    c.syncStatus = q.value(6).toInt();
    return c;
}

void CategoryRepo::bindToQuery(QSqlQuery &q, const Category &c) {
    q.addBindValue(c.id);
    q.addBindValue(c.cloudId);
    q.addBindValue(c.tenantId);
    q.addBindValue(c.name);
    q.addBindValue(c.parentId);
    q.addBindValue(c.sortOrder);
}
