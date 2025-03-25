#include "logicmgr.h"

LogicWorker::LogicWorker(QObject *parent)
{
    InitHandlers();
}

void LogicWorker::InitHandlers()
{
    //注册测试消息
    m_handlers[ID_TEXT_MSG_REQ]=[this](QJsonObject obj)
    {
        // 解析错误码
        auto err=obj["error"].toInt();
        if(err!=RSP_SUCCESS)
        {
            qDebug() << "test msg rsp err is " << err;
            return;
        }
        // 处理成功响应
        auto data=obj["data"].toString();
        qDebug() << "recv data is " << data;
    };

    //注册文件上传消息处理器
    m_handlers[ID_UPLOAD_FILE_RSP]=[this](QJsonObject obj)
    {
        auto err=obj["error"].toInt();
        if(err != RSP_SUCCESS){
            qDebug() << "File upload error: " << err;
            return;
        }

        // 解析文件传输信息
        auto name = obj["name"].toString();
        auto total_size = obj["total_size"].toInt();
        auto trans_size = obj["trans_size"].toInt();
        qDebug() << "recv : " << name << " file trans_size is " << trans_size;

        // 发送传输进度信号
        emit sig_trans_size(trans_size);
    };
}

void LogicWorker::slot_logic_process(quint16 msgid, QJsonObject obj)
{
    //检查是否有对应的处理器
    if(!m_handlers.contains(msgid)){
        qDebug() << "Unhandled message ID: " << msgid;
        return;
    }

    // 调用注册的处理器函数
    m_handlers[msgid](obj);
}

Logicmgr::~Logicmgr()
{
    m_work_thread->quit();
    m_work_thread->wait();
    m_work_thread->deleteLater();
    m_worker->deleteLater();
}

Logicmgr::Logicmgr()
{
    m_worker=new LogicWorker();
    m_work_thread=new QThread(this);
    //worker移动到子线程处理消息
    m_worker->moveToThread(m_work_thread);
    // 建立信号槽连接
    connect(this, &Logicmgr::sig_logic_process, m_worker,
            &LogicWorker::slot_logic_process);
    connect(m_worker, &LogicWorker::sig_trans_size,
            this, &Logicmgr::sig_trans_size);
    //启动线程
    m_work_thread->start();

}
