#pragma once
#include <QObject>
#include <QMap>
#include <QString>
#include "IBarcodeScanner.h"
#include "IPrinter.h"
#include "ICashDrawer.h"
#include "ICustomerDisplay.h"

class HardwareManager : public QObject {
    Q_OBJECT
public:
    static HardwareManager* instance();

    void initialize();
    void shutdown();

    IBarcodeScanner* barcodeScanner() const;
    IPrinter* printer() const;
    ICashDrawer* cashDrawer() const;
    ICustomerDisplay* customerDisplay() const;

    void setBarcodeScanner(IBarcodeScanner *scanner);
    void setPrinter(IPrinter *printer);
    void setCashDrawer(ICashDrawer *drawer);
    void setCustomerDisplay(ICustomerDisplay *display);

    bool hasBarcodeScanner() const;
    bool hasPrinter() const;
    bool hasCashDrawer() const;
    bool hasCustomerDisplay() const;

private:
    explicit HardwareManager(QObject *parent = nullptr);
    ~HardwareManager();

    IBarcodeScanner *m_barcodeScanner = nullptr;
    IPrinter *m_printer = nullptr;
    ICashDrawer *m_cashDrawer = nullptr;
    ICustomerDisplay *m_customerDisplay = nullptr;
};
