/****************************************************************************
** Meta object code from reading C++ file 'multi_navi_goal_panel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/fcu_core_rviz_swarm_goals_plugin/src/multi_navi_goal_panel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'multi_navi_goal_panel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_navi_multi_goals_pub_rviz_plugin__MultiNaviGoalsPanel_t {
    QByteArrayData data[70];
    char stringdata0[1030];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_navi_multi_goals_pub_rviz_plugin__MultiNaviGoalsPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_navi_multi_goals_pub_rviz_plugin__MultiNaviGoalsPanel_t qt_meta_stringdata_navi_multi_goals_pub_rviz_plugin__MultiNaviGoalsPanel = {
    {
QT_MOC_LITERAL(0, 0, 53), // "navi_multi_goals_pub_rviz_plu..."
QT_MOC_LITERAL(1, 54, 13), // "setMaxNumGoal"
QT_MOC_LITERAL(2, 68, 0), // ""
QT_MOC_LITERAL(3, 69, 10), // "maxNumGoal"
QT_MOC_LITERAL(4, 80, 12), // "writePose001"
QT_MOC_LITERAL(5, 93, 19), // "geometry_msgs::Pose"
QT_MOC_LITERAL(6, 113, 4), // "pose"
QT_MOC_LITERAL(7, 118, 12), // "writePose002"
QT_MOC_LITERAL(8, 131, 12), // "writePose003"
QT_MOC_LITERAL(9, 144, 12), // "writePose004"
QT_MOC_LITERAL(10, 157, 12), // "writePose005"
QT_MOC_LITERAL(11, 170, 12), // "writePose006"
QT_MOC_LITERAL(12, 183, 8), // "markPose"
QT_MOC_LITERAL(13, 192, 10), // "deleteMark"
QT_MOC_LITERAL(14, 203, 8), // "markWall"
QT_MOC_LITERAL(15, 212, 11), // "wall_startx"
QT_MOC_LITERAL(16, 224, 11), // "wall_starty"
QT_MOC_LITERAL(17, 236, 9), // "wall_endx"
QT_MOC_LITERAL(18, 246, 9), // "wall_endy"
QT_MOC_LITERAL(19, 256, 10), // "updateWall"
QT_MOC_LITERAL(20, 267, 16), // "updateMaxNumGoal"
QT_MOC_LITERAL(21, 284, 19), // "computeGlobalOffset"
QT_MOC_LITERAL(22, 304, 2), // "dx"
QT_MOC_LITERAL(23, 307, 2), // "dy"
QT_MOC_LITERAL(24, 310, 7), // "double&"
QT_MOC_LITERAL(25, 318, 6), // "out_dx"
QT_MOC_LITERAL(26, 325, 6), // "out_dy"
QT_MOC_LITERAL(27, 332, 13), // "initPoseTable"
QT_MOC_LITERAL(28, 346, 15), // "updatePoseTable"
QT_MOC_LITERAL(29, 362, 17), // "highlightTableRow"
QT_MOC_LITERAL(30, 380, 13), // "QTableWidget*"
QT_MOC_LITERAL(31, 394, 5), // "table"
QT_MOC_LITERAL(32, 400, 9), // "targetRow"
QT_MOC_LITERAL(33, 410, 13), // "isDroneAtGoal"
QT_MOC_LITERAL(34, 424, 8), // "drone_id"
QT_MOC_LITERAL(35, 433, 24), // "geometry_msgs::PoseArray"
QT_MOC_LITERAL(36, 458, 5), // "goals"
QT_MOC_LITERAL(37, 464, 10), // "curGoalIdx"
QT_MOC_LITERAL(38, 475, 11), // "NaviControl"
QT_MOC_LITERAL(39, 487, 11), // "StopControl"
QT_MOC_LITERAL(40, 499, 9), // "startNavi"
QT_MOC_LITERAL(41, 509, 21), // "refreshPoseArrayTable"
QT_MOC_LITERAL(42, 531, 10), // "pose_array"
QT_MOC_LITERAL(43, 542, 15), // "deleteGoalPoint"
QT_MOC_LITERAL(44, 558, 13), // "deleteAllMark"
QT_MOC_LITERAL(45, 572, 9), // "goalCntCB"
QT_MOC_LITERAL(46, 582, 36), // "geometry_msgs::PoseStamped::C..."
QT_MOC_LITERAL(47, 619, 8), // "statusCB"
QT_MOC_LITERAL(48, 628, 41), // "actionlib_msgs::GoalStatusArr..."
QT_MOC_LITERAL(49, 670, 8), // "statuses"
QT_MOC_LITERAL(50, 679, 13), // "checkCycle001"
QT_MOC_LITERAL(51, 693, 13), // "checkCycle002"
QT_MOC_LITERAL(52, 707, 13), // "checkCycle003"
QT_MOC_LITERAL(53, 721, 13), // "checkCycle004"
QT_MOC_LITERAL(54, 735, 13), // "checkCycle005"
QT_MOC_LITERAL(55, 749, 13), // "checkCycle006"
QT_MOC_LITERAL(56, 763, 12), // "completeNavi"
QT_MOC_LITERAL(57, 776, 9), // "cycleNavi"
QT_MOC_LITERAL(58, 786, 9), // "checkGoal"
QT_MOC_LITERAL(59, 796, 39), // "std::vector<actionlib_msgs::G..."
QT_MOC_LITERAL(60, 836, 11), // "status_list"
QT_MOC_LITERAL(61, 848, 9), // "startSpin"
QT_MOC_LITERAL(62, 858, 22), // "odom_global001_handler"
QT_MOC_LITERAL(63, 881, 28), // "nav_msgs::Odometry::ConstPtr"
QT_MOC_LITERAL(64, 910, 4), // "odom"
QT_MOC_LITERAL(65, 915, 22), // "odom_global002_handler"
QT_MOC_LITERAL(66, 938, 22), // "odom_global003_handler"
QT_MOC_LITERAL(67, 961, 22), // "odom_global004_handler"
QT_MOC_LITERAL(68, 984, 22), // "odom_global005_handler"
QT_MOC_LITERAL(69, 1007, 22) // "odom_global006_handler"

    },
    "navi_multi_goals_pub_rviz_plugin::MultiNaviGoalsPanel\0"
    "setMaxNumGoal\0\0maxNumGoal\0writePose001\0"
    "geometry_msgs::Pose\0pose\0writePose002\0"
    "writePose003\0writePose004\0writePose005\0"
    "writePose006\0markPose\0deleteMark\0"
    "markWall\0wall_startx\0wall_starty\0"
    "wall_endx\0wall_endy\0updateWall\0"
    "updateMaxNumGoal\0computeGlobalOffset\0"
    "dx\0dy\0double&\0out_dx\0out_dy\0initPoseTable\0"
    "updatePoseTable\0highlightTableRow\0"
    "QTableWidget*\0table\0targetRow\0"
    "isDroneAtGoal\0drone_id\0geometry_msgs::PoseArray\0"
    "goals\0curGoalIdx\0NaviControl\0StopControl\0"
    "startNavi\0refreshPoseArrayTable\0"
    "pose_array\0deleteGoalPoint\0deleteAllMark\0"
    "goalCntCB\0geometry_msgs::PoseStamped::ConstPtr\0"
    "statusCB\0actionlib_msgs::GoalStatusArray::ConstPtr\0"
    "statuses\0checkCycle001\0checkCycle002\0"
    "checkCycle003\0checkCycle004\0checkCycle005\0"
    "checkCycle006\0completeNavi\0cycleNavi\0"
    "checkGoal\0std::vector<actionlib_msgs::GoalStatus>\0"
    "status_list\0startSpin\0odom_global001_handler\0"
    "nav_msgs::Odometry::ConstPtr\0odom\0"
    "odom_global002_handler\0odom_global003_handler\0"
    "odom_global004_handler\0odom_global005_handler\0"
    "odom_global006_handler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_navi_multi_goals_pub_rviz_plugin__MultiNaviGoalsPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      41,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  219,    2, 0x0a /* Public */,
       4,    1,  222,    2, 0x0a /* Public */,
       7,    1,  225,    2, 0x0a /* Public */,
       8,    1,  228,    2, 0x0a /* Public */,
       9,    1,  231,    2, 0x0a /* Public */,
      10,    1,  234,    2, 0x0a /* Public */,
      11,    1,  237,    2, 0x0a /* Public */,
      12,    0,  240,    2, 0x0a /* Public */,
      13,    0,  241,    2, 0x0a /* Public */,
      14,    4,  242,    2, 0x0a /* Public */,
      19,    0,  251,    2, 0x09 /* Protected */,
      20,    0,  252,    2, 0x09 /* Protected */,
      21,    4,  253,    2, 0x09 /* Protected */,
      27,    0,  262,    2, 0x09 /* Protected */,
      28,    0,  263,    2, 0x09 /* Protected */,
      29,    2,  264,    2, 0x09 /* Protected */,
      33,    3,  269,    2, 0x09 /* Protected */,
      38,    0,  276,    2, 0x09 /* Protected */,
      39,    0,  277,    2, 0x09 /* Protected */,
      40,    0,  278,    2, 0x09 /* Protected */,
      41,    2,  279,    2, 0x09 /* Protected */,
      43,    0,  284,    2, 0x09 /* Protected */,
      44,    0,  285,    2, 0x09 /* Protected */,
      45,    1,  286,    2, 0x09 /* Protected */,
      47,    1,  289,    2, 0x09 /* Protected */,
      50,    0,  292,    2, 0x09 /* Protected */,
      51,    0,  293,    2, 0x09 /* Protected */,
      52,    0,  294,    2, 0x09 /* Protected */,
      53,    0,  295,    2, 0x09 /* Protected */,
      54,    0,  296,    2, 0x09 /* Protected */,
      55,    0,  297,    2, 0x09 /* Protected */,
      56,    0,  298,    2, 0x09 /* Protected */,
      57,    0,  299,    2, 0x09 /* Protected */,
      58,    1,  300,    2, 0x09 /* Protected */,
      61,    0,  303,    2, 0x09 /* Protected */,
      62,    1,  304,    2, 0x09 /* Protected */,
      65,    1,  307,    2, 0x09 /* Protected */,
      66,    1,  310,    2, 0x09 /* Protected */,
      67,    1,  313,    2, 0x09 /* Protected */,
      68,    1,  316,    2, 0x09 /* Protected */,
      69,    1,  319,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   15,   16,   17,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, 0x80000000 | 24, 0x80000000 | 24,   22,   23,   25,   26,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 30, QMetaType::Int,   31,   32,
    QMetaType::Bool, QMetaType::Int, 0x80000000 | 35, QMetaType::Int,   34,   36,   37,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 30, 0x80000000 | 35,   31,   42,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 46,    6,
    QMetaType::Void, 0x80000000 | 48,   49,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool, 0x80000000 | 59,   60,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 63,   64,
    QMetaType::Void, 0x80000000 | 63,   64,
    QMetaType::Void, 0x80000000 | 63,   64,
    QMetaType::Void, 0x80000000 | 63,   64,
    QMetaType::Void, 0x80000000 | 63,   64,
    QMetaType::Void, 0x80000000 | 63,   64,

       0        // eod
};

void navi_multi_goals_pub_rviz_plugin::MultiNaviGoalsPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MultiNaviGoalsPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setMaxNumGoal((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->writePose001((*reinterpret_cast< geometry_msgs::Pose(*)>(_a[1]))); break;
        case 2: _t->writePose002((*reinterpret_cast< geometry_msgs::Pose(*)>(_a[1]))); break;
        case 3: _t->writePose003((*reinterpret_cast< geometry_msgs::Pose(*)>(_a[1]))); break;
        case 4: _t->writePose004((*reinterpret_cast< geometry_msgs::Pose(*)>(_a[1]))); break;
        case 5: _t->writePose005((*reinterpret_cast< geometry_msgs::Pose(*)>(_a[1]))); break;
        case 6: _t->writePose006((*reinterpret_cast< geometry_msgs::Pose(*)>(_a[1]))); break;
        case 7: _t->markPose(); break;
        case 8: _t->deleteMark(); break;
        case 9: _t->markWall((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 10: _t->updateWall(); break;
        case 11: _t->updateMaxNumGoal(); break;
        case 12: _t->computeGlobalOffset((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 13: _t->initPoseTable(); break;
        case 14: _t->updatePoseTable(); break;
        case 15: _t->highlightTableRow((*reinterpret_cast< QTableWidget*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 16: { bool _r = _t->isDroneAtGoal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const geometry_msgs::PoseArray(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 17: _t->NaviControl(); break;
        case 18: _t->StopControl(); break;
        case 19: _t->startNavi(); break;
        case 20: _t->refreshPoseArrayTable((*reinterpret_cast< QTableWidget*(*)>(_a[1])),(*reinterpret_cast< const geometry_msgs::PoseArray(*)>(_a[2]))); break;
        case 21: _t->deleteGoalPoint(); break;
        case 22: _t->deleteAllMark(); break;
        case 23: _t->goalCntCB((*reinterpret_cast< const geometry_msgs::PoseStamped::ConstPtr(*)>(_a[1]))); break;
        case 24: _t->statusCB((*reinterpret_cast< const actionlib_msgs::GoalStatusArray::ConstPtr(*)>(_a[1]))); break;
        case 25: _t->checkCycle001(); break;
        case 26: _t->checkCycle002(); break;
        case 27: _t->checkCycle003(); break;
        case 28: _t->checkCycle004(); break;
        case 29: _t->checkCycle005(); break;
        case 30: _t->checkCycle006(); break;
        case 31: _t->completeNavi(); break;
        case 32: _t->cycleNavi(); break;
        case 33: { bool _r = _t->checkGoal((*reinterpret_cast< std::vector<actionlib_msgs::GoalStatus>(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 34: _t->startSpin(); break;
        case 35: _t->odom_global001_handler((*reinterpret_cast< const nav_msgs::Odometry::ConstPtr(*)>(_a[1]))); break;
        case 36: _t->odom_global002_handler((*reinterpret_cast< const nav_msgs::Odometry::ConstPtr(*)>(_a[1]))); break;
        case 37: _t->odom_global003_handler((*reinterpret_cast< const nav_msgs::Odometry::ConstPtr(*)>(_a[1]))); break;
        case 38: _t->odom_global004_handler((*reinterpret_cast< const nav_msgs::Odometry::ConstPtr(*)>(_a[1]))); break;
        case 39: _t->odom_global005_handler((*reinterpret_cast< const nav_msgs::Odometry::ConstPtr(*)>(_a[1]))); break;
        case 40: _t->odom_global006_handler((*reinterpret_cast< const nav_msgs::Odometry::ConstPtr(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTableWidget* >(); break;
            }
            break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTableWidget* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject navi_multi_goals_pub_rviz_plugin::MultiNaviGoalsPanel::staticMetaObject = { {
    &rviz::Panel::staticMetaObject,
    qt_meta_stringdata_navi_multi_goals_pub_rviz_plugin__MultiNaviGoalsPanel.data,
    qt_meta_data_navi_multi_goals_pub_rviz_plugin__MultiNaviGoalsPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *navi_multi_goals_pub_rviz_plugin::MultiNaviGoalsPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *navi_multi_goals_pub_rviz_plugin::MultiNaviGoalsPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_navi_multi_goals_pub_rviz_plugin__MultiNaviGoalsPanel.stringdata0))
        return static_cast<void*>(this);
    return rviz::Panel::qt_metacast(_clname);
}

int navi_multi_goals_pub_rviz_plugin::MultiNaviGoalsPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = rviz::Panel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 41)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 41;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 41)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 41;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
