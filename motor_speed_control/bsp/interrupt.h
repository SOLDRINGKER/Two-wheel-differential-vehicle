#ifndef INTERRUPT_H
#define INTERRUPT_H
#include "main.h"
#include "tim.h"
#include "sys.h"
#include "delay.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "motor.h"
#include "speed_planning.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

extern float speed_a;
extern float speed_b;
extern float target_a;
extern float target_b;
#endif //INTERRUPT_H
