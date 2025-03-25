#ifndef LOGICMGR_H
#define LOGICMGR_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <functional>
#include "tcpclient.h"
#include "global.h"
#include "Singleton.h"

class LogicWorker:public QObject
{
    Q_OBJECT
public:
    explicit LogicWorker(QObject* parent=nullptr);
    void InitHandlers();
    QMap<quint32, std::function<void(QJsonObject obj)> >m_handlers;

signals:
    void sig_trans_size(int trans_size);
public slots:
    void slot_logic_process(quint16 msgid, QJsonObject obj);
};

class Logicmgr : public QObject,public Singleton<Logicmgr>
{
    Q_OBJECT
public:
    friend class Singleton<Logicmgr>;
    ~Logicmgr();
private:
    Logicmgr();
    LogicWorker* m_worker;
    QThread* m_work_thread;
signals:
    void sig_logic_process(quint16 msgid, QJsonObject obj);
    void sig_trans_size(int trans_size);
};

#endif //LOGICMGR_H
