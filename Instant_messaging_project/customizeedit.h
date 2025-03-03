#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QDebug>
#include <QLineEdit>

class CustomizeEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget *parent=nullptr);
    void SetMaxLength(int maxLen);
protected:
    void focusOutEvent(QFocusEvent* event) override
    {
        //执行失去焦点处理逻辑
        //qDebug()<<"CustomizeEdit focusout";
        //调用基类的foucOutEvent 方法保证基类行为得到执行
        QLineEdit::focusOutEvent(event);
        //发送失去焦点的信号
        emit sig_foucus_out();
    }
private:
    void limitTextLength(QString text)
    {
        if(m_max_len<=0)
        {
            return;
        }
        //统一转成字节流
        QByteArray byteArray=text.toUtf8();
        if(byteArray.size()>m_max_len)
        {
            byteArray=byteArray.left(m_max_len);
            this->setText(QString::fromUtf8(byteArray));
        }
    }

    int m_max_len;
signals:
    void sig_foucus_out();
};

#endif // CUSTOMIZEEDIT_H
