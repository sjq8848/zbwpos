#include "ProductVM.h"

// --- ProductModel ---

ProductModel::ProductModel(QObject *parent)
    : QAbstractListModel(parent) {}

int ProductModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_products.size();
}

QVariant ProductModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_products.size())
        return {};

    const Product &p = m_products.at(index.row());
    switch (role) {
    case IdRole: return p.id;
    case BarcodeRole: return p.barcode;
    case NameRole: return p.name;
    case PriceRole: return p.price;
    case StockRole: return p.stockQty;
    case CategoryRole: return p.categoryId;
    default: return {};
    }
}

QHash<int, QByteArray> ProductModel::roleNames() const {
    return {
        {IdRole, "id"},
        {BarcodeRole, "barcode"},
        {NameRole, "name"},
        {PriceRole, "price"},
        {StockRole, "stock"},
        {CategoryRole, "categoryId"}
    };
}

void ProductModel::setProducts(const QList<Product> &products) {
    beginResetModel();
    m_products = products;
    endResetModel();
}

void ProductModel::clear() {
    beginResetModel();
    m_products.clear();
    endResetModel();
}

// --- ProductVM ---

ProductVM::ProductVM(ProductService *service, QObject *parent)
    : QObject(parent),
      m_service(service),
      m_productModel(new ProductModel(this)),
      m_isLoading(false),
      m_selectedCategoryId(0) {}

ProductModel* ProductVM::products() const {
    return m_productModel;
}

QString ProductVM::keyword() const {
    return m_keyword;
}

void ProductVM::setKeyword(const QString &keyword) {
    if (m_keyword == keyword) return;
    m_keyword = keyword;
    emit keywordChanged();
}

bool ProductVM::isLoading() const {
    return m_isLoading;
}

int ProductVM::selectedCategoryId() const {
    return m_selectedCategoryId;
}

void ProductVM::setSelectedCategoryId(int id) {
    if (m_selectedCategoryId == id) return;
    m_selectedCategoryId = id;
    emit selectedCategoryIdChanged();
}

void ProductVM::search() {
    if (m_tenantId.isEmpty() || m_storeId.isEmpty()) return;
    m_isLoading = true;
    emit isLoadingChanged();

    QList<Product> result;
    if (m_keyword.isEmpty()) {
        result = m_service->search(m_tenantId, m_storeId, "");
    } else {
        result = m_service->search(m_tenantId, m_storeId, m_keyword);
    }
    m_productModel->setProducts(result);

    m_isLoading = false;
    emit isLoadingChanged();
}

void ProductVM::selectCategory(int categoryId) {
    setSelectedCategoryId(categoryId);
    if (m_tenantId.isEmpty() || m_storeId.isEmpty()) return;

    m_isLoading = true;
    emit isLoadingChanged();

    QList<Product> result;
    if (categoryId == 0) {
        result = m_service->search(m_tenantId, m_storeId, "");
    } else {
        result = m_service->getByCategory(m_tenantId, m_storeId, categoryId);
    }
    m_productModel->setProducts(result);

    m_isLoading = false;
    emit isLoadingChanged();
}

void ProductVM::loadAll() {
    if (m_tenantId.isEmpty() || m_storeId.isEmpty()) return;

    m_isLoading = true;
    emit isLoadingChanged();

    QList<Product> result = m_service->search(m_tenantId, m_storeId, "");
    m_productModel->setProducts(result);

    m_isLoading = false;
    emit isLoadingChanged();
}

void ProductVM::setTenantStore(const QString &tenantId, const QString &storeId) {
    m_tenantId = tenantId;
    m_storeId = storeId;
}
