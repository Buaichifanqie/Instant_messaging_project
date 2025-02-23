#pragma once
#include "const.h"
#include "HttpConnection.h"

class CServer:public std::enable_shared_from_this<CServer>
{
public:
	//上下文
	CServer(boost::asio::io_context& ioc, unsigned short& port);
	//启动服务器
	void Start();
private:
	//接收器接收对端的链接
	tcp::acceptor m_acceptor;
	net::io_context& m_ioc;
};

