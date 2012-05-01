/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Fri Apr 27 14:10:34 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      37,   11,   11,   11, 0x08,
      69,   61,   11,   11, 0x08,
     105,   11,   11,   11, 0x08,
     142,  137,   11,   11, 0x08,
     178,  137,   11,   11, 0x08,
     215,   11,   11,   11, 0x08,
     245,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_startButton_clicked()\0"
    "on_stopButton_clicked()\0checked\0"
    "on_ImageData_checkBox_toggled(bool)\0"
    "on_UpdateImage_button_clicked()\0arg1\0"
    "on_vrf_spinBox_valueChanged(double)\0"
    "on_vend_spinBox_valueChanged(double)\0"
    "on_saveImage_Button_clicked()\0"
    "on_action_Save_image_triggered()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_startButton_clicked(); break;
        case 1: on_stopButton_clicked(); break;
        case 2: on_ImageData_checkBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: on_UpdateImage_button_clicked(); break;
        case 4: on_vrf_spinBox_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: on_vend_spinBox_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: on_saveImage_Button_clicked(); break;
        case 7: on_action_Save_image_triggered(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
