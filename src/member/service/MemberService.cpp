#include "MemberService.h"
#include "core/network/OfflineQueue.h"

MemberService::MemberService(MemberRepo *repo, OfflineQueue *queue, QObject *parent)
    : QObject(parent), m_repo(repo), m_queue(queue)
{
}

Member MemberService::findByCardNo(const QString &tenantId, const QString &cardNo)
{
    return m_repo->findByCardNo(tenantId, cardNo);
}

Member MemberService::findByPhone(const QString &tenantId, const QString &phone)
{
    return m_repo->findByPhone(tenantId, phone);
}

QList<Member> MemberService::search(const QString &tenantId, const QString &keyword, int limit)
{
    return m_repo->search(tenantId, keyword, limit);
}

bool MemberService::addPoints(int memberId, int points)
{
    if (points <= 0) {
        qWarning() << "addPoints: points must be positive";
        return false;
    }

    if (!m_repo->updatePoints(memberId, points)) {
        return false;
    }

    Member member = m_repo->findById(memberId);
    if (member.id > 0) {
        // Push to offline queue for sync
        QJsonObject payload;
        payload["points"] = member.points;
        m_queue->push("UPDATE", "members", memberId, payload);

        emit pointsChanged(memberId, member.points);
        emit memberUpdated(member);
    }
    return true;
}

bool MemberService::usePoints(int memberId, int points)
{
    if (points <= 0) {
        qWarning() << "usePoints: points must be positive";
        return false;
    }

    Member member = m_repo->findById(memberId);
    if (member.id == 0) {
        qWarning() << "usePoints: member not found";
        return false;
    }

    if (member.points < points) {
        qWarning() << "usePoints: insufficient points";
        return false;
    }

    if (!m_repo->updatePoints(memberId, -points)) {
        return false;
    }

    member = m_repo->findById(memberId);
    if (member.id > 0) {
        // Push to offline queue for sync
        QJsonObject payload;
        payload["points"] = member.points;
        m_queue->push("UPDATE", "members", memberId, payload);

        emit pointsChanged(memberId, member.points);
        emit memberUpdated(member);
    }
    return true;
}

bool MemberService::addBalance(int memberId, double amount)
{
    if (amount <= 0) {
        qWarning() << "addBalance: amount must be positive";
        return false;
    }

    if (!m_repo->updateBalance(memberId, amount, "recharge")) {
        return false;
    }

    Member member = m_repo->findById(memberId);
    if (member.id > 0) {
        // Push to offline queue for sync
        QJsonObject payload;
        payload["balance"] = member.balance;
        m_queue->push("UPDATE", "members", memberId, payload);

        emit balanceChanged(memberId, member.balance);
        emit memberUpdated(member);
    }
    return true;
}

bool MemberService::deductBalance(int memberId, double amount)
{
    if (amount <= 0) {
        qWarning() << "deductBalance: amount must be positive";
        return false;
    }

    Member member = m_repo->findById(memberId);
    if (member.id == 0) {
        qWarning() << "deductBalance: member not found";
        return false;
    }

    if (member.balance < amount) {
        qWarning() << "deductBalance: insufficient balance";
        return false;
    }

    if (!m_repo->updateBalance(memberId, -amount, "payment")) {
        return false;
    }

    member = m_repo->findById(memberId);
    if (member.id > 0) {
        // Push to offline queue for sync
        QJsonObject payload;
        payload["balance"] = member.balance;
        m_queue->push("UPDATE", "members", memberId, payload);

        emit balanceChanged(memberId, member.balance);
        emit memberUpdated(member);
    }
    return true;
}

double MemberService::calculateDiscount(const Member &member, double originalAmount)
{
    // Default discount is 1.0 (no discount)
    double discount = 1.0;

    // TODO: Look up member level discount from MemberLevel table
    // For now, use a simple discount calculation based on member points
    if (member.levelId > 0) {
        // Placeholder: would query member_levels table for discount rate
        // For now, use tiered discount based on total spent
        if (member.totalSpent >= 10000) {
            discount = 0.85; // 15% off for VIP
        } else if (member.totalSpent >= 5000) {
            discount = 0.90; // 10% off for Gold
        } else if (member.totalSpent >= 1000) {
            discount = 0.95; // 5% off for Silver
        }
    }

    return originalAmount * discount;
}
