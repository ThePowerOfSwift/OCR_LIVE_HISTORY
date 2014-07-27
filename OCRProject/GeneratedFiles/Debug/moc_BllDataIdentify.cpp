/****************************************************************************
** Meta object code from reading C++ file 'BllDataIdentify.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Bll/BllDataIdentify.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BllDataIdentify.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BllDataIdentify_t {
    QByteArrayData data[16];
    char stringdata[176];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BllDataIdentify_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BllDataIdentify_t qt_meta_stringdata_BllDataIdentify = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 9),
QT_MOC_LITERAL(2, 26, 0),
QT_MOC_LITERAL(3, 27, 10),
QT_MOC_LITERAL(4, 38, 6),
QT_MOC_LITERAL(5, 45, 9),
QT_MOC_LITERAL(6, 55, 10),
QT_MOC_LITERAL(7, 66, 11),
QT_MOC_LITERAL(8, 78, 12),
QT_MOC_LITERAL(9, 91, 4),
QT_MOC_LITERAL(10, 96, 5),
QT_MOC_LITERAL(11, 102, 8),
QT_MOC_LITERAL(12, 111, 9),
QT_MOC_LITERAL(13, 121, 4),
QT_MOC_LITERAL(14, 126, 21),
QT_MOC_LITERAL(15, 148, 27)
    },
    "BllDataIdentify\0readyRead\0\0DataOutput\0"
    "output\0byteArray\0imageWidth\0imageHeight\0"
    "readyReadBmp\0init\0start\0fileName\0"
    "videoType\0stop\0sessionNumTextChanged\0"
    "sessionCountDownTextChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BllDataIdentify[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   49,    2, 0x06 /* Public */,
       8,    4,   58,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   67,    2, 0x0a /* Public */,
      10,    2,   68,    2, 0x0a /* Public */,
      13,    0,   73,    2, 0x0a /* Public */,
      14,    0,   74,    2, 0x0a /* Public */,
      15,    0,   75,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QByteArray, QMetaType::Int, QMetaType::Int,    4,    5,    6,    7,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QByteArray, QMetaType::Int, QMetaType::Int,    4,    5,    6,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void BllDataIdentify::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BllDataIdentify *_t = static_cast<BllDataIdentify *>(_o);
        switch (_id) {
        case 0: _t->readyRead((*reinterpret_cast< DataOutput(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->readyReadBmp((*reinterpret_cast< DataOutput(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 2: _t->init(); break;
        case 3: _t->start((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->stop(); break;
        case 5: _t->sessionNumTextChanged(); break;
        case 6: _t->sessionCountDownTextChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BllDataIdentify::*_t)(DataOutput , QByteArray , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BllDataIdentify::readyRead)) {
                *result = 0;
            }
        }
        {
            typedef void (BllDataIdentify::*_t)(DataOutput , QByteArray , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BllDataIdentify::readyReadBmp)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject BllDataIdentify::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BllDataIdentify.data,
      qt_meta_data_BllDataIdentify,  qt_static_metacall, 0, 0}
};


const QMetaObject *BllDataIdentify::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BllDataIdentify::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BllDataIdentify.stringdata))
        return static_cast<void*>(const_cast< BllDataIdentify*>(this));
    return QObject::qt_metacast(_clname);
}

int BllDataIdentify::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void BllDataIdentify::readyRead(DataOutput _t1, QByteArray _t2, int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BllDataIdentify::readyReadBmp(DataOutput _t1, QByteArray _t2, int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
