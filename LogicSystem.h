#pragma once
#include "const.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
//using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;

class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem() {}
	bool HandleGet(std::string,std::shared_ptr<HttpConnection>);
	void RegGet(std::string,HttpHandler handler);
private:
	LogicSystem();
	std::map<std::string, HttpHandler> m_post_handlers;
	std::map<std::string, HttpHandler> m_get_handlers;
};

