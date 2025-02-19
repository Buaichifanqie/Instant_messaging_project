#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

//CRTP
class HttpMgr : public QObject,public Singleton<HttpMgr>,
                public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

public:
    ~HttpMgr();
private:
    //singleton里面getInstance里面有个new T需要构造，T是httpmgr，所以声明友元
    friend class Singleton<HttpMgr>;//让基类可以访问子类的构造函数
    HttpMgr();
    QNetworkAccessManager m_manger;
    void PostHttpReq(QUrl url,QJsonObject json,ReqId req_id,Modules mod);

private slots:
    void slot_http_finish(ReqId id,QString res, ErrorCodes err,Modules mod);
signals:
    void sig_http_finish(ReqId id,QString res, ErrorCodes err,Modules mod);
    void sig_reg_mod_finish(ReqId id,QString res,ErrorCodes err);
};

#endif // HTTPMGR_H
