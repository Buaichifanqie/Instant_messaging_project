#include "LogicSystem.h"
#include "HttpConnection.h"

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
LogicSystem::LogicSystem()
{
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection){
		beast::ostream(connection->m_response.body()) << "receive get_test req";
	});
}

