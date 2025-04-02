#include "LogicSystem.h"
#include "const.h"
#include "base64.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include "ConfigMgr.h"

using namespace std;

LogicSystem::~LogicSystem()
{
}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg,int index)
{
	m_workers[index]->PostTask(msg);
}

LogicSystem::LogicSystem()
{
	for (int i = 0; i < LOGIC_WORKER_COUNT; i++)
	{
		m_workers.push_back(std::make_shared<LogicWorker>());
	}
}