#include "BarcodeFilter.h"

BarcodeFilter::BarcodeFilter(QObject *parent)
    : QObject(parent)
{
    m_lastKeyTime.start();
}

void BarcodeFilter::setActive(bool active)
{
    m_active = active;
    if (!active) {
        m_buffer.clear();
    }
}

bool BarcodeFilter::isActive() const
{
    return m_active;
}

bool BarcodeFilter::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if (!m_active) {
        return false;
    }

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // Check if too much time has passed since last key (not a barcode scanner)
        if (m_lastKeyTime.elapsed() > MAX_INTER_KEY_MS && !m_buffer.isEmpty()) {
            // Human typing - reset buffer
            m_buffer.clear();
        }

        // Handle Enter key - barcode complete
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            // Only emit if we have a reasonable barcode length
            if (m_buffer.length() > 3) {
                emit barcodeRead(m_buffer);
            }
            m_buffer.clear();
            return false; // Let event propagate
        }

        // Only process printable characters
        if (keyEvent->text().isEmpty()) {
            return false;
        }

        // Append character to buffer
        m_buffer.append(keyEvent->text());
        m_lastKeyTime.restart();
    }

    // Don't consume the event - let it propagate
    return false;
}
