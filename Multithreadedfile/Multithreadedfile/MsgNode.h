#pragma once
#include <string>
#include "const.h"
#include <iostream>
#include <boost/asio.hpp>
using namespace std;
using boost::asio::ip::tcp;

class MsgNode
{
public:
	MsgNode(int max_len) :m_total_len(max_len), m_cur_len(0)
	{
		m_data = new char[m_total_len + 1];
		m_data[m_total_len] = '\0';
	}

	~MsgNode()
	{
		std::cout << "destruct MsgNode" << std::endl;
		delete[] m_data;
	}

	void Clear()
	{
		::memset(m_data, 0, m_total_len);
		m_cur_len = 0;
	}

	int m_cur_len;
	int m_total_len;
	char* m_data;
};

//接收消息节点
class RecvNode :public MsgNode
{
	friend class LogicSystem;
public:
	RecvNode(int max_len, short msg_id) :MsgNode(max_len), m_msg_id(msg_id) {}
	short m_msg_id;
};

//发送消息节点
class SendNode :public MsgNode
{
	friend class LogicSystem;
public:
	SendNode(const char* msg, int max_len, short msg_id) :MsgNode(max_len), m_msg_id(msg_id)
	{
		memcpy(m_data, msg, max_len);
		m_cur_len = max_len;
	}
	short m_msg_id;
};