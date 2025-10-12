//
// Created by 43034 on 2025/10/12.
//

#include "bmi088.h"
#include "main.h"
#include "spi.h"


void bmi088_write_byte(uint8_t tx_data)
{
    HAL_SPI_Transmit(&hspi1, &tx_data, 1, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
}

void bmi088_read_byte(uint8_t* rx_data, uint8_t length)
{
    HAL_SPI_Receive(&hspi1, rx_data, length, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_RX);
}

void bmi088_write_reg(uint8_t reg, uint8_t data)
{
    uint8_t tx[2];
    tx[0] = reg & 0x7F; // 假设位7为0表示写操作
    tx[1] = data;
    HAL_SPI_Transmit(&hspi1, tx, 2, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
}


void BMI088_ACCEL_NS_L(void)
{
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_RESET);
}

// 控制加速度计片选信号的高电平（禁用）
void BMI088_ACCEL_NS_H(void)
{
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_SET);
}

// 控制陀螺仪片选信号的低电平（使能）
void BMI088_GYRO_NS_L(void)
{
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_RESET);
}

// 控制陀螺仪片选信号的高电平（禁用）
void BMI088_GYRO_NS_H(void)
{
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_SET);
}

void bmi088_accel_write_single_reg(uint8_t reg, uint8_t data)
{
    BMI088_GYRO_NS_H();
    BMI088_ACCEL_NS_L();

    bmi088_write_byte(reg & 0x7F);
    bmi088_write_byte(data);

    BMI088_ACCEL_NS_H();
}

void bmi088_accel_read_reg(uint8_t reg, uint8_t* rx_data, uint8_t length)
{
    uint8_t dummy_byte;

    BMI088_GYRO_NS_H();
    BMI088_ACCEL_NS_L();

    bmi088_write_byte(reg | 0x80);
    bmi088_read_byte(&dummy_byte, 1);
    bmi088_read_byte(rx_data, length);
    BMI088_ACCEL_NS_H();
}

void bmi088_gyro_read_reg(uint8_t reg, uint8_t* rx_data, uint8_t length)
{
    BMI088_ACCEL_NS_H();
    BMI088_GYRO_NS_L();
    bmi088_write_byte(reg | 0x80);
    bmi088_read_byte(rx_data, length);
    BMI088_GYRO_NS_H();
}

void bmi088_gyro_write_single_reg(uint8_t reg, uint8_t tx_data)
{
    // 拉低陀螺仪的片选以启动通信
    BMI088_ACCEL_NS_H();

    BMI088_GYRO_NS_L();
    bmi088_write_byte(reg & 0x7F);
    bmi088_write_byte(tx_data);
    // 拉高片选以结束通信
    BMI088_GYRO_NS_H();
}


void bmi088_init(void)
{
    // Soft Reset ACCEL
    BMI088_ACCEL_NS_L();
    bmi088_write_reg(0x7E, 0xB6); // Write 0xB6 to ACC_SOFTRESET(0x7E)
    HAL_Delay(1);
    BMI088_ACCEL_NS_H();

    // Soft Reset GYRO
    BMI088_GYRO_NS_L();
    bmi088_write_reg(0x14, 0xB6); // Write 0xB6 to GYRO_SOFTRESET(0x14)
    HAL_Delay(30);
    BMI088_GYRO_NS_H();

    // Switch ACCEL to Normal Mode
    BMI088_ACCEL_NS_L();
    HAL_Delay(1);
    bmi088_write_reg(0x7D, 0x04); // Write 0x04 to ACC_PWR_CTRL(0x7D)
    HAL_Delay(1);
    BMI088_ACCEL_NS_H();
}

