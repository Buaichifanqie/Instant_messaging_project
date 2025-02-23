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
	//检测超时
	void CheckDeadline();
	//应答函数
	void WriteResponse();
	//处理请求
	void HandleReq();
	void PreParseGetParam();

	tcp::socket m_socket;
	beast::flat_buffer m_buffer{ 8192 };
	//接收请求
	http::request<http::dynamic_body> m_request;
	//回复请求
	http::response<http::dynamic_body> m_response;
	std::string m_get_url;
	std::unordered_map<std::string, std::string> m_get_params;

	net::steady_timer m_deadline{
		//初始化列表  
		m_socket.get_executor(),std::chrono::seconds(60)
	};
};

