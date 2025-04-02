#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <mutex>
#include <memory>
#include "const.h"
#include "MsgNode.h"
#include "MyQueue.h"
using namespace std;


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class CServer;
class LogicSystem;

class CSession:public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& io_context,CServer* server);
	~CSession();
	tcp::socket& GetSocket();
	std::string& GetSessionId();
	void SetUserId(int uid);
	int GetUserId();
	void Start();
	void Send(char* msg, short max_length, short msgid);
	void Send(std::string msg, short msgid);
	void TryStartAsyncWrite();
	void Close();
	void AsycncReadBody(int length);
	void AsyncReadHead(int total_len);

private:
	void asyncReadFull(std::size_t maxLength, 
		std::function<void(const boost::system::error_code&, std::size_t)> handler);
	void asyncReadLen(std::size_t read_len, std::size_t total_len,
		std::function<void(const boost::system::error_code&, std::size_t)>handler);
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);
	//成员变量
	tcp::socket m_socket;
	std::string m_session_id;
	char m_data[MAX_LENGTH];
	CServer* m_server;
	bool m_b_close;
	MyQueue <std::shared_ptr<SendNode>> m_send_que; // 无锁队列，存储SendNode指针
	std::atomic<int> send_que_size_{ 0 }; // 原子计数器跟踪队列大小
	std::atomic<bool> is_writing_{ false }; // 原子标志，防止并发写
	std::shared_ptr<RecvNode> m_recv_msg_node;
	bool m_b_head_parse;
	std::shared_ptr<MsgNode> m_recv_head_node;
	int m_user_uid;
	std::mutex m_send_lock;
};

