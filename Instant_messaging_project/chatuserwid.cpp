#include "chatuserwid.h"
#include "ui_chatuserwid.h"

ChatUserWid::ChatUserWid(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CHAT_USER_ITEM);
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

void ChatUserWid::SetInfo(QString name, QString head, QString msg)
{
    m_name=name;
    m_head=head;
    m_msg=msg;

    //加载图片
    QPixmap pixmap(m_head);
    //设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    // // 获取 ui->user_name_lb 控件的字体度量信息
    // QFontMetrics fontMetrics(ui->user_name_lb->font());
    // // 使用字体度量信息，将 m_name 文本进行右截断处理（超出部分用省略号代替）
    // // 截断后的文本宽度不会超过 ui->user_name_lb 控件的宽度
    // QString nameText = fontMetrics.elidedText(m_name, Qt::ElideRight, ui->user_name_lb->width());
    // // 获取 ui->user_chat_lib 控件的字体度量信息
    // QFontMetrics fontMetricsl(ui->user_chat_lib->font());
    // // 使用字体度量信息，将 m_msg 文本进行右截断处理（超出部分用省略号代替）
    // // 截断后的文本宽度不会超过 ui->user_chat_lib 控件的宽度
    // QString msgText = fontMetricsl.elidedText(m_msg, Qt::ElideRight, ui->user_chat_lib->width());
    int numCharsToShow = ui->user_chat_lib->width()-3;  // 显示前127个字符
    if (ui->user_chat_lib->text().length() > numCharsToShow) {
        ui->user_chat_lib->setText(QString(ui->user_chat_lib->text().left(numCharsToShow) + "..."));
    }
    if(ui->user_name_lb->text().length()>numCharsToShow){
        ui->user_name_lb->setText(QString(ui->user_name_lb->text().left(numCharsToShow)+"..."));
    }

    ui->user_name_lb->setText(m_name);
    ui->user_chat_lib->setText(m_msg);
}
