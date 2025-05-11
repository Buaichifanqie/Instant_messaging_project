#include "CServer.h"
#include <iostream>
#include "AsioIOServicePool.h"
#include "UserMgr.h"
CServer::CServer(boost::asio::io_context& io_context, short port):_io_context(io_context), _port(port),
_acceptor(io_context, tcp::endpoint(tcp::v4(),port)),_timer(_io_context,std::chrono::seconds(60))
{
	cout << "Server start success, listen on port : " << _port << endl;
	
	StartAccept();
} 

CServer::~CServer() {
	cout << "Server destruct listen on port : " << _port << endl;
}

void CServer::HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error){
	if (!error) {
		new_session->Start();
		lock_guard<mutex> lock(_mutex);
		_sessions.insert(make_pair(new_session->GetSessionId(), new_session));
	}
	else {
		cout << "session accept failed, error is " << error.what() << endl;
	}

	StartAccept();
}

void CServer::StartAccept() {
	auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();
	shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);
	_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));
}

void CServer::ClearSession(std::string session_id) {
	//lock_guard<mutex> lock(_mutex);
	std::cout << "session_id------------------" << session_id << std::endl;
	std::cout << "_session+++++++++++++" << _sessions.empty() << std::endl;
	if (_sessions.find(session_id) != _sessions.end()) {
		auto uid = _sessions[session_id]->GetUserId();

		//移除用户和session的关联
		UserMgr::GetInstance()->RmvUserSession(uid, session_id);
	}
	_sessions.erase(session_id);
	
}

//根据用户获取session
shared_ptr<CSession> CServer::GetSession(std::string uuid)
{
	auto iter = _sessions.find(uuid);
	if (iter != _sessions.end())
	{
		return iter->second;
	}
	return nullptr;
}

bool CServer::CheckValid(std::string sid)
{
	lock_guard<mutex> lock(_mutex);
	auto iter = _sessions.find(sid);
	if (iter != _sessions.end())
	{
		return true;
	}
	return false;
}

void CServer::on_timer(const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cout << "timer error" << ec.message() << std::endl;
		return;
	}
	std::vector<std::shared_ptr<CSession>> _expired_sessions;
	int session_count = 0;

	{
		lock_guard<mutex> lock(_mutex);
		time_t now = time(nullptr);
		for (auto iter = _sessions.begin(); iter != _sessions.end(); ++iter)
		{
			auto b_expired = iter->second->IsHeartbeatExpired(now);
			if (b_expired)
			{
				iter->second->Close();
				//收集过期时间
				_expired_sessions.push_back(iter->second);
				session_count++;
			}
			else
			{
				session_count++;

			}
		}
	}
	
	//设置session数量
	auto& cfg = ConfigMgr::Inst();
	auto self_name = cfg["SelfServer"]["Name"];
	auto count_str = std::to_string(session_count);
	RedisMgr::GetInstance()->HSet(LOGIN_COUNT, self_name, count_str);


	//处理过期session
	for (auto& session : _expired_sessions)
	{
		session->DealExceptionSession();
	}

	//再设置下一个60s检测
	_timer.async_wait([this](boost::system::error_code e) {
		on_timer(e);
	});
}

void CServer::StartTimer()
{
	//_timer.async_wait(std::bind(&CServer::on_timer, this, placeholders::_1));
	auto self = shared_from_this();
	_timer.async_wait([self](boost::system::error_code e) {
		self->on_timer(e);
	});
}

void CServer::StopTimer()
{
	_timer.cancel();
}
 