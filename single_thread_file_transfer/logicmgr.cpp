#include "logicmgr.h"
#include "global.h"
#include <QDebug>
#include "tcpclient.h"
#include <QDateTime>

LogicMgr::~LogicMgr()
{
    _work_thread->quit();
    _work_thread->wait();
    _work_thread->deleteLater();
    _worker->deleteLater();
}

LogicMgr::LogicMgr():QObject(nullptr)
{
    _worker = new LogicWorker();
    _work_thread = new QThread(this);
    //worker移动到子线程处理消息
    _worker->moveToThread(_work_thread);

    //连接信号和槽函数
    connect(this, &LogicMgr::sig_logic_process, _worker,
            &LogicWorker::slot_logic_process);
    connect(_worker, &LogicWorker::sig_trans_size,
            this, &LogicMgr::sig_trans_size);
    //启动线程
    _work_thread->start();
}

LogicWorker::LogicWorker(QObject* parent):QObject(parent)
{
    InitHandlers();
}

void LogicWorker::InitHandlers()
{
    //注册测试消息
    _handlers[ID_TEST_MSG_RSP] = [this](QJsonObject obj){
        auto err = obj["error"].toInt();
        if(err != RSP_SUCCESS){
            qDebug() << "test msg rsp err is " << err;
            return;
        }

        auto data = obj["data"].toString();
        qDebug() << "recv data is " << data;
    };

    //注册上传消息
    _handlers[ID_UPLOAD_FILE_RSP] = [this](QJsonObject obj){
        auto err = obj["error"].toInt();
        if(err != RSP_SUCCESS){
            qDebug() << "upload msg rsp err is " << err;
            return;
        }

        auto name = obj["name"].toString();
        auto total_size = obj["total_size"].toInt();
        auto trans_size = obj["trans_size"].toInt();

        qDebug() << "recv : " << name << " file trans_size is " << trans_size;
        emit sig_trans_size(trans_size);
        qDebug() << "Emitting sig_trans_size with value:" << trans_size;
    };
}

void LogicWorker::slot_logic_process(quint16 msgid, QJsonObject obj)
{
    if(!_handlers.contains(msgid)){
        qDebug() << "msgid: " << msgid << " not found";
        return;
    }
    _handlers[msgid](obj);
}


