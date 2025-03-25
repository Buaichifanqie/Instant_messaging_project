#include "global.h"
#include <QDateTime>
#include <QRandomGenerator>

QString generateUniqueId()
{
    //获取当前时间戳
    QString timestamp=QString::number(QDateTime::currentMSecsSinceEpoch());
    //生成一个随机数
    QString randomPart=QString::number(QRandomGenerator::global()->generate());
    //组合时间戳和随机数
    return timestamp+"_"+randomPart;
}
