#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H
#include <QListWidget>
#include <QObject>
#include <QWheelEvent>
#include <QDebug>
#include <QScrollBar>

class ChatUserList:public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget* parent=nullptr);
protected:
    //事件过滤器 实现滚动时的侧边栏的条
    bool eventFilter(QObject* watched,QEvent* event) override;
signals:
    void sig_loading_chat_user();
};

#endif // CHATUSERLIST_H
