#pragma once
#include <QObject>
#include "repository/ProductRepo.h"

class ProductRepo;
class OfflineQueue;

class ProductService : public QObject {
    Q_OBJECT
public:
    explicit ProductService(ProductRepo *repo, OfflineQueue *queue, QObject *parent = nullptr);

    Product findByBarcode(const QString &tenantId, const QString &storeId, const QString &barcode);
    QList<Product> search(const QString &tenantId, const QString &storeId, const QString &keyword, int limit = 50);
    QList<Product> getByCategory(const QString &tenantId, const QString &storeId, int categoryId);
    QList<Product> getFavorites(const QString &tenantId, const QString &storeId);

    bool updatePrice(int productId, double newPrice, const QString &reason);
    bool updateStock(int productId, double quantity, const QString &reason);

signals:
    void productUpdated(const Product &product);
    void stockLowWarning(const QString &productId, double currentStock);

private:
    ProductRepo *m_repo;
    OfflineQueue *m_queue;
};
