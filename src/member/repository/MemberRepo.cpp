#include "MemberRepo.h"
#include "core/database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MemberRepo::MemberRepo(DatabaseManager *db, QObject *parent)
    : QObject(parent), m_db(db)
{
}

Member MemberRepo::mapFromQuery(QSqlQuery &q)
{
    Member m;
    m.id = q.value("id").toInt();
    m.cloudId = q.value("cloud_id").toString();
    m.tenantId = q.value("tenant_id").toString();
    m.cardNo = q.value("card_no").toString();
    m.name = q.value("name").toString();
    m.phone = q.value("phone").toString();
    m.gender = q.value("gender").toInt();
    m.birthday = QDate::fromString(q.value("birthday").toString(), Qt::ISODate);
    m.levelId = q.value("level_id").toInt();
    m.points = q.value("points").toInt();
    m.balance = q.value("balance").toDouble();
    m.totalSpent = q.value("total_spent").toDouble();
    m.status = q.value("status").toInt();
    m.syncStatus = q.value("sync_status").toInt();
    m.createdAt = QDateTime::fromString(q.value("created_at").toString(), Qt::ISODate);
    m.updatedAt = QDateTime::fromString(q.value("updated_at").toString(), Qt::ISODate);
    return m;
}

Member MemberRepo::findById(int id)
{
    QSqlQuery q = m_db->execute(
        "SELECT * FROM members WHERE id = ?",
        {id});
    if (q.next()) {
        return mapFromQuery(q);
    }
    return Member();
}

Member MemberRepo::findByCloudId(const QString &cloudId)
{
    QSqlQuery q = m_db->execute(
        "SELECT * FROM members WHERE cloud_id = ?",
        {cloudId});
    if (q.next()) {
        return mapFromQuery(q);
    }
    return Member();
}

Member MemberRepo::findByCardNo(const QString &tenantId, const QString &cardNo)
{
    QSqlQuery q = m_db->execute(
        "SELECT * FROM members WHERE tenant_id = ? AND card_no = ?",
        {tenantId, cardNo});
    if (q.next()) {
        return mapFromQuery(q);
    }
    return Member();
}

Member MemberRepo::findByPhone(const QString &tenantId, const QString &phone)
{
    QSqlQuery q = m_db->execute(
        "SELECT * FROM members WHERE tenant_id = ? AND phone = ?",
        {tenantId, phone});
    if (q.next()) {
        return mapFromQuery(q);
    }
    return Member();
}

QList<Member> MemberRepo::findAll(const QString &tenantId)
{
    QList<Member> result;
    QSqlQuery q = m_db->execute(
        "SELECT * FROM members WHERE tenant_id = ? ORDER BY created_at DESC",
        {tenantId});
    while (q.next()) {
        result.append(mapFromQuery(q));
    }
    return result;
}

QList<Member> MemberRepo::search(const QString &tenantId, const QString &keyword, int limit)
{
    QList<Member> result;
    QString pattern = "%" + keyword + "%";
    QSqlQuery q = m_db->execute(
        "SELECT * FROM members WHERE tenant_id = ? AND "
        "(card_no LIKE ? OR name LIKE ? OR phone LIKE ?) "
        "ORDER BY created_at DESC LIMIT ?",
        {tenantId, pattern, pattern, pattern, limit});
    while (q.next()) {
        result.append(mapFromQuery(q));
    }
    return result;
}

bool MemberRepo::save(const Member &member)
{
    if (member.id > 0) {
        // Update existing
        QSqlQuery q = m_db->execute(
            "UPDATE members SET cloud_id = ?, tenant_id = ?, card_no = ?, name = ?, "
            "phone = ?, gender = ?, birthday = ?, level_id = ?, points = ?, balance = ?, "
            "total_spent = ?, status = ?, sync_status = ?, updated_at = datetime('now') "
            "WHERE id = ?",
            {member.cloudId, member.tenantId, member.cardNo, member.name,
             member.phone, member.gender, member.birthday.toString(Qt::ISODate),
             member.levelId, member.points, member.balance, member.totalSpent,
             member.status, member.syncStatus, member.id});
        if (q.lastError().type() == QSqlError::NoError) {
            emit memberSaved(member);
            return true;
        }
        qWarning() << "Failed to update member:" << q.lastError().text();
        return false;
    } else {
        // Insert new
        QSqlQuery q = m_db->execute(
            "INSERT INTO members (cloud_id, tenant_id, card_no, name, phone, gender, "
            "birthday, level_id, points, balance, total_spent, status, sync_status, created_at, updated_at) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, datetime('now'), datetime('now'))",
            {member.cloudId, member.tenantId, member.cardNo, member.name,
             member.phone, member.gender, member.birthday.toString(Qt::ISODate),
             member.levelId, member.points, member.balance, member.totalSpent,
             member.status, member.syncStatus});
        if (q.lastError().type() == QSqlError::NoError) {
            Member saved = member;
            saved.id = q.lastInsertId().toInt();
            emit memberSaved(saved);
            return true;
        }
        qWarning() << "Failed to insert member:" << q.lastError().text();
        return false;
    }
}

bool MemberRepo::remove(int id)
{
    QSqlQuery q = m_db->execute("DELETE FROM members WHERE id = ?", {id});
    if (q.lastError().type() == QSqlError::NoError) {
        emit memberRemoved(id);
        return true;
    }
    qWarning() << "Failed to delete member:" << q.lastError().text();
    return false;
}

bool MemberRepo::updatePoints(int memberId, int pointsDelta)
{
    QSqlQuery q = m_db->execute(
        "UPDATE members SET points = points + ?, updated_at = datetime('now') WHERE id = ?",
        {pointsDelta, memberId});
    if (q.lastError().type() == QSqlError::NoError) {
        return true;
    }
    qWarning() << "Failed to update points:" << q.lastError().text();
    return false;
}

bool MemberRepo::updateBalance(int memberId, double balanceDelta, const QString &reason)
{
    Q_UNUSED(reason);
    QSqlQuery q = m_db->execute(
        "UPDATE members SET balance = balance + ?, updated_at = datetime('now') WHERE id = ?",
        {balanceDelta, memberId});
    if (q.lastError().type() == QSqlError::NoError) {
        return true;
    }
    qWarning() << "Failed to update balance:" << q.lastError().text();
    return false;
}

void MemberRepo::syncFromCloud(const QList<Member> &cloudMembers)
{
    for (const Member &member : cloudMembers) {
        Member existing = findByCloudId(member.cloudId);
        if (existing.id > 0) {
            // Update existing, preserve local changes if sync_status = 0
            if (existing.syncStatus == 0) {
                // Local has pending changes, skip cloud update
                continue;
            }
            Member updated = member;
            updated.id = existing.id;
            save(updated);
        } else {
            // Insert new from cloud
            save(member);
        }
    }
}
