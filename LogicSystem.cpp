#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> con)
{
	if (m_get_handlers.find(path) == m_get_handlers.end())
	{
		return false;
	}
	m_get_handlers[path](con);
	return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
	m_get_handlers.insert(make_pair(url, handler));
}


void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
	m_post_handlers.insert(make_pair(url, handler));
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> con)
{
	if (m_post_handlers.find(path) == m_post_handlers.end())
	{
		return false;
	}
	m_post_handlers[path](con);
	return true;
}



LogicSystem::LogicSystem()
{
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection){
		beast::ostream(connection->m_response.body()) << "receive get_test req";
		int i = 0;
		for (auto& elem : connection->m_get_params)
		{
			i++;
			beast::ostream(connection->m_response.body()) << "param " << i << "key is " << elem.first;
			beast::ostream(connection->m_response.body()) << "param " << i << "value is " << elem.second << std::endl;
		}
	});

	RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
		auto body_str = boost::beast::buffers_to_string(connection->m_request.body().data());
		std::cout << "receive body is" << body_str << std::endl;
		connection->m_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success)
		{
			std::cout << "Failed to parse JSON data!" << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->m_response.body()) << jsonstr;
			return true;
		}

		if (!src_root.isMember("email"))
		{
			std::cout << "Failed to parse JSON data!" << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->m_response.body()) << jsonstr;
			return true;
		}

		auto email = src_root["email"].asString();
		
		GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);

		std::cout << "email is" << email << std::endl;
		root["error"] = rsp.error();
		root["email"] = src_root["email"];
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->m_response.body()) << jsonstr;
		return true;
	});

}

