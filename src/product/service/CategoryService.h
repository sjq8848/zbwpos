#pragma once
#include <QObject>
#include "repository/CategoryRepo.h"

class CategoryRepo;

class CategoryService : public QObject {
    Q_OBJECT
public:
    explicit CategoryService(CategoryRepo *repo, QObject *parent = nullptr);

    Category findById(int id);
    QList<Category> findAll(const QString &tenantId);
    QList<Category> getChildren(const QString &tenantId, int parentId);

private:
    CategoryRepo *m_repo;
};
