#pragma once

#define MAX_LENGTH  1024*4
//ͷ���ܳ���
#define HEAD_TOTAL_LEN 6
//ͷ��id����
#define HEAD_ID_LEN 2
//ͷ�����ݳ���
#define HEAD_DATA_LEN 4
// ���ܶ���������
#define MAX_RECVQUE  2000000
// ���Ͷ���������
#define MAX_SENDQUE 2000000

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json��������
	RPCFailed = 1002,  //RPC�������
	VarifyExpired = 1003, //��֤�����
	VarifyCodeErr = 1004, //��֤�����
	UserExist = 1005,       //�û��Ѿ�����
	PasswdErr = 1006,    //�������
	EmailNotMatch = 1007,  //���䲻ƥ��
	PasswdUpFailed = 1008,  //��������ʧ��
	PasswdInvalid = 1009,   //�������ʧ��
	TokenInvalid = 1010,   //TokenʧЧ
	UidInvalid = 1011,  //uid��Ч
};

// Defer��
class Defer {
public:
	// ����һ��lambda���ʽ���ߺ���ָ��
	Defer(std::function<void()> func) : m_func(func) {}

	// ����������ִ�д���ĺ���
	~Defer() {
		m_func();
	}

private:
	std::function<void()> m_func;
};

enum MSG_IDS {
	ID_TEST_MSG_REQ = 1001,       //������Ϣ
	ID_TEST_MSG_RSP = 1002,       //������Ϣ�ذ�
	ID_UPLOAD_FILE_REQ = 1003,    //�����ļ�����
	ID_UPLOAD_FILE_RSP = 1004,    //�����ļ��ظ�
};

//�߼�������
#define	LOGIC_WORKER_COUNT 4
//�ļ�������
#define FILE_WORKER_COUNT 4

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"