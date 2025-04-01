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
#include "LogicWorker.h"
typedef std::function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)> FunCallBack;

class LogicSystem : public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;  // 关键：声明友元关系
public:
    ~LogicSystem();
    void PostMsgToQue(std::shared_ptr<LogicNode> msg,int index);

private:
    LogicSystem();  // 私有构造函数
    std::vector<std::shared_ptr<LogicWorker>> m_workers;    
};