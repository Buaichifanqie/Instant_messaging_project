#pragma once

#define MAX_LENGTH  1024*4
//头部总长度
#define HEAD_TOTAL_LEN 6
//头部id长度
#define HEAD_ID_LEN 2
//头部数据长度
#define HEAD_DATA_LEN 4
// 接受队列最大个数
#define MAX_RECVQUE  2000000
// 发送队列最大个数
#define MAX_SENDQUE 2000000

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json解析错误
	RPCFailed = 1002,  //RPC请求错误
	VarifyExpired = 1003, //验证码过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码更新失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效
};

// Defer类
class Defer {
public:
	// 接受一个lambda表达式或者函数指针
	Defer(std::function<void()> func) : m_func(func) {}

	// 析构函数中执行传入的函数
	~Defer() {
		m_func();
	}

private:
	std::function<void()> m_func;
};

enum MSG_IDS {
	ID_TEST_MSG_REQ = 1001,       //测试消息
	ID_TEST_MSG_RSP = 1002,       //测试消息回包
	ID_UPLOAD_FILE_REQ = 1003,    //发送文件请求
	ID_UPLOAD_FILE_RSP = 1004,    //发送文件回复
};

//逻辑工作者
#define	LOGIC_WORKER_COUNT 4
//文件工作者
#define FILE_WORKER_COUNT 4

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"