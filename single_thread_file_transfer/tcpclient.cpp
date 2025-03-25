#include "tcpclient.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include "global.h"
#include <QFile>
#include <QCryptographicHash>


TcpClient::TcpClient(QObject* parent):QObject(parent),m_socket(new QTcpSocket(this))
{

}

void TcpClient::processData()
{
    while(m_buffer.size() >= TCP_HEAD_LEN)
    {
        auto head_byte=m_buffer.left(TCP_HEAD_LEN);
        QDataStream stream(head_byte);
        //设置为大端
        stream.setByteOrder(QDataStream::BigEndian);
        //读取id
        quint16 msg_id;
        stream>>msg_id;
         //读取长度
        quint32 body_length;
        stream>>body_length;
        if(m_buffer.size()>=TCP_HEAD_LEN+body_length)
        {
             //完整的消息体已经接受
            QByteArray body=m_buffer.mid(TCP_HEAD_LEN,body_length);
            //去掉完整的消息体
            m_buffer=m_buffer.mid(TCP_HEAD_LEN+body_length);
            //解析服务器发过来的消息
            QJsonDocument jsonDoc=QJsonDocument::fromJson(body);
            if(jsonDoc.isNull())
            {
                qDebug()<<"Failed to create JSON doc";
                this->m_socket->close();
                return;
            }
            if(!jsonDoc.isObject())
            {
                qDebug() << "JSON is not an object.";
                this->m_socket->close();
                return;
            }

            //获取JSON对象
            QJsonObject jsonObject=jsonDoc.object();
            //发送信号 给逻辑让逻辑处理
            emit sig_logic_process(msg_id, jsonObject);
        }
        else
        {
            //消息未完全接受，所以中断
            break;
        }
    }
}

void TcpClient::connectToServer(const QString &host, quint16 port)
{
    m_socket->connectToHost(host,port);
}

void TcpClient::dcisconnectFromServer()
{
    if(m_socket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }
    m_socket->disconnectFromHost();

    //超时立即关闭
    if(m_socket->waitForDisconnected(3000))
    {

    }
    else
    {
        //关闭错误 发信号
        emit
    }

}

void TcpClient::sendMsg(quint16 id, QByteArray data)
{
    //发送信号交给槽函数管理 因为槽函数内部是个任务队列 这样做的好处是多线程安全
    emit sig_send_msg(id,data);
}

void TcpClient::slot_send_msg(quint16 id, QByteArray body)
{
    //先判断是否连接
    if(m_socket->state() != QAbstractSocket::ConnectedState)
    {
        //发送连接错误的信号
        emit
            return;
    }
    //获取数据包长
    quint32 bodyLength=body.size();

    //创建字节数组
    QByteArray data;
    //绑定字节数组
    QDataStream stream(&data,QIODevice::WriteOnly);
    //大端模式
    stream.setByteOrder(QDataStream::BigEndian);
    //写入id
    stream<<id;
    //写入长度
    stream<<bodyLength;
    //写入包体
    data.append(data);
    //发送消息
    m_socket->write(data);

}
