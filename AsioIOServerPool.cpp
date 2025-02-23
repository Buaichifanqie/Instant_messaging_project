#include "AsioIOServerPool.h"
#include <iostream>
using namespace std;
AsioIOServicePool::AsioIOServicePool(std::size_t size)
    : m_ioServices(size), m_works(size), m_nextIOService(0)
{
    for (std::size_t i = 0; i < size; ++i) {
        m_works[i] = std::make_unique<Work>(boost::asio::make_work_guard(m_ioServices[i]));
        /*
        _works【i】 = std::unique_ptr<Work>(new Work(_ioServices【i】.get_executor()));
        */
    }
    // 遍历多个 io_context，创建多个线程，每个线程内部启动 io_context
    for (std::size_t i = 0; i < m_ioServices.size(); ++i) {
        m_threads.emplace_back([this, i]() {
            m_ioServices[i].run();
            });
    }
}
AsioIOServicePool::~AsioIOServicePool() {
    Stop();
    std::cout << "AsioIOServicePool destruct" << endl;
}
boost::asio::io_context& AsioIOServicePool::GetIOService() {
    auto& service = m_ioServices[m_nextIOService++];
    if (m_nextIOService == m_ioServices.size()) {
        m_nextIOService = 0;
    }
    return service;
}
void AsioIOServicePool::Stop() {
    // 停止所有 io_context 服务
    for (auto& ioService : m_ioServices) {
        ioService.stop();
    }

    // 释放所有 work_guard，允许 io_context 在没有任务时退出
    for (auto& work : m_works) {
        work.reset();
    }

    // 等待所有线程结束
    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}