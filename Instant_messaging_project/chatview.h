#ifndef CHATVIEW_H
#define CHATVIEW_H
#include <QWidget>
#include <QTimer>
#include <QScrollArea>
#include <QEvent>
#include <QVBoxLayout>

class ChatView:public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget* parent=Q_NULLPTR);
    void appendChatItem(QWidget* item); //尾插
    void prependChatItem(QWidget* item); //头插
    void insertChatItem(QWidget* item);//中间插
protected:
    bool eventFilter(QObject* o,QEvent* e) override;
    void paintEvent(QPaintEvent* event) override;
private slots:
    void onVScrollBarMoved(int min,int max);

private:
    void initStyleSheet();
private:
    //QWidget *m_pCenterWidget;
    QVBoxLayout* m_pVl;//垂直布局
    QScrollArea* m_pScrollArea;//滚动区域
    bool isAppended;


};

#endif // CHATVIEW_H
