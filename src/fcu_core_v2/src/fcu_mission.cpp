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
#include "fcu_core/des_and_state.h"

/**
 * 注意：工程中mission_xxx话题为发给飞控的目标值，目标位移应为FRU坐标系，目标姿态应为FRD坐标系
 */
// --- 全局配置区 ---
static uint8_t enable_pos=0;
static bool go_back = false,gnss_mode = false, move_stop = false;
constexpr int num_uav = 6;
constexpr int run_uav = 3; // 参与编队的无人机数量 (Case 3 使用)

// 修改半径为 10.0 以便在 Gazebo 中更容易观察队形细节
float Hight = 1.0f, Radius = 10.0f,radius =4.0f, Angular_speed = 0.03f, Linear_speed=0.5f; 
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
        //集群运动控制        
        case 6:  move_stop = !move_stop;    break;    //停止 s
        case 7:  start_move = !start_move;  break;    //运动 m
        case 21: gnss_mode = !gnss_mode;    break;    //gnss mode
        case 20: go_back = !go_back;        break;    //返航

        // 任务库选择
        case 109: path_id=1;    break;  //路径 1
        case 110: path_id=2;    break;  //路径 2
        case 111: path_id=3;    break;  //路径 3
        case 118: path_id=4;    break;  //路径 4
        case 112: enable_pos=1; break;  //case 1
        case 113: enable_pos=2; break;  //case 2
        case 114: enable_pos=3; break;  //case 3
        case 115: enable_pos=4; break;  //case 4
        case 116: enable_pos=5; break;  //case 5
        case 117: enable_pos=6; break;  //case 6

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
            uav[i].des_px = radius*1.5 * cos(current_phase + phase_offset);
            uav[i].des_py = radius*1.5 * sin(current_phase + phase_offset);
            uav[i].des_pz = Hight;
            float dx = 0.0f - uav[i].des_px;
            float dy = 0.0f - uav[i].des_py;
            uav[i].des_yaw = atan2(dy, dx);
            
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
                uav[i].des_yaw = 0.0f;
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
                uav[i].des_yaw = 0.0f;
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

        // 更新历史位置，供下一帧使用
        uav[i].last_des_px = uav[i].des_px;
        uav[i].last_des_py = uav[i].des_py;
    }
}

