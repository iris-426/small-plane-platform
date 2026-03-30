#include <ros/ros.h>
#include <ros/package.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <geometry_msgs/InertiaStamped.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float32MultiArray.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_broadcaster.h>
#include "quadrotor_msgs/PositionCommand.h"
#include "../mavlink/common/mavlink.h"

/**
 * 注意：工程中mission_xxx话题为发给飞控的目标值，目标位移应为FRU坐标系，目标姿态应为FRD坐标系
 */

// 定义无人机状态结构体
struct UAVState {
    // 控制量
    float px = 0.0f, py = 0.0f, pz = 0.0f, yaw = 0.0f;
    float vx = 0.0f, vy = 0.0f, vz = 0.0f;

    // 反馈量 (真值/里程计)
    float odom_px = 0.0f, odom_py = 0.0f, odom_pz = 0.0f;
    float odom_yaw = 0.0f,odom_roll=0.0f, odom_pitch=0.0f;
    float odom_vx = 0.0f, odom_vy = 0.0f, odom_vz = 0.0f;
};

static bool enable_path=false;
static bool enable_track=false;
static bool set_goal=false;
static bool use_goal_001=false;
static bool use_goal_002=false;
static bool use_goal_003=false;
static bool use_goal_004=false;
static bool use_goal_005=false;
static bool use_goal_006=false;
static uint8_t enable_pos=0;

// 创建6架无人机的结构体数组
UAVState uav[6];

// 全局变量定义：
constexpr int num_uav = 6, run_uav = 4; // 在此修改运行无人机数量
float Hight = 1.5f, Radius = 3.0f, Angular_speed = 0.15f, Linear_speed = 0.5f, d = 5.0f; // 运行高度、半径、角速度、线速度
static bool start_move = false;
int r_cmd = 0, direction = 0;

typedef enum {
  ReadyToGoal,
  ExecutingGoal
} path_track_flag;
path_track_flag path_track_status = ReadyToGoal;

void cmdHandler(const std_msgs::Int16::ConstPtr& cmd){
  switch(cmd->data){
    case 0:
        enable_path=true;
        break;
    case 3://起飞时刻记录坐标,原地悬停
        break;
    case 5:
        enable_track=true;
        break;
    case 6:
        enable_track=false;
        enable_path=false;
        enable_pos=255;
        break;
    case 7:
        start_move = !start_move;
        break;
    case 8:
        direction = 0;                            
        break;
    case 11: case 12: case 13: case 14: case 15: case 16:
        enable_track=false;
        break;    
    case 101: Linear_speed += 0.1; break;
    case 102: Linear_speed -= 0.1; break;
    case 103: direction = 1; break;
    case 104: direction = 2; break;
    case 105: direction = 3; break;
    case 106: direction = 4; break;
    case 107: Hight += 1.0; break;
    case 108: Hight -= 1.0; break;     
    case 109: r_cmd =1; break;
    case 110: r_cmd =2; break;
    case 111: r_cmd =3; break;
    case 112: enable_pos=1;enable_track=false; break;
    case 113: enable_pos=2;enable_track=false; break;
    case 114: enable_pos=3;enable_track=false; break;
    case 115: enable_pos=4;enable_track=false; break;        
    default:
        break;
  }
}

// 封装控制生成函数
void create_control(float dt) {
  
}

static ros::Publisher mission_pubs[6];// 6个任务话题的发布者数组
// 核心模板：统一提取数据并发布的逻辑
void process_mission(int index) {
  std_msgs::Float32MultiArray mission_msg;
  mission_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
  
  // 动态生成对应的 label (mission_001, mission_002 ...)
  char label[20];
  snprintf(label, sizeof(label), "mission_%03d", index + 1);
  mission_msg.layout.dim[0].label = label;
  
  mission_msg.layout.dim[0].size = 11;
  mission_msg.layout.dim[0].stride = 1;
  mission_msg.data.resize(11);
  
  // 从对应的 uav 结构体中读取控制量
  mission_msg.data[0] = uav[index].yaw; 
  mission_msg.data[1] = 0.0f;
  mission_msg.data[2] = uav[index].px; 
  mission_msg.data[3] = uav[index].py; 
  mission_msg.data[4] = uav[index].pz;
  mission_msg.data[5] = uav[index].vx; 
  mission_msg.data[6] = uav[index].vy; 
  mission_msg.data[7] = uav[index].vz;
  mission_msg.data[8] = 0.0f; 
  mission_msg.data[9] = 0.0f; 
  mission_msg.data[10] = 0.0f;  
  
  mission_pubs[index].publish(mission_msg);   // 对应飞机发布对应数据
}
// 6个独立的定时器回调，内部直接调用标准模板
void execute_mission_001(const ros::TimerEvent &event) { process_mission(0); }
void execute_mission_002(const ros::TimerEvent &event) { process_mission(1); }
void execute_mission_003(const ros::TimerEvent &event) { process_mission(2); }
void execute_mission_004(const ros::TimerEvent &event) { process_mission(3); }
void execute_mission_005(const ros::TimerEvent &event) { process_mission(4); }
void execute_mission_006(const ros::TimerEvent &event) { process_mission(5); }

