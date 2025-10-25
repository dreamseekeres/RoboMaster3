//
// Created by 43034 on 2025/10/12.
//
#include "main.h"
#include "tim.h"
#include "callback.h"
#include "bmi088.h"
#include "imu.h"
#include "remote_control.h"
// 计数变量，用于记录定时器溢出的次数

extern uint8_t rx_data_acc[6];
extern uint8_t rx_data_gyro[6];
extern uint32_t timer_tick_count;
RemoteControl rc;
uint16_t cnt;
IMU imu;
IMU* p = &imu;
// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM6)
    {
        // 在每次定时器溢出时执行的代码
        // bmi088_accel_read_reg(0x12, rx_data_acc, 6);
        // bmi088_gyro_read_reg(0x02, rx_data_gyro, 6);
        static bool rc_inited = false;
        if (!rc_inited)
        {
            rc.init(); // ✅ 初始化
            rc_inited = true;
        }
        cnt++;
        timer_tick_count++;
        acc_calculate(p);
        gyro_calculate(p);
    }
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size)
{
    if (huart->Instance == USART3)
    {
        rc.uartCallback(Size);
    }
}

