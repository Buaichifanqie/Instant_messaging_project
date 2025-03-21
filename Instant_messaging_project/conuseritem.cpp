#include "conuseritem.h"
#include "ui_conuseritem.h"



ConUserItem::ConUserItem(QWidget *parent):ListItemBase(parent),ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250,70);
}

void ConUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
{
    m_info=std::make_shared<UserInfo>(auth_info);
    //加载图片
    QPixmap pixmap(m_info->m_icon);

    //设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_info->m_name);
}

void ConUserItem::SetInfo(std::shared_ptr<AuthRsp> auth_rsp)
{
    m_info=std::make_shared<UserInfo>(auth_rsp);
    //加载图片
    QPixmap pixmap(m_info->m_icon);

    //设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_info->m_name);
}

void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
    m_info=std::make_shared<UserInfo>(uid,name,icon);
    //加载图片
    QPixmap pixmap(m_info->m_icon);

    //设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_info->m_name);
}

void ConUserItem::ShowRedPoint(bool show)
{
    if(show)
    {
        ui->red_point->show();
    }
    else
    {
        ui->red_point->hide();
    }
}

std::shared_ptr<UserInfo> ConUserItem::GetInfo()
{
    return m_info;
}
