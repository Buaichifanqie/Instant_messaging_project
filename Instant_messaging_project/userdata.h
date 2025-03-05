#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
// class UserData
// {
// public:
//     UserData();
// };

class SearchInfo{
public:
    SearchInfo(int uid,QString name,QString nick,QString desc,int sex);
    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_desc;
    int m_sex;
};


#endif // USERDATA_H
