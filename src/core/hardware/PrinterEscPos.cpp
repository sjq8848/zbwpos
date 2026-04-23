#include "PrinterEscPos.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QDebug>

// ESC/POS Commands
namespace EscPos {
    constexpr char ESC = 0x1B;
    constexpr char GS = 0x1D;
    constexpr char LF = 0x0A;

    // Initialize printer
    inline QByteArray init() { return QByteArray(2, 0).replace(0, 1, &ESC).replace(1, 1, "@"); }

    // Cut paper
    inline QByteArray cut() { return QByteArray(3, 0).replace(0, 1, &GS).replace(1, 1, "V").replace(2, 1, "\x00"); }

    // Partial cut
    inline QByteArray partialCut() { return QByteArray(3, 0).replace(0, 1, &GS).replace(1, 1, "V").replace(2, 1, "\x01"); }

    // Open cash drawer (pulse on pin 2)
    inline QByteArray openDrawerPin2() { return QByteArray(5, 0).replace(0, 1, &ESC).replace(1, 1, "p").replace(2, 1, "\x00").replace(3, 1, "\x3C").replace(4, 1, "\xF0"); }

    // Open cash drawer (pulse on pin 5)
    inline QByteArray openDrawerPin5() { return QByteArray(5, 0).replace(0, 1, &ESC).replace(1, 1, "p").replace(2, 1, "\x01").replace(3, 1, "\x3C").replace(4, 1, "\xF0"); }

    // Align center
    inline QByteArray alignCenter() { return QByteArray(2, 0).replace(0, 1, &ESC).replace(1, 1, "\x61\x01"); }

    // Align left
    inline QByteArray alignLeft() { return QByteArray(2, 0).replace(0, 1, &ESC).replace(1, 1, "\x61\x00"); }

    // Align right
    inline QByteArray alignRight() { return QByteArray(2, 0).replace(0, 1, &ESC).replace(1, 1, "\x61\x02"); }

    // Bold on
    inline QByteArray boldOn() { return QByteArray(3, 0).replace(0, 1, &ESC).replace(1, 1, "E").replace(2, 1, "\x01"); }

    // Bold off
    inline QByteArray boldOff() { return QByteArray(3, 0).replace(0, 1, &ESC).replace(1, 1, "E").replace(2, 1, "\x00"); }

    // Double height on
    inline QByteArray doubleHeightOn() { return QByteArray(3, 0).replace(0, 1, &GS).replace(1, 1, "!").replace(2, 1, "\x01"); }

    // Double width on
    inline QByteArray doubleWidthOn() { return QByteArray(3, 0).replace(0, 1, &GS).replace(1, 1, "!").replace(2, 1, "\x10"); }

    // Double height and width off
    inline QByteArray normalSize() { return QByteArray(3, 0).replace(0, 1, &GS).replace(1, 1, "!").replace(2, 1, "\x00"); }

    // Line feed
    inline QByteArray lineFeed(int n = 1) { return QByteArray(2, 0).replace(0, 1, &ESC).replace(1, 1, QByteArray(1, QString::number(n).at(0).toLatin1())); }
}

PrinterEscPos::PrinterEscPos(QObject *parent)
    : IPrinter(parent)
    , m_deviceName("ESC/POS Printer")
{
}

PrinterEscPos::~PrinterEscPos()
{
    close();
}

bool PrinterEscPos::setPort(const QString &portName, int baudRate)
{
    m_portName = portName;
    m_baudRate = baudRate;
    return true;
}

bool PrinterEscPos::open()
{
    if (m_connected) {
        return true;
    }

    if (m_portName.isEmpty()) {
        // Try to auto-detect printer port
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : ports) {
            // Common printer vendor IDs or port patterns
            if (info.portName().contains("USB", Qt::CaseInsensitive) ||
                info.portName().contains("COM", Qt::CaseInsensitive)) {
                m_portName = info.portName();
                break;
            }
        }
    }

    if (m_portName.isEmpty()) {
        m_lastError = "No printer port specified or detected";
        return false;
    }

    m_port = new QSerialPort(this);
    m_port->setPortName(m_portName);
    m_port->setBaudRate(m_baudRate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    if (m_port->open(QIODevice::ReadWrite)) {
        m_connected = true;
        m_deviceName = QString("ESC/POS Printer on %1").arg(m_portName);

        // Initialize printer
        writeCommand(QByteArray(2, 0).replace(0, 1, "\x1B").replace(1, 1, "@"));

        return true;
    } else {
        m_lastError = QString("Failed to open port %1: %2").arg(m_portName, m_port->errorString());
        delete m_port;
        m_port = nullptr;
        return false;
    }
}

bool PrinterEscPos::close()
{
    if (m_port) {
        m_port->close();
        delete m_port;
        m_port = nullptr;
    }
    m_connected = false;
    return true;
}

bool PrinterEscPos::isConnected() const
{
    return m_connected && m_port && m_port->isOpen();
}

QString PrinterEscPos::deviceName() const
{
    return m_deviceName;
}

QString PrinterEscPos::lastError() const
{
    return m_lastError;
}

bool PrinterEscPos::printReceipt(const ReceiptData &data)
{
    if (!isConnected()) {
        m_lastError = "Printer not connected";
        emit printError(m_lastError);
        return false;
    }

    QByteArray receipt = buildReceipt(data);
    write(receipt);

    emit printCompleted();
    return true;
}

