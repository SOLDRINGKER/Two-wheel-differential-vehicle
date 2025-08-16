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

// 函数声明
float Calculate_Motor_A_Speed(void);
float Calculate_Motor_B_Speed(void);
int Incremental_PI_A(float Encoder, float Target);
int Incremental_PI_B(float Encoder, float Target);
void Set_Pwm(int motor_a_pwm, int motor_b_pwm);

#endif //MOTOR_H
