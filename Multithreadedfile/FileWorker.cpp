#include "FileWorker.h"
#include "CSession.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "base64.h"
#include "ConfigMgr.h"

FileWorker::FileWorker():_b_stop(false)
{
	_work_thread = std::thread([this]() {
		while (!_b_stop) {
			std::unique_lock<std::mutex> lock(_mtx);
			_cv.wait(lock, [this]() {
				if (_b_stop) {
					return true;
				}

				if (_task_que.empty()) {
					return false;
				}

				return true;
			});

			if (_b_stop) {
				break;
			}

			auto task = _task_que.front();
			_task_que.pop();
			task_callback(task);
		}
		
	});
}

FileWorker::~FileWorker()
{
	_b_stop = true;
	_cv.notify_one();
	_work_thread.join();
}

void FileWorker::PostTask(std::shared_ptr<FileTask> task)
{
	{
		std::lock_guard<std::mutex> lock(_mtx);
		_task_que.push(task);
	}

	_cv.notify_one();
}

void FileWorker::task_callback(std::shared_ptr<FileTask> task)
{

	// ����
	std::string decoded = base64_decode(task->_file_data);

	auto file_path = ConfigMgr::Inst().GetFileOutPath();
	auto file_path_str = (file_path / task->_name).string();
	auto last = task->_last;
	//std::cout << "file_path_str is " << file_path_str << std::endl;
	std::ofstream outfile;
	//��һ����
	if (task->_seq == 1) {
		// ���ļ��������������գ��������򴴽�
		outfile.open(file_path_str, std::ios::binary | std::ios::trunc);
	}
	else {
		// ����Ϊ�ļ�
		outfile.open(file_path_str, std::ios::binary | std::ios::app);
	}


	if (!outfile) {
		std::cerr << "�޷����ļ�����д�롣" << std::endl;
		return ;
	}

	outfile.write(decoded.data(), decoded.size());
	if (!outfile) {
			std::cerr << "д���ļ�ʧ�ܡ�" << std::endl;
			return ;
	}

	outfile.close();
	if (last) {
		std::cout << "�ļ��ѳɹ�����Ϊ: " << task->_name << std::endl;
	}

}
