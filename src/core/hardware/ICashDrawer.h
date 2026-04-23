#pragma once
#include <QObject>
#include "IHardwareDevice.h"

class ICashDrawer : public QObject, public IHardwareDevice {
    Q_OBJECT
public:
    virtual bool open() = 0;
    virtual bool isOpen() const = 0;
signals:
    void opened();
};
