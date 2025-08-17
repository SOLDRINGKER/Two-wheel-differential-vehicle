#include "interrupt.h"

float speed_a = 0.00f;
float speed_b = 0.00f;
float target_a = 0.00f;
float target_b = 0.00f;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        // 1. 读取电机实际速度
        speed_a = Calculate_Motor_A_Speed();
        speed_b = Calculate_Motor_B_Speed();

        // 2. 更新S曲线规划，获取平滑的目标速度，不再传入dt参数
        target_a = S_Curve_Update(&Left_Wheel_Planner, speed_a);
        target_b = S_Curve_Update(&Right_Wheel_Planner, speed_b);

        // 3. 计算PWM控制值
        int pwm_a = Incremental_PI_A(speed_a, target_a);
        int pwm_b = Incremental_PI_B(speed_b, target_b);

        // 4. 设置电机PWM输出
        Set_Pwm(pwm_a, pwm_b);



    }
}
