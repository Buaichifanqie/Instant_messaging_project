#include "customizeedit.h"


CustomizeEdit::CustomizeEdit(QWidget *parent):QLineEdit(parent),m_max_len(0)
{
    connect(this,&QLineEdit::textChanged,this,&CustomizeEdit::limitTextLength);
}

void CustomizeEdit::SetMaxLength(int maxLen)
{
    m_max_len=maxLen;
}
