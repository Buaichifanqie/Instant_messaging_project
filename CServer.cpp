#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServerPool.h"
CServer::CServer(boost::asio::io_context& ioc, unsigned short& port):m_ioc(ioc),
m_acceptor(ioc,tcp::endpoint(tcp::v4(),port))
{

} 

void CServer::Start()
{
	auto self = shared_from_this();
	auto& ioc_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(ioc_context);
	//异步接收
	m_acceptor.async_accept(new_con->GetSocket(), [self,new_con](beast::error_code ec) {
		try {
			//出错放弃该链接，继续监听其它链接
			if (ec)
			{
				self->Start();
				return;
			}

			//连接管理类来处理连接
			//创建新连接，并且创建HttpConnection类来管理这个链接
			new_con->Start();

			//继续监听
			self->Start();
		}
		catch (std::exception& exp) {

		}
	});
}
