#include "usermgr.h"

UserMgr::~UserMgr()
{

}

void UserMgr::SetName(QString name)
{
    m_name=name;
}

void UserMgr::SetUid(int uid)
{
    m_uid=uid;
}

void UserMgr::SetToken(QString token)
{
    m_token=token;
}

UserMgr::UserMgr() {}
