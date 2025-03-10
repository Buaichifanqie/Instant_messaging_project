#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"

namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();

    QSize sizeHint()const override
    {
        return QSize(250,70);
    }

    void SetInfo(std::shared_ptr<UserInfo> user_info);

private:
    Ui::ChatUserWid *ui;
    std::shared_ptr<UserInfo>m_user_info;
};

#endif // CHATUSERWID_H
