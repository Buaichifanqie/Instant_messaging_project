#pragma once
#include "const.h"
#include <thread>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>
#include "data.h"
#include <memory>
#include <queue>
#include <mutex>
class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t lasttime):_con(con), _last_oper_time(lasttime){}
	std::unique_ptr<sql::Connection> _con;
	int64_t _last_oper_time;
};

class MySqlPool {
public:
	MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize)
		: url_(url), user_(user), pass_(pass), schema_(schema), poolSize_(poolSize), b_stop_(false),_fail_count(0){
		try {
			for (int i = 0; i < poolSize_; ++i) {
				sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
				auto*  con = driver->connect(url_, user_, pass_);
				con->setSchema(schema_);
				// 获取当前时间戳
				auto currentTime = std::chrono::system_clock::now().time_since_epoch();
				// 将时间戳转换为秒
				long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
				pool_.push(std::make_unique<SqlConnection>(con, timestamp));
				std::cout << "mysql pool init success, url is " << url_ << std::endl;
			}

			_check_thread = 	std::thread([this]() {
				int count = 0;
				while (!b_stop_) {
					if (count == 60)
					{
						checkConnectionPro();
						count = 0;
					}
					std::this_thread::sleep_for(std::chrono::seconds(1));
					count++;
				}
			});

			_check_thread.detach();
		}
		catch (sql::SQLException& e) {
			// 处理异常
			std::cout << "mysql pool init failed, error is " << e.what()<< std::endl;
		}
	}

	void checkConnection() {
		std::lock_guard<std::mutex> guard(mutex_);
		int poolsize = pool_.size();
		// 获取当前时间戳
		auto currentTime = std::chrono::system_clock::now().time_since_epoch();
		// 将时间戳转换为秒
		long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
		for (int i = 0; i < poolsize; i++) {
			auto con = std::move(pool_.front());
			pool_.pop();
			Defer defer([this, &con]() {
				pool_.push(std::move(con));
			});

			if (timestamp - con->_last_oper_time < 5) {
				continue;
			}
			
			try {
				std::unique_ptr<sql::Statement> stmt(con->_con->createStatement());
				stmt->executeQuery("SELECT 1");
				con->_last_oper_time = timestamp;
				//std::cout << "execute timer alive query , cur is " << timestamp << std::endl;
			}
			catch (sql::SQLException& e) {
				std::cout << "Error keeping connection alive: " << e.what() << std::endl;
				// 重新创建连接并替换旧的连接
				sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
				auto* newcon = driver->connect(url_, user_, pass_);
				newcon->setSchema(schema_);
				con->_con.reset(newcon);
				con->_last_oper_time = timestamp;
			}
		}
	}

	void checkConnectionPro()
	{
		//1.先读取目标处理数
		size_t targetCount;
		{
			std::lock_guard<std::mutex> guard(mutex_);
			targetCount = pool_.size();
		}
		//2.当前已经处理的数目
		size_t processedCount = 0;

		//3.时间戳
		auto now = std::chrono::system_clock::now().time_since_epoch();
		long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(now).count();

		while (processedCount < targetCount) {
			std::unique_ptr<SqlConnection> con;//连接
			{
				std::lock_guard<std::mutex> guard(mutex_);//把之前外部的加锁，粒度缩小为循环里加锁
				if (pool_.empty()) {
					break; // 如果池子为空，退出循环
					/*为啥会出现池子空呢，因为当processedCount<targetCount时都要轮询，有可能我们的
					targetCount再之后呢变小了，有可能pop池子里的连接多了导致pool_空了此时就不能走下面的逻辑了*/
				}
				con = std::move(pool_.front());
				pool_.pop();
			}

			bool healthy = true; // 连接是否健康

			//解锁后做检查/并且添加重连逻辑
			if (timestamp - con->_last_oper_time >= 5)
			{
				try
				{
					std::unique_ptr<sql::Statement> stmt(con->_con->createStatement());
					stmt->executeQuery("SELECT 1");
					con->_last_oper_time = timestamp;
				}
				catch(sql::SQLException& e)
				{
					std::cout << "Error keeping connection alive: " << e.what() << std::endl;
					healthy = false; // 连接不健康
					_fail_count++;
				}
				
				if(healthy)
				{
					std::lock_guard<std::mutex> guard(mutex_);
					pool_.push(std::move(con)); // 将连接放回池中
					cond_.notify_one(); // 通知等待的线程有连接可用
				}
				processedCount++; // 处理计数增加
			}
			while (_fail_count > 0)
			{
				//尝试重连
				auto b_res = reconnect(timestamp);
				if (b_res)
				{
					_fail_count--;
				}
				else
				{
					std::cout << "reconnect mysql failed" << std::endl;
					break;
				}
			}
		}
		
	}

	bool reconnect(long long timestemp)
	{
		try
		{
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();//获取mysql驱动实例
			auto* con = driver->connect(url_, user_, pass_);//创建连接
			con->setSchema(schema_);//设置数据库

			auto nowCon = std::make_unique<SqlConnection>(con, timestemp);//创建SqlConnection对象
			{
				std::lock_guard<std::mutex> guard(mutex_);
				pool_.push(std::move(nowCon)); // 将新连接放入连接池
			}
			std::cout << "reconnect mysql success, url is " << url_ << std::endl;
			return true;
		}
		catch (sql::SQLException)
		{
			std::cout << "reconnect mysql failed" << std::endl;
			return false;
		}
	}



	std::unique_ptr<SqlConnection> getConnection() {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this] { 
			if (b_stop_) {
				return true;
			}		
			return !pool_.empty(); });
		if (b_stop_) {
			return nullptr;
		}
		std::unique_ptr<SqlConnection> con(std::move(pool_.front()));
		pool_.pop();
		return con;
	}

	void returnConnection(std::unique_ptr<SqlConnection> con) {
		std::unique_lock<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}
		pool_.push(std::move(con));
		cond_.notify_one();
	}

	void Close() {
		b_stop_ = true;
		cond_.notify_all();
	}

	~MySqlPool() {
		std::unique_lock<std::mutex> lock(mutex_);
		while (!pool_.empty()) {
			pool_.pop();
		}
	}

private:
	std::string url_;
	std::string user_;
	std::string pass_;
	std::string schema_;
	int poolSize_;
	std::queue<std::unique_ptr<SqlConnection>> pool_;
	std::mutex mutex_;
	std::condition_variable cond_;
	std::atomic<bool> b_stop_;
	std::thread _check_thread;
	std::atomic<int> _fail_count;
};



class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	bool CheckEmail(const std::string& name, const std::string & email);
	bool UpdatePwd(const std::string& name, const std::string& newpwd);
	bool CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo);
	bool AddFriendApply(const int& from, const int& to);
	bool AuthFriendApply(const int& from, const int& to);
	bool AddFriend(const int& from, const int& to, std::string back_name);
	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string name);
	bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int offset, int limit );
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info);
private:
	std::unique_ptr<MySqlPool> pool_;
};


