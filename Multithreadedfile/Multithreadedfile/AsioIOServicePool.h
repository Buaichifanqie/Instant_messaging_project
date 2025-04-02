#pragma once
#include <vector>
#include <boost/asio.hpp>
#include "Singleton.h"
//�̳��� Singleton ģ���࣬ȷ������ֻ����һ��ʵ��
class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
	friend Singleton<AsioIOServicePool>;//������Ԫ
public:
	using IOService = boost::asio::io_context;// IO ��������
	using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;// �����ػ��������ͣ����ڱ��� io_context ����
	using WorkPtr = std::unique_ptr<Work>;// �����ػ����������ָ������

	~AsioIOServicePool();

	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
	
	//���ǲ�����ѯ�ķ�ʽ����һ�� io_context ���󣬹��û�ʹ��
	boost::asio::io_context& GetIOService();

	// ֹͣ���� io_context ���̣߳�������Դ
	void Stop();

private:
	// ���캯������ʼ�� IO ����أ�Ĭ�ϴ�СΪӲ��֧�ֵĲ����߳���
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());
	//�洢io_context����
	std::vector<IOService> m_ioServices;
	//�洢�����߳� ��������io_context;
	std::vector<WorkPtr> m_works;
	// �洢һ�鹤���̣߳��������� io_context
	std::vector<std::thread> m_threads;
	// ��ѯ����������ѡ����һ�� io_context
	std::size_t m_nextIOService;
};

