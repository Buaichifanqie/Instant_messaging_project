#include "logicmgr.h"
#include "mainwindow.h"
#include "tcpclient.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QJsonDocument>
#include "global.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUi();
    initSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUi()
{
    this->setWindowTitle("文件上传客户端");
    this->resize(200,400);
    ui->portEd->setPlaceholderText("9999");
    ui->ipEd->setPlaceholderText("127.0.0.1");
    ui->conBtn->setEnabled(true);
    ui->uploadBtn->setEnabled(true);
    ui->testBtn->setEnabled(true);
    m_file_name="";
    ui->progressBar->setValue(0);
}

void MainWindow::initSlots()
{
    //连接Tcp客户端连接状态信号到槽函数
    connect(&TcpClient::Inst(),&TcpClient::sig_connected,this,[this](bool connected){
        if(connected)
        {
            QMessageBox::information(this,"连接事件","连接成功");
            ui->conBtn->setEnabled(!connected);// 连接成功后连接按钮不可用
            ui->uploadBtn->setEnabled(connected); // 上传按钮可用
            ui->testBtn->setEnabled(connected);
        }
        else
        {
            QMessageBox::information(this,"连接事件","连接失败");
        }
    });
    //连接TCP客户端的网络错误信号到槽函数
    connect(&TcpClient::Inst(),&TcpClient::sig_net_error,this, [this](QString str){
        QMessageBox::information(this,"错误提示",str);
        ui->conBtn->setEnabled(true); // 网络错误后连接按钮可用
        ui->uploadBtn->setEnabled(false); // 上传按钮不可用
        ui->testBtn->setEnabled(false); // 测试按钮不可用
    });

    // 连接TCP客户端的逻辑处理信号到逻辑管理器
    connect(&TcpClient::Inst(),&TcpClient::sig_logic_process,Logicmgr::Inst().get(),&Logicmgr::sig_logic_process);
    // 连接逻辑管理器的传输进度信号到进度条更新槽函数
    connect(Logicmgr::Inst().get(), &Logicmgr::sig_trans_size,this, [this](int trans_size)
    {
        ui->progressBar->setValue(trans_size); // 更新进度条值
        if(trans_size >= ui->progressBar->maximum()){
            ui->uploadBtn->setEnabled(true); // 传输完成恢复上传按钮可用
            ui->testBtn->setEnabled(true); // 恢复测试按钮可用
        }
    });
}

void MainWindow::startDelay(int msecs)
{
    qDebug() << "Starting delay...";
    //创建事件循环
    QEventLoop loop;
    QTimer timer;
    connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);

    //进入事件循环
    loop.exec();
    // 延迟结束后的操作
    qDebug() << "Delay finished!";
}


void MainWindow::on_uploadBtn_clicked()
{
    ui->uploadBtn->setEnabled(false); // 禁用上传按钮

    // 打开文件
    QFile file(m_file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "文件打开失败:" << file.errorString();
        return;
    }

    // 计算文件MD5校验值
    qint64 originalPos = file.pos();
    QCryptographicHash hash(QCryptographicHash::Md5);
    if (!hash.addData(&file)) {
        qWarning() << "MD5计算失败:" <<m_file_name;
        return;
    }
    m_file_md5 = hash.result().toHex();

    // 获取文件信息
    QFileInfo fileInfo(m_file_name);
    QString fileName = fileInfo.fileName();
    int total_size = fileInfo.size();

    // 计算分块信息
    int last_seq = (total_size % MAX_FILE_LEN) ?
                       (total_size/MAX_FILE_LEN)+1 :
                       total_size/MAX_FILE_LEN;

    // 重置文件指针
    file.seek(originalPos);

    // 分块读取并发送文件
    QByteArray buffer;
    int seq = 0;
    while (!file.atEnd()) {
        buffer = file.read(MAX_FILE_LEN); // 读取分块数据
        seq++;

        // 构造上传消息
        QJsonObject jsonObj;
        // 将文件内容转换为 Base64 编码（可选）
        QString base64Data = buffer.toBase64();
        jsonObj["md5"] = m_file_md5;
        jsonObj["name"] = fileName;
        jsonObj["seq"] = seq;
        jsonObj["trans_size"] = buffer.size() + (seq-1)*MAX_FILE_LEN;
        jsonObj["total_size"] = total_size;
        jsonObj["last"] = (buffer.size() + (seq-1)*MAX_FILE_LEN == total_size) ? 1 : 0;
        jsonObj["data"] = base64Data; // Base64编码文件数据
        jsonObj["last_seq"] = last_seq;

        // 发送数据包
        QJsonDocument doc(jsonObj);
        TcpClient::Inst().sendMsg(ID_UPLOAD_FILE_REQ, doc.toJson());

        // 可选延迟控制发送速率
        // startDelay(500);
    }

    file.close(); // 关闭文件
}

void MainWindow::on_fileBtn_clicked()
{
    //打开文件对话框
    QString fileName=QFileDialog::getOpenFileName(nullptr,
        "Open File",
        "",
        "All Files (*);;Text Files (*.txt);;Image Files (*.png *.jpg);;PDF Files (*.pdf)"
        );
    if(!fileName.isEmpty())
    {
        // 成功选择文件时的处理流程
        qDebug() << "Selected file:" << fileName;
        m_file_name=fileName;
        ui->fileLb->setText(m_file_name);
        QFileInfo fileInfo(m_file_name);
        int total_size=m_file_name.size();
        ui->progressBar->setRange(0,total_size);
        ui->progressBar->setValue(0);
    }
    else
    {
        // 用户取消选择时的处理
        qDebug() << "No file selected.";
    }
}

void MainWindow::on_testBtn_clicked()
{
    QJsonObject jsonObj;
    jsonObj["data"]="hello world!!!";
    QJsonDocument doc(jsonObj);
    auto send_data=doc.toJson();
    TcpClient::Inst().sendMsg(ID_TEXT_MSG_REQ,send_data);
}

void MainWindow::on_conBtn_clicked()
{
    ui->conBtn->setEnabled(false);
    bool is_ok;//用于检测转换是否成功
    quint16 port=ui->portEd->text().toUShort(&is_ok);// 转换为 ushort
    if(!is_ok)
    {
        // 如果转换失败，可以根据需要处理错误
        qWarning() << "Conversion failed for string:" << ui->portEd->text();
        port = 0;
    }
    TcpClient::Inst().connectToServer(ui->portEd->text(),port);
}







