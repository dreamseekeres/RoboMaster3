//
// Created by 43034 on 2025/10/25.
//

#ifndef __REMOTE_CONTROL_H__
#define __REMOTE_CONTROL_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"
#include "stdint.h"
#include "stdbool.h"
//================= 数据结构 =================//

typedef enum
{
    SWITCH_UP = 0, // 上
    SWITCH_MID = 1, // 中
    SWITCH_DOWN = 2 // 下
} SwitchState;


typedef struct
{
    uint16_t ch0;
    uint16_t ch1;
    uint16_t ch2;
    uint16_t ch3;
    uint8_t s1;
    uint8_t s2;
} RC_Data_t;

typedef struct
{
    float ch0;
    float ch1;
    float ch2;
    float ch3;
    SwitchState s1;
    SwitchState s2;
} last_RC_Data_t;

//================= 遥控器类 =================//

class RemoteControl
{
public:
    void init(); // 初始化 UART + DMA 接收
    void handle(); // 解包（只解析通道和开关）
    void uartCallback(uint16_t size); // 在 HAL UART 回调中调用
    bool isConnected(); // 返回连接状态

private:
    void updateConnection();
    float mapChannelToRange(uint16_t raw_value);
    SwitchState getSState(uint8_t state);

public:
    uint8_t rx_buf[18];
    uint8_t rx_data[18];
    uint32_t last_tick;
    bool connected;
    RC_Data_t rc_data;
    last_RC_Data_t last_rc_data;
};
#ifdef __cplusplus
}
#endif


#endif
