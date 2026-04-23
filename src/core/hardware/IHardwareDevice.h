#pragma once
#include <QString>

class IHardwareDevice {
public:
    virtual ~IHardwareDevice() = default;
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool isConnected() const = 0;
    virtual QString deviceName() const = 0;
    virtual QString lastError() const = 0;
};
