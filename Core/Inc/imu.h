//
// Created by 43034 on 2025/10/12.
//

#ifndef TRY_IMU_H
#define TRY_IMU_H

#ifdef __cplusplus
extern "C" {
#endif
struct IMU
{
    // 存储计算后的加速度和陀螺仪数据（三维）
    float acc_[3]; // 加速度数据 (x, y, z)
    float gyro_[3]; // 陀螺仪数据 (x, y, z)
};


void gyro_calculate(struct IMU* p);
void acc_calculate(struct IMU* p);

#ifdef __cplusplus
}
#endif
#endif //TRY_IMU_H


