#pragma once
#include <QObject>
#include <QList>
#include "core/database/Models.h"

class DatabaseManager;

class CategoryRepo : public QObject {
    Q_OBJECT
public:
    explicit CategoryRepo(DatabaseManager *db, QObject *parent = nullptr);

    Category findById(int id);
    QList<Category> findAll(const QString &tenantId);
    QList<Category> getChildren(const QString &tenantId, int parentId);
    bool save(const Category &category);
    bool remove(int id);

private:
    Category mapFromQuery(class QSqlQuery &q);
    void bindToQuery(class QSqlQuery &q, const Category &c);

    DatabaseManager *m_db;
};
