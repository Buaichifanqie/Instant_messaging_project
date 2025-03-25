/****************************************************************************
** Meta object code from reading C++ file 'logicmgr.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../logicmgr.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'logicmgr.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LogicWorker_t {
    QByteArrayData data[7];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LogicWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LogicWorker_t qt_meta_stringdata_LogicWorker = {
    {
QT_MOC_LITERAL(0, 0, 11), // "LogicWorker"
QT_MOC_LITERAL(1, 12, 14), // "sig_trans_size"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 10), // "trans_size"
QT_MOC_LITERAL(4, 39, 18), // "slot_logic_process"
QT_MOC_LITERAL(5, 58, 5), // "msgid"
QT_MOC_LITERAL(6, 64, 3) // "obj"

    },
    "LogicWorker\0sig_trans_size\0\0trans_size\0"
    "slot_logic_process\0msgid\0obj"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LogicWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::UShort, QMetaType::QJsonObject,    5,    6,

       0        // eod
};

void LogicWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LogicWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sig_trans_size((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->slot_logic_process((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< QJsonObject(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LogicWorker::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LogicWorker::sig_trans_size)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LogicWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LogicWorker.data,
    qt_meta_data_LogicWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LogicWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LogicWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LogicWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LogicWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void LogicWorker::sig_trans_size(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_Logicmgr_t {
    QByteArrayData data[7];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Logicmgr_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Logicmgr_t qt_meta_stringdata_Logicmgr = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Logicmgr"
QT_MOC_LITERAL(1, 9, 17), // "sig_logic_process"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 5), // "msgid"
QT_MOC_LITERAL(4, 34, 3), // "obj"
QT_MOC_LITERAL(5, 38, 14), // "sig_trans_size"
QT_MOC_LITERAL(6, 53, 10) // "trans_size"

    },
    "Logicmgr\0sig_logic_process\0\0msgid\0obj\0"
    "sig_trans_size\0trans_size"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Logicmgr[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06 /* Public */,
       5,    1,   29,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UShort, QMetaType::QJsonObject,    3,    4,
    QMetaType::Void, QMetaType::Int,    6,

       0        // eod
};

void Logicmgr::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Logicmgr *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sig_logic_process((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< QJsonObject(*)>(_a[2]))); break;
        case 1: _t->sig_trans_size((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Logicmgr::*)(quint16 , QJsonObject );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Logicmgr::sig_logic_process)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Logicmgr::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Logicmgr::sig_trans_size)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Logicmgr::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Logicmgr.data,
    qt_meta_data_Logicmgr,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Logicmgr::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Logicmgr::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Logicmgr.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Singleton<Logicmgr>"))
        return static_cast< Singleton<Logicmgr>*>(this);
    return QObject::qt_metacast(_clname);
}

int Logicmgr::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Logicmgr::sig_logic_process(quint16 _t1, QJsonObject _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Logicmgr::sig_trans_size(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