// 里程计数据统一处理逻辑
void process_odom(int index, const nav_msgs::Odometry::ConstPtr& odom) {
  uav[index].odom_px=(float)odom->pose.pose.position.x;
  uav[index].odom_py=-(float)odom->pose.pose.position.y;
  uav[index].odom_pz=(float)odom->pose.pose.position.z;
  float quaternion_odom[4]={(float)odom->pose.pose.orientation.w,
                            (float)odom->pose.pose.orientation.x,
                            (float)odom->pose.pose.orientation.y,
                            (float)odom->pose.pose.orientation.z};
  mavlink_quaternion_to_euler(quaternion_odom, &uav[index].odom_roll, &uav[index].odom_pitch, &uav[index].odom_yaw);
  uav[index].odom_pitch=-uav[index].odom_pitch;
  uav[index].odom_yaw=-uav[index].odom_yaw;
}

void odom_global001_handler(const nav_msgs::Odometry::ConstPtr& odom) { process_odom(0, odom); }
void odom_global002_handler(const nav_msgs::Odometry::ConstPtr& odom) { process_odom(1, odom); }
void odom_global003_handler(const nav_msgs::Odometry::ConstPtr& odom) { process_odom(2, odom); }
void odom_global004_handler(const nav_msgs::Odometry::ConstPtr& odom) { process_odom(3, odom); }
void odom_global005_handler(const nav_msgs::Odometry::ConstPtr& odom) { process_odom(4, odom); }
void odom_global006_handler(const nav_msgs::Odometry::ConstPtr& odom) { process_odom(5, odom); }

int main(int argc, char **argv) {
  ros::init(argc, argv, "fcu_mission");
  ros::NodeHandle nh("~");
  ros::Subscriber comm=nh.subscribe<std_msgs::Int16>("/fcu_command/command", 100, cmdHandler);
  ros::Subscriber odom001=nh.subscribe<nav_msgs::Odometry>("odom_global_001_pc", 100, odom_global001_handler);
  ros::Subscriber odom002=nh.subscribe<nav_msgs::Odometry>("odom_global_002_pc", 100, odom_global002_handler);
  ros::Subscriber odom003=nh.subscribe<nav_msgs::Odometry>("odom_global_003_pc", 100, odom_global003_handler);
  ros::Subscriber odom004=nh.subscribe<nav_msgs::Odometry>("odom_global_004_pc", 100, odom_global004_handler);
  ros::Subscriber odom005=nh.subscribe<nav_msgs::Odometry>("odom_global_005_pc", 100, odom_global005_handler);
  ros::Subscriber odom006=nh.subscribe<nav_msgs::Odometry>("odom_global_006_pc", 100, odom_global006_handler);

  mission_pubs[0] = nh.advertise<std_msgs::Float32MultiArray>("mission_001",100);
  mission_pubs[1] = nh.advertise<std_msgs::Float32MultiArray>("mission_002",100);
  mission_pubs[2] = nh.advertise<std_msgs::Float32MultiArray>("mission_003",100);
  mission_pubs[3] = nh.advertise<std_msgs::Float32MultiArray>("mission_004",100);
  mission_pubs[4] = nh.advertise<std_msgs::Float32MultiArray>("mission_005",100);
  mission_pubs[5] = nh.advertise<std_msgs::Float32MultiArray>("mission_006",100);

  ros::Timer timer_mission_001 = nh.createTimer(ros::Duration(0.1),execute_mission_001,false);
  ros::Timer timer_mission_002 = nh.createTimer(ros::Duration(0.1),execute_mission_002,false);
  ros::Timer timer_mission_003 = nh.createTimer(ros::Duration(0.1),execute_mission_003,false);
  ros::Timer timer_mission_004 = nh.createTimer(ros::Duration(0.1),execute_mission_004,false);
  ros::Timer timer_mission_005 = nh.createTimer(ros::Duration(0.1),execute_mission_005,false);
  ros::Timer timer_mission_006 = nh.createTimer(ros::Duration(0.1),execute_mission_006,false);

  ros::Rate loop_rate(200);
  while (ros::ok()) {
    ros::spinOnce();
    
    static tf::TransformBroadcaster br;
    tf::Transform transform;
    tf::Quaternion q;
    transform.setOrigin(tf::Vector3(0, 0,0)); q.setRPY(0, 0, 0); transform.setRotation(q);
    br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "map", "uwb"));
    br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "map", "world"));

    static ros::Time last_time = ros::Time::now();//计算dt
    ros::Time now = ros::Time::now();
    float dt = (now - last_time).toSec();
    last_time = now;
    
    // --- 控制量生成 ---
    create_control(dt);

    loop_rate.sleep();
  }
  return 0;
}