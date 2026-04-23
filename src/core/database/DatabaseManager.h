#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariantList>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool open(const QString &dbPath);
    void close();
    bool isOpen() const;
    bool runMigrations();

    QSqlQuery execute(const QString &sql, const QVariantList &params = {});
    QSqlDatabase database() const;

private:
    QSqlDatabase m_db;
};
