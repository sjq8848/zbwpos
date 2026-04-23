#pragma once
#include <QAbstractListModel>
#include <QList>
#include "core/database/Models.h"

struct CartItem {
    QString barcode;
    QString name;
    double unitPrice = 0;
    double quantity = 0;
    double discountRate = 1.0;
    double subtotal = 0;
    int productId = 0;
};

class CartItemModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        BarcodeRole = Qt::UserRole + 1,
        NameRole,
        UnitPriceRole,
        QuantityRole,
        DiscountRole,
        SubtotalRole,
        ProductIdRole
    };

    explicit CartItemModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addItem(const CartItem &item);
    void updateQuantity(int index, double quantity);
    void updateDiscount(int index, double rate);
    void removeItem(int index);
    void clear();
    CartItem getItem(int index) const;
    QList<CartItem> allItems() const;
    int itemCount() const;
    double totalSubtotal() const;

private:
    QList<CartItem> m_items;
};
