#pragma once
#include "const.h"

class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	HttpConnection(tcp::socket socket);
	void Start();

private:
	//��ⳬʱ
	void CheckDeadline();
	//Ӧ����
	void WriteResponse();
	//��������
	void HandleReq();
	tcp::socket m_socket;
	beast::flat_buffer m_buffer{ 8192 };
	//��������
	http::request<http::dynamic_body> m_request;
	//�ظ�����
	http::response<http::dynamic_body> m_response;
	net::steady_timer m_deadline{
		//��ʼ���б�  
		m_socket.get_executor(),std::chrono::seconds(60)
	};
};

