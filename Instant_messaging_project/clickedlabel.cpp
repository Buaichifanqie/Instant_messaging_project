#include "clickedlabel.h"
#include <QMouseEvent>
ClickedLabel::ClickedLabel(QWidget* parent):QLabel (parent),m_curstate(ClickLbState::Normal)
{
    this->setCursor(Qt::PointingHandCursor);
}
// 处理鼠标点击事件
void ClickedLabel::mousePressEvent(QMouseEvent* event)  {
    if (event->button() == Qt::LeftButton) {
        if(m_curstate == ClickLbState::Normal){
            qDebug()<<"clicked , change to selected hover: "<< m_selected_hover;
            m_curstate = ClickLbState::Selected;
            setProperty("state",m_selected_press);
            repolish(this);
            update();
        }else{
            qDebug()<<"clicked , change to normal hover: "<< m_normal_hover;
            m_curstate = ClickLbState::Normal;
            setProperty("state",m_normal_press);
            repolish(this);
            update();
        }
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(m_curstate == ClickLbState::Normal){
            qDebug()<<"clicked , change to selected hover: "<< m_selected_hover;
            m_curstate = ClickLbState::Selected;
            setProperty("state",m_selected_hover);
            repolish(this);
            update();
        }else{
            qDebug()<<"clicked , change to normal hover: "<< m_normal_hover;
            m_curstate = ClickLbState::Normal;
            setProperty("state",m_normal_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(),m_curstate);
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}
// 处理鼠标悬停进入事件
void ClickedLabel::enterEvent(QEvent* event) {
    // 在这里处理鼠标悬停进入的逻辑
    if(m_curstate == ClickLbState::Normal){
        qDebug()<<"enter , change to normal hover: "<< m_normal_hover;
        setProperty("state",m_normal_hover);
        repolish(this);
        update();
    }else{
        qDebug()<<"enter , change to selected hover: "<< m_selected_hover;
        setProperty("state",m_selected_hover);
        repolish(this);
        update();
    }
    QLabel::enterEvent(event);
}
// 处理鼠标悬停离开事件
void ClickedLabel::leaveEvent(QEvent* event){
    // 在这里处理鼠标悬停离开的逻辑
    if(m_curstate == ClickLbState::Normal){
        qDebug()<<"leave , change to normal : "<<m_normal;
        setProperty("state",m_normal);
        repolish(this);
        update();
    }else{
        qDebug()<<"leave , change to normal hover: "<< m_selected;
        setProperty("state",m_selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}
void ClickedLabel::SetState(QString normal, QString hover, QString press,
                            QString select, QString select_hover, QString select_press)
{
    m_normal = normal;
    m_normal_hover = hover;
    m_normal_press = press;
    m_selected = select;
    m_selected_hover = select_hover;
    m_selected_press = select_press;
    setProperty("state",normal);
    repolish(this);
}
ClickLbState ClickedLabel::GetCurState(){
    return m_curstate;
}

bool ClickedLabel::SetCurState(ClickLbState state)
{
    m_curstate=state;
    if(m_curstate==ClickLbState::Normal)
    {
        setProperty("state",m_normal);
        repolish(this);
    }
    else if(m_curstate==ClickLbState::Selected)
    {
        setProperty("state",m_selected);
        repolish(this);
    }
    return true;
}

void ClickedLabel::ResetNormalState()
{
    m_curstate=ClickLbState::Normal;
    setProperty("state",m_normal);
    repolish(this);
}
