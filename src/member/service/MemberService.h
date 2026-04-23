#pragma once
#include <QObject>
#include "repository/MemberRepo.h"

class MemberRepo;
class OfflineQueue;

class MemberService : public QObject {
    Q_OBJECT
public:
    explicit MemberService(MemberRepo *repo, OfflineQueue *queue, QObject *parent = nullptr);

    Member findByCardNo(const QString &tenantId, const QString &cardNo);
    Member findByPhone(const QString &tenantId, const QString &phone);
    QList<Member> search(const QString &tenantId, const QString &keyword, int limit = 50);

    bool addPoints(int memberId, int points);
    bool usePoints(int memberId, int points);
    bool addBalance(int memberId, double amount);   // recharge
    bool deductBalance(int memberId, double amount); // payment

    double calculateDiscount(const Member &member, double originalAmount);

signals:
    void memberUpdated(const Member &member);
    void pointsChanged(int memberId, int newPoints);
    void balanceChanged(int memberId, double newBalance);

private:
    MemberRepo *m_repo;
    OfflineQueue *m_queue;
};
