#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkRequest>
#include <Qstring>
#include <QUrl>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonParseError>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager=new QNetworkAccessManager();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //获取文本框内容
    QString text=ui->inputEdit->toPlainText();

    ui->inputEdit->clear();

    ui->outputEdit->insertPlainText("-->");
    ui->outputEdit->insertPlainText(text);
    ui->outputEdit->insertPlainText("\n\n");

    //发送请求对象
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.deepseek.com/chat/completions"));

    //设置固定头
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    //设置自定义的头
    request.setRawHeader("Accept","application/json");
    request.setRawHeader("Authorization","自己的密钥");

    QJsonArray messages;

    QJsonObject systemMsg;
    systemMsg["role"]="system";
    systemMsg["content"]="You are a helpful assistant";

    QJsonObject userMsg;
    userMsg["role"]="user";
    userMsg["content"]=text;

    messages.append(systemMsg);
    messages.append(userMsg);

    QJsonObject requestBody;
    requestBody["messages"]=messages;
    requestBody["model"]="deepseek-chat";
    requestBody["max_tokens"]=2048;
    requestBody["stream"]=true;
    requestBody["temperature"]=1;

    //发送请求
    currentAIReply.clear(); // 每次请求前清空缓存
    QNetworkReply *reply = manager->post(request,QJsonDocument(requestBody).toJson());

    // 处理数据（优化后的流式处理）
    connect(reply, &QNetworkReply::readyRead, this, [=] {
        QByteArray data = reply->readAll();
        QString buffer = QString::fromUtf8(data);

        // 处理SSE格式数据
        QStringList chunks = buffer.split("\n\n");
        foreach (const QString &chunk, chunks) {
            if(chunk.startsWith("data: ")) {
                QString jsonStr = chunk.mid(6).trimmed();
                if(jsonStr == "[DONE]") continue;

                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);
                if(error.error == QJsonParseError::NoError) {
                    QString content = doc.object()["choices"].toArray().first()
                    .toObject()["delta"].toObject()["content"].toString();
                    if(!content.isEmpty()) {
                        ui->outputEdit->moveCursor(QTextCursor::End);
                        ui->outputEdit->insertPlainText(content);
                        currentAIReply += content; // 实时累积AI回复
                    }
                }
            }
        }
    });

    // 当网络请求完成时，连接回复完成的信号到处理槽函数
    connect(reply, &QNetworkReply::finished, this, [=] {
        // ==== 获取AI回复内容 ====
        // 获取输出文本框中的完整对话历史
        QString fullText = ui->outputEdit->toPlainText();

        // ==== 定位对话分割位置 ====
        // 查找最后一次用户输入标记的位置（格式：-->用户输入内容）
        int lastUserInputPos = fullText.lastIndexOf("-->" + text); // text应为用户最后输入内容

        // ==== 提取AI回复 ====
        // 从用户输入结束位置开始截取AI回复内容：
        // 1. 起始位置 = 用户输入位置 + 用户输入文本长度 + 2（"> "分隔符长度）
        // 2. 使用trimmed()去除首尾空白字符
        QString aiReply = fullText.mid(lastUserInputPos + text.length() + 2).trimmed();

        // ==== 保存到对话历史 ====
        if (!aiReply.isEmpty()) {
            QJsonObject aiMsg;
            aiMsg["role"] = "assistant";    // 标识消息角色为AI助手
            aiMsg["content"] = aiReply;      // 存储AI回复内容
            conversationHistory.append(aiMsg); // 将消息对象添加到对话历史数组
        }

        // ==== UI界面更新 ====
        ui->outputEdit->moveCursor(QTextCursor::End);  // 移动光标到文本框末尾
        ui->outputEdit->insertPlainText("\n\n\n\n");   // 插入换行分隔对话段落

        // ==== 资源清理 ====
        reply->deleteLater();  // 安全释放网络回复对象
    });
}




