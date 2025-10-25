//
// Created by 43034 on 2025/10/25.
//

#include "remote_control.h"
#include "string.h"
#include "usart.h"




extern UART_HandleTypeDef huart3;  // 假设使用 UART3

//------------------------------------------------------------

void RemoteControl::init()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_buf, sizeof(rx_buf));
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);  // 禁止半传输中断
    last_tick = HAL_GetTick();
    connected = false;
}

//------------------------------------------------------------

void RemoteControl::updateConnection()
{
    uint32_t now = HAL_GetTick();
    // 断连时间阈值：100ms
    connected = (now - last_tick) < 100;
}

//------------------------------------------------------------

bool RemoteControl::isConnected()
{
    updateConnection();
    return connected;
}

//------------------------------------------------------------
// 该函数应在 HAL_UARTEx_RxEventCallback() 中被调用
void RemoteControl::uartCallback(uint16_t size)
{
    // 控制帧固定长度为 18 字节
    if (size == 18)
    {
        memcpy(rx_data, rx_buf, size);
        last_tick = HAL_GetTick();
        handle();
    }

    // 重新开启 DMA 接收
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_buf, sizeof(rx_buf));
}






/**
 * @brief 将遥控器通道原始值映射到-1到1范围
 * @param raw_value 原始通道值 (364-1684)
 * @return 归一化后的值 (-1.0到1.0)
 */
float RemoteControl::mapChannelToRange(uint16_t raw_value)
{
    // 通道数据范围参数
    const uint16_t CH_MIN = 364;
    const uint16_t CH_MID = 1024;
    const uint16_t CH_MAX = 1684;

    // 计算正向和负向的范围
    const float POSITIVE_RANGE = (float)(CH_MAX - CH_MID);  // 660
    const float NEGATIVE_RANGE = (float)(CH_MID - CH_MIN);  // 660

    // 映射到-1到1范围
    if (raw_value > CH_MID) {
        // 正向范围：1024-1684 -> 0.0-1.0
        return (float)(raw_value - CH_MID) / POSITIVE_RANGE;
    } else if (raw_value < CH_MID) {
        // 负向范围：364-1024 -> -1.0-0.0
        return (float)(raw_value - CH_MID) / NEGATIVE_RANGE;
    } else {
        // 中间值
        return 0.0f;
    }
}



SwitchState RemoteControl::getSState(uint8_t state)
{
    SwitchState s_state;
    // S2开关的映射关系与S1相同
    switch (state) {
    case 1:
        s_state = SWITCH_UP;
        break;
    case 3:
        s_state = SWITCH_MID;
        break;
    case 2:
        s_state = SWITCH_DOWN;
        break;
    default:
        // 如果收到未知值，保持之前的状态
        break;
    }
    return s_state;
}








//------------------------------------------------------------
// 解包函数：解析通道与开关
void RemoteControl::handle()
{
    // ===== 通道数据（每个通道 11bit） =====
    rc_data.ch0 = ((uint16_t)rx_data[0] | ((uint16_t)rx_data[1] << 8)) & 0x07FF;
    rc_data.ch1 = (((uint16_t)rx_data[1] >> 3) | ((uint16_t)rx_data[2] << 5)) & 0x07FF;
    rc_data.ch2 = (((uint16_t)rx_data[2] >> 6) | ((uint16_t)rx_data[3] << 2) | ((uint16_t)rx_data[4] << 10)) & 0x07FF;
    rc_data.ch3 = (((uint16_t)rx_data[4] >> 1) | ((uint16_t)rx_data[5] << 7)) & 0x07FF;
    last_rc_data.ch0 = mapChannelToRange(rc_data.ch0);
    last_rc_data.ch1 = mapChannelToRange(rc_data.ch1);
    last_rc_data.ch2 = mapChannelToRange(rc_data.ch2);
    last_rc_data.ch3 = mapChannelToRange(rc_data.ch3);
    // ===== 开关状态（s1、s2） =====
    rc_data.s1 = ((rx_data[5] >> 4) & 0x000C) >> 2; // 高 4 位的前 2 位
    rc_data.s2 = ((rx_data[5] >> 4) & 0x0003);      // 高 4 位的后 2 位
    last_rc_data.s1 = getSState(rc_data.s1);
    last_rc_data.s2 = getSState(rc_data.s2);
}



