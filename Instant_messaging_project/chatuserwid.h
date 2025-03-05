#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include "listitembase.h"

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

    void SetInfo(QString name,QString head,QString msg);

private:
    Ui::ChatUserWid *ui;
    QString m_name;
    QString m_head;
    QString m_msg;
};

#endif // CHATUSERWID_H
