/****************************************************************************
** Meta object code from reading C++ file 'BllRealTimeTrans.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Bll/BllRealTimeTrans.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BllRealTimeTrans.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BllRealTimeTrans_t {
    QByteArrayData data[37];
    char stringdata[498];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BllRealTimeTrans_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BllRealTimeTrans_t qt_meta_stringdata_BllRealTimeTrans = {
    {
QT_MOC_LITERAL(0, 0, 16),
QT_MOC_LITERAL(1, 17, 12),
QT_MOC_LITERAL(2, 30, 0),
QT_MOC_LITERAL(3, 31, 6),
QT_MOC_LITERAL(4, 38, 13),
QT_MOC_LITERAL(5, 52, 8),
QT_MOC_LITERAL(6, 61, 10),
QT_MOC_LITERAL(7, 72, 16),
QT_MOC_LITERAL(8, 89, 11),
QT_MOC_LITERAL(9, 101, 11),
QT_MOC_LITERAL(10, 113, 6),
QT_MOC_LITERAL(11, 120, 10),
QT_MOC_LITERAL(12, 131, 12),
QT_MOC_LITERAL(13, 144, 18),
QT_MOC_LITERAL(14, 163, 13),
QT_MOC_LITERAL(15, 177, 7),
QT_MOC_LITERAL(16, 185, 19),
QT_MOC_LITERAL(17, 205, 14),
QT_MOC_LITERAL(18, 220, 8),
QT_MOC_LITERAL(19, 229, 20),
QT_MOC_LITERAL(20, 250, 14),
QT_MOC_LITERAL(21, 265, 10),
QT_MOC_LITERAL(22, 276, 12),
QT_MOC_LITERAL(23, 289, 5),
QT_MOC_LITERAL(24, 295, 10),
QT_MOC_LITERAL(25, 306, 11),
QT_MOC_LITERAL(26, 318, 14),
QT_MOC_LITERAL(27, 333, 11),
QT_MOC_LITERAL(28, 345, 4),
QT_MOC_LITERAL(29, 350, 14),
QT_MOC_LITERAL(30, 365, 20),
QT_MOC_LITERAL(31, 386, 17),
QT_MOC_LITERAL(32, 404, 13),
QT_MOC_LITERAL(33, 418, 16),
QT_MOC_LITERAL(34, 435, 22),
QT_MOC_LITERAL(35, 458, 25),
QT_MOC_LITERAL(36, 484, 13)
    },
    "BllRealTimeTrans\0statuChanged\0\0status\0"
    "connectToHost\0serverIp\0serverPort\0"
    "disconnectToHost\0clientLogin\0handleLogin\0"
    "result\0descriptor\0requestHorse\0"
    "handleRequestHorse\0requestRaceID\0"
    "session\0handleRequestRaceID\0submitRaceTime\0"
    "raceTime\0handleSubmitRaceTime\0"
    "submitRealData\0DataOutput\0outputStruct\0"
    "array\0imageWidth\0imageHeight\0"
    "submitWINOrPLA\0DataOutput&\0type\0"
    "submitQINOrQPL\0handleSubmitRealData\0"
    "handleReceiveData\0handleConnect\0"
    "handleDisConnect\0sendBufferDataToServer\0"
    "requestRaceIDForKeepAlive\0submitRaceEnd"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BllRealTimeTrans[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  119,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,  122,    2, 0x0a /* Public */,
       7,    0,  127,    2, 0x0a /* Public */,
       8,    0,  128,    2, 0x0a /* Public */,
       9,    2,  129,    2, 0x0a /* Public */,
      12,    0,  134,    2, 0x0a /* Public */,
      13,    2,  135,    2, 0x0a /* Public */,
      14,    1,  140,    2, 0x0a /* Public */,
      16,    2,  143,    2, 0x0a /* Public */,
      17,    1,  148,    2, 0x0a /* Public */,
      19,    2,  151,    2, 0x0a /* Public */,
      20,    4,  156,    2, 0x0a /* Public */,
      26,    2,  165,    2, 0x0a /* Public */,
      29,    2,  170,    2, 0x0a /* Public */,
      30,    2,  175,    2, 0x0a /* Public */,
      31,    2,  180,    2, 0x0a /* Public */,
      32,    0,  185,    2, 0x0a /* Public */,
      33,    0,  186,    2, 0x0a /* Public */,
      34,    0,  187,    2, 0x0a /* Public */,
      35,    1,  188,    2, 0x0a /* Public */,
      36,    0,  191,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Int,   10,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Int,   10,   11,
    QMetaType::Void, 0x80000000 | 21, QMetaType::QByteArray, QMetaType::Int, QMetaType::Int,   22,   23,   24,   25,
    QMetaType::Int, 0x80000000 | 27, QMetaType::QString,   22,   28,
    QMetaType::Int, 0x80000000 | 27, QMetaType::QString,   22,   28,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Int,   10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,

       0        // eod
};

void BllRealTimeTrans::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BllRealTimeTrans *_t = static_cast<BllRealTimeTrans *>(_o);
        switch (_id) {
        case 0: _t->statuChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->connectToHost((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< qint32(*)>(_a[2]))); break;
        case 2: _t->disconnectToHost(); break;
        case 3: _t->clientLogin(); break;
        case 4: _t->handleLogin((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->requestHorse(); break;
        case 6: _t->handleRequestHorse((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->requestRaceID((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->handleRequestRaceID((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->submitRaceTime((*reinterpret_cast< qint32(*)>(_a[1]))); break;
        case 10: _t->handleSubmitRaceTime((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->submitRealData((*reinterpret_cast< DataOutput(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 12: { int _r = _t->submitWINOrPLA((*reinterpret_cast< DataOutput(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 13: { int _r = _t->submitQINOrQPL((*reinterpret_cast< DataOutput(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 14: _t->handleSubmitRealData((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->handleReceiveData((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 16: _t->handleConnect(); break;
        case 17: _t->handleDisConnect(); break;
        case 18: _t->sendBufferDataToServer(); break;
        case 19: _t->requestRaceIDForKeepAlive((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->submitRaceEnd(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BllRealTimeTrans::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BllRealTimeTrans::statuChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject BllRealTimeTrans::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BllRealTimeTrans.data,
      qt_meta_data_BllRealTimeTrans,  qt_static_metacall, 0, 0}
};


const QMetaObject *BllRealTimeTrans::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BllRealTimeTrans::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BllRealTimeTrans.stringdata))
        return static_cast<void*>(const_cast< BllRealTimeTrans*>(this));
    return QObject::qt_metacast(_clname);
}

int BllRealTimeTrans::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void BllRealTimeTrans::statuChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
