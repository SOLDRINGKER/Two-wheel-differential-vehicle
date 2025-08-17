#include "speed_planning.h"

// 全局S曲线规划器实例
S_Curve_Planner Left_Wheel_Planner = {0};
S_Curve_Planner Right_Wheel_Planner = {0};

/**
 * 函数功能：初始化S型速度曲线规划器
 * 入口参数：规划器指针，起始速度，目标速度，最大加速度
 * 返回  值：无
 */
void S_Curve_Init(S_Curve_Planner *planner, float start_speed, float target_speed, float max_acc)
{
    planner->start_speed = start_speed;
    planner->target_speed = target_speed;
    planner->current_speed = start_speed;
    planner->actual_speed = start_speed;
    planner->max_acc = max_acc;
    planner->state = IDLE;
    planner->target_reached = 0;
    planner->speed_threshold = 0.005f; // 5mm/s的阈值判断是否达到目标速度
}

/**
 * 函数功能：设置新的目标速度
 * 入口参数：规划器指针，新的目标速度
 * 返回  值：无
 */
void S_Curve_Set_Target(S_Curve_Planner *planner, float target_speed)
{
    float max_speed = 0.1f; // 最大速度限制，单位m/s

    // 目标速度限幅
    if(target_speed > max_speed) target_speed = max_speed;
    if(target_speed < 0) target_speed = 0;

    // 只有当实际速度已经达到目标速度(或接近)时，才接受新的目标速度
    if(planner->target_reached || planner->state == IDLE)
    {
        planner->start_speed = planner->actual_speed;
        planner->target_speed = target_speed;
        planner->target_reached = 0;

        // 根据当前实际速度和目标速度设置初始状态
        if(fabsf(planner->actual_speed - target_speed) < planner->speed_threshold)
        {
            // 当前速度已经接近目标速度
            planner->state = STEADY;
            planner->current_speed = target_speed;
        }
        else if(planner->actual_speed < target_speed)
        {
            // 需要加速
            planner->state = ACCELERATING;
        }
        else
        {
            // 需要减速
            planner->state = DECELERATING;
        }
    }
}

/**
 * 函数功能：更新S型速度曲线规划，计算当前速度
 * 入口参数：规划器指针，实际测量速度
 * 返回  值：当前规划速度(m/s)
 */
float S_Curve_Update(S_Curve_Planner *planner, float actual_speed)
{
    // 更新实际测量速度
    planner->actual_speed = actual_speed;

    // 检查是否达到目标速度
    if(fabsf(actual_speed - planner->target_speed) <= planner->speed_threshold)
    {
        planner->target_reached = 1;
        planner->state = STEADY;
        planner->current_speed = planner->target_speed;
        return planner->current_speed;
    }

    // 根据当前状态更新速度
    switch(planner->state)
    {
        case IDLE:
            // 空闲状态，不改变速度
            break;

        case ACCELERATING:
            // 加速过程
            if(actual_speed < planner->target_speed)
            {
                // 计算加速度，这里使用S形加速曲线
                float remaining = planner->target_speed - actual_speed;

                // S形加速曲线：当接近目标速度时减小加速度
                float acc_factor = 1.0f - expf(-remaining / (planner->max_acc * 0.2f));
                float delta_speed = planner->max_acc * 0.1f * acc_factor; // 假设控制周期为0.1s

                // 更新当前规划速度
                planner->current_speed = actual_speed + delta_speed;

                // 确保不超过目标速度
                if(planner->current_speed > planner->target_speed)
                    planner->current_speed = planner->target_speed;
            }
            else
            {
                // 已达到或超过目标速度
                planner->state = STEADY;
                planner->current_speed = planner->target_speed;
                planner->target_reached = 1;
            }
            break;

        case STEADY:
            // 匀速阶段
            planner->current_speed = planner->target_speed;
            break;

        case DECELERATING:
            // 减速过程
            if(actual_speed > planner->target_speed)
            {
                // 计算减速度，这里使用S形减速曲线
                float remaining = actual_speed - planner->target_speed;

                // S形减速曲线：当接近目标速度时减小减速度
                float dec_factor = 1.0f - expf(-remaining / (planner->max_acc * 0.2f));
                float delta_speed = planner->max_acc * 0.01f * dec_factor; // 假设控制周期为0.01s

                // 更新当前规划速度
                planner->current_speed = actual_speed - delta_speed;

                // 确保不低于目标速度
                if(planner->current_speed < planner->target_speed)
                    planner->current_speed = planner->target_speed;
            }
            else
            {
                // 已达到或低于目标速度
                planner->state = STEADY;
                planner->current_speed = planner->target_speed;
                planner->target_reached = 1;
            }
            break;
    }

    return planner->current_speed;
}

/**
 * 函数功能：设置左右轮目标速度（带S形速度规划）
 * 入口参数：左轮目标速度，右轮目标速度(m/s)
 * 返回  值：无
 */
void Set_Target_Speed_Smooth(float speed_left, float speed_right)
{
    S_Curve_Set_Target(&Left_Wheel_Planner, speed_left);
    S_Curve_Set_Target(&Right_Wheel_Planner, speed_right);
}
