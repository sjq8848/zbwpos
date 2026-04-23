#pragma once
#include <QObject>
#include <QKeyEvent>
#include <QElapsedTimer>

class BarcodeFilter : public QObject {
    Q_OBJECT
public:
    explicit BarcodeFilter(QObject *parent = nullptr);

    void setActive(bool active);
    bool isActive() const;

signals:
    void barcodeRead(const QString &barcode);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QString m_buffer;
    QElapsedTimer m_lastKeyTime;
    bool m_active = true;
    static constexpr int MAX_INTER_KEY_MS = 50; // barcode scanners type fast
};
