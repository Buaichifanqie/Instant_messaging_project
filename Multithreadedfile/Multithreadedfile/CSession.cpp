#include "CSession.h"
#include "CServer.h"
#include <iostream>
#include <sstream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "LogicSystem.h"
CSession::CSession(boost::asio::io_context& io_context, CServer* server) :
	m_socket(io_context),
	m_server(server),
	m_b_close(false),
	m_b_head_parse(false),
	m_user_uid(0)
{
	//生成uuid
	boost::uuids::uuid a_uuid = boost::uuids::random_generator() ();
	m_session_id = boost::uuids::to_string(a_uuid);
	//创建接收消息的节点
	m_recv_head_node = make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession()
{
	std::cout << "~CSession destruct" << endl;
}

tcp::socket& CSession::GetSocket()
{
	return m_socket;
}

std::string& CSession::GetSessionId()
{
	return m_session_id;
}

void CSession::SetUserId(int uid)
{
	m_user_uid = uid;
}

int CSession::GetUserId()
{
	return m_user_uid;
}

void CSession::Start()
{
	AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::Send(std::string msg, short msgid) {
    int current_size;
    do {
        current_size = send_que_size_.load(std::memory_order_relaxed);
        if (current_size >= MAX_SENDQUE) {
            std::cout << "Queue full" << std::endl;
            return;
        }
    } while (!send_que_size_.compare_exchange_weak(
        current_size, current_size + 1,
        std::memory_order_release,
        std::memory_order_relaxed));

    std::shared_ptr<SendNode> new_node = make_shared<SendNode>(msg.c_str(), msg.length(), msgid);
    m_send_que.push(new_node);
    TryStartAsyncWrite();
}

void CSession::Send(char* msg, short max_length, short msgid) {
    int current_size;
    do {
        current_size = send_que_size_.load(std::memory_order_relaxed);
        if (current_size >= MAX_SENDQUE) {
            std::cout << "Queue full" << std::endl;
            return;
        }
    } while (!send_que_size_.compare_exchange_weak(
        current_size, current_size + 1,
        std::memory_order_release,
        std::memory_order_relaxed));

    std::shared_ptr<SendNode> new_node = make_shared<SendNode>(msg, max_length, msgid);
    m_send_que.push(new_node);
    TryStartAsyncWrite();
}

void CSession::TryStartAsyncWrite() {
    bool expected = false;
    if (!is_writing_.compare_exchange_strong(
        expected, true,
        std::memory_order_acquire,
        std::memory_order_relaxed)) {
        return;
    }
    auto node_ptr = m_send_que.pop();
    if (!node_ptr) {
        is_writing_.store(false, std::memory_order_release);
        return;
    }
    send_que_size_.fetch_sub(1, std::memory_order_relaxed);
    // 从 std::unique_ptr<std::shared_ptr<SendNode>> 中获取 std::shared_ptr<SendNode>
    std::shared_ptr<SendNode> shared_node = *node_ptr;
    // 从 std::shared_ptr<SendNode> 中获取 SendNode*
    SendNode* node = shared_node.get();
    boost::asio::async_write(m_socket, boost::asio::buffer(node->m_data, node->m_total_len),
        [this, shared_node](boost::system::error_code ec, std::size_t /*length*/) {
            if (ec) {
                Close(); // 错误处理
                return;
            }
            is_writing_.store(false, std::memory_order_release);
            TryStartAsyncWrite(); // 处理下一条消息
        });
}

//关闭会话
void CSession::Close()
{
    m_socket.close();
    m_b_close = true;
}

//异步读消息体
void CSession::AsycncReadBody(int total_len)
{
    auto self = shared_from_this();
    asyncReadFull(total_len, [self, this, total_len](const boost::system::error_code&ec,std::size_t bytes_transfered) {
        try
        {
            if (ec)
            {
                std::cout << "handle read failed, error is " << ec.what() << endl;
                Close();
                m_server->ClearSession(m_session_id);
                return;
            }
            if (bytes_transfered < total_len)
            {
                std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
                    << total_len << "]" << endl;
                Close();
                m_server->ClearSession(m_session_id);
                return;
            }
            memcpy(m_recv_msg_node->m_data, m_data, bytes_transfered);
            m_recv_msg_node->m_cur_len += bytes_transfered;
            m_recv_msg_node->m_data[m_recv_msg_node->m_total_len] = '\0';
            
            LogicSystem::GetInstance()->PostMsgToQue(make_shared<LogicNode>(shared_from_this(), m_recv_msg_node));
            AsyncReadHead(HEAD_TOTAL_LEN);
        } 
        catch (std::exception& e)
        {
            std::cout << "Exception code is " << e.what() << endl;
        }
    });
}


//异步读取消息头
void CSession::AsyncReadHead(int total_len)
{
    auto self = shared_from_this();
    asyncReadFull(HEAD_TOTAL_LEN, [self, this](const boost::system::error_code& ec,std::size_t bytes_transfered) {
        try
        {
            if (ec)
            {
                std::cout << "handle read failed, error is " << ec.what() << endl;
                Close();
                m_server->ClearSession(m_session_id);
                return;
            }
            if (bytes_transfered < HEAD_TOTAL_LEN)
            {
                std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
                    << HEAD_TOTAL_LEN << "]" << endl;
                Close();
                m_server->ClearSession(m_session_id);
                return;
            }
            m_recv_head_node->Clear();
            memcpy(m_recv_head_node->m_data, m_data, bytes_transfered);

            short msg_id = 0;
            memcpy(&msg_id, m_recv_head_node->m_data, HEAD_ID_LEN);
            msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
            std::cout << "msg_id is " << msg_id << endl;
            if (msg_id > MAX_LENGTH)
            {
                std::cout << "invalid msg_id is " << msg_id << endl;
                m_server->ClearSession(m_session_id);
                return;
            }

            int msg_len = 0;
            memcpy(&msg_len, m_recv_head_node->m_data + HEAD_ID_LEN, HEAD_DATA_LEN);
            msg_len = boost::asio::detail::socket_ops::network_to_host_long(msg_len);

            // 验证消息长度有效性
            if (msg_len > MAX_LENGTH) {
                std::cout << "invalid data length is " << msg_len << endl;
                m_server->ClearSession(m_session_id);
                return;
            }

            //创建消息接收节点
            m_recv_msg_node = make_shared<RecvNode>(msg_len, msg_id);
            AsycncReadBody(msg_len);
        }
        catch (std::exception& e) 
        {
            std::cout << "Exception code is " << e.what() << endl;
        }
    });
}

//读取指定长度的完整数据
void CSession::asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
    //::memset 明确调用的是全局命名空间中的 memset
    ::memset(m_data, 0, MAX_LENGTH);
    asyncReadLen(0, maxLength, handler);
}

//分段读取辅助函数
void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len, std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
    auto self = shared_from_this();
    m_socket.async_read_some(boost::asio::buffer(m_data + read_len, total_len - read_len),
        [read_len,total_len,handler,self](const boost::system::error_code& ec,std::size_t bytesTransfered) {
            if (ec)
            {
                // 出现错误，调用回调函数
                handler(ec, read_len + bytesTransfered);
                return;
            }
            if (read_len + bytesTransfered >= total_len)
            {
                handler(ec, read_len + bytesTransfered);
                return;
            }
            self->asyncReadLen(read_len + bytesTransfered, total_len, handler);
        });
}

void CSession::HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self) {
    try {
        if (!error) {
            std::lock_guard<std::mutex> lock(m_send_lock);
            m_send_que.pop(); // 移除已发送消息

            // 继续发送队列中的下一条消息
            if (!m_send_que.empty()) {
                auto& msgnode = *m_send_que.front();
                boost::asio::async_write(m_socket,
                    boost::asio::buffer(msgnode->m_data, msgnode->m_total_len),
                    std::bind(&CSession::HandleWrite, this,
                        std::placeholders::_1,
                        shared_self));
            }
        }
        else {
            std::cout << "handle write failed, error is " << error.what() << endl;
            Close();
            m_server->ClearSession(m_session_id);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception code : " << e.what() << endl;
    }
}

LogicNode::LogicNode(shared_ptr<CSession>  session,
    shared_ptr<RecvNode> recvnode) :m_session(session), m_recvnode(recvnode) {

}





