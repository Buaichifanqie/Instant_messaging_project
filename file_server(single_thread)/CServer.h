#pragma once
#include <boost/asio.hpp>
#include "CSession.h"
#include <map>
#include <memory>
#include <mutex>

using namespace std;

class CServer
{
public:
	CServer(boost::asio::io_context& io_context,short port);
	~CServer();
	void ClearSession(std::string);
private:
	void HandleAccept(std::shared_ptr<CSession>, const boost::system::error_code& error);
	void StartAccept();
	boost::asio::io_context& m_io_context;
	short m_port;
	tcp::acceptor m_acceptor;
	std::map<std::string, std::shared_ptr<CSession>> m_session;
	std::mutex m_mutex;
};

