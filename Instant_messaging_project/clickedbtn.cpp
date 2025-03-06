#include "clickedbtn.h"
#include "global.h"
#include <QVariant>

ClickedBtn::ClickedBtn(QWidget *parent): QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
}

ClickedBtn::~ClickedBtn()
{

}

void ClickedBtn::SetState(QString normal, QString hover, QString press)
{
    m_hover = hover;
    m_normal = normal;
    m_press = press;
    setProperty("state", normal);
    repolish(this);
    update();
}

void ClickedBtn::enterEvent(QEvent *event)
{
    setProperty("state", m_hover);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void ClickedBtn::leaveEvent(QEvent *event)
{
    setProperty("state", m_normal); // 修复这里
    repolish(this);
    update();
    QPushButton::leaveEvent(event); // 修复这里
}

void ClickedBtn::mousePressEvent(QMouseEvent *event)
{
    setProperty("state", m_press);
    repolish(this);
    update();
    QPushButton::mousePressEvent(event);
}

void ClickedBtn::mouseReleaseEvent(QMouseEvent *event)
{
    setProperty("state", m_normal); // 修复这里
    repolish(this);
    update();
    QPushButton::mouseReleaseEvent(event);
}

void ClickedBtn::repolish(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}
