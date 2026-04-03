#include <ros/ros.h>
#include <ros/package.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

// --- 从仿真算法移植的头文件 ---
#include <cmath>                 // 用于数学计算 (sin, cos, atan2, hypot等)
#include <geometry_msgs/Point.h> // 用于 UAVControl 中的 startpoint
#include <std_msgs/Bool.h>       // 预留标准布尔类型
#include <set>

// --- 实机框架专属头文件 ---
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
// --- 全局配置区 ---
static uint8_t enable_pos=0;
static bool go_back = false,gnss_mode = true, move_stop = false;
constexpr int num_uav = 6;
constexpr int run_uav = 3; // 参与编队的无人机数量 (Case 3 使用)

// 修改半径为 10.0 以便在 Gazebo 中更容易观察队形细节
float Hight = 1.0f, Radius = 20.0f,radius =4.0f, Angular_speed = 0.2f, Linear_speed=1.0f; 
static bool start_move = false;
int direction = 0,path_id = 1;

// --- 核心数据结构定义 ---

// 能力标志位 F_i = [g, c, u, v]
struct Capability {
    bool g; // GNSS: 是否有绝对定位
    bool c; // Comm: 是否有通讯能力
    bool u; // UWB: 是否有测距能力
    bool v; // Vision: 是否有测角能力
};

// 邻居观测值 (模拟传感器输入)
struct NeighborObs {
    float distance; // UWB 测距 (m)
    float bearing;  // 视觉/磁罗盘 测得的世界系方位角 (rad) (-PI ~ PI)
};

struct UAVControl {
    ros::Publisher setpoint_pub;
    ros::Publisher des_odom_pub; //用于发布期望状态
    ros::Subscriber state_sub;
    ros::Subscriber odom_sub;

    // 控制量
    float px = 0.0f, py = 0.0f, pz = 0.0f, yaw = 0.0f;
    float vx = 0.0f, vy = 0.0f, vz = 0.0f;
    
    // 反馈量 (真值/里程计)
    float odom_px = 0.0f, odom_py = 0.0f, odom_pz = 0.0f;
    float odom_yaw = 0.0f,odom_roll=0.0f, odom_pitch=0.0f;
    float odom_vx = 0.0f, odom_vy = 0.0f, odom_vz = 0.0f;

    // 轨迹预装载量 (Desired State - World Frame)
    // 存储本机在当前时刻依据任务库计算出的理想状态
    float des_px = 0.0f, des_py = 0.0f, des_pz = 0.0f, des_yaw = 0.0f;
    float des_vx = 0.0f, des_vy = 0.0f, des_vz = 0.0f;

    // [新增] 用于差分计算速度的辅助变量
    float last_des_px = 0.0f;
    float last_des_py = 0.0f;
    bool traj_initialized = false; // 防止第一帧速度激增
    
    // 状态量
    int state = 0;//自身阶段
    bool GNSS_mode = false; // true=位置控制, false=速度控制
    bool takeoff = false;
    geometry_msgs::Point startpoint = {};

    // --- 算法核心数据 ---
    Capability cap = {false, false, false, false}; // 本机能力
    std::vector<NeighborObs> neighbors;            // 对其他所有飞机的观测

    // 初始化函数
    void init(int total_uavs) {
        neighbors.resize(total_uavs);
    }
};

std::vector<UAVControl> uav(num_uav);

// --- 命令处理 ---
void cmdHandler(const std_msgs::Int16::ConstPtr& cmd){
    switch(cmd->data){
        case 0://运行路径 p
            break;
        case 5://运行 r  
            break;
        case 20://返航
            go_back = !go_back;
            break;
        case 21://gnss mode
            gnss_mode = !gnss_mode;
            break;    
        case 109://路径 1
            path_id=1; break;  
        case 110://路径 2
            path_id=2; break;
        case 111://路径 3
            path_id=3; break;      
        case 118://路径 4
            path_id=4; break;    
        case 112://case 1
            enable_pos=1; break;
        case 113://case 2
            enable_pos=2; break;
        case 114://case 3 
            enable_pos=3; break;   
        case 115://case 4
            enable_pos=4; break;
        case 116://case 5
            enable_pos=5; break;
        case 117://case 6
            enable_pos=6; break;
        case 6://停止 s
            move_stop = !move_stop; break;
        case 7://运动 m
            start_move = !start_move; break;
        // 起飞控制
        case 11: uav[0].takeoff = true; break;
        case 12: uav[1].takeoff = true; break;
        case 13: uav[2].takeoff = true; break;
        case 14: uav[3].takeoff = true; break;
        case 15: uav[4].takeoff = true; break;
        case 16: uav[5].takeoff = true; break;
        // 调试控制
        case 107: Hight += 1.0;         break;
        case 108: Hight -= 1.0;         break;     
        case 101: Linear_speed +=0.1;   break;
        case 102: Linear_speed -=0.1;   break;
        case 103: direction = 1;        break;//向前
        case 104: direction = 2;        break;//向后
        case 105: direction = 3;        break;//向左
        case 106: direction = 4;        break;//向右
        case 8:   direction = 0;        break; // stop
        default: break;
    }
}

