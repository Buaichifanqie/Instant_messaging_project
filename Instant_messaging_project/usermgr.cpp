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

int UserMgr::GetUid()
{
    return m_uid;
}

QString UserMgr::GetName()
{
    return m_name;
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::GetApplyList()
{
    return m_apply_list;
}

bool UserMgr::AlreadyApply(int uid)
{
    // if(m_apply_map)
    for(auto& apply:m_apply_list)
    {
        if(apply->m_uid==uid)
        {
            return true;
        }
    }
    return false;
}

void UserMgr::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    m_user_info=user_info;
}

void UserMgr::AddApplyList(std::shared_ptr<ApplyInfo> app)
{
    m_apply_list.push_back(app);
}

void UserMgr::AddApplyMap(std::shared_ptr<ApplyInfo> app)
{

}

void UserMgr::AppendApplyList(QJsonArray array)
{
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue &value : array) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name,
                                                desc, icon, nick, sex, status);
        m_apply_list.push_back(info);
    }
}

bool UserMgr::CheckFriendById(int uid)
{
    auto iter=m_friend_map.find(uid);
    if(iter==m_friend_map.end())
    {
        return false;
    }
    return true;
}

void UserMgr::AddFriend(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto friend_info=std::make_shared<FriendInfo>(auth_rsp);
    m_friend_map[friend_info->m_uid]=friend_info;
}

void UserMgr::AddFriend(std::shared_ptr<AuthInfo> auth_info)
{
    auto friend_info=std::make_shared<FriendInfo>(auth_info);
    m_friend_map[friend_info->m_uid]=friend_info;
}

std::shared_ptr<FriendInfo> UserMgr::GetFriendById(int uid)
{
    auto find_it=m_friend_map.find(uid);
    if(find_it==m_friend_map.end())
    {
        return nullptr;
    }
    return *find_it;
}

UserMgr::UserMgr() {}
