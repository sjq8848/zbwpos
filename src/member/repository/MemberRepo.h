#pragma once
#include <QObject>
#include <QList>
#include "core/database/Models.h"

class DatabaseManager;

class MemberRepo : public QObject {
    Q_OBJECT
public:
    explicit MemberRepo(DatabaseManager *db, QObject *parent = nullptr);

    Member findById(int id);
    Member findByCloudId(const QString &cloudId);
    Member findByCardNo(const QString &tenantId, const QString &cardNo);
    Member findByPhone(const QString &tenantId, const QString &phone);
    QList<Member> findAll(const QString &tenantId);
    QList<Member> search(const QString &tenantId, const QString &keyword, int limit = 50);

    bool save(const Member &member);
    bool remove(int id);

    bool updatePoints(int memberId, int pointsDelta);
    bool updateBalance(int memberId, double balanceDelta, const QString &reason);

    void syncFromCloud(const QList<Member> &cloudMembers);

signals:
    void memberSaved(const Member &member);
    void memberRemoved(int id);

private:
    Member mapFromQuery(class QSqlQuery &q);
    DatabaseManager *m_db;
};
