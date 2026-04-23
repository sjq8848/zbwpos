#include "ProductRepo.h"
#include "core/database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ProductRepo::ProductRepo(DatabaseManager *db, QObject *parent)
    : QObject(parent), m_db(db) {}

Product ProductRepo::findById(int id) {
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, barcode, sku, name, "
        "category_id, unit, price, cost_price, stock_qty, min_stock, "
        "status, image_url, sync_status, created_at, updated_at "
        "FROM products WHERE id=?",
        {id});
    if (q.next()) return mapFromQuery(q);
    return Product();
}

Product ProductRepo::findByCloudId(const QString &cloudId) {
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, barcode, sku, name, "
        "category_id, unit, price, cost_price, stock_qty, min_stock, "
        "status, image_url, sync_status, created_at, updated_at "
        "FROM products WHERE cloud_id=?",
        {cloudId});
    if (q.next()) return mapFromQuery(q);
    return Product();
}

Product ProductRepo::findByBarcode(const QString &tenantId, const QString &storeId, const QString &barcode) {
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, barcode, sku, name, "
        "category_id, unit, price, cost_price, stock_qty, min_stock, "
        "status, image_url, sync_status, created_at, updated_at "
        "FROM products WHERE tenant_id=? AND store_id=? AND barcode=?",
        {tenantId, storeId, barcode});
    if (q.next()) return mapFromQuery(q);
    return Product();
}

QList<Product> ProductRepo::findAll(const QString &tenantId, const QString &storeId) {
    QList<Product> result;
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, barcode, sku, name, "
        "category_id, unit, price, cost_price, stock_qty, min_stock, "
        "status, image_url, sync_status, created_at, updated_at "
        "FROM products WHERE tenant_id=? AND store_id=? AND status=1 "
        "ORDER BY name",
        {tenantId, storeId});
    while (q.next()) result.append(mapFromQuery(q));
    return result;
}

QList<Product> ProductRepo::search(const QString &tenantId, const QString &storeId, const QString &keyword, int limit) {
    QList<Product> result;
    QString pattern = "%" + keyword + "%";
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, barcode, sku, name, "
        "category_id, unit, price, cost_price, stock_qty, min_stock, "
        "status, image_url, sync_status, created_at, updated_at "
        "FROM products WHERE tenant_id=? AND store_id=? AND status=1 "
        "AND (name LIKE ? OR barcode LIKE ?) "
        "ORDER BY name LIMIT ?",
        {tenantId, storeId, pattern, pattern, limit});
    while (q.next()) result.append(mapFromQuery(q));
    return result;
}

QList<Product> ProductRepo::getByCategory(const QString &tenantId, const QString &storeId, int categoryId) {
    QList<Product> result;
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, barcode, sku, name, "
        "category_id, unit, price, cost_price, stock_qty, min_stock, "
        "status, image_url, sync_status, created_at, updated_at "
        "FROM products WHERE tenant_id=? AND store_id=? AND category_id=? AND status=1 "
        "ORDER BY name",
        {tenantId, storeId, categoryId});
    while (q.next()) result.append(mapFromQuery(q));
    return result;
}

QList<Product> ProductRepo::getLowStock(const QString &tenantId, const QString &storeId, double threshold) {
    QList<Product> result;
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, barcode, sku, name, "
        "category_id, unit, price, cost_price, stock_qty, min_stock, "
        "status, image_url, sync_status, created_at, updated_at "
        "FROM products WHERE tenant_id=? AND store_id=? AND status=1 "
        "AND stock_qty <= min_stock AND min_stock > 0 "
        "ORDER BY stock_qty",
        {tenantId, storeId});
    while (q.next()) result.append(mapFromQuery(q));
    return result;
}

QList<Product> ProductRepo::getFavorites(const QString &tenantId, const QString &storeId, int limit) {
    QList<Product> result;
    QSqlQuery q = m_db->execute(
        "SELECT id, cloud_id, tenant_id, store_id, barcode, sku, name, "
        "category_id, unit, price, cost_price, stock_qty, min_stock, "
        "status, image_url, sync_status, created_at, updated_at "
        "FROM products WHERE tenant_id=? AND store_id=? AND status=1 "
        "ORDER BY updated_at DESC LIMIT ?",
        {tenantId, storeId, limit});
    while (q.next()) result.append(mapFromQuery(q));
    return result;
}

