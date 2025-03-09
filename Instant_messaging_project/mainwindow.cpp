#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpmgr.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_login_dlg=new LoginDialog(this);
    // 设置登录对话框的窗口标志
    // Qt::CustomizeWindowHint：允许自定义窗口的外观和行为
    // Qt::FramelessWindowHint：移除窗口的边框和标题栏，使窗口无边框
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(m_login_dlg);
    // m_login_dlg->show();

    //创建和注册消息连接
    connect(m_login_dlg,&LoginDialog::switchRegister,this,&MainWindow::SlotSwitchReg);
    //连接登录界面忘记密码信号
    connect(m_login_dlg,&LoginDialog::switchReset,this,&MainWindow::SlotSwitchReset);

    //连接创建聊天界面的信号
    connect(TcpMgr::GetInstance().get(),&TcpMgr::sig_swich_chatdlg,this,&MainWindow::SlotSwitchChat);

    //emit TcpMgr::GetInstance()->sig_swich_chatdlg();
}

MainWindow::~MainWindow()
{
    delete ui;
    // if(m_login_dlg)
    // {
    //     delete m_login_dlg;
    //     m_login_dlg=nullptr;
    // }

    // if(m_reg_dlg)
    // {
    //     delete m_reg_dlg;
    //     m_reg_dlg=nullptr;
    // }
}

void MainWindow::SlotSwitchReg()
{
    m_reg_dlg=new RegisterDialog(this);


    // 设置注册对话框的窗口标志
    // 同样使用 Qt::CustomizeWindowHint 和 Qt::FramelessWindowHint
    // 使注册对话框也无边框且可自定义
    m_reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    //连接注册界面返回的信号
    connect(m_reg_dlg,&RegisterDialog::sigSwitchLogin,this,&MainWindow::SlotSwitchLogin);

    setCentralWidget(m_reg_dlg);
    m_login_dlg->hide();
    m_reg_dlg->show();
}

void MainWindow::SlotSwitchLogin()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_login_dlg = new LoginDialog(this);
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);
    m_reg_dlg->hide();
    m_login_dlg->show();
    //连接登录界面注册信号
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    //连接登录忘记密码信号
    connect(m_login_dlg,&LoginDialog::switchReset,this,&MainWindow::SlotSwitchReset);
}

void MainWindow::SlotSwitchReset()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_reset_dlg = new ResetDialog(this);
    m_reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_reset_dlg);
    m_login_dlg->hide();
    m_reset_dlg->show();
    //注册返回登录信号和槽函数
    connect(m_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin2);
}

void MainWindow::SlotSwitchLogin2()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_login_dlg = new LoginDialog(this);
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);
    m_reset_dlg->hide();
    m_login_dlg->show();
    //连接登录界面忘记密码信号
    connect(m_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    //连接登录界面注册信号
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
}

void MainWindow::SlotSwitchChat()
{
    m_chat_dlg=new ChatDialog();
    m_chat_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_chat_dlg);
    m_chat_dlg->show();
    m_login_dlg->hide();
    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    this->setMinimumSize(QSize(1050,900));
}
