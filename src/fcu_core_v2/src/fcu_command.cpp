#include <ros/ros.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <std_msgs/Int16.h>

static char buf[16] = {0};
static std_msgs::Int16 cmd;

int main(int argc, char **argv) {

  ros::init(argc, argv, "fcu_command");
  ros::NodeHandle nh("~");
  ros::Publisher command = nh.advertise<std_msgs::Int16>("command",100);

  while (ros::ok()) {
    // 获取从键盘输入的数据
    printf("请输入指令：\n");
    ssize_t size = read(STDIN_FILENO, buf, sizeof(buf));
    
    if(size > 0){
      // size=2 代表 "x\n", size=3 代表 "xx\n"
      if(size != 2 && size != 3){
        printf("指令错误！(请输入1或2位字符)\n");
        continue;
      }

      // 【修改点2】: 恢复字符串结尾处理，这是 strcmp 正常工作的前提
      if (buf[size - 1] == '\n') {
        buf[size - 1] = '\0';
      } else {
        buf[size] = '\0'; // 保险起见
      }

    } else {
      printf("禁用指令\n");
      ros::shutdown();
      return 0;
    }

    // 【修改点3】: 优先判断多字符指令 (移植自旧版本)
    // 必须放在 switch(buf[0]) 之前，防止例如 "aa" 被 switch 中的 'a' 截获
    if (strcmp(buf, "v+") == 0) {
      printf("加速\n");
      cmd.data = 101;
      command.publish(cmd);
    } 
    else if (strcmp(buf, "v-") == 0) {
      printf("减速\n");
      cmd.data = 102;
      command.publish(cmd);
    }
    else if (strcmp(buf, "ww") == 0) {
      printf("向前\n");
      cmd.data = 103;
      command.publish(cmd);
    }
    else if (strcmp(buf, "ss") == 0) {
      printf("向后\n");
      cmd.data = 104;
      command.publish(cmd);
    }
    else if (strcmp(buf, "aa") == 0) {
      printf("向左\n");
      cmd.data = 105;
      command.publish(cmd);
    }
    else if (strcmp(buf, "dd") == 0) {
      printf("向右\n");
      cmd.data = 106;
      command.publish(cmd);
    }
    else if (strcmp(buf, "h+") == 0) {
      printf("高度上升\n");
      cmd.data = 107;
      command.publish(cmd);
    }
    else if (strcmp(buf, "h-") == 0) {
      printf("高度下降\n");
      cmd.data = 108;
      command.publish(cmd);
    }
    else if (strcmp(buf, "r1") == 0) {
      printf("r1\n");
      cmd.data = 109;
      command.publish(cmd);
    }
    else if (strcmp(buf, "r2") == 0) {
      printf("r2\n");
      cmd.data = 110;
      command.publish(cmd);
    }
    else if (strcmp(buf, "r3") == 0) {
      printf("r3\n");
      cmd.data = 111;
      command.publish(cmd);
    }
    else if (strcmp(buf, "c1") == 0) {
      printf("case 1\n");
      cmd.data = 112;
      command.publish(cmd);
    }
    else if (strcmp(buf, "c2") == 0) {
      printf("case 2\n");
      cmd.data = 113;
      command.publish(cmd);
    }
    else if (strcmp(buf, "c3") == 0) {
      printf("case 3\n");
      cmd.data = 114;
      command.publish(cmd);
    }
    else if (strcmp(buf, "c4") == 0) {
      printf("case 4\n");
      cmd.data = 115;
      command.publish(cmd);
    }
    else if (strcmp(buf, "c5") == 0) {
      printf("case 5\n");
      cmd.data = 116;
      command.publish(cmd);
    }
    else if (strcmp(buf, "c6") == 0) {
      printf("case 6\n");
      cmd.data = 117;
      command.publish(cmd);
    }
    else {
      // 【修改点4】: 如果不是多字符指令，进入新版本的单字符判断逻辑
      switch(buf[0]){
        case 'p':
          printf("执行路径\n");
          cmd.data=0;
          command.publish(cmd);
          break;
        case 'a':
          printf("解锁\n");
          cmd.data=1;
          command.publish(cmd);
          break;
        case 'd':
          printf("锁定\n");
          cmd.data=2;
          command.publish(cmd);
          break;
        case 't':
          printf("起飞\n");
          cmd.data=3;
          command.publish(cmd);
          break;
        case 'l':
          printf("降落\n");
          cmd.data=4;
          command.publish(cmd);
          break;
        case 'r':
          printf("运行\n");
          cmd.data=5;
          command.publish(cmd);
          break;
        case 's':
          printf("停止\n");
          cmd.data=6;
          command.publish(cmd);
          break;
        case 'm':{
          static bool start_move = false;
          start_move = !start_move;
          printf("运动：%s\n", start_move ? "ture" : "false");
          cmd.data=7;
          command.publish(cmd);}
          break;  
        case 'q':
          printf("目标机停止运动\n");
          cmd.data=8;
          command.publish(cmd); 
          break;     
        case '+':
          printf("视觉定位\n");
          cmd.data=9;
          command.publish(cmd); 
          break;     
        case '-':
          printf("真值\n");
          cmd.data=10;
          command.publish(cmd); 
          break;  
        case '1':
          printf("1号机起飞\n");
          cmd.data=11;
          command.publish(cmd);
          break;
        case '2':
          printf("2号机起飞\n");
          cmd.data=12;
          command.publish(cmd); 
          break;
        case '3':
          printf("3号机起飞\n");
          cmd.data=13;
          command.publish(cmd); 
          break;
        case '4':
          printf("4号机起飞\n");
          cmd.data=14;
          command.publish(cmd); 
          break;    
        case '5':
          printf("5号机起飞\n");
          cmd.data=15;
          command.publish(cmd); 
          break;       
        case '6':
          printf("6号机起飞\n");
          cmd.data=16;
          command.publish(cmd); 
          break;     
        case 'b':
          printf("开始同步\n");
          cmd.data=200;
          command.publish(cmd); 
          break;
        // 注释掉的部分保留为注释
        // case 'q': ... 
        // case 'w': ...
        // case 'e': ...
        default:
          printf("非法指令！\n");
          break;
      }
    }
  }

  return 0;
}