#include "MysqlDao.h"
#include "ConfigMgr.h"

MysqlPool::MysqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolsize)
	:m_url(url),m_user(user),m_pass(pass),m_schema(schema),m_poolSize(poolsize),m_b_stop(false)
{
	try {
		for (int i = 0; i < m_poolSize; ++i)
		{
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
			auto* con = driver->connect(m_url, m_user, m_pass);
			con->setSchema(m_schema);
			//获取当前时间戳
			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			//转成秒
			long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
			m_pool.push(std::make_unique<SqlConnection>(con, timestamp));
		}

		//启线程
		m_check_thread = std::thread([this]() {
			while (!m_b_stop)
			{
				checkConnection();
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			});
		m_check_thread.detach();
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what() << std::endl;
		std::cerr << "MySQL error code: " << e.getErrorCode() << std::endl;
		std::cerr << "SQLState: " << e.getSQLState() << std::endl;
	}
}

void MysqlPool::checkConnection() {
	std::lock_guard<std::mutex> guard(m_mutex);
	int poolsize = m_pool.size();
	auto currentTime = std::chrono::system_clock::now().time_since_epoch();
	long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

	for (int i = 0; i < poolsize; i++) {
		auto con = std::move(m_pool.front());
		m_pool.pop();
		Defer defer([this, &con]() {
			m_pool.push(std::move(con));
			});

		if (timestamp - con->m_last_oper_time < 5) {
			continue;
		}

		try {
			if (!con->m_con || con->m_con->isClosed()) {
				throw sql::SQLException("Connection is closed or invalid");
			}

			std::unique_ptr<sql::Statement> stmt(con->m_con->createStatement());
			stmt->executeQuery("SELECT 1");
			con->m_last_oper_time = timestamp;
			std::cout << "execute timer alive query, cur is " << timestamp << std::endl;
		}
		catch (sql::SQLException& e) {
			std::cout << "Error keeping connection alive: " << e.what() << std::endl;
			try {
				sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
				auto* newcon = driver->connect(m_url, m_user, m_pass);
				if (newcon) {
					newcon->setSchema(m_schema);
					con->m_con.reset(newcon);
					con->m_last_oper_time = timestamp;
				}
				else {
					std::cerr << "Failed to reconnect to MySQL!" << std::endl;
				}
			}
			catch (sql::SQLException& e) {
				std::cerr << "SQLException during reconnect: " << e.what() << std::endl;
			}
		}
	}
}


std::unique_ptr<SqlConnection> MysqlPool::getConnection()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cond.wait(lock,[this] {
		if (m_b_stop)
		{
			return true;
		}
		return !m_pool.empty();
	});
	if (m_b_stop)
	{
		return nullptr;
	}
	std::unique_ptr<SqlConnection> con(std::move(m_pool.front()));
	m_pool.pop();
	return con;
}

void MysqlPool::returnConnection(std::unique_ptr<SqlConnection> con)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_b_stop)
	{
		return;
	}
	m_pool.push(std::move(con));
	m_cond.notify_one();
	//std::unique_lock<std::mutex> lock(m_mutex);
	//if (!m_b_stop) {
	//	m_pool.push(std::move(con));
	//	mm_cond.notify_one();
	//}
}

void MysqlPool::Close()
{
	m_b_stop = true;
	m_cond.notify_all();
}

MysqlPool::~MysqlPool()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (!m_pool.empty())
	{
		m_pool.pop();
	}
}

MysqlDao::MysqlDao()
{
	auto& cfg = ConfigMgr::Inst();
	const auto& host = cfg["Mysql"]["Host"];
	const auto& port = cfg["Mysql"]["Port"];
	const auto& pwd = cfg["Mysql"]["Passwd"];
	const auto& schema = cfg["Mysql"]["Schema"];
	const auto& user = cfg["Mysql"]["User"];
	m_pool.reset(new MysqlPool(host + ":" + port, user, pwd, schema, 5));
}

MysqlDao::~MysqlDao() {
	m_pool->Close();
}

