#pragma once
#include "IBarcodeScanner.h"
#include "IPrinter.h"
#include "ICashDrawer.h"
#include "ICustomerDisplay.h"

class DeviceFactory {
public:
    static IBarcodeScanner* createBarcodeScanner();
    static IPrinter* createPrinter();
    static ICashDrawer* createCashDrawer();
    static ICustomerDisplay* createCustomerDisplay();
};
