#!/usr/bin/env python3

import rospy
import tf2_ros
import math
from geometry_msgs.msg import TransformStamped, PoseStamped
from nav_msgs.msg import Path
from tf2_ros import TransformBroadcaster

def main():
    rospy.init_node('drone_trajectory')

    # 参数：无人机前缀（如 drone1_）
    drone_prefix = rospy.get_param('～drone_prefix', 'drone1_')
    rate = rospy.Rate(30)  # 30 Hz

    # TF Broadcaster
    br = TransformBroadcaster()

    # Path publisher
    path_pub = rospy.Publisher(f'/{drone_prefix}path', Path, queue_size=10)
    path_msg = Path()
    path_msg.header.frame_id = "world"

    start_time = rospy.Time.now().to_sec()

    while not rospy.is_shutdown():
        current_time = rospy.Time.now()
        t = current_time.to_sec() - start_time

        # 示例轨迹：圆形飞行 (半径 1.5m, 高度 1m, 周期 10s)
        radius = 1.5
        height = 1.0
        omega = 2 * math.pi / 10.0  # 10秒一圈

        x = radius * math.cos(omega * t)
        y = radius * math.sin(omega * t)
        z = height

        # === 发布 TF: world -> droneX_base_link ===
        transform = TransformStamped()
        transform.header.stamp = current_time
        transform.header.frame_id = "world"
        transform.child_frame_id = f"{drone_prefix}base_link"
        transform.transform.translation.x = x
        transform.transform.translation.y = y
        transform.transform.translation.z = z
        transform.transform.rotation.w = 1.0  # 无旋转
        br.sendTransform(transform)

        # === 更新并发布 Path ===
        pose = PoseStamped()
        pose.header.stamp = current_time
        pose.header.frame_id = "world"
        pose.pose.position.x = x
        pose.pose.position.y = y
        pose.pose.position.z = z
        pose.pose.orientation.w = 1.0

        path_msg.poses.append(pose)
        path_msg.header.stamp = current_time
        path_pub.publish(path_msg)

        rate.sleep()

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass
