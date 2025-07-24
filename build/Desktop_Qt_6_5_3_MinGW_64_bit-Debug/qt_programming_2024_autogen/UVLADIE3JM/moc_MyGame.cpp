/****************************************************************************
** Meta object code from reading C++ file 'MyGame.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/MyGame.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyGame.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMyGameENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMyGameENDCLASS = QtMocHelpers::stringData(
    "MyGame",
    "showSettings",
    "",
    "showBattleScene",
    "handleSceneChangeRequest",
    "SceneID",
    "id",
    "returnFromSettings",
    "handleIceSceneRestart"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMyGameENDCLASS_t {
    uint offsetsAndSizes[18];
    char stringdata0[7];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[25];
    char stringdata5[8];
    char stringdata6[3];
    char stringdata7[19];
    char stringdata8[22];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMyGameENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMyGameENDCLASS_t qt_meta_stringdata_CLASSMyGameENDCLASS = {
    {
        QT_MOC_LITERAL(0, 6),  // "MyGame"
        QT_MOC_LITERAL(7, 12),  // "showSettings"
        QT_MOC_LITERAL(20, 0),  // ""
        QT_MOC_LITERAL(21, 15),  // "showBattleScene"
        QT_MOC_LITERAL(37, 24),  // "handleSceneChangeRequest"
        QT_MOC_LITERAL(62, 7),  // "SceneID"
        QT_MOC_LITERAL(70, 2),  // "id"
        QT_MOC_LITERAL(73, 18),  // "returnFromSettings"
        QT_MOC_LITERAL(92, 21)   // "handleIceSceneRestart"
    },
    "MyGame",
    "showSettings",
    "",
    "showBattleScene",
    "handleSceneChangeRequest",
    "SceneID",
    "id",
    "returnFromSettings",
    "handleIceSceneRestart"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMyGameENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x08,    1 /* Private */,
       3,    0,   45,    2, 0x08,    2 /* Private */,
       4,    1,   46,    2, 0x08,    3 /* Private */,
       7,    0,   49,    2, 0x08,    5 /* Private */,
       8,    0,   50,    2, 0x08,    6 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void,

 // enums: name, alias, flags, count, data

 // enum data: key, value

       0        // eod
};

Q_CONSTINIT const QMetaObject MyGame::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMyGameENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMyGameENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMyGameENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MyGame, std::true_type>,
        // method 'showSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showBattleScene'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleSceneChangeRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<SceneID, std::false_type>,
        // method 'returnFromSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleIceSceneRestart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MyGame::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyGame *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->showSettings(); break;
        case 1: _t->showBattleScene(); break;
        case 2: _t->handleSceneChangeRequest((*reinterpret_cast< std::add_pointer_t<SceneID>>(_a[1]))); break;
        case 3: _t->returnFromSettings(); break;
        case 4: _t->handleIceSceneRestart(); break;
        default: ;
        }
    }
}

const QMetaObject *MyGame::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyGame::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMyGameENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MyGame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
