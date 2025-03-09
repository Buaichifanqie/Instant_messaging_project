#include "searchlist.h"
#include <QJsonDocument>
#include <QScrollBar>
#include "tcpmgr.h"
#include "customizeedit.h"
#include "loadingdlg.h"
#include "adduseritem.h"
#include "findsuccessdlg.h"
#include "userdata.h"
#include "findfaildlg.h"

SearchList::SearchList(QWidget *parent):QListWidget(parent),m_find_dlg(nullptr), m_search_edit(nullptr), m_send_pending(false)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目
    addTipItem();
    //连接搜索条目
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::CloseFindDlg()
{
    if(m_find_dlg)
    {
        m_find_dlg->hide();
        m_find_dlg=nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{
    m_search_edit=edit;
}

void SearchList::waitPending(bool pending)
{
    if(pending)
    {
        m_loadingDialog=new LoadingDlg(this);
        m_loadingDialog->setModal(true);
        m_loadingDialog->show();
        m_send_pending=pending;
    }
    else
    {
        m_loadingDialog->hide();
        m_loadingDialog->deleteLater();
        m_send_pending=pending;
    }
}

void SearchList::addTipItem()
{
    // 创建一个无效的QWidget对象，用于占位或特殊用途
    auto *invalid_item = new QWidget();
    // 创建一个QListWidgetItem对象，用于添加到QListWidget中
    QListWidgetItem *item_tmp = new QListWidgetItem;
    // 设置item_tmp的大小为250x10像素
    item_tmp->setSizeHint(QSize(250, 10));
    // 将item_tmp添加到QListWidget中
    this->addItem(item_tmp);
    // 为invalid_item设置对象名称，方便后续识别或样式设置
    invalid_item->setObjectName("invalid_item");
    // 将invalid_item设置为item_tmp的部件（即item_tmp显示的内容是invalid_item）
    this->setItemWidget(item_tmp, invalid_item);
    // 设置item_tmp为不可选中状态
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);

    // 创建一个AddUserItem对象，用于添加用户项
    auto *add_user_item = new AddUserItem();
    // 创建另一个QListWidgetItem对象，用于添加到QListWidget中
    QListWidgetItem *item = new QListWidgetItem;
    // 设置item的大小为add_user_item的推荐大小
    item->setSizeHint(add_user_item->sizeHint());
    // 将item添加到QListWidget中
    this->addItem(item);
    // 将add_user_item设置为item的部件（即item显示的内容是add_user_item）
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* widget=this->itemWidget(item);//获取自定义widget对象
    if(!widget)
    {
        qDebug()<<"slot item clicked widget is nullptr";
        return;
    }
    //对自定义的widget进行操作，将item转化为基类ListItemBase
    ListItemBase* customItem=qobject_cast<ListItemBase*>(widget);
    if(!customItem)
    {
        qDebug()<<"slot item clicked widget is nullptr";
        return;
    }

    auto itemType=customItem->GetItemType();
    if(itemType==ListItemType::INVALID_ITEM)
    {
        qDebug()<<"slot invalid item clicked";
        return;
    }

    if(itemType==ListItemType::ADD_USER_TIP_ITEM)
    {
        if(m_send_pending)
        {
            return;
        }

        if(!m_search_edit)
        {
            return;
        }

        waitPending(true);
        auto search_edit=dynamic_cast<CustomizeEdit*>(m_search_edit);
        auto uid_str=search_edit->text();

        QJsonObject jsonObj;
        jsonObj["uid"]=uid_str;

        QJsonDocument doc(jsonObj);
        QByteArray jsonData=doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_SEARCH_USER_REQ,jsonData);

        return;
    }

    //清除弹出框
    CloseFindDlg();

}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    waitPending(false);
    if(si==nullptr)
    {
        m_find_dlg=std::make_shared<FindFailDlg>(this);
    }
    else
    {
        //两种情况 一种是搜的都是自己的朋友了 一种是未添加好友
        //查找是否已经是好友了 todo....
        m_find_dlg=std::make_shared<FindSuccessDlg>(this);
        std::dynamic_pointer_cast<FindSuccessDlg>(m_find_dlg)->SetSearchInfo(si);
    }
    m_find_dlg->show();
}
