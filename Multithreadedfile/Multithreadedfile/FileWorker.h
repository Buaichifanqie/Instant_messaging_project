#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include "MyQueue.h"

class CSession;
class FileTask
{
public:
	FileTask(std::shared_ptr<CSession> session, std::string name,
		int seq, int total_size, int trans_size, int last,
		std::string file_data) :m_session(session), m_seq(seq),
		m_name(name), m_total_size(total_size), m_trans_size(trans_size),
		m_last(last), m_file_data(file_data)
	{

	}


	~FileTask() {}
	std::shared_ptr<CSession> m_session;

	int m_seq;
	std::string m_name;
	int m_total_size;
	int m_trans_size;
	int m_last;
	std::string m_file_data;
};

class FileWorker
{
public:
	FileWorker();
	~FileWorker();
	void PostTask(std::shared_ptr<FileTask> task);
private:
	void task_callback(std::shared_ptr<FileTask> task);
	std::thread m_work_thread;
	MyQueue<std::shared_ptr<FileTask>>  m_task_que;
	std::atomic<bool> m_b_stop;
	std::mutex m_mtx;
	std::condition_variable m_cond;
};

