#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
class CSession;

class UserMgr:public Singleton<UserMgr>
{
	friend class Singleton<UserMgr>;

public:
	~UserMgr();
	std::shared_ptr<CSession>GetSession(int uid);
	void SetUserSession(int uid, std::shared_ptr<CSession>session);
	void RmvUserSession(int uid);
	bool IsActive();
private:
	UserMgr();
	std::atomic<bool> m_is_active;                // »á»°»îÔ¾×´Ì¬
	std::mutex m_session_mtx;
	std::unordered_map<int, std::shared_ptr<CSession>> m_uid_to_session;
};

