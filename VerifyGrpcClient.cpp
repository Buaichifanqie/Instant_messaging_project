#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"


RPConPool::RPConPool(size_t poolsize, std::string host, std::string port):m_poolSize(poolsize),m_host(host)
,m_port(port),m_b_stop(false)
{
	for (size_t i = 0; i < m_poolSize; i++)
	{
		std::shared_ptr<Channel> m_channel = grpc::CreateChannel(host+":"+port,
			grpc::InsecureChannelCredentials());
		m_connections.push(VarifyService::NewStub(m_channel));//m_channel相当于电话 m_stub信使需要电话
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	Close();
	while (!m_connections.empty())
	{
		m_connections.pop();
	}
}

void RPConPool::Close()
{
	m_b_stop = true;
	m_cond.notify_all();
}

std::unique_ptr<VarifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cond.wait(lock, [this]() {
		if (m_b_stop)
		{
			return true;
		}
		return !m_connections.empty();
	});

	if (m_b_stop)
	{
		return nullptr;
	}
	auto context = std::move(m_connections.front());
	m_connections.pop();
	return context;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_b_stop)
	{
		return;
	}
	m_connections.push(std::move(context));
	m_cond.notify_one();
}

VerifyGrpcClient::VerifyGrpcClient()
{
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
	m_pool.reset(new RPConPool(5, host, port));
}



	