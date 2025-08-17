#ifndef MOTOR_H
#define MOTOR_H
#include "main.h"
#include "tim.h"
#include "sys.h"
#include "delay.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "speed_planning.h"

// 卡尔曼滤波器结构体
typedef struct {
    float x;       // 状态估计(速度)
    float P;       // 估计误差协方差
    float Q;       // 过程噪声协方差
    float R;       // 测量噪声协方差
    float K;       // 卡尔曼增益
} KalmanFilter;

// 函数声明
void Speed_Planner_Init(void);
void Set_Target_Speed_Smooth(float speed_left, float speed_right);
void Kalman_Filter_Init(void);
float Calculate_Motor_A_Speed(void);
float Calculate_Motor_B_Speed(void);
int Incremental_PI_A(float Encoder, float Target);
int Incremental_PI_B(float Encoder, float Target);
void Set_Pwm(int motor_a_pwm, int motor_b_pwm);

#endif //MOTOR_H
