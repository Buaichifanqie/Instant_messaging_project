#include "FileWorker.h"
#include "CSession.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "base64.h"
#include "ConfigMgr.h"
FileWorker::FileWorker():m_b_stop(false)
{
	m_work_thread = std::thread([this]()
		{
			while (!m_b_stop)
			{
				std::unique_lock<std::mutex> lock(m_mtx);
				m_cond.wait(lock, [this]()
					{
						if (m_b_stop)
						{
							return true;
						}
						if (m_task_que.empty())
						{
							return false;
						}
						return true;
					});
				if (m_b_stop)
				{
					return;
				}
				auto task1 = m_task_que.front();
				std::shared_ptr<FileTask> task = *task1.release();
				m_task_que.pop();
				task_callback(task);
			}
		});
}

FileWorker::~FileWorker()
{
	m_b_stop = true;
	m_cond.notify_one();
	m_work_thread.join();
}

void FileWorker::PostTask(std::shared_ptr<FileTask> task)
{
	m_task_que.push(task);
	m_cond.notify_one();
}

void FileWorker::task_callback(std::shared_ptr<FileTask> task)
{
	//����
	std::string decoded = base64_decode(task->m_file_data);
	
	auto file_path = ConfigMgr::Inst().GetFileOutPath();
	auto file_path_str = (file_path / task->m_name).string();
	auto last = task->m_last;

	std::ofstream outfile;
	//��һ����
	if (task->m_seq == 1)
	{
		//���ļ� ������ھ���գ������ھʹ���
		outfile.open(file_path_str, std::ios::binary | std::ios::trunc);
	}
	else
	{
		//�����ļ�
		outfile.open(file_path_str, std::ios::binary | std::ios::app);
	}

	if (!outfile) {
		std::cerr << "�޷����ļ�����д�롣" << std::endl;
		return;
	}

	outfile.write(decoded.data(), decoded.size());
	if (!outfile)
	{
		std::cerr << "д���ļ�ʧ�ܡ�" << std::endl;
		return;
	}

	outfile.close();
	if (last)
	{
		std::cout<< "�ļ��ѳɹ�����Ϊ: " << task->m_name << std::endl;
	}
}	
