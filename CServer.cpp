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
	//�첽����
	m_acceptor.async_accept(new_con->GetSocket(), [self,new_con](beast::error_code ec) {
		try {
			//������������ӣ�����������������
			if (ec)
			{
				self->Start();
				return;
			}

			//���ӹ���������������
			//���������ӣ����Ҵ���HttpConnection���������������
			new_con->Start();

			//��������
			self->Start();
		}
		catch (std::exception& exp) {

		}
	});
}
