#pragma once
#include "const.h"
#include <thread>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/statement.h>

 
class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t lasttime);
	std::unique_ptr<sql::Connection> m_con;
	int64_t m_last_oper_time;
};

class MysqlPool
{
public:
	MysqlPool(const std::string& url, const std::string& user, const std::string& pass,
		const std::string& schema, int poolsize);
	void checkConnection();
	std::unique_ptr<SqlConnection> getConnection();
	void returnConnection(std::unique_ptr<SqlConnection> con);
	void Close();
	~MysqlPool();
private:
	std::string m_url;
	std::string m_user;
	std::string m_pass;
	std::string m_schema;
	int m_poolSize;
	std::queue<std::unique_ptr<SqlConnection>> m_pool;
	std::mutex m_mutex;
	std::condition_variable m_cond;
	std::atomic<bool> m_b_stop;
	std::thread m_check_thread;
};

struct UserInfo {
	std::string name;
	std::string pwd;
	int uid;
	std::string email;
};

class MysqlDao {
public:
	MysqlDao();
	~MysqlDao();
	int	RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	/*
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& newpwd);
	bool CheckPwd(const std::string& name, const std::string& pwe,const UserInfo& userinfo);
	*/
private:
	std::unique_ptr<MysqlPool>m_pool;
};