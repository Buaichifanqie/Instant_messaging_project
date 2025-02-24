#pragma once
#include "const.h"

class RedisConPool {
public:
    RedisConPool(size_t poolsize, const char* host, int port, const char* pwd);
    ~RedisConPool();
    redisContext* getConnection();
    void returnConnection(redisContext* context);
    void Close();
private:
    std::atomic<bool> m_b_stop;
    size_t m_poolSize;
    const char* m_host;
    int m_port;
    std::queue<redisContext*> m_connections;//后期优化成无锁队列
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

class RedisMgr : public Singleton<RedisMgr>
{
    friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    bool Get(const std::string& key, std::string& value);
    bool Set(const std::string& key, const std::string& value);
    bool Auth(const std::string& password);
    bool LPush(const std::string& key, const std::string& value);
    bool LPop(const std::string& key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);
    bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string& key, const std::string& hkey);
    bool Del(const std::string& key);
    bool ExistsKey(const std::string& key);
    void Close();

private:
    RedisMgr();
    std::unique_ptr<RedisConPool> m_con_pool;
};



