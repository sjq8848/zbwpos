#include "DeviceFactory.h"
#include "PrinterEscPos.h"
#include "BarcodeFilter.h"

// Stub implementations for desktop testing
class StubBarcodeScanner : public IBarcodeScanner {
    Q_OBJECT
public:
    explicit StubBarcodeScanner(QObject *parent = nullptr) : IBarcodeScanner(parent) {}
    bool open() override { return true; }
    bool close() override { return true; }
    bool isConnected() const override { return true; }
    QString deviceName() const override { return "Stub Barcode Scanner"; }
    QString lastError() const override { return QString(); }
    void startScanning() override {}
    void stopScanning() override {}
};

class StubPrinter : public IPrinter {
    Q_OBJECT
public:
    explicit StubPrinter(QObject *parent = nullptr) : IPrinter(parent) {}
    bool open() override { return true; }
    bool close() override { return true; }
    bool isConnected() const override { return true; }
    QString deviceName() const override { return "Stub Printer"; }
    QString lastError() const override { return QString(); }
    bool printReceipt(const ReceiptData &data) override {
        Q_UNUSED(data)
        emit printCompleted();
        return true;
    }
    bool printTestPage() override { return true; }
    bool cutPaper() override { return true; }
    bool openCashDrawer() override { return true; }
    int printerStatus() const override { return 0; }
};

class StubCashDrawer : public ICashDrawer {
    Q_OBJECT
public:
    explicit StubCashDrawer(QObject *parent = nullptr) : ICashDrawer(parent), m_open(false) {}
    bool open() override { m_open = true; emit opened(); return true; }
    bool close() override { m_open = false; return true; }
    bool isConnected() const override { return true; }
    QString deviceName() const override { return "Stub Cash Drawer"; }
    QString lastError() const override { return QString(); }
    bool isOpen() const override { return m_open; }
private:
    bool m_open;
};

class StubCustomerDisplay : public ICustomerDisplay {
    Q_OBJECT
public:
    explicit StubCustomerDisplay(QObject *parent = nullptr) : ICustomerDisplay(parent) {}
    bool open() override { return true; }
    bool close() override { return true; }
    bool isConnected() const override { return true; }
    QString deviceName() const override { return "Stub Customer Display"; }
    QString lastError() const override { return QString(); }
    bool showWelcome() override { return true; }
    bool showItem(const QString &name, double price) override {
        Q_UNUSED(name)
        Q_UNUSED(price)
        return true;
    }
    bool showTotal(double amount) override {
        Q_UNUSED(amount)
        return true;
    }
    bool showPayment(double paid, double change) override {
        Q_UNUSED(paid)
        Q_UNUSED(change)
        return true;
    }
    bool clear() override { return true; }
};

IBarcodeScanner* DeviceFactory::createBarcodeScanner()
{
#ifdef ZBW_PLATFORM_ANDROID
    // Android implementation would go here
    return nullptr;
#else
    // For desktop, barcode scanner uses keyboard input via BarcodeFilter
    // Return nullptr here - BarcodeFilter handles barcode detection globally
    return nullptr;
#endif
}

IPrinter* DeviceFactory::createPrinter()
{
#ifdef ZBW_PLATFORM_ANDROID
    // Android Bluetooth/USB printer implementation would go here
    return nullptr;
#else
    // For desktop, use ESC/POS over serial port
    return new PrinterEscPos();
#endif
}

ICashDrawer* DeviceFactory::createCashDrawer()
{
#ifdef ZBW_PLATFORM_ANDROID
    // Android implementation would go here
    return nullptr;
#else
    return new StubCashDrawer();
#endif
}

ICustomerDisplay* DeviceFactory::createCustomerDisplay()
{
#ifdef ZBW_PLATFORM_ANDROID
    // Android implementation would go here
    return nullptr;
#else
    return new StubCustomerDisplay();
#endif
}

// Include moc for stub classes
#include "DeviceFactory.moc"
