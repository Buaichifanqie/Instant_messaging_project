#include "tcpmgr.h"
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QByteArray>
#include "usermgr.h"

TcpMgr::~TcpMgr()
{

}

TcpMgr::TcpMgr():m_host(""),m_port(0),m_b_recv_pending(false),m_message_id(0),m_message_len(0)
{
    QObject::connect(&m_socket,&QTcpSocket::connected,[&](){
        qDebug() << "Connected to server!";
        //连接建立后发送消息
        emit sig_con_success(true);
    });
    QObject::connect(&m_socket,&QTcpSocket::readyRead,[&](){
        // 当有数据可读时，读取所有数据
        // 读取所有数据并追加到缓冲区
        m_buffer.append(m_socket.readAll());

        QDataStream stream(&m_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);

        forever
        {
            //先解析头部
            if(!m_b_recv_pending){
                // 检查缓冲区中的数据是否足够解析出一个消息头（消息ID + 消息长度）
                if (m_buffer.size() < static_cast<int>(sizeof(quint16) * 2)) {
                    return; // 数据不够，等待更多数据
                }
                // 预读取消息ID和消息长度，但不从缓冲区中移除
                stream >> m_message_id >> m_message_len;
                //将buffer 中的前四个字节移除
                m_buffer = m_buffer.mid(sizeof(quint16) * 2);
                // 输出读取的数据
                qDebug() << "Message ID:" << m_message_id << ", Length:" << m_message_len;
            }
            //buffer剩余长读是否满足消息体长度，不满足则退出继续等待接受
            if(m_buffer.size()<m_message_len)
            {
                m_b_recv_pending=true;
                return;
            }
            m_b_recv_pending=false;
            //读消息体
            QByteArray messageBody=m_buffer.mid(0,m_message_len);
            qDebug() << "receive body msg is " << messageBody ;
            m_buffer=m_buffer.mid(m_message_len);
        }
    });
    //5.15 之后版本
    //       QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
    //           Q_UNUSED(socketError)
    //           qDebug() << "Error:" << _socket.errorString();
    //       });
    // 处理错误（适用于Qt 5.15之前的版本）
    QObject::connect(&m_socket, static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),
                     [&](QTcpSocket::SocketError socketError) {
                         qDebug() << "Error:" << m_socket.errorString() ;
                         switch (socketError) {
                         case QTcpSocket::ConnectionRefusedError:
                             qDebug() << "Connection Refused!";
                             emit sig_con_success(false);
                             break;
                         case QTcpSocket::RemoteHostClosedError:
                             qDebug() << "Remote Host Closed Connection!";
                             break;
                         case QTcpSocket::HostNotFoundError:
                             qDebug() << "Host Not Found!";
                             emit sig_con_success(false);
                             break;
                         case QTcpSocket::SocketTimeoutError:
                             qDebug() << "Connection Timeout!";
                             emit sig_con_success(false);
                             break;
                         case QTcpSocket::NetworkError:
                             qDebug() << "Network Error!";
                             break;
                         default:
                             qDebug() << "Other Error!";
                             break;
                         }
                     });
    // 处理连接断开
    QObject::connect(&m_socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Disconnected from server.";
    });
    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
}

