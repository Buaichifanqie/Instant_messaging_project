#pragma once
#include <vector>
#include <boost/asio.hpp>
#include "Singleton.h"
//继承自 Singleton 模板类，确保该类只能有一个实例
class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
	friend Singleton<AsioIOServicePool>;//声明友元
public:
	using IOService = boost::asio::io_context;// IO 服务类型
	using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;// 工作守护程序类型，用于保持 io_context 运行
	using WorkPtr = std::unique_ptr<Work>;// 工作守护程序的智能指针类型

	~AsioIOServicePool();

	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
	
	//我们采用轮询的方式返回一个 io_context 对象，供用户使用
	boost::asio::io_context& GetIOService();

	// 停止所有 io_context 和线程，清理资源
	void Stop();

private:
	// 构造函数，初始化 IO 服务池，默认大小为硬件支持的并发线程数
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());
	//存储io_context对象
	std::vector<IOService> m_ioServices;
	//存储工作线程 用于运行io_context;
	std::vector<WorkPtr> m_works;
	// 存储一组工作线程，用于运行 io_context
	std::vector<std::thread> m_threads;
	// 轮询索引，用于选择下一个 io_context
	std::size_t m_nextIOService;
};

