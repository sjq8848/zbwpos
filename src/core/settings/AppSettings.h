#pragma once
#include <QObject>
#include <QSettings>
#include <QString>

class AppSettings : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(QString tenantId READ tenantId WRITE setTenantId NOTIFY tenantIdChanged)
    Q_PROPERTY(QString storeId READ storeId WRITE setStoreId NOTIFY storeIdChanged)
    Q_PROPERTY(int lastCashierId READ lastCashierId WRITE setLastCashierId NOTIFY lastCashierIdChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);

    QString serverUrl() const;
    void setServerUrl(const QString &url);
    QString tenantId() const;
    void setTenantId(const QString &id);
    QString storeId() const;
    void setStoreId(const QString &id);
    int lastCashierId() const;
    void setLastCashierId(int id);
    QString theme() const;
    void setTheme(const QString &theme);

    void sync();

signals:
    void serverUrlChanged();
    void tenantIdChanged();
    void storeIdChanged();
    void lastCashierIdChanged();
    void themeChanged();

private:
    QSettings m_settings;
};
