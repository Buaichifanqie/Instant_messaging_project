#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

#include <QTextEdit>
#include <QBoxLayout>
#include "bubbleframe.h"

class TextBubble:public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(ChatRole role,const QString &text,QWidget* parent=nullptr);
protected:
    bool eventFilter(QObject* o,QEvent *e);
private:
    void adjustTextHeight();
    void setPlainText(const QString &text);
    void initStyleSheet();
private:
    QTextEdit* m_pTextEdit;
};

#endif // TEXTBUBBLE_H
