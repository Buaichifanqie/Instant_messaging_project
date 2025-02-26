#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->reg_btn,&QPushButton::clicked,this,&LoginDialog::switchRegister);
}

LoginDialog::~LoginDialog()
{
    qDebug()<<"destrut LoginDlg";
    delete ui;
}