// --- 辅助函数 ---
// 角度归一化 (-PI, PI]
float normalize_angle(float angle) {
    while (angle > M_PI) angle -= 2.0 * M_PI;
    while (angle <= -M_PI) angle += 2.0 * M_PI;
    return angle;
}

// --- 实机传感器数据获取 (虚拟相对感知) ---
// 在实机实验中，利用高精度的全局 Odom 直接计算相对距离和角度
void update_sensor_data() {
    for (int i = 0; i < num_uav; i++) {
        for (int j = 0; j < num_uav; j++) {
        if (i == j) continue;

        // 利用全局里程计获取两机之间的相对坐标差
        float dx = uav[j].odom_px - uav[i].odom_px;
        float dy = uav[j].odom_py - uav[i].odom_py;
        
        // 计算纯几何距离作为虚拟 UWB 测量值
        float true_dist = std::hypot(dx, dy);
        // 限制一个最小距离防止奇异点
        if(true_dist < 0.01f) true_dist = 0.01f; 
        
        // 计算世界系下的方位角作为虚拟 视觉/罗盘 测量值
        float true_world_angle = atan2(dy, dx);

        // 赋值给本机对邻居的观测数组
        uav[i].neighbors[j].distance = true_dist;      
        uav[i].neighbors[j].bearing = normalize_angle(true_world_angle);  

        }
    }
}

