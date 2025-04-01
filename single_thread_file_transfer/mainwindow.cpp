#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpclient.h"
#include "global.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QFileDialog>
#include "logicmgr.h"
#include <QFile>
#include <QCryptographicHash>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
    initSlots();
}

void MainWindow::initUI(){
    this->setWindowTitle("文件上传客户端");
    this->resize(512,240);
    ui->portEd->setPlaceholderText("8090");
    ui->ipEd->setPlaceholderText("127.0.0.1");
    ui->conBtn->setEnabled(true);
    ui->uploadBtn->setEnabled(false);
    ui->testBtn->setEnabled(false);
    _file_name = "";
    ui->progressBar->setValue(0);
}

void MainWindow::initSlots(){
    connect(&TcpClient::Inst(),&TcpClient::sig_connected,
            this,[this](bool connected){
        if(connected){
            QMessageBox::information(this,"连接提示","连接成功");
            ui->conBtn->setEnabled(!connected);
            ui->uploadBtn->setEnabled(connected);
            ui->testBtn->setEnabled(connected);
        }else{
            QMessageBox::information(this,"连接提示","连接失败");
        }
    });

    connect(&TcpClient::Inst(), &TcpClient::sig_net_error,
            this, [this](QString str){
         QMessageBox::information(this,"错误提示",str);
         ui->conBtn->setEnabled(true);
         ui->uploadBtn->setEnabled(false);
         ui->testBtn->setEnabled(false);
    });

    connect(&TcpClient::Inst(), &TcpClient::sig_logic_process,
            LogicMgr::Inst().get(), &LogicMgr::sig_logic_process);

    connect(LogicMgr::Inst().get(), &LogicMgr::sig_trans_size,
            this, [this](int trans_size){
        ui->progressBar->setValue(trans_size);
        qDebug()<<"!!!!!!"<<trans_size;
        if(trans_size >= ui->progressBar->maximum()){
            ui->uploadBtn->setEnabled(true);
            ui->testBtn->setEnabled(true);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fileBtn_clicked()
{
    // 打开文件对话框
      QString fileName = QFileDialog::getOpenFileName(nullptr,
          "Open File",
          "",
          "All Files (*);;Text Files (*.txt);;Image Files (*.png *.jpg);;PDF Files (*.pdf)");

      if (!fileName.isEmpty()) {
          qDebug() << "Selected file:" << fileName;
          _file_name = fileName;
          ui->fileLb->setText(_file_name);

          QFileInfo fileInfo(_file_name); // 创建 QFileInfo 对象
          int total_size = fileInfo.size();
          ui->progressBar->setRange(0,total_size);
          ui->progressBar->setValue(0);
      } else {
          qDebug() << "No file selected.";
      }
}

void MainWindow::on_testBtn_clicked()
{
    QJsonObject jsonObj;
    jsonObj["data"]= "hello world!";
    QJsonDocument doc(jsonObj);
    auto send_data = doc.toJson();
    TcpClient::Inst().sendMsg(ID_TEST_MSG_REQ, send_data);
}

void MainWindow::on_conBtn_clicked()
{
    ui->conBtn->setEnabled(false);
    bool ok; // 用于检测转换是否成功
    quint16 port = ui->portEd->text().toUShort(&ok); // 转换为 ushort

    if (!ok) {
        // 如果转换失败，可以根据需要处理错误
        qWarning() << "Conversion failed for string:" << ui->portEd->text();
        port = 0;
    }

    TcpClient::Inst().connectToServer(ui->ipEd->text(),port);
}

void MainWindow::startDelay(int msecs) {
    qDebug() << "Starting delay...";

    // 创建事件循环
    QEventLoop loop;

    // 设置定时器，5000毫秒后退出事件循环
    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(msecs);

    // 进入事件循环
    loop.exec();

    // 延迟结束后的操作
    qDebug() << "Delay finished!";
}

void MainWindow::on_uploadBtn_clicked()
{
    ui->uploadBtn->setEnabled(false);
    // 打开文件
       QFile file(_file_name);
       if (!file.open(QIODevice::ReadOnly)) {
           qWarning() << "Could not open file:" << file.errorString();
           return;
       }

       // 保存当前文件指针位置
       qint64 originalPos = file.pos();
       QCryptographicHash hash(QCryptographicHash::Md5);
       if (!hash.addData(&file)) {
           qWarning() << "Failed to read data from file:" << _file_name;
           return ;
       }

       _file_md5 = hash.result().toHex(); // 返回十六进制字符串

    // 读取文件内容并发送
    QByteArray buffer;
    int seq = 0;

    QFileInfo fileInfo(_file_name); // 创建 QFileInfo 对象

    QString fileName = fileInfo.fileName(); // 获取文件名
    qDebug() << "文件名是：" << fileName; // 输出文件名
    int total_size = fileInfo.size();
    int last_seq = 0;
    if(total_size % MAX_FILE_LEN){
        last_seq = (total_size/MAX_FILE_LEN)+1;
    }else{
        last_seq = total_size/MAX_FILE_LEN;
    }

    // 恢复文件指针到原来的位置
    file.seek(originalPos);

    while (!file.atEnd()) {
        //每次读取2048字节发送
        buffer = file.read(MAX_FILE_LEN);
        QJsonObject jsonObj;
        // 将文件内容转换为 Base64 编码（可选）
        QString base64Data = buffer.toBase64();
        //qDebug() << "send data is " << base64Data;
        ++seq;
        jsonObj["md5"] = _file_md5;
        jsonObj["name"] = fileName;
        jsonObj["seq"] = seq;
        jsonObj["trans_size"] = buffer.size() + (seq-1)*MAX_FILE_LEN;
        jsonObj["total_size"] = total_size;

        if(buffer.size() + (seq-1)*MAX_FILE_LEN == total_size){
            jsonObj["last"] = 1;
        }else{
            jsonObj["last"] = 0;
        }

        jsonObj["data"]= base64Data;
        jsonObj["last_seq"] = last_seq;
        QJsonDocument doc(jsonObj);
        auto send_data = doc.toJson();
        TcpClient::Inst().sendMsg(ID_UPLOAD_FILE_REQ, send_data);
        //startDelay(500);
     }

    //关闭文件
    file.close();

}
