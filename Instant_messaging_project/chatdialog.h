#ifndef CHATDIALOG_H
#define CHATDIALOG_H
#include "global.h"
#include "userdata.h"
#include <QDialog>
#include <statewidget.h>
#include <QListWidgetItem>
namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    //测试
    void addChatUserList();
    void ClearLabelState(StateWidget* lb);

protected:
    bool eventFilter(QObject* watched,QEvent* event)override;
    void handleGlobalMousePress(QMouseEvent* event);
private:
    void AddLBGroup(StateWidget* lb);
    void ShowSearch(bool bsearch=false);
    Ui::ChatDialog *ui;
private:
    ChatUIMode m_mode;
    ChatUIMode m_state;
    bool m_b_loading;
    QList <StateWidget*>m_lb_list;
    QMap<int,QListWidgetItem*> m_chat_items_added;
private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString& str);


public slots:
    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply);
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
};

#endif // CHATDIALOG_H
