#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>
#include <QRandomGenerator>
#include <chatuserwid.h>
#include "loadingdlg.h"
#include <QMouseEvent>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog),m_mode(ChatUIMode::ChatMode),m_state(ChatUIMode::ChatMode)
    ,m_b_loading(false)
{
    ui->setupUi(this);
    ui->add_btn->SetState("normal","hover","press");
    ui->search_edit->SetMaxLength(15);

    //放大镜搜索小图标
    QAction* searchAction=new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/rec/search.png"));
    ui->search_edit->addAction(searchAction,QLineEdit::LeadingPosition);//头部
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));
    //实现快速删除的小图标
    QAction* clearAction=new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/rec/close_transparent.png"));
    //初始时不显示清除u图标
    //清除动作添加到LineEdit的末尾位置
    ui->search_edit->addAction(clearAction,QLineEdit::TrailingPosition);

    //当需要显示清除图标时，更改为实际的清楚图标
    connect(ui->search_edit,&QLineEdit::textChanged,[clearAction](const QString &text){
        if(!text.isEmpty())
        {
            clearAction->setIcon(QIcon(":/rec/close_search.png"));
        }
        else
        {
            clearAction->setIcon(QIcon(":/rec/close_transparent.png"));//文本为空时 切换为透明的
        }
    });

    //连接触发信号到信号槽 用于清楚文本
    connect(clearAction,&QAction::triggered,[this,clearAction](){
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/rec/close_transparent.png"));
        ui->search_edit->clearFocus();
        // 清楚按钮按下则不显示搜索框
        ShowSearch(false);
    });

    ui->search_edit->SetMaxLength(15);

    ShowSearch(false);
    //连接加载信号和槽
    connect(ui->chat_user_list,&ChatUserList::sig_loading_chat_user,this,
            &ChatDialog::slot_loading_chat_user);
    addChatUserList();

    QPixmap pixmap(":/rec/head_2.jpg");
    ui->side_head_lb->setPixmap(pixmap);//把图片设置到QLabel上
    QPixmap scaledPixmap = pixmap.scaled( ui->side_head_lb->size(), Qt::KeepAspectRatio); // 将图片缩放到label的大小
    ui->side_head_lb->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
    ui->side_head_lb->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小

    ui->side_chat_lb->setProperty("state","normal");
    ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

    AddLBGroup(ui->side_chat_lb);
    AddLBGroup(ui->side_contact_lb);
    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);

    //链接搜索框输入变化
    connect(ui->search_edit,&QLineEdit::textChanged,this,&ChatDialog::slot_text_changed);

    //检测鼠标点击的位置判断是否需要清空搜索框
    this->installEventFilter(this);//安装事件过滤器

    //设置聊天选中状态
    ui->side_chat_lb->SetSelected(true);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}


std::vector<QString>  strs ={"hello world !",
                             "nice to meet u",
                             "New year，new life",
                             "You have to love yourself",
                             "My love is written in the wind ever since the whole world is you"};
std::vector<QString> heads = {
    ":/rec/head_1.jpg",
    ":/rec/head_2.jpg",
    ":/rec/head_3.jpg",
    ":/rec/head_4.jpg",
    ":/rec/head_5.jpg"
};
std::vector<QString> names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};

void ChatDialog::addChatUserList()
{
    // 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();
        auto *chat_user_wid = new ChatUserWid();
        chat_user_wid->SetInfo(names[name_i], heads[head_i], strs[str_i]);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}

void ChatDialog::ClearLabelState(StateWidget *lb)
{
    for(auto& ele:m_lb_list)
    {
        if(ele==lb)
        {
            continue;
        }
        ele->ClearState();
    }
}

bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::MouseButtonPress)
    {
        //事件转换成鼠标事件
        QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
        //处理鼠标事件
        handleGlobalMousePress(mouseEvent);
    }
    return QDialog::eventFilter(watched,event);
}

void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    //实现点击位置的判断和逻辑
    //先判断释放处于搜索 如果不处于搜索模式直接返回
    if(m_mode!=ChatUIMode::SearchMode)
    {
        return;
    }
    //将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList=ui->search_list->mapFromGlobal(event->globalPos());
    //判断点击位置是否在搜索列表范围呢
    if(!ui->search_list->rect().contains(posInSearchList))
    {
        //如果不在聊天列表内 清空搜索框
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::AddLBGroup(StateWidget *lb)
{
    m_lb_list.push_back(lb);
}

void ChatDialog::ShowSearch(bool bsearch)
{
    if(bsearch)
    {
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        m_mode=ChatUIMode::SearchMode;
    }
    else if(m_state==ChatUIMode::ChatMode)
    {
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        m_mode=ChatUIMode::ChatMode;
    }
    else if(m_state==ChatUIMode::ContactMode)
    {
        ui->chat_user_list->hide();
        ui->con_user_list->show();
        ui->search_list->hide();
        m_mode=ChatUIMode::ContactMode;
    }
}

void ChatDialog::slot_loading_chat_user()
{
    if(m_b_loading)
    {
        return;
    }
    m_b_loading=true;
    LoadingDlg* loadingDialog=new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug()<<"add new data to list.....";
    addChatUserList();
    //加载完成后关闭对话框
    loadingDialog->deleteLater();

    m_b_loading=false;
}

void ChatDialog::slot_side_chat()
{
    qDebug()<<"recive side chat clicked";
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    m_state=ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact()
{
    qDebug() << "receive side contact clicked";
    ClearLabelState(ui->side_contact_lb);
    // 设置
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    m_state = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_text_changed(const QString &str)
{
    if(!str.isEmpty())
    {
        ShowSearch(true);
    }
}
