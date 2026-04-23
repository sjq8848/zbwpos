#pragma once
#include <QObject>
#include <QList>
#include "core/database/Models.h"

class DatabaseManager;
class ProductApi;

class ProductRepo : public QObject {
    Q_OBJECT
public:
    explicit ProductRepo(DatabaseManager *db, QObject *parent = nullptr);

    Product findById(int id);
    Product findByCloudId(const QString &cloudId);
    Product findByBarcode(const QString &tenantId, const QString &storeId, const QString &barcode);
    QList<Product> findAll(const QString &tenantId, const QString &storeId);
    QList<Product> search(const QString &tenantId, const QString &storeId, const QString &keyword, int limit = 50);
    QList<Product> getByCategory(const QString &tenantId, const QString &storeId, int categoryId);
    QList<Product> getLowStock(const QString &tenantId, const QString &storeId, double threshold);
    QList<Product> getFavorites(const QString &tenantId, const QString &storeId, int limit = 20);

    bool save(const Product &product);
    bool remove(int id);

    void syncFromCloud(const QList<Product> &cloudProducts);

signals:
    void productSaved(const Product &product);
    void productRemoved(int id);

private:
    Product mapFromQuery(class QSqlQuery &q);
    void bindToQuery(class QSqlQuery &q, const Product &p);

    DatabaseManager *m_db;
};
