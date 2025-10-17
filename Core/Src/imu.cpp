//
// Created by 43034 on 2025/10/12.
//
#include "imu.h"
#include "main.h"
#include "bmi088.h"

#define g 9.81f

extern uint8_t rx_data_acc[6];
extern uint8_t rx_data_gyro[6];

void acc_calculate(IMU* p)
{
    uint8_t rx_acc_range_raw;
    bmi088_accel_read_reg(0x41, &rx_acc_range_raw, 1);
    float rx_acc_range = (float)(rx_acc_range_raw + 1) * 3.f * g;

    bmi088_accel_read_reg(0x12, rx_data_acc, 6);

    p->acc_[0] = (float)(int16_t)(rx_data_acc[0] | rx_data_acc[1] << 8) / 32768.0f * rx_acc_range;
    p->acc_[1] = (float)(int16_t)(rx_data_acc[2] | rx_data_acc[3] << 8) / 32768.0f * rx_acc_range;
    p->acc_[2] = (float)(int16_t)(rx_data_acc[4] | rx_data_acc[5] << 8) / 32768.0f * rx_acc_range;
}

void gyro_calculate(IMU* p)
{
    uint8_t rx_gyro_range_raw;
    bmi088_gyro_read_reg(0x0F, &rx_gyro_range_raw, 1);
    float rx_gyro_range = 2000.f / (float)(rx_gyro_range_raw + 1);

    bmi088_gyro_read_reg(0x02, rx_data_gyro, 6);

    p->gyro_[0] = static_cast<float>(rx_data_gyro[0] | (uint16_t)rx_data_gyro[1] << 8) / 32768.0f * rx_gyro_range;
    p->gyro_[1] = static_cast<float>((int16_t)(rx_data_gyro[2] | (uint16_t)rx_data_gyro[3] << 8)) / 32768.0f *
        rx_gyro_range;
    p->gyro_[2] = static_cast<float>((int16_t)(rx_data_gyro[4] | (uint16_t)rx_data_gyro[5] << 8)) / 32768.0f *
        rx_gyro_range;
}
