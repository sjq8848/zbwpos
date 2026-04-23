#pragma once
#include <QObject>
#include <QJsonObject>
#include <QList>
#include "IHardwareDevice.h"

struct ReceiptData {
    QString storeName;
    QString orderNo;
    QString cashierName;
    QString memberInfo;
    QList<QJsonObject> items;  // {name, price, qty, subtotal}
    double totalAmount = 0;
    double discountAmount = 0;
    double finalAmount = 0;
    QList<QJsonObject> payments; // {method, amount}
    double change = 0;
};

class IPrinter : public QObject, public IHardwareDevice {
    Q_OBJECT
public:
    virtual bool printReceipt(const ReceiptData &data) = 0;
    virtual bool printTestPage() = 0;
    virtual bool cutPaper() = 0;
    virtual bool openCashDrawer() = 0;
    virtual int printerStatus() const = 0; // 0=ok, 1=offline, 2=paper out, etc.
signals:
    void printCompleted();
    void printError(const QString &error);
};
