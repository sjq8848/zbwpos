#include "PaymentRecordModel.h"

PaymentRecordModel::PaymentRecordModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int PaymentRecordModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_records.count();
}

QVariant PaymentRecordModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_records.count())
        return QVariant();

    const PaymentRecord &record = m_records.at(index.row());
    switch (role) {
    case MethodRole:
        return record.method;
    case AmountRole:
        return record.amount;
    case ReferenceRole:
        return record.referenceNo;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PaymentRecordModel::roleNames() const
{
    return {
        {MethodRole, "method"},
        {AmountRole, "amount"},
        {ReferenceRole, "referenceNo"}
    };
}

void PaymentRecordModel::addRecord(const PaymentRecord &record)
{
    beginInsertRows(QModelIndex(), m_records.count(), m_records.count());
    m_records.append(record);
    endInsertRows();
}

void PaymentRecordModel::removeRecord(int index)
{
    if (index < 0 || index >= m_records.count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_records.removeAt(index);
    endRemoveRows();
}

void PaymentRecordModel::clear()
{
    if (m_records.isEmpty())
        return;

    beginResetModel();
    m_records.clear();
    endResetModel();
}

double PaymentRecordModel::totalAmount() const
{
    double total = 0;
    for (const PaymentRecord &record : m_records)
        total += record.amount;
    return total;
}

QList<PaymentRecord> PaymentRecordModel::allRecords() const
{
    return m_records;
}