void TcpMgr::initHandlers()
{
    //auto self = shared_from_this();
    m_handlers.insert(ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<< "handle id is "<< id ;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
           qDebug() << "Failed to create QJsonDocument.";
           return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, err is Json Parse Err" << err ;
            emit sig_login_failed(err);
            return;
        }

        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Login Failed, err is " << err ;
            emit sig_login_failed(err);
            return;
        }

        auto uid=jsonObj["uid"].toInt();
        auto name=jsonObj["name"].toString();
        auto nick=jsonObj["nick"].toString();
        auto icon=jsonObj["icon"].toString();
        auto sex=jsonObj["sex"].toInt();

        auto user_info=std::make_shared<UserInfo>(uid,name,nick,icon,sex);
        UserMgr::GetInstance()->SetUserInfo(user_info);
        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());

        if(jsonObj.contains("apply_list"))
        {
            UserMgr::GetInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
        }

        emit sig_swich_chatdlg();

    });


    m_handlers.insert(ID_SEARCH_USER_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<< "handle id is "<< id ;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_SEARCH_USER_RSP Failed, err is Json Parse Err" << err ;
            emit sig_user_search(nullptr);
            return;
        }

        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_SEARCH_USER_RSP Failed, err is " << err ;
            emit sig_user_search(nullptr);
            return;
        }
        auto search_info=std::make_shared<SearchInfo> (jsonObj["uid"].toInt(),
            jsonObj["name"].toString(),jsonObj["nick"].toString(),
            jsonObj["desc"].toString(),jsonObj["sex"].toInt(),jsonObj["icon"].toString());
        emit sig_user_search(search_info);
    });

    m_handlers.insert(ID_ADD_FRIEND_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<< "handle id is "<< id ;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_ADD_FRIEND_RSP Failed, err is Json Parse Err" << err ;

            return;
        }

        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_ADD_FRIEND_RSP Failed, err is " << err ;

            return;
        }
        qDebug()<<"ID_ADD_FRIEND_RSP Success";
    });

    m_handlers.insert(ID_NOTIFY_ADD_FRIEND_REQ, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<< "handle id is "<< id ;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ Failed, err is Json Parse Err" << err ;
            return;
        }

        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ Failed, err is " << err ;
            return;
        }

        int from_uid=jsonObj["applyuid"].toInt();
        QString name=jsonObj["name"].toString();
        QString desc=jsonObj["desc"].toString();
        QString icon=jsonObj["icon"].toString();
        QString nick=jsonObj["nick"].toString();
        int sex=jsonObj["sex"].toInt();

        auto apply_info=std::make_shared<AddFriendApply>(
                              from_uid,name,desc,icon,nick,sex);

        emit sig_friend_apply(apply_info);

        qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ Friend REQ Success";
    });

    m_handlers.insert(ID_NOTIFY_AUTH_FRIEND_REQ,[this](ReqId id,int len,QByteArray data)
    {
        Q_UNUSED(len);
        qDebug()<<"handle id is"<<id<<"data is"<<data;

        QJsonDocument jsonDoc=QJsonDocument::fromJson(data);

        if(jsonDoc.isNull())
        {
            qDebug()<<"Failed to create QJsonDocument.";
        }

        QJsonObject jsonObj=jsonDoc.object();
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_NOTIFY_AUTH_FRIEND_REQ Failed, err is Json Parse Err" << err ;
            return;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_NOTIFY_AUTH_FRIEND_REQ Failed, err is " << err ;
            return;
        }

        int from_uid=jsonObj["fromuid"].toInt();
        QString name=jsonObj["name"].toString();
        QString icon=jsonObj["icon"].toString();
        QString nick=jsonObj["nick"].toString();
        int sex=jsonObj["sex"].toInt();

        auto auth_info=std::make_shared<AuthInfo>(from_uid,name,nick,icon,sex);
        emit sig_add_auth_friend(auth_info);

    });

    m_handlers.insert(ID_AUTH_FRIEND_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto uid = jsonObj["uid"].toInt();
        auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        emit sig_auth_rsp(rsp);

        qDebug() << "Auth Friend Success " ;
    });

}

void TcpMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter=m_handlers.find(id);
    if(find_iter==m_handlers.end())
    {
        qDebug()<<"not found id ["<<id<<"] to handle";
        return;
    }
    find_iter.value()(id,len,data);
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug()<<"receive tcp connect signal";
    //尝试连接服务器
    qDebug()<<"Connection to server";
    m_host=si.Host;
    m_port=static_cast<uint16_t>(si.Port.toUInt());
    m_socket.connectToHost(si.Host,m_port);
}

void TcpMgr::slot_send_data(ReqId reqId, QString dataBytes)
{
    uint16_t id = reqId;

    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(dataBytes.length());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // 设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);

    // 写入ID和长度
    out << id << len;

    // 添加字符串数据
    block.append(dataBytes);

    // 发送数据
    m_socket.write(block);
    qDebug() << "tcp mgr send byte data is " << block ;

}
