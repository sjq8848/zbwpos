#pragma once
#include <QObject>
#include <QAbstractListModel>
#include "service/MemberService.h"

class MemberModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { IdRole = Qt::UserRole + 1, CardNoRole, NameRole, PhoneRole, PointsRole, BalanceRole };
    explicit MemberModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setMembers(const QList<Member> &members);
    void clear();

private:
    QList<Member> m_members;
};

class MemberVM : public QObject {
    Q_OBJECT
    Q_PROPERTY(MemberModel* members READ members CONSTANT)
    Q_PROPERTY(QString keyword READ keyword WRITE setKeyword NOTIFY keywordChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QVariantMap currentMember READ currentMember NOTIFY currentMemberChanged)

public:
    explicit MemberVM(MemberService *service, QObject *parent = nullptr);

    MemberModel* members() const;
    QString keyword() const;
    void setKeyword(const QString &keyword);
    bool isLoading() const;
    QVariantMap currentMember() const;

    Q_INVOKABLE void search();
    Q_INVOKABLE void findByCardNo(const QString &cardNo);
    Q_INVOKABLE void findByPhone(const QString &phone);
    Q_INVOKABLE void clearCurrentMember();
    Q_INVOKABLE void loadAll();

    void setTenantStore(const QString &tenantId);

signals:
    void keywordChanged();
    void isLoadingChanged();
    void currentMemberChanged();

private:
    MemberService *m_service;
    MemberModel *m_memberModel;
    QString m_keyword;
    bool m_isLoading = false;
    Member m_currentMember;
    QString m_tenantId;
};
