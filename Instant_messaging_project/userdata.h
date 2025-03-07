#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
#include <memory>
// class UserData
// {
// public:
//     UserData();
// };

class SearchInfo{
public:
    SearchInfo(int uid,QString name,QString nick,QString desc,int sex);
    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_desc;
    int m_sex;
};

class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc);
    int m_from_uid;
    QString m_name;
    QString m_desc;
};

struct ApplyInfo {
    ApplyInfo(int uid, QString name, QString desc, QString icon, QString nick, int sex, int status)
        : m_uid(uid), m_name(name), m_desc(desc), m_icon(icon), m_nick(nick),m_sex(sex), m_status(status) {}
    void SetIcon(QString head) {
        m_icon = head;
    }
    int m_uid;
    QString m_name;
    QString m_desc;
    QString m_icon;
    QString m_nick;
    int m_sex;
    int m_status;
};

struct AuthInfo
{
    AuthInfo(int uid,QString name,QString nick,QString icon,int sex)
        :m_uid(uid),m_name(name),m_nick(nick),m_icon(icon),m_sex(sex)
    {

    }

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
};

struct AuthRsp
{
    AuthRsp(int peer_uid,QString peer_name,QString peer_nick,QString peer_icon,int peer_sex)
        :m_uid(peer_uid),m_name(peer_name),m_nick(peer_nick),m_icon(peer_icon),m_sex(peer_sex)
    {

    }
    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
};


struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int sex):
        m_uid(uid), m_name(name), m_nick(nick), m_icon(icon), m_sex(sex) {}

    UserInfo(std::shared_ptr<AuthInfo> auth):
        m_uid(auth->m_uid), m_name(auth->m_name), m_nick(auth->m_nick),
        m_icon(auth->m_icon), m_sex(auth->m_sex) {}

    UserInfo(int uid, QString name, QString icon):
        m_uid(uid), m_name(name), m_icon(icon),m_nick(name),m_sex(0){}

    UserInfo(std::shared_ptr<AuthRsp> auth):
        m_uid(auth->m_uid), m_name(auth->m_name), m_nick(auth->m_nick),
        m_icon(auth->m_icon), m_sex(auth->m_sex) {}

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
};


#endif // USERDATA_H
