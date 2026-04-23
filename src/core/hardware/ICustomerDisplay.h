#pragma once
#include <QObject>
#include "IHardwareDevice.h"

class ICustomerDisplay : public QObject, public IHardwareDevice {
    Q_OBJECT
public:
    virtual bool showWelcome() = 0;
    virtual bool showItem(const QString &name, double price) = 0;
    virtual bool showTotal(double amount) = 0;
    virtual bool showPayment(double paid, double change) = 0;
    virtual bool clear() = 0;
};
