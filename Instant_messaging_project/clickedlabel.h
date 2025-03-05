#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>
#include "global.h"


class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget *parent=nullptr);
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");
    ClickLbState GetCurState();
private:
    QString m_normal;
    QString m_normal_hover;
    QString m_normal_press;

    QString m_selected;
    QString m_selected_hover;
    QString m_selected_press;

    ClickLbState m_curstate;
signals:
    void clicked(void);

};

#endif // CLICKEDLABEL_H
