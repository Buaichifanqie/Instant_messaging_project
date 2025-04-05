#pragma once
#include <string>
#include <hiredis.h>
//采用单例模式，通过Redis的原子操作来确保锁的互斥性。
class DistLock
{
public:
	static DistLock& Init();
	~DistLock();

	//尝试获取分布式锁
	std::string acquireLock(redisContext* context, const std::string& lockName,
		int lockTimeout, int acquireTimeout);
	//释放分布式锁
	bool releaseLock(redisContext* context, const std::string& lockName, const std::string& identifier);
private:
	//默认构造函数
	DistLock() = default;
};

