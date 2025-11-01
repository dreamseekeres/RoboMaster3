//
// Created by 43034 on 2025/11/1.
//

#include "cmsis_os2.h"
#include "User_tasks.h"
//
// uint32_t send = 0;
// uint32_t recv = 0;
// osMessageQueueId_t message_queue_handle;
// osMessageQueueAttr_t message_queue_recv;
//
//
// osThreadId defaultTaskHandle;
// constexpr osThreadAttr_t taskTask_attributes = {
//     .name = "taskTask",
//     .stack_size = 256 * 4,
//     .priority = osPriorityNormal,
// }
//
// void test_task(void *pvParameters){
//     while(1){
//         const auto tick = osKernelGetTickCount();
//         ++counter;
//         osDelayUntil(tick+1);
//     }
// };
//
//
// constexpr osThreadAttr_t taskTask_attributes = {
//     .name = "taskTask",
//     .stack_size = 256 * 4,
//     .priority = osPriorityNormal,
// }
//
// void test_task(void *pvParameters){
//     while(1){
//         const auto tick = osKernelGetTickCount();
//         ++counter;
//         osDelayUntil(tick+1);
//     }
// };
//
//
//
//
// void user_tasks_init(){
//     test_task_handle = osThreadNew(test_task, nullptr, &test_task_attributes);
//     test_task_handle = osThreadNew(test_task, nullptr, &test_task_attributes);
//
// }

// Created by 43034 on 2025/11/1.

#include "cmsis_os2.h"
#include "User_tasks.h"

// 全局变量
osThreadId_t sendTaskHandle;
osThreadId_t recvTaskHandle;
osMessageQueueId_t message_queue_handle;

// 计数器（如果需要）
volatile uint32_t counter = 0;

// 任务属性
constexpr osThreadAttr_t sendTask_attributes = {
    .name = "SendTask",
    .stack_size = 256 * 4,
    .priority = osPriorityNormal,
};

constexpr osThreadAttr_t recvTask_attributes = {
    .name = "RecvTask",
    .stack_size = 256 * 4,
    .priority = osPriorityNormal,
};

// 发送任务：每 100ms 发送一个递增的数字
void send_task(void *pvParameters) {
    uint32_t send_value = 0;
    while (1) {
        ++send_value;
        if (osMessageQueuePut(message_queue_handle, &send_value, 0U, 0U) != osOK) {
            // 队列满，可处理错误（如日志、LED 报警等）
        }
        osDelay(100); // 每 100ms 发一次
    }
}

// 接收任务：从队列接收并更新 counter
void recv_task(void *pvParameters) {
    uint32_t recv_value = 0;
    while (1) {
        if (osMessageQueueGet(message_queue_handle, &recv_value, nullptr, osWaitForever) == osOK) {
            counter = recv_value; // 或做其他处理
        }
    }
}

// 用户任务初始化函数
void user_tasks_init() {
    // 创建消息队列：最多存 10 个 uint32_t 类型的消息
    message_queue_handle = osMessageQueueNew(10, sizeof(uint32_t), nullptr);
    if (message_queue_handle == nullptr) {
        // 队列创建失败，应处理错误（如死循环、断言等）
        while (1);
    }

    // 创建发送和接收任务
    sendTaskHandle = osThreadNew(send_task, nullptr, &sendTask_attributes);
    recvTaskHandle = osThreadNew(recv_task, nullptr, &recvTask_attributes);

    // 注意：不要重复创建同名任务或覆盖句柄
}