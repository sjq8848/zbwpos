#include "NavigationManager.h"

NavigationManager::NavigationManager(QObject *parent)
    : QObject(parent)
{
}

QString NavigationManager::currentPage() const
{
    return m_currentPage;
}

void NavigationManager::goTo(const QString &page)
{
    if (m_currentPage != page) {
        m_previousPage = m_currentPage;
        m_currentPage = page;
        emit currentPageChanged();
    }
}

void NavigationManager::goBack()
{
    if (!m_previousPage.isEmpty() && m_currentPage != m_previousPage) {
        QString temp = m_currentPage;
        m_currentPage = m_previousPage;
        m_previousPage = temp;
        emit currentPageChanged();
    }
}
