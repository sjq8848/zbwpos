#pragma once
#include "IPrinter.h"
#include <QByteArray>

class QSerialPort;

class PrinterEscPos : public IPrinter {
    Q_OBJECT
public:
    explicit PrinterEscPos(QObject *parent = nullptr);
    ~PrinterEscPos();

    bool setPort(const QString &portName, int baudRate = 9600);

    bool open() override;
    bool close() override;
    bool isConnected() const override;
    QString deviceName() const override;
    QString lastError() const override;

    bool printReceipt(const ReceiptData &data) override;
    bool printTestPage() override;
    bool cutPaper() override;
    bool openCashDrawer() override;
    int printerStatus() const override;

private:
    QByteArray buildReceipt(const ReceiptData &data);
    void write(const QByteArray &data);
    void writeCommand(const QByteArray &cmd);
    QByteArray alignCenter();
    QByteArray alignLeft();
    QByteArray bold(bool on);
    QByteArray doubleHeight(bool on);
    QByteArray doubleWidth(bool on);
    QByteArray textSize(int size);
    QByteArray lineFeed(int lines = 1);

    QSerialPort *m_port = nullptr;
    QString m_portName;
    int m_baudRate = 9600;
    QString m_lastError;
    QString m_deviceName;
    bool m_connected = false;
};
