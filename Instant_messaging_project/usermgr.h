#ifndef USERMGR_H
#define USERMGR_H
#include <memory>
#include <QObject>
#include "singleton.h"

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
private:
    UserMgr();
    QString m_name;
    QString m_token;
    int m_uid;
};

#endif // USERMGR_H
