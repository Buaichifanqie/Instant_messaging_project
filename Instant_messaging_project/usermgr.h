#ifndef USERMGR_H
#define USERMGR_H
#include <memory>
#include <QObject>
#include "singleton.h"
#include "userdata.h"
#include <vector>
#include <QJsonArray>
class UserMgr:public QObject,public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~UserMgr();
    void SetName(QString name);
    void SetUid(int uid);
    void SetToken(QString token);
    int GetUid();
    QString GetName();
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    bool AlreadyApply(int uid);
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void AddApplyList(std::shared_ptr<ApplyInfo> app);
    void AddApplyMap(std::shared_ptr<ApplyInfo> app);
    void AppendApplyList(QJsonArray array);
    bool CheckFriendById(int uid);
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    std::shared_ptr<FriendInfo> GetFriendById(int uid);
private:
    UserMgr();
    QString m_name;
    QString m_token;
    int m_uid;
    std::vector<std::shared_ptr<ApplyInfo>> m_apply_list;
    std::shared_ptr<UserInfo> m_user_info;
    QMap<int,std::shared_ptr<FriendInfo>> m_friend_map;
};

#endif // USERMGR_H
