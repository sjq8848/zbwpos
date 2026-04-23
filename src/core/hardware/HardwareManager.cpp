#include "HardwareManager.h"
#include "DeviceFactory.h"

HardwareManager* HardwareManager::instance()
{
    static HardwareManager instance;
    return &instance;
}

HardwareManager::HardwareManager(QObject *parent)
    : QObject(parent)
{
}

HardwareManager::~HardwareManager()
{
    shutdown();
}

void HardwareManager::initialize()
{
    // Create default devices via factory
    if (!m_barcodeScanner) {
        m_barcodeScanner = DeviceFactory::createBarcodeScanner();
    }
    if (!m_printer) {
        m_printer = DeviceFactory::createPrinter();
    }
    if (!m_cashDrawer) {
        m_cashDrawer = DeviceFactory::createCashDrawer();
    }
    if (!m_customerDisplay) {
        m_customerDisplay = DeviceFactory::createCustomerDisplay();
    }

    // Open all devices
    if (m_barcodeScanner) {
        m_barcodeScanner->open();
    }
    if (m_printer) {
        m_printer->open();
    }
    if (m_cashDrawer) {
        m_cashDrawer->open();
    }
    if (m_customerDisplay) {
        m_customerDisplay->open();
    }
}

void HardwareManager::shutdown()
{
    // Close and delete all devices
    if (m_barcodeScanner) {
        m_barcodeScanner->close();
        delete m_barcodeScanner;
        m_barcodeScanner = nullptr;
    }
    if (m_printer) {
        m_printer->close();
        delete m_printer;
        m_printer = nullptr;
    }
    if (m_cashDrawer) {
        m_cashDrawer->close();
        delete m_cashDrawer;
        m_cashDrawer = nullptr;
    }
    if (m_customerDisplay) {
        m_customerDisplay->close();
        delete m_customerDisplay;
        m_customerDisplay = nullptr;
    }
}

IBarcodeScanner* HardwareManager::barcodeScanner() const
{
    return m_barcodeScanner;
}

IPrinter* HardwareManager::printer() const
{
    return m_printer;
}

ICashDrawer* HardwareManager::cashDrawer() const
{
    return m_cashDrawer;
}

ICustomerDisplay* HardwareManager::customerDisplay() const
{
    return m_customerDisplay;
}

void HardwareManager::setBarcodeScanner(IBarcodeScanner *scanner)
{
    if (m_barcodeScanner) {
        m_barcodeScanner->close();
        delete m_barcodeScanner;
    }
    m_barcodeScanner = scanner;
}

void HardwareManager::setPrinter(IPrinter *printer)
{
    if (m_printer) {
        m_printer->close();
        delete m_printer;
    }
    m_printer = printer;
}

void HardwareManager::setCashDrawer(ICashDrawer *drawer)
{
    if (m_cashDrawer) {
        m_cashDrawer->close();
        delete m_cashDrawer;
    }
    m_cashDrawer = drawer;
}

void HardwareManager::setCustomerDisplay(ICustomerDisplay *display)
{
    if (m_customerDisplay) {
        m_customerDisplay->close();
        delete m_customerDisplay;
    }
    m_customerDisplay = display;
}

bool HardwareManager::hasBarcodeScanner() const
{
    return m_barcodeScanner != nullptr && m_barcodeScanner->isConnected();
}

bool HardwareManager::hasPrinter() const
{
    return m_printer != nullptr && m_printer->isConnected();
}

bool HardwareManager::hasCashDrawer() const
{
    return m_cashDrawer != nullptr && m_cashDrawer->isConnected();
}

bool HardwareManager::hasCustomerDisplay() const
{
    return m_customerDisplay != nullptr && m_customerDisplay->isConnected();
}