void update_all_mission_states(float dt) {
    
    static geometry_msgs::Point swarm_center= {}; 
    switch (path_id)// 1. 先计算这一时刻的目标位置 (Position Only)
    {
    case 1://原地绕圈
        static float current_phase = 0.0f;
        if(start_move){
            current_phase += Angular_speed * dt;
        }   
        for (int i = 0; i < num_uav; i++) {
            float phase_offset = 2.0 * M_PI / run_uav * i;                     

            // 只计算位置
            uav[i].des_px = radius*2.0 * cos(current_phase + phase_offset);
            uav[i].des_py = radius*2.0 * sin(current_phase + phase_offset);
            uav[i].des_pz = Hight;
            
            // 速度稍后统一计算
        }
        break;

    case 2: // 编队整体绕大圆
        {
            static float theta = 0.0f;
            if (start_move) {
                theta += Angular_speed/3 * dt;;
            }

            // 大圆中心位置
            swarm_center.x = Radius * cos(theta);
            swarm_center.y = -Radius * sin(theta);

            // 编队朝向 (依然需要几何计算，但这属于位置逻辑)
            // 切向方向：dx/dt ~ -sin, dy/dt ~ -cos (顺时针)
            float psi = atan2(-cos(theta), -sin(theta));

            for (int i = 0; i < run_uav; i++) {
                float phi = psi + 2.0f * M_PI / run_uav * i;

                // 只计算位置
                uav[i].des_px = swarm_center.x + radius * cos(phi);
                uav[i].des_py = swarm_center.y + radius * sin(phi);
                uav[i].des_pz = Hight;
            }
        }
        break;
    
    case 3: // 编队整体走直线
        {
            if(start_move){
                swarm_center.x += Linear_speed * dt;
            }
            

            for (int i = 0; i < run_uav; i++) {
                float phi = 2.0f * M_PI / run_uav* i;
                
                uav[i].des_px = swarm_center.x + radius * cos(phi);
                uav[i].des_py = swarm_center.y + radius * sin(phi);
                uav[i].des_pz = Hight;
            }
        }
        break;    

    case 4: // 航点导航式正方形轨迹
        {
            static int wp_idx = 0; // 航点索引状态机
            float target_x = 0.0f, target_y = 0.0f;

            // 1. 在 switch 中设定四个角点
            switch (wp_idx) {
                case 0: target_x = 20.0f; target_y = 0.0f;  break;
                case 1: target_x = 20.0f; target_y = 20.0f; break;
                case 2: target_x = 0.0f;  target_y = 20.0f; break;
                case 3: target_x = 0.0f;  target_y = 0.0f;  break;
            }

            // 2. 计算朝向目标点的矢量和距离
            float dx = target_x - swarm_center.x;
            float dy = target_y - swarm_center.y;
            float dist = std::hypot(dx, dy);

            // 3. 距离检测与步进积分 (仅在 start_move 为 true 时执行)
            if (start_move) {
                if (dist < 0.1f) {
                    wp_idx = (wp_idx + 1) % 4; // 到达阈值，切换到下一个点
                } else {
                    // 沿着单位方向向量以 Linear_speed 移动 dt 的距离
                    swarm_center.x += (dx / dist) * Linear_speed * dt;
                    swarm_center.y += (dy / dist) * Linear_speed * dt;
                }
            }

            // 4. 围绕当前的 swarm_center 分配各机期望位置
            for (int i = 0; i < run_uav; i++) {
                float phi = 2.0f * M_PI / run_uav * i;
                uav[i].des_px = swarm_center.x + radius * cos(phi);
                uav[i].des_py = swarm_center.y + radius * sin(phi);
                uav[i].des_pz = Hight;
            }
        }
        break;
    
    default:
        break;
    }

    for (int i = 0; i < num_uav; i++) { // 2. [核心修改] 统一通过差分计算速度 v = (pos_now - pos_last) / dt
        // 如果是第一帧，先初始化 last_pos，避免速度激增
        if (!uav[i].traj_initialized) {
            uav[i].last_des_px = uav[i].des_px;
            uav[i].last_des_py = uav[i].des_py;
            uav[i].traj_initialized = true;
        }

        // 使用差分法计算速度，避免解析求导
        if (dt > 0.0001f) {
            uav[i].des_vx = (uav[i].des_px - uav[i].last_des_px) / dt;
            uav[i].des_vy = (uav[i].des_py - uav[i].last_des_py) / dt;
            uav[i].des_vz = 0.0f;
        } else {
            uav[i].des_vx = 0.0f; uav[i].des_vy = 0.0f; uav[i].des_vz = 0.0f;
        }

        // 自动计算期望 Yaw (朝向速度方向) 只有当有明显运动时才更新 Yaw，防止停止时乱转
        if (std::hypot(uav[i].des_vx, uav[i].des_vy) > 0.1f) {
            uav[i].des_yaw = atan2(uav[i].des_vy, uav[i].des_vx);
        }

        // 更新历史位置，供下一帧使用
        uav[i].last_des_px = uav[i].des_px;
        uav[i].last_des_py = uav[i].des_py;
    }
}

// 封装控制生成函数
void create_control(float dt) {
    if(gnss_mode){
        for (int i = 0; i < run_uav; i++) {
            uav[i].GNSS_mode = true;
            uav[i].px = uav[i].des_px;
            uav[i].py = uav[i].des_py;
            uav[i].pz = uav[i].des_pz;
            uav[i].yaw = uav[i].des_yaw;
        }
    } else {
        for (int i = 0; i < run_uav; i++) {
            uav[i].GNSS_mode = false;
            uav[i].vx = uav[i].des_vx;
            uav[i].vy = uav[i].des_vy;
            uav[i].vz = uav[i].des_vz;
            uav[i].yaw = uav[i].des_yaw;
        }
    }
  
}

