#include "CServer.h"
#include <iostream>
#include "AsioIOServicePool.h"
#include "UserMgr.h"
CServer::CServer(boost::asio::io_context& io_context, short port):_io_context(io_context), _port(port),
_acceptor(io_context, tcp::endpoint(tcp::v4(),port))
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

bool CServer::CheckValid(std::string uuid)
{
	auto iter = _sessions.find(uuid);
	if (iter != _sessions.end())
	{
		return true;
	}
	return false;
}
