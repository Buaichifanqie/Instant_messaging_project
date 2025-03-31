//#pragma once
//
//#include "Singleton.h"
//#include <thread>
//#include "CSession.h"
//#include <map>
//#include <functional>
//#include "const.h"
//#include <json/json.h>
//#include <json/value.h>
//#include <json/reader.h>
//#include <unordered_map>
//#include "MyQueue.h"
//
//typedef std::function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)> FunCallBack;
//
//class LogicSystem : public Singleton<LogicSystem> {
//public:
//	~LogicSystem();
//	void PostMsgToQue(shared_ptr < LogicNode> msg);
//private:
//    LogicSystem();  // 私有构造函数
//
//    void DealMsg();
//    void RegisterCallBacks();
//
//    MyQueue<std::shared_ptr<LogicNode>> m_msg_que;      // 无锁队列
//    std::atomic<bool> m_b_stop{ false };
//    std::mutex m_mutex;
//    std::condition_variable m_consume;
//    std::thread m_worker_thread;
//    std::map<short, FunCallBack> m_fun_callbacks;
//};
//

#pragma once

#include "Singleton.h"
#include <thread>
#include "CSession.h"
#include <map>
#include <functional>
#include "const.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>
#include "MyQueue.h"

typedef std::function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)> FunCallBack;

class LogicSystem : public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;  // 关键：声明友元关系
public:
    ~LogicSystem();
    void PostMsgToQue(std::shared_ptr<LogicNode> msg);

private:
    LogicSystem();  // 私有构造函数

    void DealMsg();
    void RegisterCallBacks();

    MyQueue<std::shared_ptr<LogicNode>> m_msg_que;      // 无锁队列
    std::atomic<bool> m_b_stop{ false };
    std::mutex m_mutex;
    std::condition_variable m_consume;
    std::thread m_worker_thread;
    std::map<short, FunCallBack> m_fun_callbacks;
};