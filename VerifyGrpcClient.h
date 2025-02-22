#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

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
		
		Status status = m_stub->GetVarifyCode(&context, request, &reply);
		if (status.ok())
		{
			return reply;
		}
		else
		{
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}
private:
	VerifyGrpcClient()
	{
		std::shared_ptr<Channel> m_channel = grpc::CreateChannel("0.0.0.0:50051", 
			grpc::InsecureChannelCredentials());
		m_stub = VarifyService::NewStub(m_channel);//m_channel相当于电话 m_stub信使需要电话
	}
	std::unique_ptr<VarifyService::Stub> m_stub;//相当一个媒介只有通过它才能通信
};

