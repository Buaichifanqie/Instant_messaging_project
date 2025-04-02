#include "LogicWorker.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
//#include "FileSystem.h"
#include "CSession.h"

LogicWorker::LogicWorker()
{
	//注册回调函数
	RegisterCallBacks();
	m_work_thread = std::thread(
		[this]() {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.wait(lock,
				[this]()
				{
					if (m_b_stop)
					{
						return true;
					}
					if (m_task_que.empty())
					{
						return false;
					}
					return true;
				});
			if (m_b_stop)
			{
				return;
			}
			auto task1 = m_task_que.front();
			std::shared_ptr<LogicNode> task = *task1.release();
			task_callback(task);
			m_task_que.pop();
		});
}

LogicWorker::~LogicWorker()
{
	m_b_stop = true;
	m_cv.notify_one();
	m_work_thread.join();
}

void LogicWorker::PostTask(std::shared_ptr<LogicNode> task)
{
	//无锁队列不用加锁
	m_task_que.push(task);
	m_cv.notify_one();
}

void LogicWorker::RegisterCallBacks()
{
	m_fun_callbacks[ID_TEST_MSG_REQ] = [this](shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
		{
			Json::Reader reader;
			Json::Value root;
			reader.parse(msg_data, root);
			auto data = root["data"].asString();
			std::cout << "recv test data is  " << data << std::endl;

			Json::Value rtvalue;	
			Defer defer([this, &rtvalue, session]()
				{
					std::string return_str = rtvalue.toStyledString();
					session->Send(return_str, ID_TEST_MSG_RSP);
				});
			rtvalue["error"] = ErrorCodes::Success;
			rtvalue["data"] = data;
		};
	m_fun_callbacks[ID_UPLOAD_FILE_REQ] = [this](shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
		{
			Json::Reader reader;
			Json::Value root;
			reader.parse(msg_data, root);
			auto seq = root["seq"].asInt();
			auto name = root["name"].asString();
			auto total_size = root["total_size"].asInt();
			auto trans_size = root["trans_size"].asInt();
			auto last = root["last"].asInt();
			auto file_data = root["data"].asString();
			Json::Value rtvalue;
			Defer defer([this, &rtvalue, session]()
				{
					std::string return_str = rtvalue.toStyledString();
					session->Send(return_str, ID_UPLOAD_FILE_RSP);
				});

			std::hash<std::string> hash_fn;
			size_t hash_value = hash_fn(name);
			int index = hash_value % FILE_WORKER_COUNT;
			std::cout << "Hash value: " << hash_value << std::endl;

		};
}

void LogicWorker::task_callback(std::shared_ptr<LogicNode> task)
{
	cout << "recv_msg id  is " << task->m_recvnode->m_msg_id << endl;
	auto call_back_iter = m_fun_callbacks.find(task->m_recvnode->m_msg_id);
	if (call_back_iter == m_fun_callbacks.end())
	{
		return;
	}
	call_back_iter->second(task->m_session, task->m_recvnode->m_msg_id,
		std::string(task->m_recvnode->m_data, task->m_recvnode->m_cur_len));
}
