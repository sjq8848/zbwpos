#include "ProductService.h"
#include "core/network/OfflineQueue.h"
#include <QDebug>

ProductService::ProductService(ProductRepo *repo, OfflineQueue *queue, QObject *parent)
    : QObject(parent), m_repo(repo), m_queue(queue) {}

Product ProductService::findByBarcode(const QString &tenantId, const QString &storeId, const QString &barcode) {
    return m_repo->findByBarcode(tenantId, storeId, barcode);
}

QList<Product> ProductService::search(const QString &tenantId, const QString &storeId, const QString &keyword, int limit) {
    return m_repo->search(tenantId, storeId, keyword, limit);
}

QList<Product> ProductService::getByCategory(const QString &tenantId, const QString &storeId, int categoryId) {
    return m_repo->getByCategory(tenantId, storeId, categoryId);
}

QList<Product> ProductService::getFavorites(const QString &tenantId, const QString &storeId) {
    return m_repo->getFavorites(tenantId, storeId);
}

bool ProductService::updatePrice(int productId, double newPrice, const QString &reason) {
    Product product = m_repo->findById(productId);
    if (product.id == 0) {
        qWarning() << "ProductService::updatePrice: product not found, id=" << productId;
        return false;
    }

    product.price = newPrice;
    if (!m_repo->save(product)) {
        qWarning() << "ProductService::updatePrice: save failed for id=" << productId;
        return false;
    }

    // Push to offline queue for sync
    m_queue->push("UPDATE", "products", productId, product.toJson());

    qDebug() << "ProductService::updatePrice: id=" << productId
             << "newPrice=" << newPrice << "reason=" << reason;

    emit productUpdated(product);
    return true;
}

bool ProductService::updateStock(int productId, double quantity, const QString &reason) {
    Product product = m_repo->findById(productId);
    if (product.id == 0) {
        qWarning() << "ProductService::updateStock: product not found, id=" << productId;
        return false;
    }

    product.stockQty = quantity;
    if (!m_repo->save(product)) {
        qWarning() << "ProductService::updateStock: save failed for id=" << productId;
        return false;
    }

    // Push to offline queue for sync
    m_queue->push("UPDATE", "products", productId, product.toJson());

    qDebug() << "ProductService::updateStock: id=" << productId
             << "newQty=" << quantity << "reason=" << reason;

    emit productUpdated(product);

    // Check low stock warning
    if (product.minStock > 0 && product.stockQty <= product.minStock) {
        emit stockLowWarning(QString::number(productId), product.stockQty);
    }

    return true;
}
