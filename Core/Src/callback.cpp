//
// Created by 43034 on 2025/10/12.
//
#include "main.h"
#include "tim.h"
#include "callback.h"
#include "bmi088.h"
// 计数变量，用于记录定时器溢出的次数

extern uint8_t rx_data_accel[6];
extern uint8_t rx_data_gyro[6];
extern uint32_t timer_tick_count;
// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM6)
    {
        // 在每次定时器溢出时执行的代码
        bmi088_accel_read_reg(0x12, rx_data_accel, 6);
        bmi088_gyro_read_reg(0x02, rx_data_gyro, 6);
        timer_tick_count++;
    }
}
