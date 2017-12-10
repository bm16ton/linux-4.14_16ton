/****************************************************************************
** Meta object code from reading C++ file 'wpagui.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../wpagui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wpagui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WpaGui[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      41,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x0a,
      21,    7,    7,    7, 0x0a,
      36,    7,    7,    7, 0x0a,
      53,    7,    7,    7, 0x0a,
      65,    7,    7,    7, 0x0a,
      80,    7,    7,    7, 0x0a,
      92,    7,    7,    7, 0x0a,
     105,    7,    7,    7, 0x0a,
     112,    7,    7,    7, 0x0a,
     127,    7,    7,    7, 0x0a,
     138,  134,    7,    7, 0x0a,
     160,  156,    7,    7, 0x0a,
     188,    7,    7,    7, 0x0a,
     202,    7,    7,    7, 0x0a,
     217,  213,    7,    7, 0x0a,
     240,    7,    7,    7, 0x0a,
     262,    7,    7,    7, 0x0a,
     282,    7,    7,    7, 0x0a,
     306,    7,    7,    7, 0x0a,
     328,    7,    7,    7, 0x0a,
     341,    7,    7,    7, 0x0a,
     361,    7,    7,    7, 0x0a,
     382,    7,    7,    7, 0x0a,
     402,    7,    7,    7, 0x0a,
     415,  213,    7,    7, 0x0a,
     438,    7,    7,    7, 0x0a,
     468,    7,    7,    7, 0x0a,
     494,    7,    7,    7, 0x0a,
     534,  521,    7,    7, 0x0a,
     592,    7,    7,    7, 0x0a,
     609,    7,    7,    7, 0x0a,
     621,    7,    7,    7, 0x0a,
     641,  635,    7,    7, 0x0a,
     657,    7,    7,    7, 0x0a,
     666,    7,    7,    7, 0x0a,
     688,  683,    7,    7, 0x0a,
     713,    7,    7,    7, 0x0a,
     724,    7,    7,    7, 0x0a,
     739,    7,    7,    7, 0x09,
     760,  756,    7,    7, 0x09,
     815,  809,    7,    7, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_WpaGui[] = {
    "WpaGui\0\0parse_argv()\0updateStatus()\0"
    "updateNetworks()\0helpIndex()\0"
    "helpContents()\0helpAbout()\0disconnect()\0"
    "scan()\0eventHistory()\0ping()\0msg\0"
    "processMsg(char*)\0req\0processCtrlReq(const char*)\0"
    "receiveMsgs()\0connectB()\0sel\0"
    "selectNetwork(QString)\0editSelectedNetwork()\0"
    "editListedNetwork()\0removeSelectedNetwork()\0"
    "removeListedNetwork()\0addNetwork()\0"
    "enableAllNetworks()\0disableAllNetworks()\0"
    "removeAllNetworks()\0saveConfig()\0"
    "selectAdapter(QString)\0"
    "updateNetworkDisabledStatus()\0"
    "enableListedNetwork(bool)\0"
    "disableListedNetwork(bool)\0type,sec,msg\0"
    "showTrayMessage(QSystemTrayIcon::MessageIcon,int,QString)\0"
    "showTrayStatus()\0wpsDialog()\0peersDialog()\0"
    "index\0tabChanged(int)\0wpsPbc()\0"
    "wpsGeneratePin()\0text\0wpsApPinChanged(QString)\0"
    "wpsApPin()\0addInterface()\0languageChange()\0"
    "how\0trayActivated(QSystemTrayIcon::ActivationReason)\0"
    "event\0closeEvent(QCloseEvent*)\0"
};

void WpaGui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WpaGui *_t = static_cast<WpaGui *>(_o);
        switch (_id) {
        case 0: _t->parse_argv(); break;
        case 1: _t->updateStatus(); break;
        case 2: _t->updateNetworks(); break;
        case 3: _t->helpIndex(); break;
        case 4: _t->helpContents(); break;
        case 5: _t->helpAbout(); break;
        case 6: _t->disconnect(); break;
        case 7: _t->scan(); break;
        case 8: _t->eventHistory(); break;
        case 9: _t->ping(); break;
        case 10: _t->processMsg((*reinterpret_cast< char*(*)>(_a[1]))); break;
        case 11: _t->processCtrlReq((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 12: _t->receiveMsgs(); break;
        case 13: _t->connectB(); break;
        case 14: _t->selectNetwork((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->editSelectedNetwork(); break;
        case 16: _t->editListedNetwork(); break;
        case 17: _t->removeSelectedNetwork(); break;
        case 18: _t->removeListedNetwork(); break;
        case 19: _t->addNetwork(); break;
        case 20: _t->enableAllNetworks(); break;
        case 21: _t->disableAllNetworks(); break;
        case 22: _t->removeAllNetworks(); break;
        case 23: _t->saveConfig(); break;
        case 24: _t->selectAdapter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 25: _t->updateNetworkDisabledStatus(); break;
        case 26: _t->enableListedNetwork((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 27: _t->disableListedNetwork((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: _t->showTrayMessage((*reinterpret_cast< QSystemTrayIcon::MessageIcon(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 29: _t->showTrayStatus(); break;
        case 30: _t->wpsDialog(); break;
        case 31: _t->peersDialog(); break;
        case 32: _t->tabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->wpsPbc(); break;
        case 34: _t->wpsGeneratePin(); break;
        case 35: _t->wpsApPinChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 36: _t->wpsApPin(); break;
        case 37: _t->addInterface(); break;
        case 38: _t->languageChange(); break;
        case 39: _t->trayActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 40: _t->closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WpaGui::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WpaGui::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_WpaGui,
      qt_meta_data_WpaGui, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WpaGui::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WpaGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WpaGui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WpaGui))
        return static_cast<void*>(const_cast< WpaGui*>(this));
    if (!strcmp(_clname, "Ui::WpaGui"))
        return static_cast< Ui::WpaGui*>(const_cast< WpaGui*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int WpaGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 41)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 41;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
