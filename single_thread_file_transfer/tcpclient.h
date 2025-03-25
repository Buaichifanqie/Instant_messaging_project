#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QJsonObject>


class TcpClient : public QObject
{
    Q_OBJECT
public:
    //单例模式返回静态局部变量
    static TcpClient& Inst()
    {
        static TcpClient client;
        return client;
    }

    //删除拷贝构造和拷贝赋值
    TcpClient(const TcpClient&)=delete;
    TcpClient& operator=(const TcpClient&)=delete;

    //连接服务器
    void connectToServer(const QString& host,quint16 port);
    //断开连接
    void dcisconnectFromServer();

    //发送消息
    void sendMsg(quint16 id,QByteArray data);

signals:
    void sig_send_msg(quint16 id,QByteArray data);
    void sig_logic_process(quint16 id, QJsonObject obj);
    void sig_connected(bool);
    void sig_net_error(QString);
public slots:
    //发送数据槽函数
    void slot_send_msg(quint16 id,QByteArray body);

private:
    //构造函数设为私有
    explicit TcpClient(QObject* parent=nullptr);

    //socket类用来和通讯的客户端
    QTcpSocket* m_socket;
    //缓存区
    QByteArray m_buffer;

    //处理接收到的完整数据包
    void processData();
};

#endif // TCPCLIENT_H
