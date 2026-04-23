#pragma once
#include <QObject>
#include <QAbstractListModel>
#include "service/ProductService.h"

class ProductModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { IdRole = Qt::UserRole + 1, BarcodeRole, NameRole, PriceRole, StockRole, CategoryRole };
    explicit ProductModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setProducts(const QList<Product> &products);
    void clear();

private:
    QList<Product> m_products;
};

class ProductVM : public QObject {
    Q_OBJECT
    Q_PROPERTY(ProductModel* products READ products CONSTANT)
    Q_PROPERTY(QString keyword READ keyword WRITE setKeyword NOTIFY keywordChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(int selectedCategoryId READ selectedCategoryId WRITE setSelectedCategoryId NOTIFY selectedCategoryIdChanged)

public:
    explicit ProductVM(ProductService *service, QObject *parent = nullptr);

    ProductModel* products() const;
    QString keyword() const;
    void setKeyword(const QString &keyword);
    bool isLoading() const;
    int selectedCategoryId() const;
    void setSelectedCategoryId(int id);

    Q_INVOKABLE void search();
    Q_INVOKABLE void selectCategory(int categoryId);
    Q_INVOKABLE void loadAll();

    void setTenantStore(const QString &tenantId, const QString &storeId);

signals:
    void keywordChanged();
    void isLoadingChanged();
    void selectedCategoryIdChanged();

private:
    ProductService *m_service;
    ProductModel *m_productModel;
    QString m_keyword;
    bool m_isLoading = false;
    int m_selectedCategoryId = 0;
    QString m_tenantId;
    QString m_storeId;
};
