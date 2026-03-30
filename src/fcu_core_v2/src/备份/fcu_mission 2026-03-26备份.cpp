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
    // 控制目标量 (原 px1, py1, pz1, yaw1 等)
    float px = 0.0f;
    float py = 0.0f;
    float pz = 0.0f;
    float yaw = 0.0f;

    // 里程计反馈量 (原 pos_odom_001_x, pos_odom_001_roll 等)
    float odom_x = 0.0f;
    float odom_y = 0.0f;
    float odom_z = 0.0f;
    float odom_roll = 0.0f;
    float odom_pitch = 0.0f;
    float odom_yaw = 0.0f;
};

// 定义单时刻全集群航点结构体
struct Waypoint {
    double timestamp;
    struct {
        float x, y, z, yaw;
    } uavs[6];
};

static bool enable_path=false;
static bool enable_track=false;
static bool get_pos_cmd=false;
static bool follow_forward=false;
static bool follow_down=false;
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
// 轨迹数据提升为全局变量
std::vector<Waypoint> trajectory_data;

// 起飞点记录（根据要求：其他的先不要改变，暂时保留为独立变量）
float pos_takeoff_001_x=0.0f; float pos_takeoff_001_y=0.0f; float pos_takeoff_001_z=0.0f; float pos_takeoff_001_yaw=0.0f;
float pos_takeoff_002_x=0.0f; float pos_takeoff_002_y=0.0f; float pos_takeoff_002_z=0.0f; float pos_takeoff_002_yaw=0.0f;
float pos_takeoff_003_x=0.0f; float pos_takeoff_003_y=0.0f; float pos_takeoff_003_z=0.0f; float pos_takeoff_003_yaw=0.0f;
float pos_takeoff_004_x=0.0f; float pos_takeoff_004_y=0.0f; float pos_takeoff_004_z=0.0f; float pos_takeoff_004_yaw=0.0f;
float pos_takeoff_005_x=0.0f; float pos_takeoff_005_y=0.0f; float pos_takeoff_005_z=0.0f; float pos_takeoff_005_yaw=0.0f;
float pos_takeoff_006_x=0.0f; float pos_takeoff_006_y=0.0f; float pos_takeoff_006_z=0.0f; float pos_takeoff_006_yaw=0.0f;

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

static std_msgs::Float32MultiArray mission_001;
static ros::Publisher mission_pub_001;

