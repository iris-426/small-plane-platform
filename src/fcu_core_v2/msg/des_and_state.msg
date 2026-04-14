std_msgs/Header header    # 包含时间戳，方便以后做数据同步或录制rosbag
uint8 uav_id              # 飞机的ID编号 (0 到 5)

# 期望位置 (World Frame)
float32 des_px
float32 des_py
float32 des_pz

# 期望速度
float32 des_vx
float32 des_vy
float32 des_vz

# 状态机阶段
uint8 state               # 当前自身阶段 (1, 2, 3, 4等)

# 能力标志位 Capability
bool cap_g                # 是否有GNSS定位
bool cap_c                # 是否有通讯能力
bool cap_u                # 是否有UWB测距能力
bool cap_v                # 是否有视觉测角能力
