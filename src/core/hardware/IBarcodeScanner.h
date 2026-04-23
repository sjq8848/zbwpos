#pragma once
#include <QObject>
#include "IHardwareDevice.h"

class IBarcodeScanner : public QObject, public IHardwareDevice {
    Q_OBJECT
public:
    virtual void startScanning() = 0;
    virtual void stopScanning() = 0;
signals:
    void barcodeRead(const QString &barcode);
};
