#ifndef USERMGR_H
#define USERMGR_H
#include <memory>
#include <QObject>
#include "singleton.h"
#include "userdata.h"
#include <vector>

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
private:
    UserMgr();
    QString m_name;
    QString m_token;
    int m_uid;
    std::vector<std::shared_ptr<ApplyInfo>> m_apply_list;
};

#endif // USERMGR_H
