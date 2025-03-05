#include "MessageTextEdit.h"
#include <QDebug>
#include <QMessageBox>

// 构造函数，初始化MessageTextEdit
MessageTextEdit::MessageTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    // 设置样式表，去掉边框（注释掉的代码）
    // this->setStyleSheet("border: none;");

    // 设置最大高度为60像素
    this->setMaximumHeight(60);

    // 连接textChanged信号到textEditChanged槽（注释掉的代码）
    // connect(this,SIGNAL(textChanged()),this,SLOT(textEditChanged()));
}

// 析构函数
MessageTextEdit::~MessageTextEdit()
{
    // 析构时不执行任何操作
}

// 获取消息列表
QVector<MsgInfo> MessageTextEdit::getMsgList()
{
    // 清空临时消息列表
    mGetMsgList.clear();

    // 获取文本编辑框中的纯文本内容
    QString doc = this->document()->toPlainText();
    QString text = ""; // 用于存储文本信息
    int indexUrl = 0;  // URL索引
    int count = mMsgList.size(); // 消息列表的大小

    // 遍历文档中的每个字符
    for(int index = 0; index < doc.size(); index++)
    {
        // 如果遇到对象替换字符（通常是图片或其他嵌入对象）
        if(doc[index] == QChar::ObjectReplacementCharacter)
        {
            // 如果之前有文本内容，将其添加到消息列表中
            if(!text.isEmpty())
            {
                QPixmap pix;
                insertMsgList(mGetMsgList, "text", text, pix);
                text.clear();
            }

            // 遍历消息列表，找到与当前对象匹配的消息
            while(indexUrl < count)
            {
                MsgInfo msg = mMsgList[indexUrl];
                if(this->document()->toHtml().contains(msg.content, Qt::CaseSensitive))
                {
                    indexUrl++;
                    mGetMsgList.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        else
        {
            // 如果不是对象替换字符，将其添加到文本中
            text.append(doc[index]);
        }
    }

    // 如果最后还有未处理的文本，将其添加到消息列表中
    if(!text.isEmpty())
    {
        QPixmap pix;
        insertMsgList(mGetMsgList, "text", text, pix);
        text.clear();
    }

    // 清空消息列表并清空文本编辑框
    mMsgList.clear();
    this->clear();

    // 返回获取到的消息列表
    return mGetMsgList;
}

// 拖拽进入事件处理
void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    // 如果拖拽源是自身，忽略事件；否则接受事件
    if(event->source() == this)
        event->ignore();
    else
        event->accept();
}

// 拖拽释放事件处理
void MessageTextEdit::dropEvent(QDropEvent *event)
{
    // 从MIME数据中插入内容
    insertFromMimeData(event->mimeData());
    event->accept();
}

// 按键事件处理
void MessageTextEdit::keyPressEvent(QKeyEvent *e)
{
    // 如果按下回车键且没有按下Shift键，触发send信号
    if((e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) && !(e->modifiers() & Qt::ShiftModifier))
    {
        emit send();
        return;
    }

    // 否则调用基类的按键事件处理
    QTextEdit::keyPressEvent(e);
}

// 从URL列表插入文件
void MessageTextEdit::insertFileFromUrl(const QStringList &urls)
{
    // 如果URL列表为空，直接返回
    if(urls.isEmpty())
        return;

    // 遍历URL列表，插入图片或文本文件
    foreach (QString url, urls) {
        if(isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

// 插入图片
void MessageTextEdit::insertImages(const QString &url)
{
    // 加载图片
    QImage image(url);

    // 按比例缩放图片
    if(image.width() > 120 || image.height() > 80)
    {
        if(image.width() > image.height())
        {
            image = image.scaledToWidth(120, Qt::SmoothTransformation);
        }
        else
        {
            image = image.scaledToHeight(80, Qt::SmoothTransformation);
        }
    }

    // 获取文本光标并插入图片
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(image, url);

    // 将图片信息添加到消息列表
    insertMsgList(mMsgList, "image", url, QPixmap::fromImage(image));
}

// 插入文本文件
void MessageTextEdit::insertTextFile(const QString &url)
{
    // 获取文件信息
    QFileInfo fileInfo(url);

    // 如果是目录，弹出提示框并返回
    if(fileInfo.isDir())
    {
        QMessageBox::information(this, "提示", "只允许拖拽单个文件!");
        return;
    }

    // 如果文件大小超过100M，弹出提示框并返回
    if(fileInfo.size() > 100 * 1024 * 1024)
    {
        QMessageBox::information(this, "提示", "发送的文件大小不能大于100M");
        return;
    }

    // 获取文件图标并插入到文本编辑框中
    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(), url);

    // 将文件信息添加到消息列表
    insertMsgList(mMsgList, "file", url, pix);
}

// 判断是否可以插入MIME数据
bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

// 从MIME数据插入内容
void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{
    // 获取URL列表
    QStringList urls = getUrl(source->text());

    // 如果URL列表为空，直接返回
    if(urls.isEmpty())
        return;

    // 遍历URL列表，插入图片或文本文件
    foreach (QString url, urls) {
        if(isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

// 判断URL是否为图片
bool MessageTextEdit::isImage(QString url)
{
    // 支持的图片格式
    QString imageFormat = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    QStringList imageFormatList = imageFormat.split(",");

    // 获取文件后缀
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();

    // 如果后缀在支持的图片格式列表中，返回true
    if(imageFormatList.contains(suffix, Qt::CaseInsensitive)) {
        return true;
    }
    return false;
}

// 将消息插入消息列表
void MessageTextEdit::insertMsgList(QVector<MsgInfo> &list, QString flag, QString text, QPixmap pix)
{
    // 创建消息结构体并设置属性
    MsgInfo msg;
    msg.msgFlag = flag;
    msg.content = text;
    msg.pixmap = pix;

    // 将消息添加到列表中
    list.append(msg);
}

// 从文本中提取URL
QStringList MessageTextEdit::getUrl(QString text)
{
    QStringList urls;

    // 如果文本为空，直接返回空列表
    if(text.isEmpty()) return urls;

    // 按行分割文本
    QStringList list = text.split("\n");

    // 遍历每一行，提取URL
    foreach (QString url, list) {
        if(!url.isEmpty()) {
            QStringList str = url.split("///");
            if(str.size() >= 2)
                urls.append(str.at(1));
        }
    }
    return urls;
}

// 获取文件图标并生成Pixmap
QPixmap MessageTextEdit::getFileIconPixmap(const QString &url)
{
    // 获取文件图标
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    // 获取文件大小并格式化
    QString strFileSize = getFileSize(fileinfo.size());

    // 设置字体
    QFont font(QString("宋体"), 10, QFont::Normal, false);
    QFontMetrics fontMetrics(font);

    // 计算文件名和文件大小的文本尺寸
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());
    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);

    // 计算最大宽度
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() : FileSize.width();

    // 创建Pixmap并填充背景
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    // 绘制文件图标、文件名和文件大小
    QPainter painter;
    painter.begin(&pix);
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40, 40));
    painter.setPen(Qt::black);
    QRect rectText(50 + 10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    QRect rectFile(50 + 10, textSize.height() + 5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();

    return pix;
}

// 格式化文件大小
QString MessageTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;

    // 根据文件大小选择合适的单位
    if(size < 1024) {
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1024) {
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size < 1024 * 1024 * 1024) {
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else {
        num = size / 1024.0 / 1024.0 / 1024.0;
        Unit = "GB";
    }

    // 返回格式化后的文件大小
    return QString::number(num, 'f', 2) + " " + Unit;
}

// 文本编辑框内容改变时的槽函数（未实现）
void MessageTextEdit::textEditChanged()
{
    // qDebug() << "text changed!" << endl;
}
