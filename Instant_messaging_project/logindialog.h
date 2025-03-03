#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
private:
    void initHead();
    //收到服务器回包后做处理
    void initHttpHandlers();
    void showTip(QString str,bool b_ok);
    bool checkUserValid();
    bool checkPwdValid();
    Ui::LoginDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
    bool enableBtn(bool);
    QMap<TipErr, QString> m_tip_errs;
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    int m_uid;
    QString m_token;
private slots:
    void slot_forget_pwd();
    void on_login_btn_clicked();
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    void slot_tcp_con_finish(bool bsuccess);
    void slot_login_failed(int);

signals:
    void switchRegister();
    void switchReset();
    //收到http的gateServer的信号后向 把信号发送给Tcp的管理者
    void sig_connect_tcp(ServerInfo);
};

#endif // LOGINDIALOG_H
