#include "CartVM.h"
#include <QVariantMap>

CartVM::CartVM(QObject *parent)
    : QObject(parent)
    , m_itemModel(new CartItemModel(this))
{
}

CartItemModel* CartVM::items() const
{
    return m_itemModel;
}

int CartVM::itemCount() const
{
    return m_itemModel->itemCount();
}

double CartVM::subtotal() const
{
    return m_itemModel->totalSubtotal();
}

double CartVM::discount() const
{
    return m_discount;
}

double CartVM::total() const
{
    return subtotal() * m_memberDiscountRate - m_discount;
}

QString CartVM::memberId() const
{
    return m_memberId;
}

void CartVM::addItem(const QString &barcode, const QString &name, double price, int productId, double quantity)
{
    // Check if barcode already exists, increment quantity if so
    QList<CartItem> items = m_itemModel->allItems();
    for (int i = 0; i < items.count(); ++i) {
        if (items.at(i).barcode == barcode) {
            double newQty = items.at(i).quantity + quantity;
            m_itemModel->updateQuantity(i, newQty);
            emit cartChanged();
            return;
        }
    }

    // Add new item
    CartItem item;
    item.barcode = barcode;
    item.name = name;
    item.unitPrice = price;
    item.quantity = quantity;
    item.discountRate = m_memberDiscountRate;
    item.productId = productId;
    item.subtotal = price * quantity * m_memberDiscountRate;
    m_itemModel->addItem(item);
    emit cartChanged();
}

void CartVM::updateQuantity(int index, double quantity)
{
    if (quantity <= 0) {
        removeItem(index);
        return;
    }
    m_itemModel->updateQuantity(index, quantity);
    emit cartChanged();
}

void CartVM::removeItem(int index)
{
    m_itemModel->removeItem(index);
    emit cartChanged();
}

void CartVM::applyDiscount(double discount)
{
    m_discount = discount;
    emit cartChanged();
}

void CartVM::applyMemberDiscount(double rate)
{
    setMemberDiscount(rate);
}

void CartVM::clear()
{
    m_itemModel->clear();
    m_discount = 0;
    m_memberDiscountRate = 1.0;
    m_memberId.clear();
    emit cartChanged();
    emit memberChanged();
}

QVariantMap CartVM::getItemData(int index)
{
    CartItem item = m_itemModel->getItem(index);
    QVariantMap map;
    map["barcode"] = item.barcode;
    map["name"] = item.name;
    map["unitPrice"] = item.unitPrice;
    map["quantity"] = item.quantity;
    map["discountRate"] = item.discountRate;
    map["subtotal"] = item.subtotal;
    map["productId"] = item.productId;
    return map;
}

void CartVM::setMemberDiscount(double rate)
{
    m_memberDiscountRate = rate;
    // Update discount rate on all existing items
    for (int i = 0; i < m_itemModel->itemCount(); ++i) {
        m_itemModel->updateDiscount(i, rate);
    }
    emit cartChanged();
}

void CartVM::setMemberId(const QString &memberId)
{
    if (m_memberId != memberId) {
        m_memberId = memberId;
        emit memberChanged();
    }
}
