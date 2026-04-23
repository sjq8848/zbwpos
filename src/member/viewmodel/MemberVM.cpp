#include "MemberVM.h"

// --- MemberModel ---

MemberModel::MemberModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int MemberModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_members.count();
}

QVariant MemberModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_members.count())
        return QVariant();

    const Member &member = m_members.at(index.row());
    switch (role) {
    case IdRole:
        return member.id;
    case CardNoRole:
        return member.cardNo;
    case NameRole:
        return member.name;
    case PhoneRole:
        return member.phone;
    case PointsRole:
        return member.points;
    case BalanceRole:
        return member.balance;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MemberModel::roleNames() const
{
    return {
        {IdRole, "memberId"},
        {CardNoRole, "cardNo"},
        {NameRole, "name"},
        {PhoneRole, "phone"},
        {PointsRole, "points"},
        {BalanceRole, "balance"}
    };
}

void MemberModel::setMembers(const QList<Member> &members)
{
    beginResetModel();
    m_members = members;
    endResetModel();
}

void MemberModel::clear()
{
    beginResetModel();
    m_members.clear();
    endResetModel();
}

// --- MemberVM ---

MemberVM::MemberVM(MemberService *service, QObject *parent)
    : QObject(parent)
    , m_service(service)
    , m_memberModel(new MemberModel(this))
    , m_isLoading(false)
{
}

MemberModel* MemberVM::members() const
{
    return m_memberModel;
}

QString MemberVM::keyword() const
{
    return m_keyword;
}

void MemberVM::setKeyword(const QString &keyword)
{
    if (m_keyword != keyword) {
        m_keyword = keyword;
        emit keywordChanged();
    }
}

bool MemberVM::isLoading() const
{
    return m_isLoading;
}

QVariantMap MemberVM::currentMember() const
{
    QVariantMap map;
    if (m_currentMember.id > 0) {
        map["id"] = m_currentMember.id;
        map["cloudId"] = m_currentMember.cloudId;
        map["tenantId"] = m_currentMember.tenantId;
        map["cardNo"] = m_currentMember.cardNo;
        map["name"] = m_currentMember.name;
        map["phone"] = m_currentMember.phone;
        map["gender"] = m_currentMember.gender;
        map["birthday"] = m_currentMember.birthday.toString(Qt::ISODate);
        map["levelId"] = m_currentMember.levelId;
        map["points"] = m_currentMember.points;
        map["balance"] = m_currentMember.balance;
        map["totalSpent"] = m_currentMember.totalSpent;
        map["status"] = m_currentMember.status;
    }
    return map;
}

void MemberVM::search()
{
    if (m_tenantId.isEmpty() || m_keyword.isEmpty())
        return;

    m_isLoading = true;
    emit isLoadingChanged();

    QList<Member> results = m_service->search(m_tenantId, m_keyword);
    m_memberModel->setMembers(results);

    m_isLoading = false;
    emit isLoadingChanged();
}

void MemberVM::findByCardNo(const QString &cardNo)
{
    if (m_tenantId.isEmpty())
        return;

    m_isLoading = true;
    emit isLoadingChanged();

    Member member = m_service->findByCardNo(m_tenantId, cardNo);
    if (member.id > 0) {
        m_currentMember = member;
        emit currentMemberChanged();
    }

    m_isLoading = false;
    emit isLoadingChanged();
}

void MemberVM::findByPhone(const QString &phone)
{
    if (m_tenantId.isEmpty())
        return;

    m_isLoading = true;
    emit isLoadingChanged();

    Member member = m_service->findByPhone(m_tenantId, phone);
    if (member.id > 0) {
        m_currentMember = member;
        emit currentMemberChanged();
    }

    m_isLoading = false;
    emit isLoadingChanged();
}

void MemberVM::clearCurrentMember()
{
    m_currentMember = Member();
    emit currentMemberChanged();
}

void MemberVM::loadAll()
{
    if (m_tenantId.isEmpty())
        return;

    m_isLoading = true;
    emit isLoadingChanged();

    QList<Member> results = m_service->search(m_tenantId, "");
    m_memberModel->setMembers(results);

    m_isLoading = false;
    emit isLoadingChanged();
}
