/****************************************************************************
** Meta object code from reading C++ file 'SystemLog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Bll/SystemLog/SystemLog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SystemLog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SystemLog_t {
    QByteArrayData data[15];
    char stringdata[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SystemLog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SystemLog_t qt_meta_stringdata_SystemLog = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 11),
QT_MOC_LITERAL(2, 22, 0),
QT_MOC_LITERAL(3, 23, 11),
QT_MOC_LITERAL(4, 35, 6),
QT_MOC_LITERAL(5, 42, 8),
QT_MOC_LITERAL(6, 51, 9),
QT_MOC_LITERAL(7, 61, 18),
QT_MOC_LITERAL(8, 80, 7),
QT_MOC_LITERAL(9, 88, 7),
QT_MOC_LITERAL(10, 96, 11),
QT_MOC_LITERAL(11, 108, 14),
QT_MOC_LITERAL(12, 123, 12),
QT_MOC_LITERAL(13, 136, 5),
QT_MOC_LITERAL(14, 142, 4)
    },
    "SystemLog\0dataEntered\0\0MoSystemLog\0"
    "append\0logTitle\0logDetail\0SystemLog::LogType\0"
    "logType\0isEmpty\0getFirstLog\0removeFirstLog\0"
    "takeFirstLog\0clear\0size"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SystemLog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    3,   57,    2, 0x0a /* Public */,
       9,    0,   64,    2, 0x0a /* Public */,
      10,    0,   65,    2, 0x0a /* Public */,
      11,    0,   66,    2, 0x0a /* Public */,
      12,    0,   67,    2, 0x0a /* Public */,
      13,    0,   68,    2, 0x0a /* Public */,
      14,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 7,    5,    6,    8,
    QMetaType::Bool,
    0x80000000 | 3,
    QMetaType::Void,
    0x80000000 | 3,
    QMetaType::Void,
    QMetaType::Int,

       0        // eod
};

void SystemLog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SystemLog *_t = static_cast<SystemLog *>(_o);
        switch (_id) {
        case 0: _t->dataEntered((*reinterpret_cast< MoSystemLog(*)>(_a[1]))); break;
        case 1: _t->append((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< SystemLog::LogType(*)>(_a[3]))); break;
        case 2: { bool _r = _t->isEmpty();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { MoSystemLog _r = _t->getFirstLog();
            if (_a[0]) *reinterpret_cast< MoSystemLog*>(_a[0]) = _r; }  break;
        case 4: _t->removeFirstLog(); break;
        case 5: { MoSystemLog _r = _t->takeFirstLog();
            if (_a[0]) *reinterpret_cast< MoSystemLog*>(_a[0]) = _r; }  break;
        case 6: _t->clear(); break;
        case 7: { int _r = _t->size();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SystemLog::*_t)(MoSystemLog );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SystemLog::dataEntered)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject SystemLog::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SystemLog.data,
      qt_meta_data_SystemLog,  qt_static_metacall, 0, 0}
};


const QMetaObject *SystemLog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SystemLog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SystemLog.stringdata))
        return static_cast<void*>(const_cast< SystemLog*>(this));
    return QObject::qt_metacast(_clname);
}

int SystemLog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void SystemLog::dataEntered(MoSystemLog _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
