#ifndef __SPEED_PLANNING_H
#define __SPEED_PLANNING_H

#include "main.h"
#include <stdint.h>
#include <math.h>

// 简化的S曲线状态枚举
typedef enum {
    IDLE = 0,         // 空闲状态
    ACCELERATING,     // 加速状态
    STEADY,           // 匀速状态
    DECELERATING      // 减速状态
} S_Curve_State;

// S型曲线参数结构体
typedef struct {
    float start_speed;    // 起始速度 (m/s)
    float target_speed;   // 目标速度 (m/s)
    float current_speed;  // 当前规划速度 (m/s)
    float actual_speed;   // 实际测量速度 (m/s)
    float max_acc;        // 最大加速度 (m/s^2)
    S_Curve_State state;  // 当前状态
    uint8_t target_reached; // 是否到达目标速度标志
    float speed_threshold;  // 判断到达目标的速度阈值
} S_Curve_Planner;

// 函数声明
void S_Curve_Init(S_Curve_Planner *planner, float start_speed, float target_speed, float max_acc);
float S_Curve_Update(S_Curve_Planner *planner, float actual_speed);
void S_Curve_Set_Target(S_Curve_Planner *planner, float target_speed);
void Set_Target_Speed_Smooth(float speed_left, float speed_right);

extern S_Curve_Planner Left_Wheel_Planner, Right_Wheel_Planner;

#endif
