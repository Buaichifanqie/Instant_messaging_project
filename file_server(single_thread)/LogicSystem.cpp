#include "LogicSystem.h"
#include "const.h"
#include "base64.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include "ConfigMgr.h"

using namespace std;

LogicSystem::LogicSystem() :m_b_stop(false) {
	RegisterCallBacks();
	m_worker_thread = std::thread(&LogicSystem::DealMsg, this);
}

void LogicSystem::DealMsg() {
	while (true) {
		std::unique_ptr<shared_ptr<LogicNode>> msg_ptr;

		// 处理所有可用消息
		while (auto msg_node1 = m_msg_que.pop()) {
			std::shared_ptr<LogicNode>* rawSharedPtr = msg_node1.release();
			std::shared_ptr<LogicNode> msg_node;
			if (rawSharedPtr)
			{
				msg_node = std::move(*rawSharedPtr);
				delete rawSharedPtr;
			}
			cout << "recv_msg id is " << msg_node->m_recvnode->m_msg_id << endl;

			auto callback = m_fun_callbacks.find(msg_node->m_recvnode->m_msg_id);
			if (callback == m_fun_callbacks.end()) {
				cout << "msg handler not found: " << msg_node->m_recvnode->m_msg_id << endl;
				continue;
			}

			callback->second(
				msg_node->m_session,
				msg_node->m_recvnode->m_msg_id,
				string(msg_node->m_recvnode->m_data, msg_node->m_recvnode->m_cur_len)
			);
		}

		// 检查停止条件
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			if (m_b_stop.load()) break;

			// 等待新消息或停止信号
			m_consume.wait(lock, [this]() {
				return !m_msg_que.empty() || m_b_stop.load();
				});
		}
	}

	// 处理剩余消息
	while (auto msg_node1 = m_msg_que.pop()) {
		std::shared_ptr<LogicNode>* rawSharedPtr = msg_node1.release();
		std::shared_ptr<LogicNode> msg_node;
		if (rawSharedPtr)
		{
			msg_node = std::move(*rawSharedPtr);
			delete rawSharedPtr;
		}
		// 同上处理逻辑
		auto callback = m_fun_callbacks.find(msg_node->m_recvnode->m_msg_id);
		if (callback != m_fun_callbacks.end()) {
			callback->second(
				msg_node->m_session,
				msg_node->m_recvnode->m_msg_id,
				string(msg_node->m_recvnode->m_data, msg_node->m_recvnode->m_cur_len)
			);
		}
	}
}

LogicSystem::~LogicSystem() {
	m_b_stop = true;
	m_consume.notify_one();
	m_worker_thread.join();
}

// LogicSystem.cpp 修改
void LogicSystem::PostMsgToQue(shared_ptr<LogicNode> msg) {
	m_msg_que.push(msg);  // 无锁插入
	m_consume.notify_one(); // 总是通知
}



void LogicSystem::RegisterCallBacks() {
	m_fun_callbacks[ID_TEST_MSG_REQ] = [this](std::shared_ptr<CSession> session, const short& msg_id,
		const std::string& msg_data) {
			Json::Reader reader;
			Json::Value root;
			reader.parse(msg_data, root);
			auto data = root["data"].asString();
			std::cout << "recv test data is  " << data << std::endl;

			Json::Value  rtvalue;
			Defer defer([this, &rtvalue, session]() {
				std::string return_str = rtvalue.toStyledString();
				session->Send(return_str, ID_TEST_MSG_RSP);
				});

			rtvalue["error"] = ErrorCodes::Success;
			rtvalue["data"] = data;
		};

	m_fun_callbacks[ID_UPLOAD_FILE_REQ] = [this](std::shared_ptr<CSession> session, const short& msg_id,
		const std::string& msg_data) {
			Json::Reader reader;
			Json::Value root;
			reader.parse(msg_data, root);
			auto data = root["data"].asString();
			//std::cout << "recv file data is  " << data << std::endl;

			Json::Value  rtvalue;
			Defer defer([this, &rtvalue, session]() {
				std::string return_str = rtvalue.toStyledString();
				session->Send(return_str, ID_UPLOAD_FILE_RSP);
				});

			// 解码
			std::string decoded = base64_decode(data);

			auto seq = root["seq"].asInt();
			auto name = root["name"].asString();
			auto total_size = root["total_size"].asInt();
			auto trans_size = root["trans_size"].asInt();
			auto file_path = ConfigMgr::Inst().GetFileOutPath();
			auto file_path_str = (file_path / name).string();
			std::cout << "file_path_str is " << file_path_str << std::endl;
			std::ofstream outfile;
			//第一个包
			if (seq == 1) {
				// 打开文件，如果存在则清空，不存在则创建
				outfile.open(file_path_str, std::ios::binary | std::ios::trunc);
			}
			else {
				// 保存为文件
				outfile.open(file_path_str, std::ios::binary | std::ios::app);
			}


			if (!outfile) {
				std::cerr << "无法打开文件进行写入。" << std::endl;
				return;
			}

			outfile.write(decoded.data(), decoded.size());
			if (!outfile) {
				std::cerr << "写入文件失败。" << std::endl;
				return;
			}

			outfile.close();
			std::cout << "文件已成功保存为: " << name << std::endl;

			rtvalue["error"] = ErrorCodes::Success;
			rtvalue["total_size"] = total_size;
			rtvalue["seq"] = seq;
			rtvalue["name"] = name;
			rtvalue["trans_size"] = trans_size;
		};
}