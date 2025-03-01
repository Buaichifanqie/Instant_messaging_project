#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"
#include "timerbtn.h"


namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_get_code_clicked();
    void slot_reg_mod_finish(ReqId id,QString res,ErrorCodes err);
    void on_confirm_btn_clicked();

    void on_pushButton_clicked();

    void on_cancel_btn_clicked();
private:
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVarifyValid();


    void ChangeTipPage();
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
    QMap<TipErr, QString> m_tip_errs;
    void initHttpHandlers();
    void showTip(QString str,bool b_ok);
    Ui::RegisterDialog *ui;
    QMap<ReqId,std::function<void(const QJsonObject&)>> m_handlers;

    QTimer* m_countdown_timer;
    int m_countdown;

signals:
    void sigSwitchLogin();

};

#endif // REGISTERDIALOG_H