int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd) {
	auto con = m_pool->getConnection();
	if (con == nullptr || con->m_con == nullptr) {
		std::cerr << "Failed to get MySQL connection!" << std::endl;
		return -1;
	}

	try {
		// 准备调用存储过程
		std::unique_ptr<sql::PreparedStatement> stmt(con->m_con->prepareStatement("CALL reg_user(?,?,?,@result)"));
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);

		// 执行存储过程
		stmt->execute();
		std::cout << "Stored procedure executed successfully." << std::endl;

		// 获取输出参数
		std::unique_ptr<sql::Statement> stmtResult(con->m_con->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));

		if (res->next()) {
			int result = res->getInt("result");
			std::cout << "Result: " << result << std::endl;
			m_pool->returnConnection(std::move(con));

			// 根据返回值处理逻辑
			if (result == -2) {
				std::cerr << "Username already exists!" << std::endl;
				return -2; // 用户名已存在
			}
			else if (result == -3) {
				std::cerr << "Email already exists!" << std::endl;
				return -3; // 邮箱已存在
			}
			else if (result == -1) {
				std::cerr << "An error occurred in the stored procedure!" << std::endl;
				return -1; // 存储过程发生异常
			}
			else {
				return result; // 返回新用户的 uid
			}
		}
		else {
			std::cerr << "No result from SELECT @result!" << std::endl;
			m_pool->returnConnection(std::move(con));
			return -1;
		}
	}
	catch (sql::SQLException& e) {
		m_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return -1;
	}
}

//bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
//	auto con = m_pool->getConnection();
//	try {
//		if (con == nullptr) {
//			return false;
//		}
//
//		// 准备查询语句
//		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("SELECT email FROM user WHERE name = ?"));
//
//		// 绑定参数
//		pstmt->setString(1, name);
//
//		// 执行查询
//		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
//
//		// 遍历结果集
//		while (res->next()) {
//			std::cout << "Check Email: " << res->getString("email") << std::endl;
//			if (email != res->getString("email")) {
//				m_pool->returnConnection(std::move(con));
//				return false;
//			}
//			m_pool->returnConnection(std::move(con));
//			return true;
//		}
//		return false;
//	}
//	catch (sql::SQLException& e) {
//		m_pool->returnConnection(std::move(con));
//		std::cerr << "SQLException: " << e.what();
//		std::cerr << " (MySQL error code: " << e.getErrorCode();
//		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
//		return false;
//	}
//}

//bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd) {
//	auto con = m_pool->getConnection();
//	try {
//		if (con == nullptr) {
//			return false;
//		}
//
//		// 准备查询语句
//		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
//
//		// 绑定参数
//		pstmt->setString(2, name);
//		pstmt->setString(1, newpwd);
//
//		// 执行更新
//		int updateCount = pstmt->executeUpdate();
//
//		std::cout << "Updated rows: " << updateCount << std::endl;
//		m_pool->returnConnection(std::move(con));
//		return true;
//	}
//	catch (sql::SQLException& e) {
//		m_pool->returnConnection(std::move(con));
//		std::cerr << "SQLException: " << e.what();
//		std::cerr << " (MySQL error code: " << e.getErrorCode();
//		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
//		return false;
//	}
//}

//bool MysqlDao::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo) {
//	auto con = m_pool->getConnection();
//	if (con == nullptr) {
//		return false;
//	}
//
//	Defer defer([this, &con]() {
//		m_pool->returnConnection(std::move(con));
//		});
//
//	try {
//
//
//		// 准备SQL语句
//		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("SELECT * FROM user WHERE email = ?"));
//		pstmt->setString(1, email); // 将username替换为你要查询的用户名
//
//		// 执行查询
//		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
//		std::string origin_pwd = "";
//		// 遍历结果集
//		while (res->next()) {
//			origin_pwd = res->getString("pwd");
//			// 输出查询到的密码
//			std::cout << "Password: " << origin_pwd << std::endl;
//			break;
//		}
//
//		if (pwd != origin_pwd) {
//			return false;
//		}
//		userInfo.name = res->getString("name");
//		userInfo.email = res->getString("email");
//		userInfo.uid = res->getInt("uid");
//		userInfo.pwd = origin_pwd;
//		return true;
//	}
//	catch (sql::SQLException& e) {
//		std::cerr << "SQLException: " << e.what();
//		std::cerr << " (MySQL error code: " << e.getErrorCode();
//		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
//		return false;
//	}
//}

SqlConnection::SqlConnection(sql::Connection* con, int64_t lasttime)
	:m_con(con),m_last_oper_time(lasttime)
{

}
