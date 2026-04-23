#include "CartItemModel.h"

CartItemModel::CartItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int CartItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_items.count();
}

QVariant CartItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.count())
        return QVariant();

    const CartItem &item = m_items.at(index.row());
    switch (role) {
    case BarcodeRole:
        return item.barcode;
    case NameRole:
        return item.name;
    case UnitPriceRole:
        return item.unitPrice;
    case QuantityRole:
        return item.quantity;
    case DiscountRole:
        return item.discountRate;
    case SubtotalRole:
        return item.subtotal;
    case ProductIdRole:
        return item.productId;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CartItemModel::roleNames() const
{
    return {
        {BarcodeRole, "barcode"},
        {NameRole, "name"},
        {UnitPriceRole, "unitPrice"},
        {QuantityRole, "quantity"},
        {DiscountRole, "discountRate"},
        {SubtotalRole, "subtotal"},
        {ProductIdRole, "productId"}
    };
}

void CartItemModel::addItem(const CartItem &item)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(item);
    endInsertRows();
}

void CartItemModel::updateQuantity(int index, double quantity)
{
    if (index < 0 || index >= m_items.count())
        return;

    m_items[index].quantity = quantity;
    m_items[index].subtotal = m_items[index].unitPrice * quantity * m_items[index].discountRate;
    emit dataChanged(this->index(index), this->index(index), {QuantityRole, SubtotalRole});
}

void CartItemModel::updateDiscount(int index, double rate)
{
    if (index < 0 || index >= m_items.count())
        return;

    m_items[index].discountRate = rate;
    m_items[index].subtotal = m_items[index].unitPrice * m_items[index].quantity * rate;
    emit dataChanged(this->index(index), this->index(index), {DiscountRole, SubtotalRole});
}

void CartItemModel::removeItem(int index)
{
    if (index < 0 || index >= m_items.count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();
}

void CartItemModel::clear()
{
    if (m_items.isEmpty())
        return;

    beginResetModel();
    m_items.clear();
    endResetModel();
}

CartItem CartItemModel::getItem(int index) const
{
    if (index < 0 || index >= m_items.count())
        return CartItem();
    return m_items.at(index);
}

QList<CartItem> CartItemModel::allItems() const
{
    return m_items;
}

int CartItemModel::itemCount() const
{
    return m_items.count();
}

double CartItemModel::totalSubtotal() const
{
    double total = 0;
    for (const CartItem &item : m_items)
        total += item.subtotal;
    return total;
}
