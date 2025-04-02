#include "FileSystem.h"
#include "const.h"

FileSystem::~FileSystem()
{

}


FileSystem::FileSystem()
{
	for (int i = 0; i < FILE_WORKER_COUNT; i++)
	{
		m_file_workers.push_back(std::make_shared<FileWorker>());
	}
}


void FileSystem::PostMsgTpQue(shared_ptr<FileTask> msg, int index)
{
	m_file_workers[index]->PostTask(msg);
}
