#include "interrupt.h"

float speed_a = 0.00f;
float speed_b = 0.00f;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        // 1. 读取电机实际速度
        speed_a = Calculate_Motor_A_Speed();
        speed_b = Calculate_Motor_B_Speed();

        float target_a = 0.05f;  // 0.05 m/s
        float target_b = 0.05f;  // 0.05 m/s

        // 3. 计算PWM控制值
        int pwm_a = Incremental_PI_A(speed_a, target_a);
        int pwm_b = Incremental_PI_B(speed_b, target_b);

        // 4. 设置电机PWM输出
        Set_Pwm(pwm_a, pwm_b);
    }
}