bool ProductRepo::save(const Product &product) {
    QSqlQuery q(m_db->database());
    if (product.id > 0) {
        q.prepare(
            "INSERT OR REPLACE INTO products "
            "(id, cloud_id, tenant_id, store_id, barcode, sku, name, "
            "category_id, unit, price, cost_price, stock_qty, min_stock, "
            "status, image_url, sync_status, updated_at) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 1, CURRENT_TIMESTAMP)");
    } else {
        q.prepare(
            "INSERT OR REPLACE INTO products "
            "(cloud_id, tenant_id, store_id, barcode, sku, name, "
            "category_id, unit, price, cost_price, stock_qty, min_stock, "
            "status, image_url, sync_status) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 1)");
    }
    bindToQuery(q, product);

    if (!q.exec()) {
        qWarning() << "ProductRepo::save failed:" << q.lastError().text();
        return false;
    }

    Product saved = product;
    if (saved.id == 0) saved.id = q.lastInsertId().toInt();
    emit productSaved(saved);
    return true;
}

bool ProductRepo::remove(int id) {
    QSqlQuery q = m_db->execute("DELETE FROM products WHERE id=?", {id});
    if (q.numRowsAffected() > 0) {
        emit productRemoved(id);
        return true;
    }
    return false;
}

void ProductRepo::syncFromCloud(const QList<Product> &cloudProducts) {
    for (const Product &p : cloudProducts) {
        // Try to find existing by cloud_id
        Product existing = findByCloudId(p.cloudId);
        if (existing.id > 0) {
            // Update existing, keep local id, mark synced
            QSqlQuery q(m_db->database());
            q.prepare(
                "UPDATE products SET "
                "tenant_id=?, store_id=?, barcode=?, sku=?, name=?, "
                "category_id=?, unit=?, price=?, cost_price=?, stock_qty=?, min_stock=?, "
                "status=?, image_url=?, sync_status=0, updated_at=CURRENT_TIMESTAMP "
                "WHERE id=?");
            q.addBindValue(p.tenantId);
            q.addBindValue(p.storeId);
            q.addBindValue(p.barcode);
            q.addBindValue(p.sku);
            q.addBindValue(p.name);
            q.addBindValue(p.categoryId);
            q.addBindValue(p.unit);
            q.addBindValue(p.price);
            q.addBindValue(p.costPrice);
            q.addBindValue(p.stockQty);
            q.addBindValue(p.minStock);
            q.addBindValue(p.status);
            q.addBindValue(p.imageUrl);
            q.addBindValue(existing.id);
            if (!q.exec()) {
                qWarning() << "syncFromCloud update failed:" << q.lastError().text();
            }
        } else {
            // Insert new from cloud, mark synced
            QSqlQuery q(m_db->database());
            q.prepare(
                "INSERT INTO products "
                "(cloud_id, tenant_id, store_id, barcode, sku, name, "
                "category_id, unit, price, cost_price, stock_qty, min_stock, "
                "status, image_url, sync_status) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0)");
            q.addBindValue(p.cloudId);
            q.addBindValue(p.tenantId);
            q.addBindValue(p.storeId);
            q.addBindValue(p.barcode);
            q.addBindValue(p.sku);
            q.addBindValue(p.name);
            q.addBindValue(p.categoryId);
            q.addBindValue(p.unit);
            q.addBindValue(p.price);
            q.addBindValue(p.costPrice);
            q.addBindValue(p.stockQty);
            q.addBindValue(p.minStock);
            q.addBindValue(p.status);
            q.addBindValue(p.imageUrl);
            if (!q.exec()) {
                qWarning() << "syncFromCloud insert failed:" << q.lastError().text();
            }
        }
    }
}

Product ProductRepo::mapFromQuery(QSqlQuery &q) {
    Product p;
    p.id = q.value(0).toInt();
    p.cloudId = q.value(1).toString();
    p.tenantId = q.value(2).toString();
    p.storeId = q.value(3).toString();
    p.barcode = q.value(4).toString();
    p.sku = q.value(5).toString();
    p.name = q.value(6).toString();
    p.categoryId = q.value(7).toInt();
    p.unit = q.value(8).toString();
    p.price = q.value(9).toDouble();
    p.costPrice = q.value(10).toDouble();
    p.stockQty = q.value(11).toDouble();
    p.minStock = q.value(12).toDouble();
    p.status = q.value(13).toInt();
    p.imageUrl = q.value(14).toString();
    p.syncStatus = q.value(15).toInt();
    p.createdAt = q.value(16).toDateTime();
    p.updatedAt = q.value(17).toDateTime();
    return p;
}

void ProductRepo::bindToQuery(QSqlQuery &q, const Product &p) {
    if (p.id > 0) {
        q.addBindValue(p.id);
    }
    q.addBindValue(p.cloudId);
    q.addBindValue(p.tenantId);
    q.addBindValue(p.storeId);
    q.addBindValue(p.barcode);
    q.addBindValue(p.sku);
    q.addBindValue(p.name);
    q.addBindValue(p.categoryId);
    q.addBindValue(p.unit);
    q.addBindValue(p.price);
    q.addBindValue(p.costPrice);
    q.addBindValue(p.stockQty);
    q.addBindValue(p.minStock);
    q.addBindValue(p.status);
    q.addBindValue(p.imageUrl);
}
