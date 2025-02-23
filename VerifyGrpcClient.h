#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
#include <queue>


using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool
{
public:
	RPConPool(size_t poolsize, std::string host, std::string port);
	~RPConPool();
	void Close();

	// 取信使
	std::unique_ptr<VarifyService::Stub> getConnection();

	// 回收信使
	void returnConnection(std::unique_ptr<VarifyService::Stub> stub);

private:
	std::atomic<bool> m_b_stop;
	size_t m_poolSize;
	std::string m_host;
	std::string m_port;
	std::queue<std::unique_ptr<VarifyService::Stub>> m_connections;
	std::condition_variable m_cond;
	std::mutex m_mutex;
};

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	GetVarifyRsp GetVarifyCode(std::string email)
	{
		ClientContext context;
		GetVarifyRsp reply;
		GetVarifyReq request;
		request.set_email(email);
		auto m_stub = m_pool->getConnection();

		Status status = m_stub->GetVarifyCode(&context, request, &reply);
		if (status.ok())
		{
			m_pool->returnConnection(std::move(m_stub));
			return reply;
		}
		else
		{
			m_pool->returnConnection(std::move(m_stub));
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}
private:
	VerifyGrpcClient();
	std::unique_ptr<RPConPool> m_pool;
};

