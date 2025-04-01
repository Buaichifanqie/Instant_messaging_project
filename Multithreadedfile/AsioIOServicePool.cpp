#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(std::size_t size) :m_ioServices(size), m_works(size), m_nextIOService(0)
{
	for (std::size_t i = 0; i < size; ++i)
	{
		//// Ϊÿ�� io_context ����һ�������ػ�����
		m_works[i] = std::make_unique<Work>(boost::asio::make_work_guard(m_ioServices[i]));
	}
	// ������� io_context��Ϊÿ�� io_context ����һ���̣߳�����ÿ���߳������� io_context
	for (std::size_t i = 0; i < m_ioServices.size(); ++i)
	{
		m_threads.emplace_back([this,i]() {
			m_ioServices[i].run();
		});
	}
}

AsioIOServicePool::~AsioIOServicePool() {
	std::cout << "AsioIOServicePool destruct" << endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	auto& service = m_ioServices[m_nextIOService++];
	if (m_nextIOService == m_ioServices.size())
	{
		m_nextIOService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop()
{
	for (auto& ioService : m_ioServices)
	{
		ioService.stop();
	}

	for (auto& work : m_works)
	{
		work.reset();
	}
	for (auto& thread : m_threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
}

