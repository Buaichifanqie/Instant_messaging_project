#pragma once
#include <thread>
#include <queue>
#include <condition_variable>
#include <memory>
#include "MsgNode.h"
#include <mutex>
#include <unordered_map>

class CSession;
class LogicNode
{
public:
	LogicNode(std::shared_ptr<CSession>, std::shared_ptr<RecvNode>);
	std::shared_ptr<CSession> m_session;
	std::shared_ptr<RecvNode> m_recvnode;
};

using FunCallBack = function<void(shared_ptr<CSession>, const short& msg_id, const string& msg_data)>;

class LogicWorker
{
public:
	LogicWorker();
	~LogicWorker();
	void PostTask(std::shared_ptr<LogicNode> task);
	void RegisterCallBacks();
private:
	void task_callback(std::shared_ptr<LogicNode>);
	std::mutex m_mutex;
	std::thread m_work_thread;
	MyQueue<std::shared_ptr<LogicNode>> m_task_que;
	std::atomic<bool> m_b_stop;
	std::condition_variable m_cv;
	std::unordered_map<short, FunCallBack> m_fun_callbacks;	
};