// --- 动态更新无人机能力与状态机 ---
    void update_capabilities() {
    std::set<int> gnss_ids   = {0, 1, 2, 3, 4, 5};                // 拥有 GNSS 的ID集合
    std::set<int> comm_ids   = {};                // 拥有 通讯 的ID集合
    std::set<int> uwb_ids    = {0,1};             // 拥有 UWB 的ID集合
    std::set<int> vision_ids = {0, 1, 2, 3, 4, 5}; // 拥有 视觉 的ID集合

    if(enable_pos == 1) { gnss_ids = {0,1,2};}
    if(enable_pos == 2) { gnss_ids = {}; comm_ids = {0,1,2}; uwb_ids = {0,1,2};}
    if(enable_pos == 3) { gnss_ids = {}; comm_ids = {}; uwb_ids = {0,1,2};}
    if(enable_pos == 4) { gnss_ids = {}; comm_ids = {}; uwb_ids = {};}

    for(int i=0; i<num_uav; i++){
        // --- 自动能力分配 count() 返回 1 表示存在，0 表示不存在 --- 
        uav[i].cap.g = gnss_ids.count(i); 
        uav[i].cap.c = comm_ids.count(i);
        uav[i].cap.u = uwb_ids.count(i); 
        uav[i].cap.v = vision_ids.count(i); 

        // 有 GNSS 则位置控制
        if(uav[i].cap.g){
        uav[i].GNSS_mode = true;
        } else {
        uav[i].GNSS_mode = false; 
        }

        // 状态机分级更新
        if(uav[i].cap.g){
        uav[i].state = 1; // 有 GNSS 则 Lv4
        }else if(uav[i].cap.c){
        uav[i].state = 2; // 有通讯则 Lv3
        }else if(uav[i].cap.u && uav[i].cap.v && uwb_ids.size() >= 2){
        uav[i].state = 3; // 有 UWB 和 视觉 则 Lv2
        }else if(uav[i].cap.v){
        uav[i].state = 4; // 仅有 视觉 则 Lv1
        }

        if(enable_pos == 5) {uav[i].state = 5;}// 强制绝对速度飞行（测试用）
    }
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
  
    // --- 核心修改：根据 GNSS_mode 决定下发逻辑 ---
    float send_px = 0.0f, send_py = 0.0f;
    float send_vx = 0.0f, send_vy = 0.0f;

    if (uav[index].GNSS_mode) {
        // 【位置控制模式】
        send_px = uav[index].px;
        send_py = uav[index].py;
        // 安全补丁：防止目标点恰好为(0,0)时，实机飞控误判为速度模式
        if (fabs(send_px) < 1e-5 && fabs(send_py) < 1e-5) {
            send_px = 0.0001f; 
        }
        // 速度清零，确保飞控不混淆
        send_vx = 0.0f;
        send_vy = 0.0f;
    } else {
        // 【速度控制模式】
        // 强制 px, py 为 0，触发实机飞控的速度控制条件
        send_px = 0.0f;
        send_py = 0.0f;
        // 下发速度
        send_vx = uav[index].vx;
        send_vy = uav[index].vy;
    }

    // 从对应的 uav 结构体中读取控制量并打包
    mission_msg.data[0] = uav[index].yaw; 
    mission_msg.data[1] = 0.0f;
    mission_msg.data[2] = send_px; 
    mission_msg.data[3] = send_py; 
    mission_msg.data[4] = uav[index].pz;   // Z轴无论如何都用位置控制
    mission_msg.data[5] = send_vx; 
    mission_msg.data[6] = send_vy; 
    mission_msg.data[7] = 0.0f;            // vz 目前不用，高度全靠 pz
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

  uav[index].odom_vx = (float)odom->twist.twist.linear.x;
  uav[index].odom_vy = -(float)odom->twist.twist.linear.y; 
  uav[index].odom_vz = (float)odom->twist.twist.linear.z;

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

    for(int i = 0; i < num_uav; i++){
        uav[i].init(num_uav);
    }

    ros::Subscriber comm=nh.subscribe<std_msgs::Int16>("/keyboard_command", 100, cmdHandler);
    ros::Subscriber odom001=nh.subscribe<nav_msgs::Odometry>("odom_global_001", 100, odom_global001_handler);
    ros::Subscriber odom002=nh.subscribe<nav_msgs::Odometry>("odom_global_002", 100, odom_global002_handler);
    ros::Subscriber odom003=nh.subscribe<nav_msgs::Odometry>("odom_global_003", 100, odom_global003_handler);
    ros::Subscriber odom004=nh.subscribe<nav_msgs::Odometry>("odom_global_004", 100, odom_global004_handler);
    ros::Subscriber odom005=nh.subscribe<nav_msgs::Odometry>("odom_global_005", 100, odom_global005_handler);
    ros::Subscriber odom006=nh.subscribe<nav_msgs::Odometry>("odom_global_006", 100, odom_global006_handler);

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
        
        /// --- 传感器数据更新 (虚拟相对感知) ---
        update_sensor_data();

        /// --- 能力状态更新 ---
        update_capabilities();

        // --- 任务状态更新 ---
        update_all_mission_states(dt);

        // --- 控制量生成 ---
        create_control(dt);

        loop_rate.sleep();
    }
    return 0;
}