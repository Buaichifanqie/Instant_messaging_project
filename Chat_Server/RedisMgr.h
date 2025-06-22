#pragma once
#include "const.h"
#include "hiredis.h"
#include <queue>
#include <atomic>
#include <mutex>
#include "Singleton.h"
class RedisConPool {
public:
	RedisConPool(size_t poolSize, const char* host, int port, const char* pwd)
		: poolSize_(poolSize), host_(host), port_(port), b_stop_(false), pwd_(pwd), counter_(0),fail_count_(0){
		for (size_t i = 0; i < poolSize_; ++i) {
			auto* context = redisConnect(host, port);
			if (context == nullptr || context->err != 0) {
				if (context != nullptr) {
					redisFree(context);
				}
				continue;
			}

			auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
			if (reply->type == REDIS_REPLY_ERROR) {
				std::cout << "认证失败" << std::endl;
				//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
				freeReplyObject(reply);
				continue;
			}

			//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
			freeReplyObject(reply);
			std::cout << "认证成功" << std::endl;
			connections_.push(context);
		}

		check_thread_ = std::thread([this]() {
			while (!b_stop_) {
				counter_++;
				if (counter_ >= 60) {
					checkThreadPro();
					counter_ = 0;
				}

				std::this_thread::sleep_for(std::chrono::seconds(1)); // 每隔 30 秒发送一次 PING 命令
			}	
		});

	}

	~RedisConPool() {

	}

	void ClearConnections() {
		std::lock_guard<std::mutex> lock(mutex_);
		while (!connections_.empty()) {
			auto* context = connections_.front();
			redisFree(context);
			connections_.pop();
		}
	}

	redisContext* getConnection() {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this] { 
			if (b_stop_) {
				return true;
			}
			return !connections_.empty();
			});
		//如果停止则直接返回空指针
		if (b_stop_) {
			return  nullptr;
		}
		auto* context = connections_.front();  
		connections_.pop();
		return context;
	}

	//写一个非阻塞的获取连接的函数
	redisContext* getConNonBlock()
	{
		std::lock_guard<std::mutex>lock(mutex_);
		if (b_stop_)
		{
			return nullptr;
		}
		if (connections_.empty())
		{
			return nullptr;
		}
		auto* context = connections_.front();
		connections_.pop();
		return context;
	}

	void returnConnection(redisContext* context) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}
		connections_.push(context);
		cond_.notify_one();
	}

	void Close() {
		b_stop_ = true;
		cond_.notify_all();
		check_thread_.join();
	}

