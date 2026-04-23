#include "PaymentVM.h"

PaymentVM::PaymentVM(QObject *parent)
    : QObject(parent)
    , m_recordModel(new PaymentRecordModel(this))
{
}

PaymentRecordModel* PaymentVM::paymentRecords() const
{
    return m_recordModel;
}

double PaymentVM::amountDue() const
{
    return m_amountDue;
}

double PaymentVM::amountPaid() const
{
    return m_recordModel->totalAmount();
}

double PaymentVM::change() const
{
    // Change only for cash payments
    double paid = amountPaid();
    if (paid > m_amountDue && m_currentMethod == ZBW::PAY_CASH) {
        return paid - m_amountDue;
    }
    return 0;
}

QString PaymentVM::currentMethod() const
{
    return m_currentMethod;
}

bool PaymentVM::canComplete() const
{
    return amountPaid() >= m_amountDue && m_amountDue > 0;
}

int PaymentVM::paymentState() const
{
    return static_cast<int>(m_state);
}

void PaymentVM::setAmountDue(double amount)
{
    if (m_amountDue != amount) {
        m_amountDue = amount;
        emit amountChanged();
    }
}

void PaymentVM::setMethod(const QString &method)
{
    m_currentMethod = method;
    m_state = PaymentState::SelectingMethod;
    m_inputAmount = 0;
    emit methodChanged();
    emit stateChanged();
}

void PaymentVM::inputAmount(double amount)
{
    m_inputAmount = amount;
    m_state = PaymentState::InputtingAmount;
    emit stateChanged();
}

void PaymentVM::quickCash(int preset)
{
    m_inputAmount = preset;
    m_currentMethod = ZBW::PAY_CASH;
    m_state = PaymentState::InputtingAmount;
    emit methodChanged();
    emit stateChanged();

    // Auto-add partial payment for quick cash
    addPartialPayment();
}

void PaymentVM::addPartialPayment()
{
    if (m_inputAmount <= 0 || m_currentMethod.isEmpty()) {
        emit paymentFailed(tr("Invalid payment amount or method"));
        return;
    }

    PaymentRecord record;
    record.method = m_currentMethod;
    record.amount = m_inputAmount;
    record.referenceNo = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_recordModel->addRecord(record);

    m_inputAmount = 0;

    emit amountChanged();

    if (amountPaid() >= m_amountDue) {
        m_state = PaymentState::Completed;
        emit stateChanged();
        // Auto-complete when fully paid
        complete();
    } else {
        m_state = PaymentState::PartialPaid;
        emit stateChanged();
    }
}

void PaymentVM::removePartialPayment(int index)
{
    m_recordModel->removeRecord(index);
    emit amountChanged();

    if (m_recordModel->rowCount() == 0) {
        m_state = PaymentState::SelectingMethod;
        emit stateChanged();
    }
}

void PaymentVM::complete()
{
    if (!canComplete()) {
        emit paymentFailed(tr("Insufficient payment amount"));
        return;
    }

    m_state = PaymentState::Completed;
    emit stateChanged();
    emit paymentCompleted(m_recordModel->allRecords(), change());
}

void PaymentVM::cancel()
{
    reset();
}

void PaymentVM::reset()
{
    m_recordModel->clear();
    m_currentMethod.clear();
    m_inputAmount = 0;
    m_amountDue = 0;
    m_state = PaymentState::Idle;
    emit amountChanged();
    emit methodChanged();
    emit stateChanged();
}
