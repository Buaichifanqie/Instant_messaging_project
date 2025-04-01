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
    //单例模式，返回静态局部变量
    static TcpClient& Inst(){
        static TcpClient client;
        return client;
    }
    ~TcpClient();

    //删除拷贝构造和拷贝赋值
    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    //连接到服务器
    void connectToServer(const QString& host, quint16 port);

    //断开连接
    void disconnectFromServer();

    //发送消息
    void sendMsg(quint16 id, QByteArray data);

    bool isConnected();

private:
    //构造函数变为私有
    explicit TcpClient(QObject *parent = nullptr);

    //QTcpSocket类，用来通信的客户端
    QTcpSocket* _socket;

    //缓存收到的数据
    QByteArray _buffer;

    void processData();
signals:
    void sig_send_msg(quint16 id, QByteArray data);
    void sig_connected(bool);
    void sig_logic_process(quint16 id, QJsonObject obj);
    //监听网络错误的信号
    void sig_net_error(QString);
public slots:

    //连接成功触发的槽函数
    void slot_connected();
    //tcp缓冲区有数据可读，触发读取消息槽函数
    void slot_ready_read();
    //捕获到断开连接触发的槽函数
    void slot_disconnected();
    //捕获到出错信号触发的槽函数
    void slot_error_occured(QAbstractSocket::SocketError socketError);
    //发送消息槽函数
    void slot_send_msg(quint16 id, QByteArray data);

};

#endif // TCPCLIENT_H
