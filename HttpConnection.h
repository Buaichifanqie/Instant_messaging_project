#pragma once
#include "const.h"

class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	HttpConnection(boost::asio::io_context& ioc);
	void Start();
	inline tcp::socket& GetSocket()
	{
		return m_socket;
	}
private:
	//��ⳬʱ
	void CheckDeadline();
	//Ӧ����
	void WriteResponse();
	//��������
	void HandleReq();
	void PreParseGetParam();

	tcp::socket m_socket;
	beast::flat_buffer m_buffer{ 8192 };
	//��������
	http::request<http::dynamic_body> m_request;
	//�ظ�����
	http::response<http::dynamic_body> m_response;
	std::string m_get_url;
	std::unordered_map<std::string, std::string> m_get_params;

	net::steady_timer m_deadline{
		//��ʼ���б�  
		m_socket.get_executor(),std::chrono::seconds(60)
	};
};

