#include "UserMgr.h"
#include "CSession.h"
UserMgr::~UserMgr()
{
	m_uid_to_session.clear();
}

std::shared_ptr<CSession> UserMgr::GetSession(int uid)
{
	std::lock_guard<std::mutex> lock(m_session_mtx);
	auto iter = m_uid_to_session.find(uid);
	return (iter != m_uid_to_session.end()) ? iter->second : nullptr;
}

void UserMgr::SetUserSession(int uid, std::shared_ptr<CSession> session)
{
	std::lock_guard<std::mutex>lock(m_session_mtx);
	m_uid_to_session[uid] = session;
}

void UserMgr::RmvUserSession(int uid)
{
	{
		std::lock_guard<std::mutex> lock(m_session_mtx); // 自动加锁
		m_uid_to_session.erase(uid); // STL erase保证线程安全
	}
}

bool UserMgr::IsActive()
{
	return m_is_active.load(std::memory_order_relaxed);
}

UserMgr::UserMgr()
{

}

