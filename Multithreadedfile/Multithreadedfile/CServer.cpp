#include "CServer.h"
#include <iostream>
#include "AsioIOServicePool.h"
#include "UserMgr.h"
CServer::CServer(boost::asio::io_context& io_context, short port):
	m_io_context(io_context),m_port(port),m_acceptor(io_context,tcp::endpoint(tcp::v4(),port))
{
	std::cout << "[Server] Started successfully, listening on port: "
		<< m_port << std::endl;
	StartAccept();  // �����첽���ӽ���
}

CServer::~CServer()
{
	std::cout << "[Server] Shutting down on port: " << m_port << std::endl;
}

void CServer::ClearSession(std::string uuid)
{
	// ʹ��C++17�ĳ�ʼ��if���
	if (auto it = m_session.find(uuid); it != m_session.end()) {
		// �Ƴ��û��ͻỰ�Ĺ���
		UserMgr::GetInstance()->RmvUserSession(m_session[uuid]->GetUserId());
	}

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_session.erase(uuid);
		std::cout << "[Session] Removed session: " << uuid << std::endl;
	}
}

//���������ӽ�����ɵĻص�
void CServer::HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error)
{
	if (!error)
	{
		new_session->Start();
		lock_guard<mutex> lock(m_mutex);
		m_session.insert(make_pair(new_session->GetSessionId(), new_session));
	}
	else
	{
		cout << "session accept failed, error is " << error.what() << endl;
	}
	StartAccept();
}

//�����첽���ӽ��� ��IO����ػ�ȡIO�����ģ������»Ự����
void CServer::StartAccept()
{
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	auto new_session = std::make_shared<CSession>(io_context, this);

	m_acceptor.async_accept(new_session->GetSocket(), 
		[this,new_session](const boost::system::error_code& error)
		{
			HandleAccept(new_session, error);
		});
}

