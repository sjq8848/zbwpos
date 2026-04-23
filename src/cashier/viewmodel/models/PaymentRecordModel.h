#pragma once
#include <QAbstractListModel>
#include <QList>

struct PaymentRecord {
    QString method;
    double amount = 0;
    QString referenceNo;
};

class PaymentRecordModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { MethodRole = Qt::UserRole + 1, AmountRole, ReferenceRole };
    explicit PaymentRecordModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addRecord(const PaymentRecord &record);
    void removeRecord(int index);
    void clear();
    double totalAmount() const;
    QList<PaymentRecord> allRecords() const;

private:
    QList<PaymentRecord> m_records;
};
