#include "motor.h"



// 卡尔曼滤波器实例
KalmanFilter KalmanA, KalmanB;

/**
 * 函数功能：初始化卡尔曼滤波器
 * 入口参数：滤波器结构体指针
 * 返回  值：无
 */
void Kalman_Init(KalmanFilter *filter) {
    filter->x = 0;         // 初始速度估计: 0 m/s
    filter->P = 1.0f;      // 初始估计误差较高
    filter->Q = 0.001f;    // 过程噪声(速度变化)
    filter->R = 0.05f;     // 测量噪声(编码器噪声)
    filter->K = 0;         // 初始卡尔曼增益
}

/**
 * 函数功能：更新卡尔曼滤波器
 * 入口参数：滤波器结构体指针，新测量值
 * 返回  值：滤波后的估计值
 */
float Kalman_Update(KalmanFilter *filter, float measurement) {
    // 预测步骤
    // (无状态模型更新，因为我们假设速度在两次更新之间基本保持不变)
    filter->P = filter->P + filter->Q;

    // 更新步骤
    filter->K = filter->P / (filter->P + filter->R);
    filter->x = filter->x + filter->K * (measurement - filter->x);
    filter->P = (1 - filter->K) * filter->P;

    return filter->x;
}

/**
 * 函数功能：初始化所有卡尔曼滤波器
 * 入口参数：无
 * 返回  值：无
 */
void Kalman_Filter_Init(void) {
    Kalman_Init(&KalmanA);
    Kalman_Init(&KalmanB);
}

/**
 * 函数功能：读取电机A编码器并计算实际速度
 * 入口参数：无
 * 返回  值：电机A实际速度，单位m/s
 */
float Calculate_Motor_A_Speed(void)
{
    // 定义常量
    const float wheel_perimeter = 3.1415926f * 0.065f;     // 轮子周长 = π * 直径(0.065m) = 0.204m
    const float encoder_precision = 13.0f * 30.0f * 4.0f;  // 编码器精度 = 编码器线数(13) * 减速比(30) * 倍频数(4) = 1560
    const float control_frequency = 100.0f;                // 控制频率(Hz)

    // 1. 直接读取TIM2编码器计数值
    short encoder_value = (short)TIM2->CNT;

    // 2. 立即重置计数器
    TIM2->CNT = 0;

    // 3. 计算实际速度(m/s)
    float raw_speed = encoder_value * control_frequency * wheel_perimeter / encoder_precision;

    // 4. 应用卡尔曼滤波
    float filtered_speed = Kalman_Update(&KalmanA, raw_speed);

    // 4. 速度限幅 - 防止异常值
    float max_speed = 0.1f;  // 最大速度限制为0.1m/s
    if(filtered_speed > max_speed) filtered_speed = max_speed;
    if(filtered_speed < -max_speed) filtered_speed = -max_speed;

    return filtered_speed;
}

/**
 * 函数功能：读取电机B编码器并计算实际速度
 * 入口参数：无
 * 返回  值：电机B实际速度，单位m/s
 */
float Calculate_Motor_B_Speed(void)
{
    // 定义常量
    const float wheel_perimeter = 3.1415926f * 0.065f;     // 轮子周长 = π * 直径(0.065m) = 0.204m
    const float encoder_precision = 13.0f * 30.0f * 4.0f;  // 编码器精度 = 编码器线数(13) * 减速比(30) * 倍频数(4) = 1560
    const float control_frequency = 100.0f;                // 控制频率(Hz)

    // 1. 直接读取TIM4编码器计数值
    short encoder_value = (short)TIM4->CNT;

    // 2. 立即重置计数器
    TIM4->CNT = 0;

    // 3. 根据安装方向调整极性
    encoder_value = -encoder_value;  // 右轮需要取反

    // 4. 计算实际速度(m/s)
    float raw_speed = encoder_value * control_frequency * wheel_perimeter / encoder_precision;

     // 5. 应用卡尔曼滤波
    float filtered_speed = Kalman_Update(&KalmanB, raw_speed);

    // 5. 速度限幅 - 防止异常值
    float max_speed = 0.1f;  // 最大速度限制为0.1m/s
    if(filtered_speed > max_speed) filtered_speed = max_speed;
    if(filtered_speed < -max_speed) filtered_speed = -max_speed;

    return filtered_speed;
}

/**
 * 函数功能：电机A的增量式PI控制器
 * 入口参数：编码器测量值，目标速度
 * 返回  值：电机PWM
 */
int Incremental_PI_A(float Encoder, float Target)
{
    static float Bias, Pwm, Last_bias;
    static float Velocity_KP = 500.0f;  // 电机A的PI参数Kp
    static float Velocity_KI = 500.0f;  // 电机A的PI参数Ki

    Bias = Target - Encoder;  // 计算偏差
    Pwm += Velocity_KP * (Bias - Last_bias) + Velocity_KI * Bias; // PI控制器

    // PWM限幅
    if(Pwm > 7200) Pwm = 7200;
    if(Pwm < -7200) Pwm = -7200;

    Last_bias = Bias;  // 保存上一次偏差
    return Pwm;
}

/**
 * 函数功能：电机B的增量式PI控制器
 * 入口参数：编码器测量值，目标速度
 * 返回  值：电机PWM
 */
int Incremental_PI_B(float Encoder, float Target)
{
    static float Bias, Pwm, Last_bias;
    static float Velocity_KP = 500.0f;  // 电机B的PI参数Kp
    static float Velocity_KI = 500.0f;  // 电机B的PI参数Ki

    Bias = Target - Encoder;  // 计算偏差
    Pwm += Velocity_KP * (Bias - Last_bias) + Velocity_KI * Bias; // PI控制器

    // PWM限幅
    if(Pwm > 7200) Pwm = 7200;
    if(Pwm < -7200) Pwm = -7200;

    Last_bias = Bias;  // 保存上一次偏差
    return Pwm;
}

/**
 * 函数功能：设置电机PWM
 * 入口参数：左右电机PWM值
 * 返回  值：无
 */
void Set_Pwm(int motor_a_pwm, int motor_b_pwm)
{
    // 电机A控制（左电机）- 使用TIM1 CH1
    if(motor_a_pwm < 0) // 反转
    {
        HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, -motor_a_pwm);
    }
    else // 正转
    {
        HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, motor_a_pwm);
    }

    // 电机B控制（右电机）- 使用TIM1 CH4
    if(motor_b_pwm < 0) // 反转
    {
        HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, -motor_b_pwm);
    }
    else // 正转
    {
        HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, motor_b_pwm);
    }
}
