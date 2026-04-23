#include "CategoryService.h"

CategoryService::CategoryService(CategoryRepo *repo, QObject *parent)
    : QObject(parent), m_repo(repo) {}

Category CategoryService::findById(int id) {
    return m_repo->findById(id);
}

QList<Category> CategoryService::findAll(const QString &tenantId) {
    return m_repo->findAll(tenantId);
}

QList<Category> CategoryService::getChildren(const QString &tenantId, int parentId) {
    return m_repo->getChildren(tenantId, parentId);
}
