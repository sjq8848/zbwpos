#pragma once
#include <QObject>
#include "models/PaymentRecordModel.h"
#include "core/Common.h"

enum class PaymentState { Idle, SelectingMethod, InputtingAmount, PartialPaid, Completed };

class PaymentVM : public QObject {
    Q_OBJECT
    Q_PROPERTY(PaymentRecordModel* paymentRecords READ paymentRecords CONSTANT)
    Q_PROPERTY(double amountDue READ amountDue NOTIFY amountChanged)
    Q_PROPERTY(double amountPaid READ amountPaid NOTIFY amountChanged)
    Q_PROPERTY(double change READ change NOTIFY amountChanged)
    Q_PROPERTY(QString currentMethod READ currentMethod NOTIFY methodChanged)
    Q_PROPERTY(bool canComplete READ canComplete NOTIFY amountChanged)
    Q_PROPERTY(int paymentState READ paymentState NOTIFY stateChanged)

public:
    explicit PaymentVM(QObject *parent = nullptr);

    PaymentRecordModel* paymentRecords() const;
    double amountDue() const;
    double amountPaid() const;
    double change() const;
    QString currentMethod() const;
    bool canComplete() const;
    int paymentState() const;

    void setAmountDue(double amount);

    Q_INVOKABLE void setMethod(const QString &method);
    Q_INVOKABLE void inputAmount(double amount);
    Q_INVOKABLE void quickCash(int preset); // 10/20/50/100/200
    Q_INVOKABLE void addPartialPayment();
    Q_INVOKABLE void removePartialPayment(int index);
    Q_INVOKABLE void complete();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void reset();

signals:
    void amountChanged();
    void methodChanged();
    void stateChanged();
    void paymentCompleted(const QList<PaymentRecord> &records, double change);
    void paymentFailed(const QString &error);

private:
    PaymentRecordModel *m_recordModel;
    double m_amountDue = 0;
    QString m_currentMethod;
    PaymentState m_state = PaymentState::Idle;
    double m_inputAmount = 0;
};
