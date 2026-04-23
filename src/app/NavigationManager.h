#pragma once
#include <QObject>
#include <QString>

class NavigationManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentPage READ currentPage NOTIFY currentPageChanged)

public:
    explicit NavigationManager(QObject *parent = nullptr);

    QString currentPage() const;

    Q_INVOKABLE void goTo(const QString &page);
    Q_INVOKABLE void goBack();

signals:
    void currentPageChanged();

private:
    QString m_currentPage = "cashier";
    QString m_previousPage;
};
