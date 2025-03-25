#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>

//定义Tcp包头长度
#define TCP_HEAD_LEN 6
//Tcp ID长度
#define TCP_ID_LEN 2
//tcp 长度字段长度
#define TCP_LEN_LEN 4
//最大文件长度
#define MAX_FILE_LEN 2048

enum MsgID
{
    ID_TEXT_MSG_REQ=1001, //测试消息
    ID_TEXT_MSG_RES=1002, //测试消息回包
    ID_UPLOAD_FILE_REQ=1003, //发送文件请求
    ID_UPLOAD_FILE_RSP=1004, //发送文件回复
};


enum RspErr
{
    RSP_SUCCESS=0, //正常回包
    RSP_INVALID_ID=1,//消息id错误
};

extern QString generateUniqueId();


#endif // GLOBAL_H
