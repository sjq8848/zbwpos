#include "AppSettings.h"

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
    , m_settings("ZBW", "ZBW POS")
{
}

QString AppSettings::serverUrl() const
{
    return m_settings.value("general/serverUrl").toString();
}

void AppSettings::setServerUrl(const QString &url)
{
    if (serverUrl() != url) {
        m_settings.setValue("general/serverUrl", url);
        emit serverUrlChanged();
    }
}

QString AppSettings::tenantId() const
{
    return m_settings.value("general/tenantId").toString();
}

void AppSettings::setTenantId(const QString &id)
{
    if (tenantId() != id) {
        m_settings.setValue("general/tenantId", id);
        emit tenantIdChanged();
    }
}

QString AppSettings::storeId() const
{
    return m_settings.value("general/storeId").toString();
}

void AppSettings::setStoreId(const QString &id)
{
    if (storeId() != id) {
        m_settings.setValue("general/storeId", id);
        emit storeIdChanged();
    }
}

int AppSettings::lastCashierId() const
{
    return m_settings.value("general/lastCashierId").toInt();
}

void AppSettings::setLastCashierId(int id)
{
    if (lastCashierId() != id) {
        m_settings.setValue("general/lastCashierId", id);
        emit lastCashierIdChanged();
    }
}

QString AppSettings::theme() const
{
    return m_settings.value("general/theme", "light").toString();
}

void AppSettings::setTheme(const QString &theme)
{
    if (this->theme() != theme) {
        m_settings.setValue("general/theme", theme);
        emit themeChanged();
    }
}

void AppSettings::sync()
{
    m_settings.sync();
}