private:
	void checkThreadPro()
	{
		size_t pool_size;
		{
			std::lock_guard<std::mutex>lock(mutex_);
			pool_size = connections_.size();

		}

		for (int i = 0; i < pool_size && !b_stop_; i++) {
			/*为啥要判断b_stop_, 因为redis开线程的时候我们没有detach(分离)
			所以我们要等线程汇合，mysql里写的时候我们时deatch了两种写法都写一下*/
			redisContext* context = nullptr;
			//1.加锁获取连接
			context = getConNonBlock();
			if (context == nullptr)
			{
				break;
			}
			redisReply* reply = nullptr;
			try {
				reply = (redisReply*)redisCommand(context, "PING");//发送一个PING命令
				//2.先看底层I/O协议有没有错误
				if (context->err)
				{
					std::cout << "redis ping failed, error is " << context->errstr << std::endl;
					if (reply != nullptr)
					{
						freeReplyObject(reply);
					}
					redisFree(context);//释放redis连接
					fail_count_++;//失败次数加1
					continue;
				}
				//3.再看redis自身返回的是不是错误
				if (reply == nullptr || reply->type != REDIS_REPLY_STATUS || strcmp(reply->str, "PONG") != 0)
				{
					std::cout << "redis ping failed, reply is null or not PONG" << std::endl;
					if (reply != nullptr)
					{
						freeReplyObject(reply);
					}
					redisFree(context);//释放redis连接
					fail_count_++;//失败次数加1
					continue;
				}
				//4.如果都没有问题则将连接放回连接池
				std::cout << "redis ping success" << std::endl;
				std::cout << "connection alive" << std::endl;
				freeReplyObject(reply);//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
				returnConnection(context);//将连接放回连接池
			}
			catch (std::exception& exp)
			{
				if (reply != nullptr)//reply相当于一个应答（就是我们做一些查询它会有一些回复）当他不为空我们就释放
				{
					freeReplyObject(reply);//出现异常我们释放redisCommand执行后返回的redisReply所占用的内存
				}
				redisFree(context);//释放redis连接
				fail_count_++;//失败次数加1
			}
		}
		//在for外面做重连
		while (fail_count_ > 0)
		{
			auto res = reconnect();
			if (res)
			{
				fail_count_--;
			}
			else
			{
				//留给下一次再尝试
				break;
			}
		}
	}


	bool reconnect()
	{
		auto* context = redisConnect(host_, port_);//重新连接
		if (context == nullptr || context->err != 0) {
			if (context != nullptr) {//如果连接失败则释放连接
				redisFree(context);//释放redis连接
			}
			std::cout << "redis reconnect failed" << std::endl;
			return false;
		}	

		auto reply = (redisReply*)redisCommand(context,"Auth %s",pwd_);//重新密码认证
		if (reply->type == REDIS_REPLY_ERROR)
		{
			std::cout << "redis reconnect auth failed 认证失败" << std::endl;
			//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
			freeReplyObject(reply);
			redisFree(context);//释放redis连接
			return false;
		}

		//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		std::cout << "redis reconnect auth success 认证成功" << std::endl;
		returnConnection(context); //将连接放回连接池
		return true;
	}

	void checkThread() {
		std::lock_guard<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}
		auto pool_size = connections_.size();
		for (int i = 0; i < pool_size && !b_stop_; i++) {
			auto* context = connections_.front();
			connections_.pop();
			try {
				auto reply = (redisReply*)redisCommand(context, "PING");
				if (!reply) {
					std::cout << "reply is null, redis ping failed: " << std::endl;
					connections_.push(context);
					continue;
				}
				freeReplyObject(reply);
				connections_.push(context);
			}
			catch(std::exception& exp){
				std::cout << "Error keeping connection alive: " << exp.what() << std::endl;
				redisFree(context);
				context = redisConnect(host_, port_);
				if (context == nullptr || context->err != 0) {
					if (context != nullptr) {
						redisFree(context);
					}
					continue;
				}

				auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd_);
				if (reply->type == REDIS_REPLY_ERROR) {
					std::cout << "认证失败" << std::endl;
					//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
					freeReplyObject(reply);
					continue;
				}

				//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
				freeReplyObject(reply);
				std::cout << "认证成功" << std::endl;
				connections_.push(context);
			}
		}
	}
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	const char* host_;
	const char* pwd_;
	int port_;
	std::queue<redisContext*> connections_;
	std::mutex mutex_;
	std::condition_variable cond_;
	std::thread  check_thread_;
	int counter_;

	std::atomic<int> fail_count_;

};

class RedisMgr: public Singleton<RedisMgr>, 
	public std::enable_shared_from_this<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
	~RedisMgr();
	bool Get(const std::string &key, std::string& value);
	bool Set(const std::string &key, const std::string &value);
	bool LPush(const std::string &key, const std::string &value);
	bool LPop(const std::string &key, std::string& value);
	bool RPush(const std::string& key, const std::string& value);
	bool RPop(const std::string& key, std::string& value);
	bool HSet(const std::string &key, const std::string  &hkey, const std::string &value);
	bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
	std::string HGet(const std::string &key, const std::string &hkey);
	bool HDel(const std::string& key, const std::string& field);
	bool Del(const std::string &key);
	bool ExistsKey(const std::string &key);
	void Close() {
		_con_pool->Close();
		_con_pool->ClearConnections();
	}

	std::string acquireLock(const std::string& lockName, int lockTimeout, int aquireTimeout);
	bool releaseLock(const std::string& lockName, const std::string& identifier);


	void IncreaseCount(std::string server_name);
	void DecreaseCount(std::string server_name);
	void InitCount(std::string server_name);
	void DelCount(std::string server_name);
private:
	RedisMgr();
	unique_ptr<RedisConPool>  _con_pool;
};