void cmdHandler(const std_msgs::Int16::ConstPtr& cmd){
  switch(cmd->data){
    case 0:
        enable_path=true;
        break;
    case 3://起飞时刻记录坐标,原地悬停
        enable_pos=0;
        pos_takeoff_001_x=uav[0].odom_x; pos_takeoff_001_y=uav[0].odom_y; pos_takeoff_001_z=uav[0].odom_z; pos_takeoff_001_yaw=uav[0].odom_yaw;
        pos_takeoff_002_x=uav[1].odom_x; pos_takeoff_002_y=uav[1].odom_y; pos_takeoff_002_z=uav[1].odom_z; pos_takeoff_002_yaw=uav[1].odom_yaw;
        pos_takeoff_003_x=uav[2].odom_x; pos_takeoff_003_y=uav[2].odom_y; pos_takeoff_003_z=uav[2].odom_z; pos_takeoff_003_yaw=uav[2].odom_yaw;
        pos_takeoff_004_x=uav[3].odom_x; pos_takeoff_004_y=uav[3].odom_y; pos_takeoff_004_z=uav[3].odom_z; pos_takeoff_004_yaw=uav[3].odom_yaw;
        pos_takeoff_005_x=uav[4].odom_x; pos_takeoff_005_y=uav[4].odom_y; pos_takeoff_005_z=uav[4].odom_z; pos_takeoff_005_yaw=uav[4].odom_yaw;
        pos_takeoff_006_x=uav[5].odom_x; pos_takeoff_006_y=uav[5].odom_y; pos_takeoff_006_z=uav[5].odom_z; pos_takeoff_006_yaw=uav[5].odom_yaw;
        if(set_goal){
          // 发布mission
          mission_001.layout.dim.push_back(std_msgs::MultiArrayDimension());
          mission_001.layout.dim[0].label = "mission_001";
          mission_001.layout.dim[0].size = 11;
          mission_001.layout.dim[0].stride = 1;
          mission_001.data.resize(11);
          mission_001.data[0] = 0.0f; 
          mission_001.data[1] = 0.0f;
          mission_001.data[2] = pos_takeoff_001_x; 
          mission_001.data[3] = pos_takeoff_001_y;
          mission_001.data[4] = 1.0f;
          mission_001.data[5]=0; 
          mission_001.data[6]=0; 
          mission_001.data[7]=0;
          mission_001.data[8]=0; 
          mission_001.data[9]=0; 
          mission_001.data[10]=0;
          mission_pub_001.publish(mission_001);
        }
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

// 临时计算变量保持原样
static float yaw=0.0f, yaw_rate=0.0f;
static float px=0.0f, py=0.0f, pz=0.0f;
static float vx=0.0f, vy=0.0f, vz=0.0f;
static float ax=0.0f, ay=0.0f, az=0.0f;
static float theta=0.0f;

static int goal_point = 0;
void SetGoal(int id, float target_x, float target_y, float target_z, float target_yaw)
{
  if (id >= 1 && id <= 6) {
    uav[id-1].px = target_x;
    uav[id-1].py = target_y;
    uav[id-1].pz = target_z;
    uav[id-1].yaw = target_yaw;
  }
}

bool IsReachGoal(int id, float dis)
{
  if (id >= 1 && id <= 6) {
    if(abs(uav[id-1].px - uav[id-1].odom_x) < dis && abs(uav[id-1].py - uav[id-1].odom_y) < dis){
      return true;
    }
  }
  return false;
}


// 封装轨迹加载函数 (新增)
void load_trajectory() {
  std::string package_name = "fcu_core";   

  std::string pkg_path = ros::package::getPath(package_name);  // 获取功能包的绝对路径
  
  if (pkg_path.empty()) {
      ROS_ERROR("没找到包: %s. ", package_name.c_str());
      return;
  }
  
  std::string file_path = pkg_path + "/path/traj.csv"; // 拼接得到完整路径
  std::ifstream file(file_path);
  if (file.is_open()) {
    std::string line;
    bool is_header = true;
    while (std::getline(file, line)) {
      if (is_header) { is_header = false; continue; } // 跳过表头
      std::stringstream ss(line);
      std::string val;
      Waypoint wp;
      std::getline(ss, val, ','); wp.timestamp = std::stod(val);
      for (int i = 0; i < 6; ++i) {
        std::getline(ss, val, ','); wp.uavs[i].x = std::stof(val);
        std::getline(ss, val, ','); wp.uavs[i].y = std::stof(val);
        std::getline(ss, val, ','); wp.uavs[i].z = std::stof(val);
        std::getline(ss, val, ','); wp.uavs[i].yaw = std::stof(val);
      }
      trajectory_data.push_back(wp);
    }
    file.close();
    ROS_INFO("轨迹预加载完成: %zu 个点.", trajectory_data.size());
  } else {
    ROS_WARN("无法打开轨迹文件: %s. enable_path 将无法工作.", file_path.c_str());
  }
}

// 封装控制生成函数
void create_control(float dt) {
  static int traj_counter = 0;
  static bool last_enable_path = false;
  static geometry_msgs::Point circle_center= {};

  // 检测 enable_path 的上升沿，确保每次开启时从头开始
  if (enable_path && !last_enable_path) {
    traj_counter = 0;
  }
  last_enable_path = enable_path;

  if(enable_track){
    theta+=M_PI/20/200;
    for(int i=0; i<6; i++) {
      uav[i].yaw = 0.0f;
      uav[i].px = 1.0*cosf(theta+M_PI*2*i/6)+2;
      uav[i].py = 1.0*sinf(theta+M_PI*2*i/6)+2;
      uav[i].pz = 0.6;
    }
  }else if(enable_path){
    // --- 修改后的 200Hz 适配 10Hz 逻辑 ---
    int idx = traj_counter / 20; // 200Hz 循环，每 20 次循环切换一个 10Hz 的点
    if (!trajectory_data.empty() && idx < trajectory_data.size()) {
      const Waypoint& current_wp = trajectory_data[idx];
      for (int i = 0; i < 6; ++i) {
        uav[i].px = current_wp.uavs[i].x;
        uav[i].py = current_wp.uavs[i].y;
        uav[i].pz = current_wp.uavs[i].z;
        uav[i].yaw = current_wp.uavs[i].yaw;
      }
      if (start_move) {// 仅在 start_move 为 true 时推进轨迹
        traj_counter++; 
      }
    } else if (idx >= trajectory_data.size()) {
      // 轨迹播放完毕，此处可选择重置或维持现状
      // traj_counter = 0; 
    }
  }else{
      switch(enable_pos){
        case 1:
          switch (direction) {
            case 1: circle_center.x += Linear_speed * dt; break;
            case 2: circle_center.x -= Linear_speed * dt; break;
            case 3: circle_center.y -= Linear_speed * dt; break;
            case 4: circle_center.y += Linear_speed * dt; break;
          }
          for(int i=0; i<6; i++){
            uav[i].px = circle_center.x;
            uav[i].py = circle_center.y + i*d;
            uav[i].pz = Hight;
          }
          break;

        case 2:
          switch (direction) {
            case 1: circle_center.x += Linear_speed * dt; break;
            case 2: circle_center.x -= Linear_speed * dt; break;
            case 3: circle_center.y -= Linear_speed * dt; break;
            case 4: circle_center.y += Linear_speed * dt; break;
          }
          uav[0].px = circle_center.x; 
          uav[0].py = circle_center.y; 
          uav[0].pz = Hight;

          uav[1].px = circle_center.x; 
          uav[1].py = circle_center.y + d; 
          uav[1].pz = Hight;

          uav[2].px = circle_center.x; 
          uav[2].py = circle_center.y - d; 
          uav[2].pz = Hight;

          uav[3].px = circle_center.x + 2*d; 
          uav[3].py = circle_center.y; 
          uav[3].pz = Hight;

          uav[4].px = circle_center.x + 2*d; 
          uav[4].py = circle_center.y + 2*d; 
          uav[4].pz = Hight;

          uav[5].px = circle_center.x + 2*d; 
          uav[5].py = circle_center.y - 2*d; 
          uav[5].pz = Hight;

          break;  
        case 3:
          switch (direction) {
            case 1: circle_center.x += Linear_speed * dt; break;
            case 2: circle_center.x -= Linear_speed * dt; break;
            case 3: circle_center.y -= Linear_speed * dt; break;
            case 4: circle_center.y += Linear_speed * dt; break;
          }
          uav[0].px = circle_center.x; 
          uav[0].py = circle_center.y; 
          uav[0].pz = Hight;

          uav[1].px = circle_center.x; 
          uav[1].py = circle_center.y + d; 
          uav[1].pz = Hight;

          uav[2].px = circle_center.x; 
          uav[2].py = circle_center.y - d; 
          uav[2].pz = Hight;

          uav[3].px = circle_center.x + 2*d; 
          uav[3].py = circle_center.y; 
          uav[3].pz = Hight;
          break;  
        case 4:
          break;
      }
  }
}

// 执行任务函数 001-006
void execute_mission_001(const ros::TimerEvent &event){
  if(get_pos_cmd) return;
  if(set_goal && !use_goal_001) return;
  mission_001.layout.dim.push_back(std_msgs::MultiArrayDimension());
  mission_001.layout.dim[0].label = "mission_001";
  mission_001.layout.dim[0].size = 11;
  mission_001.layout.dim[0].stride = 1;
  mission_001.data.resize(11);
  mission_001.data[0]=uav[0].yaw; 
  mission_001.data[1]=0.0;
  mission_001.data[2]=uav[0].px; 
  mission_001.data[3]=uav[0].py; 
  mission_001.data[4]=uav[0].pz;
  mission_001.data[5]=vx; 
  mission_001.data[6]=vy; 
  mission_001.data[7]=vz;
  mission_001.data[8]=ax; 
  mission_001.data[9]=ay; 
  mission_001.data[10]=az;
  mission_pub_001.publish(mission_001);
  use_goal_001=false;     
}

static std_msgs::Float32MultiArray mission_002;
static ros::Publisher mission_pub_002;
void execute_mission_002(const ros::TimerEvent &event){
  if(get_pos_cmd || (set_goal && !use_goal_002)) return;
  mission_002.layout.dim.push_back(std_msgs::MultiArrayDimension());
  mission_002.layout.dim[0].label = "mission_002";
  mission_002.layout.dim[0].size = 11;
  mission_002.layout.dim[0].stride = 1;
  mission_002.data.resize(11);
  mission_002.data[0]=uav[1].yaw; 
  mission_002.data[1]=0.0f;
  mission_002.data[2]=uav[1].px; 
  mission_002.data[3]=uav[1].py; 
  mission_002.data[4]=uav[1].pz;
  mission_002.data[5]=0.0f; 
  mission_002.data[6]=0.0f; 
  mission_002.data[7]=0.0f;
  mission_002.data[8]=0.0f; 
  mission_002.data[9]=0.0f; 
  mission_002.data[10]=0.0f;
  mission_pub_002.publish(mission_002);
  use_goal_002=false;
}

static std_msgs::Float32MultiArray mission_003;
static ros::Publisher mission_pub_003;
void execute_mission_003(const ros::TimerEvent &event){
  if(get_pos_cmd || (set_goal && !use_goal_003)) return;
  mission_003.layout.dim.push_back(std_msgs::MultiArrayDimension());
  mission_003.layout.dim[0].label = "mission_003";
  mission_003.layout.dim[0].size = 11;
  mission_003.layout.dim[0].stride = 1;
  mission_003.data.resize(11);
  mission_003.data[0]=uav[2].yaw; 
  mission_003.data[1]=0.0f;
  mission_003.data[2]=uav[2].px; 
  mission_003.data[3]=uav[2].py; 
  mission_003.data[4]=uav[2].pz;
  mission_003.data[5]=0.0f; 
  mission_003.data[6]=0.0f; 
  mission_003.data[7]=0.0f;
  mission_003.data[8]=0.0f; 
  mission_003.data[9]=0.0f; 
  mission_003.data[10]=0.0f;
  mission_pub_003.publish(mission_003);
  use_goal_003=false;
}

static std_msgs::Float32MultiArray mission_004;
static ros::Publisher mission_pub_004;
void execute_mission_004(const ros::TimerEvent &event){
  if(get_pos_cmd || (set_goal && !use_goal_004)) return;
  mission_004.layout.dim.push_back(std_msgs::MultiArrayDimension());
  mission_004.layout.dim[0].label = "mission_004";
  mission_004.layout.dim[0].size = 11;
  mission_004.layout.dim[0].stride = 1;
  mission_004.data.resize(11);
  mission_004.data[0]=uav[3].yaw; 
  mission_004.data[1]=0.0f;
  mission_004.data[2]=uav[3].px; 
  mission_004.data[3]=uav[3].py; 
  mission_004.data[4]=uav[3].pz;
  mission_004.data[5]=0.0f; 
  mission_004.data[6]=0.0f; 
  mission_004.data[7]=0.0f;
  mission_004.data[8]=0.0f; 
  mission_004.data[9]=0.0f; 
  mission_004.data[10]=0.0f;
  mission_pub_004.publish(mission_004);
  use_goal_004=false;
}

static std_msgs::Float32MultiArray mission_005;
static ros::Publisher mission_pub_005;
void execute_mission_005(const ros::TimerEvent &event){
  if(get_pos_cmd || (set_goal && !use_goal_005)) return;
  mission_005.layout.dim.push_back(std_msgs::MultiArrayDimension());
  mission_005.layout.dim[0].label = "mission_005";
  mission_005.layout.dim[0].size = 11;
  mission_005.layout.dim[0].stride = 1;
  mission_005.data.resize(11);
  mission_005.data[0]=uav[4].yaw; 
  mission_005.data[1]=0.0f;
  mission_005.data[2]=uav[4].px; 
  mission_005.data[3]=uav[4].py; 
  mission_005.data[4]=uav[4].pz;
  mission_005.data[5]=0.0f; 
  mission_005.data[6]=0.0f; 
  mission_005.data[7]=0.0f;
  mission_005.data[8]=0.0f; 
  mission_005.data[9]=0.0f; 
  mission_005.data[10]=0.0f;
  mission_pub_005.publish(mission_005);
  use_goal_005=false;
}

static std_msgs::Float32MultiArray mission_006;
static ros::Publisher mission_pub_006;
void execute_mission_006(const ros::TimerEvent &event){
  if(get_pos_cmd || (set_goal && !use_goal_006)) return;
  mission_006.layout.dim.push_back(std_msgs::MultiArrayDimension());
  mission_006.layout.dim[0].label = "mission_006";
  mission_006.layout.dim[0].size = 11;
  mission_006.layout.dim[0].stride = 1;
  mission_006.data.resize(11);
  mission_006.data[0]=uav[5].yaw; 
  mission_006.data[1]=0.0f;
  mission_006.data[2]=uav[5].px; 
  mission_006.data[3]=uav[5].py; 
  mission_006.data[4]=uav[5].pz;
  mission_006.data[5]=0.0f; 
  mission_006.data[6]=0.0f; 
  mission_006.data[7]=0.0f;
  mission_006.data[8]=0.0f; 
  mission_006.data[9]=0.0f; 
  mission_006.data[10]=0.0f;
  mission_pub_006.publish(mission_006);
  use_goal_006=false;
}

// 里程计数据统一处理逻辑
void process_odom(int index, const nav_msgs::Odometry::ConstPtr& odom) {
  uav[index].odom_x=(float)odom->pose.pose.position.x;
  uav[index].odom_y=-(float)odom->pose.pose.position.y;
  uav[index].odom_z=(float)odom->pose.pose.position.z;
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

void pos_cmd_handler(const quadrotor_msgs::PositionCommand::ConstPtr& pose_plan)
{
  if(follow_forward||follow_down) return;
  get_pos_cmd=true;
  mission_001.layout.dim.push_back(std_msgs::MultiArrayDimension());
  mission_001.layout.dim[0].label = "mission_001";
  mission_001.layout.dim[0].size = 11;
  mission_001.layout.dim[0].stride = 1;
  mission_001.data.resize(11);
  mission_001.data[0]=-pose_plan->yaw; 
  mission_001.data[1]=-pose_plan->yaw_dot;
  mission_001.data[2]=pose_plan->position.x; 
  mission_001.data[3]=-pose_plan->position.y; 
  mission_001.data[4]=pose_plan->position.z;
  mission_001.data[5]=pose_plan->velocity.x; 
  mission_001.data[6]=-pose_plan->velocity.y; 
  mission_001.data[7]=pose_plan->velocity.z;
  mission_001.data[8]=pose_plan->acceleration.x; 
  mission_001.data[9]=-pose_plan->acceleration.y; 
  mission_001.data[10]=pose_plan->acceleration.z;
  mission_pub_001.publish(mission_001);
}

void follow_handler(const std_msgs::Float32MultiArray::ConstPtr& follow){
  if(follow_forward){
    get_pos_cmd=true;
    //发布mission
    if(follow->data[2]==0.0f&&follow->data[3]==0.0f){ 
      printf("No tracking!\n"); 
      return; 
    }
    float global_dx = follow->data[2] * cosf(uav[0].odom_yaw) - follow->data[3] * sinf(uav[0].odom_yaw);
    float global_dy = follow->data[2] * sinf(uav[0].odom_yaw) + follow->data[3] * cosf(uav[0].odom_yaw);
    mission_001.layout.dim.push_back(std_msgs::MultiArrayDimension());
    mission_001.layout.dim[0].label = "mission_001";
    mission_001.layout.dim[0].size = 11;
    mission_001.layout.dim[0].stride = 1;
    mission_001.data.resize(11);
    mission_001.data[0]=uav[0].odom_yaw+follow->data[0]; 
    mission_001.data[1]=0.0f;
    mission_001.data[2]=uav[0].odom_x+global_dx; 
    mission_001.data[3]=uav[0].odom_y+global_dy; 
    mission_001.data[4]=0.0f;
    mission_001.data[5]=0.0f; 
    mission_001.data[6]=0.0f; 
    mission_001.data[7]=0.0f;
    mission_001.data[8]=0.0f; 
    mission_001.data[9]=0.0f; 
    mission_001.data[10]=0.0f;
    mission_pub_001.publish(mission_001); 
  }else if(follow_down){
    get_pos_cmd=true;
    //发布mission
    if(follow->data[2]==0.0f&&follow->data[3]==0.0f){ 
      printf("No tracking!\n"); 
      return; 
    }
    float global_dx = follow->data[2] * cosf(uav[0].odom_yaw) - follow->data[3] * sinf(uav[0].odom_yaw);
    float global_dy = follow->data[2] * sinf(uav[0].odom_yaw) + follow->data[3] * cosf(uav[0].odom_yaw);
    mission_001.layout.dim.push_back(std_msgs::MultiArrayDimension());
    mission_001.layout.dim[0].label = "mission_001";
    mission_001.layout.dim[0].size = 11;
    mission_001.layout.dim[0].stride = 1;
    mission_001.data.resize(11);
    mission_001.data[0]=0.0f; 
    mission_001.data[1]=0.0f;
    mission_001.data[2]=uav[0].odom_x+global_dx; 
    mission_001.data[3]=uav[0].odom_y+global_dy; 
    mission_001.data[4]=0.0f;
    mission_001.data[5]=0.0f; 
    mission_001.data[6]=0.0f; 
    mission_001.data[7]=0.0f;
    mission_001.data[8]=0.0f; 
    mission_001.data[9]=0.0f; 
    mission_001.data[10]=0.0f;
    mission_pub_001.publish(mission_001);
  }
}

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
  ros::Subscriber pos_cmd=nh.subscribe<quadrotor_msgs::PositionCommand>("pos_cmd", 100, pos_cmd_handler);
  ros::Subscriber mission_follow=nh.subscribe<std_msgs::Float32MultiArray>("mission_follow", 100, follow_handler);

  mission_pub_001 = nh.advertise<std_msgs::Float32MultiArray>("mission_001",100);
  mission_pub_002 = nh.advertise<std_msgs::Float32MultiArray>("mission_002",100);
  mission_pub_003 = nh.advertise<std_msgs::Float32MultiArray>("mission_003",100);
  mission_pub_004 = nh.advertise<std_msgs::Float32MultiArray>("mission_004",100);
  mission_pub_005 = nh.advertise<std_msgs::Float32MultiArray>("mission_005",100);
  mission_pub_006 = nh.advertise<std_msgs::Float32MultiArray>("mission_006",100);
  
  ros::Timer timer_mission_001 = nh.createTimer(ros::Duration(0.1),execute_mission_001,false);
  ros::Timer timer_mission_002 = nh.createTimer(ros::Duration(0.1),execute_mission_002,false);
  ros::Timer timer_mission_003 = nh.createTimer(ros::Duration(0.1),execute_mission_003,false);
  ros::Timer timer_mission_004 = nh.createTimer(ros::Duration(0.1),execute_mission_004,false);
  ros::Timer timer_mission_005 = nh.createTimer(ros::Duration(0.1),execute_mission_005,false);
  ros::Timer timer_mission_006 = nh.createTimer(ros::Duration(0.1),execute_mission_006,false);

  // --- 轨迹文件预加载 ---
  load_trajectory();

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