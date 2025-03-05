#include "chatview.h"
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>

ChatView::ChatView(QWidget *parent):QWidget(parent)
    ,isAppended(false)
{
    // 创建一个 QVBoxLayout 对象，用于管理窗口中的布局
    QVBoxLayout* pMainLayout = new QVBoxLayout();
    // 将创建的布局设置为当前窗口的主布局
    this->setLayout(pMainLayout);
    // 将主布局的边距设置为 0，以去除布局周围的空白区域
    pMainLayout->setMargin(0);


    // 创建一个 QScrollArea 对象，用于显示可滚动的内容
    m_pScrollArea = new QScrollArea();
    // 设置 QScrollArea 的对象名称，便于在样式表（QSS）中对其进行样式设置
    m_pScrollArea->setObjectName("chat_area");
    // 将 QScrollArea 添加到主布局中，使其成为窗口的一部分
    pMainLayout->addWidget(m_pScrollArea);

    // 创建一个新的 QWidget 对象，并将其父对象设置为当前类的实例。
    // 这个 QWidget 将作为聊天背景的容器。
    QWidget* w = new QWidget(this);
    // 设置该 QWidget 的对象名称为 "chat_bg"，便于在样式表（QSS）中通过名称进行样式设置。
    w->setObjectName("chat_bg");
    // 启用自动填充背景，确保该 QWidget 的背景可以被样式表或调色板设置。
    w->setAutoFillBackground(true);
    // 创建一个新的 QVBoxLayout 布局对象，用于管理聊天背景容器中的子控件。
    QVBoxLayout* pHLayout_1 = new QVBoxLayout();
    // 在布局中添加一个占位的 QWidget，并设置其伸展因子为 100000。
    // 这个占位 QWidget 的作用是让布局在垂直方向上可以无限扩展，从而实现滚动区域的动态扩展。
    pHLayout_1->addWidget(new QWidget(), 100000);
    // 将创建的 QVBoxLayout 布局设置为聊天背景容器 w 的布局。
    w->setLayout(pHLayout_1);
    // 将聊天背景容器 w 设置为滚动区域（QScrollArea）的滚动内容。
    // 这样，当聊天内容超出容器大小时，滚动区域会自动显示滚动条。
    m_pScrollArea->setWidget(w);
    // 设置滚动区域的垂直滚动条策略为 "始终关闭"。
    // 这意味着垂直滚动条不会显示，滚动行为可能通过其他方式（如代码控制）实现。
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 获取滚动区域的垂直滚动条对象。
    QScrollBar* pVScrollBar = m_pScrollArea->verticalScrollBar();
    // 连接垂直滚动条的 rangeChanged 信号到当前类的 onVScrollBarMoved 槽函数。
    // 当滚动条的范围发生变化时（例如，聊天内容更新导致滚动范围改变），会调用 onVScrollBarMoved 函数，
    // 可以在这个函数中实现滚动条的自动滚动或其他逻辑。
    connect(pVScrollBar, &QScrollBar::rangeChanged, this, &ChatView::onVScrollBarMoved);

    // 以下代码的功能是将垂直滚动条放置到滚动区域的上方，而不是默认的并排位置
    // 同时对滚动区域进行一些初始化设置

    // 创建一个水平布局管理器 pHLayout_2，用于管理滚动区域和垂直滚动条的布局
    QHBoxLayout *pHLayout_2 = new QHBoxLayout();
    // 将垂直滚动条 pVScrollBar 添加到水平布局中，并且将其对齐到右侧
    // 第二个参数 0 表示该滚动条在布局中所占的伸缩系数为 0，即不会根据布局大小变化而伸缩
    // Qt::AlignRight 表示将滚动条对齐到水平布局的右侧
    pHLayout_2->addWidget(pVScrollBar, 0, Qt::AlignRight);
    // 设置水平布局的外边距为 0，即布局与周围元素之间没有间距
    pHLayout_2->setMargin(0);
    // 将创建好的水平布局 pHLayout_2 设置为滚动区域 m_pScrollArea 的布局
    // 这样滚动区域和垂直滚动条就会按照水平布局的规则进行排列
    m_pScrollArea->setLayout(pHLayout_2);
    // 隐藏垂直滚动条，使其在界面上不可见
    // 可能后续会根据特定条件再显示该滚动条
    pVScrollBar->setHidden(true);
    // 设置滚动区域的子控件可以根据滚动区域的大小自动调整大小
    // 当滚动区域的大小发生变化时，其内部的子控件会相应地进行缩放
    m_pScrollArea->setWidgetResizable(true);
    // 为滚动区域 m_pScrollArea 安装事件过滤器，事件过滤器为当前对象（this）
    // 这样当前对象就可以拦截并处理滚动区域的各种事件，例如鼠标事件、键盘事件等
    m_pScrollArea->installEventFilter(this);
    // 调用 initStyleSheet 函数，用于初始化界面的样式表
    // 样式表可以控制界面元素的外观，如颜色、字体、边框等
    initStyleSheet();

}

void ChatView::appendChatItem(QWidget *item)
{
    QVBoxLayout* vl=qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    vl->insertWidget(vl->count()-1,item);
    isAppended=true;
}

void ChatView::prependChatItem(QWidget *item)
{

}

void ChatView::insertChatItem(QWidget *item)
{

}

bool ChatView::eventFilter(QObject *o, QEvent *e)
{
    if(e->type()==QEvent::Enter && o==m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum()==0);
    }
    else if(e->type()==QEvent::Leave && o==m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(o,e);
}

void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

void ChatView::onVScrollBarMoved(int min, int max)
{
    if(isAppended)
    {
        QScrollBar* pVScrollBar=m_pScrollArea->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        //500毫秒可能调用多次
        QTimer::singleShot(500,[this](){
            isAppended=false;
        });
    }
}

void ChatView::initStyleSheet()
{

}
