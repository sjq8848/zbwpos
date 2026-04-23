#pragma once
#include <QObject>
#include "models/CartItemModel.h"

class CartVM : public QObject {
    Q_OBJECT
    Q_PROPERTY(CartItemModel* items READ items CONSTANT)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY cartChanged)
    Q_PROPERTY(double subtotal READ subtotal NOTIFY cartChanged)
    Q_PROPERTY(double discount READ discount NOTIFY cartChanged)
    Q_PROPERTY(double total READ total NOTIFY cartChanged)
    Q_PROPERTY(QString memberId READ memberId NOTIFY memberChanged)

public:
    explicit CartVM(QObject *parent = nullptr);

    CartItemModel* items() const;
    int itemCount() const;
    double subtotal() const;
    double discount() const;
    double total() const;
    QString memberId() const;

    Q_INVOKABLE void addItem(const QString &barcode, const QString &name, double price, int productId, double quantity = 1);
    Q_INVOKABLE void updateQuantity(int index, double quantity);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void applyDiscount(double discount);
    Q_INVOKABLE void applyMemberDiscount(double rate);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantMap getItemData(int index);

    void setMemberDiscount(double rate);
    void setMemberId(const QString &memberId);

signals:
    void cartChanged();
    void memberChanged();

private:
    CartItemModel *m_itemModel;
    double m_discount = 0;
    double m_memberDiscountRate = 1.0;
    QString m_memberId;
};
