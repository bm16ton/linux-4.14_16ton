/****************************************************************************
** Meta object code from reading C++ file 'eventhistory.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../eventhistory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eventhistory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EventListModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_EventListModel[] = {
    "EventListModel\0"
};

void EventListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EventListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EventListModel::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_EventListModel,
      qt_meta_data_EventListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EventListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EventListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EventListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EventListModel))
        return static_cast<void*>(const_cast< EventListModel*>(this));
    return QAbstractTableModel::qt_metacast(_clname);
}

int EventListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_EventHistory[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   14,   13,   13, 0x0a,
      45,   41,   13,   13, 0x0a,
      62,   13,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_EventHistory[] = {
    "EventHistory\0\0msgs\0addEvents(WpaMsgList)\0"
    "msg\0addEvent(WpaMsg)\0languageChange()\0"
};

void EventHistory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EventHistory *_t = static_cast<EventHistory *>(_o);
        switch (_id) {
        case 0: _t->addEvents((*reinterpret_cast< WpaMsgList(*)>(_a[1]))); break;
        case 1: _t->addEvent((*reinterpret_cast< WpaMsg(*)>(_a[1]))); break;
        case 2: _t->languageChange(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EventHistory::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EventHistory::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EventHistory,
      qt_meta_data_EventHistory, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EventHistory::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EventHistory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EventHistory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EventHistory))
        return static_cast<void*>(const_cast< EventHistory*>(this));
    if (!strcmp(_clname, "Ui::EventHistory"))
        return static_cast< Ui::EventHistory*>(const_cast< EventHistory*>(this));
    return QDialog::qt_metacast(_clname);
}

int EventHistory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