// 封装控制生成函数
void create_control(int i, float dt){    
    if(gnss_mode) {
        uav[i].state = 1; // 强制进入 Case 1
    }
    switch (uav[i].state)
    {
    case 1://绝对位置控制
        uav[i].GNSS_mode = true;
        uav[i].px = uav[i].des_px;
        uav[i].py = uav[i].des_py;
        uav[i].pz = Hight;
        uav[i].yaw = uav[i].des_yaw;
    break;
    case 2:// --- Case 2: 速度控制 ---
        uav[i].GNSS_mode = false;
        uav[i].vx = uav[i].des_vx;
        uav[i].vy = uav[i].des_vy;
        uav[i].vz = 0.0f;
        uav[i].yaw = uav[i].des_yaw;
    break;
    case 3:// --- Case 3: 静默跟随 (UWB+Vision, No Comm, No GNSS) ---
        {
        // 变量：用于累加"本机期望位置(在本地系下)"
        float sum_local_des_x = 0.0f;
        float sum_local_des_y = 0.0f;
        int valid_obs_count = 0;

        // 遍历所有邻居 (作为参考点)
        for(int j = 0; j < run_uav; j++) {
            if(i == j) continue;

            if(uav[i].cap.u && uav[i].cap.v && uav[j].cap.u) {
                
                // 步骤 1: 建立本地感知系 (获取邻居实际相对位置)
                float dist = uav[i].neighbors[j].distance;
                float bear = uav[i].neighbors[j].bearing; 
                
                // P_j_meas (相对于本机 i 的位置)
                float p_j_meas_x = dist * cos(bear);
                float p_j_meas_y = dist * sin(bear);

                // 步骤 2: 计算期望相对关系 (从任务库)
                // Delta_P_ij_des = P_j_des - P_i_des
                float delta_p_ij_des_x = uav[j].des_px - uav[i].des_px;
                float delta_p_ij_des_y = uav[j].des_py - uav[i].des_py;

                // 步骤 3: 投影与解算
                // P_i_target_local = P_j_meas - Delta_P_ij_des
                float p_i_target_local_x = p_j_meas_x - delta_p_ij_des_x;
                float p_i_target_local_y = p_j_meas_y - delta_p_ij_des_y;

                // 累加
                sum_local_des_x += p_i_target_local_x;
                sum_local_des_y += p_i_target_local_y;
                valid_obs_count++;
            }
        }

        // 步骤 4: 误差生成与控制
        // 基础前馈速度
        float v_cmd_x = uav[i].des_vx; 
        float v_cmd_y = uav[i].des_vy;
        float Kp = 0.3f; // 队形保持增益

        if(valid_obs_count > 0) {
            float local_des_x = sum_local_des_x / valid_obs_count;
            float local_des_y = sum_local_des_y / valid_obs_count;

            float err_x = local_des_x; 
            float err_y = local_des_y;
            float err_mag = std::hypot(err_x, err_y);

            // 引入平滑死区 (例如0.2米)
            float deadzone = 0.2f; 
            if(err_mag > deadzone) {
                // 扣除死区后的有效误差 (连续变化，避免刚出死区就突变)
                float valid_err_ratio = (err_mag - deadzone) / err_mag;
                v_cmd_x += Kp * err_x * valid_err_ratio;
                v_cmd_y += Kp * err_y * valid_err_ratio;
            }
        }
        
        // 【安全优化】：实机速度限幅 (防止 UWB 跳变导致指令突变失控)
        float max_v = 1.0f; // 最大允许速度 1.0 m/s
        float current_v_mag = std::hypot(v_cmd_x, v_cmd_y);
        if (current_v_mag > max_v) {
            v_cmd_x = (v_cmd_x / current_v_mag) * max_v;
            v_cmd_y = (v_cmd_y / current_v_mag) * max_v;
        }

        // 赋值控制量
        uav[i].vx = v_cmd_x;
        uav[i].vy = v_cmd_y;
        uav[i].pz = Hight; 
        uav[i].yaw = uav[i].des_yaw; 
        }
    break;
    case 4: // --- Case 4: 弱感知视觉跟随 (Vision Only, No UWB, No Comms) ---
        {

            // [阶段 A] 基础前馈
            float v_cmd_x = uav[i].des_vx * 1.0f; 
            float v_cmd_y = uav[i].des_vy * 1.0f;

            // [阶段 B] 遍历与筛选 (Data Collection)
            struct VisionData {
                int id;
                float theta_meas; // 观测角度
                float theta_des;  // 期望角度
                float dist_theory;// 理论距离(用于增益调度)
            };
            std::vector<VisionData> vis_data;

            for(int j = 0; j < run_uav; j++) {
                if(i == j) continue; 
                // 仅使用视觉能力
                if(uav[i].cap.v) {
                    //任务解析
                    float dx_des = uav[j].des_px - uav[i].des_px;
                    float dy_des = uav[j].des_py - uav[i].des_py;
                    float theta_des = atan2(dy_des, dx_des);
                    float dist_theory = std::hypot(dx_des, dy_des); // 统一使用 std::hypot
                    if(dist_theory < 0.1f) dist_theory = 0.1f; 

                    float theta_meas = uav[i].neighbors[j].bearing;//感知获取
                    
                    vis_data.push_back({j, theta_meas, theta_des, dist_theory});
                }
            }

            // [阶段 C] 优选基线逻辑 (Baseline Selection)
            // 如果能看到 2 个以上邻居，寻找夹角差异最大的一对
            int best_idx1 = -1, best_idx2 = -1;
            float max_span = -1.0f;

            if (vis_data.size() >= 2) {
                for(size_t a = 0; a < vis_data.size(); a++) {
                    for(size_t b = a + 1; b < vis_data.size(); b++) {
                        // 计算两邻居的观测夹角 (0~PI)
                        float span = fabs(normalize_angle(vis_data[a].theta_meas - vis_data[b].theta_meas));
                        if(span > M_PI) span = 2*M_PI - span; // 取劣弧

                        // 寻找张角最大的一对 (几何GDOP最好)
                        if(span > max_span) {
                            max_span = span;
                            best_idx1 = a;
                            best_idx2 = b;
                        }
                    }
                }
            } else if (vis_data.size() == 1) {
                // 只有一个邻居，没得选
                best_idx1 = 0;
            }

            // [阶段 D] 基于优选基线的控制律合成
            float sum_corr_x = 0.0f;
            float sum_corr_y = 0.0f;
            int valid_count = 0;
            
            // 1. 横向控制 (只针对优选出的邻居)
            std::vector<int> selected_indices;
            if(best_idx1 != -1) selected_indices.push_back(best_idx1);
            if(best_idx2 != -1) selected_indices.push_back(best_idx2);

            float deadzone_theta = 5.0f * M_PI / 180.0f; // 5度死区，避免过度纠正

            for(int idx : selected_indices) {
                VisionData& d = vis_data[idx];
                
                // 角度误差
                float e_theta = normalize_angle(d.theta_meas - d.theta_des);

                // 切向向量
                float n_perp_x = -sin(d.theta_meas);
                float n_perp_y = cos(d.theta_meas);

                float K_omega = 0.5f; 
                if(fabs(e_theta) < deadzone_theta) {
                    e_theta = 0.0f; // 在死区内不进行控制
                }
                float v_corr_mag = K_omega * e_theta * d.dist_theory;

                sum_corr_x += v_corr_mag * n_perp_x;
                sum_corr_y += v_corr_mag * n_perp_y;
                valid_count++;
            }

            if(valid_count > 0) {
                v_cmd_x += sum_corr_x / valid_count;
                v_cmd_y += sum_corr_y / valid_count;
            }

            // 2. 纵向控制 (单目测距与死区控制)
            if (best_idx1 != -1 && best_idx2 != -1) {
                float K_dist = 0.2f; // 纵向距离恢复增益；经验固定值 (0.1f)；决定了无人机超出死区后的推拉力度。
                float deadzone_ratio = 0.3f; // 死区比例系数；经验固定值 (0.4f)；允许测距波动的安全容差范围（±40%）。

                float v_push_x = 0.0f; // 累加推拉速度分量X；循环累加后取平均；用于保持编队宏观尺度的附加速度。
                float v_push_y = 0.0f; // 累加推拉速度分量Y；循环累加后取平均；用于保持编队宏观尺度的附加速度。

                // 使用循环遍历两个优选邻居，避免代码重复
                int target_indices[2] = {best_idx1, best_idx2};
                for(int idx : target_indices) {
                    VisionData& d = vis_data[idx];

                    float diff = uav[i].neighbors[d.id].distance - d.dist_theory; // 绝对距离误差；实际距离-理论距离；衡量当前偏远(正)还是偏近(负)。
                    float deadzone = d.dist_theory * deadzone_ratio; // 绝对死区阈值；理论距离*死区比例；免控区域的具体半径。
                    float err = 0.0f; // 有效补偿误差量；扣除死区后的剩余误差；触发平滑纵向速度补偿的激发量。
                    
                    if (diff > deadzone) {
                        err = diff - deadzone; // 实际距离超出死区上限：太远，需靠近
                    } else if (diff < -deadzone) {
                        err = diff + deadzone; // 实际距离低于死区下限：太近，需远离
                    }
                    
                    // 沿着观测角方向推拉
                    v_push_x += K_dist * err * cos(d.theta_meas);
                    v_push_y += K_dist * err * sin(d.theta_meas);
                }

                // 取两个基线控制量的平均值并叠加到最终速度指令上
                v_cmd_x += v_push_x / 2.0f;
                v_cmd_y += v_push_y / 2.0f;
            }

            // 【安全优化】：实机速度限幅 (防止视觉跳变导致指令突变失控)
            float max_v = 1.0f; // 最大允许速度 1.0 m/s
            float current_v_mag = std::hypot(v_cmd_x, v_cmd_y);
            if (current_v_mag > max_v) {
                v_cmd_x = (v_cmd_x / current_v_mag) * max_v;
                v_cmd_y = (v_cmd_y / current_v_mag) * max_v;
            }

            uav[i].vx = v_cmd_x;
            uav[i].vy = v_cmd_y;
            uav[i].pz = Hight;           
            uav[i].yaw = uav[i].des_yaw; 
        }
        break;

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
static ros::Publisher des_state_pubs[6]; // <--- 新增：用于发布期望状态与能力的发布者数组
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
    mission_msg.data[0] = uav[index].des_yaw; // yaw 
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

    // --- 新增：打包并发布期望状态自定义话题 ---
    fcu_core::des_and_state state_msg;    
    // 1. 表头与基本信息
    state_msg.header.stamp = ros::Time::now();
    state_msg.header.frame_id = "world"; // 统一在世界系下
    state_msg.uav_id = index;            // 飞机编号
    // 2. 期望位置
    state_msg.des_px = uav[index].des_px;
    state_msg.des_py = uav[index].des_py;
    state_msg.des_pz = uav[index].des_pz;
    // 3. 期望速度
    state_msg.des_vx = uav[index].des_vx;
    state_msg.des_vy = uav[index].des_vy;
    state_msg.des_vz = uav[index].des_vz;
    // 4. 当前状态机阶段
    state_msg.state = uav[index].state;
    // 5. 能力标志位 (布尔值)
    state_msg.cap_g = uav[index].cap.g;
    state_msg.cap_c = uav[index].cap.c;
    state_msg.cap_u = uav[index].cap.u;
    state_msg.cap_v = uav[index].cap.v;
    // 6. 发布
    des_state_pubs[index].publish(state_msg);
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

    des_state_pubs[0] = nh.advertise<fcu_core::des_and_state>("/des_state_001", 100);
    des_state_pubs[1] = nh.advertise<fcu_core::des_and_state>("/des_state_002", 100);
    des_state_pubs[2] = nh.advertise<fcu_core::des_and_state>("/des_state_003", 100);
    des_state_pubs[3] = nh.advertise<fcu_core::des_and_state>("/des_state_004", 100);
    des_state_pubs[4] = nh.advertise<fcu_core::des_and_state>("/des_state_005", 100);
    des_state_pubs[5] = nh.advertise<fcu_core::des_and_state>("/des_state_006", 100);

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
        if(!move_stop) {
            for(int i = 0; i < run_uav; i++) {
                create_control(i, dt);
            }
        }

        loop_rate.sleep();
    }
    return 0;
}