bool PrinterEscPos::printTestPage()
{
    if (!isConnected()) {
        m_lastError = "Printer not connected";
        return false;
    }

    QByteArray test;
    test.append("\x1B@");  // Initialize
    test.append("\x1B\x61\x01");  // Center
    test.append("\x1B\x45\x01");  // Bold on
    test.append("ZBW POS\n");
    test.append("\x1B\x45\x00");  // Bold off
    test.append("Test Page\n\n");
    test.append("\x1B\x61\x00");  // Left
    test.append(QString("Date: %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toUtf8());
    test.append("\n\n\n");
    test.append("\x1D\x56\x00");  // Cut

    write(test);
    return true;
}

bool PrinterEscPos::cutPaper()
{
    if (!isConnected()) {
        m_lastError = "Printer not connected";
        return false;
    }

    // Feed some paper before cutting
    writeCommand(QByteArray("\x1B\x64\x03"));  // Feed 3 lines

    // Cut paper
    writeCommand(QByteArray("\x1D\x56\x00"));
    return true;
}

bool PrinterEscPos::openCashDrawer()
{
    if (!isConnected()) {
        m_lastError = "Printer not connected";
        return false;
    }

    // Send open drawer command - pulse on pin 2
    writeCommand(QByteArray("\x1B\x70\x00\x3C\xF0"));
    return true;
}

int PrinterEscPos::printerStatus() const
{
    // This would require DTR/DSR or specific status commands
    // For now, just check if port is open
    if (!isConnected()) {
        return 1; // Offline
    }
    return 0; // OK
}

QByteArray PrinterEscPos::buildReceipt(const ReceiptData &data)
{
    QByteArray receipt;

    // Initialize printer
    receipt.append("\x1B@");

    // Header - centered, bold
    receipt.append("\x1B\x61\x01");  // Center align
    receipt.append("\x1B\x45\x01");  // Bold on
    receipt.append(data.storeName.toUtf8());
    receipt.append("\n");
    receipt.append("\x1B\x45\x00");  // Bold off

    // Order info
    receipt.append(QString("Order: %1\n").arg(data.orderNo).toUtf8());
    receipt.append(QString("Cashier: %1\n").arg(data.cashierName).toUtf8());
    receipt.append(QString("Date: %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toUtf8());

    if (!data.memberInfo.isEmpty()) {
        receipt.append(QString("Member: %1\n").arg(data.memberInfo).toUtf8());
    }

    receipt.append("\n");

    // Items - left aligned
    receipt.append("\x1B\x61\x00");  // Left align
    receipt.append("--------------------------------\n");

    for (const QJsonObject &item : data.items) {
        QString name = item["name"].toString();
        double price = item["price"].toDouble();
        int qty = item["qty"].toInt();
        double subtotal = item["subtotal"].toDouble();

        // Format: Item name, price x qty
        QString line = QString("%1\n").arg(name);
        receipt.append(line.toUtf8());

        QString detail = QString("  %1 x %2 = %3\n")
            .arg(QString::number(price, 'f', 2))
            .arg(qty)
            .arg(QString::number(subtotal, 'f', 2));
        receipt.append(detail.toUtf8());
    }

    receipt.append("--------------------------------\n");

    // Totals
    receipt.append(QString("Subtotal: %1\n").arg(QString::number(data.totalAmount, 'f', 2)).toUtf8());

    if (data.discountAmount > 0) {
        receipt.append(QString("Discount: -%1\n").arg(QString::number(data.discountAmount, 'f', 2)).toUtf8());
    }

    receipt.append("\x1B\x45\x01");  // Bold on
    receipt.append(QString("TOTAL: %1\n").arg(QString::number(data.finalAmount, 'f', 2)).toUtf8());
    receipt.append("\x1B\x45\x00");  // Bold off

    // Payments
    if (!data.payments.isEmpty()) {
        receipt.append("\n");
        for (const QJsonObject &payment : data.payments) {
            QString method = payment["method"].toString();
            double amount = payment["amount"].toDouble();
            receipt.append(QString("%1: %2\n").arg(method, QString::number(amount, 'f', 2)).toUtf8());
        }

        if (data.change > 0) {
            receipt.append(QString("Change: %1\n").arg(QString::number(data.change, 'f', 2)).toUtf8());
        }
    }

    // Footer
    receipt.append("\n");
    receipt.append("\x1B\x61\x01");  // Center align
    receipt.append("Thank you for shopping!\n");
    receipt.append("Welcome back!\n");

    // Feed and cut
    receipt.append("\n\n\n");
    receipt.append("\x1D\x56\x00");  // Cut

    return receipt;
}

void PrinterEscPos::write(const QByteArray &data)
{
    if (m_port && m_port->isOpen()) {
        m_port->write(data);
        m_port->flush();
    }
}

void PrinterEscPos::writeCommand(const QByteArray &cmd)
{
    write(cmd);
}

QByteArray PrinterEscPos::alignCenter()
{
    return QByteArray("\x1B\x61\x01");
}

QByteArray PrinterEscPos::alignLeft()
{
    return QByteArray("\x1B\x61\x00");
}

QByteArray PrinterEscPos::bold(bool on)
{
    return on ? QByteArray("\x1B\x45\x01") : QByteArray("\x1B\x45\x00");
}

QByteArray PrinterEscPos::doubleHeight(bool on)
{
    return on ? QByteArray("\x1D\x21\x01") : QByteArray("\x1D\x21\x00");
}

QByteArray PrinterEscPos::doubleWidth(bool on)
{
    return on ? QByteArray("\x1D\x21\x10") : QByteArray("\x1D\x21\x00");
}

QByteArray PrinterEscPos::textSize(int size)
{
    Q_UNUSED(size)
    return QByteArray();
}

QByteArray PrinterEscPos::lineFeed(int lines)
{
    return QByteArray("\x1B\x64" + QByteArray(1, static_cast<char>(lines)));
}
