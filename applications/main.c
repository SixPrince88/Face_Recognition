/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-22     RT-Thread    first version
 */

#include <rtthread.h>
#define LOG_TAG "main"
#define DBG_LVL LOG_LVL_DBG
#include <ulog.h>
#include <user_gy906.h>
#include <user_oled.h>
#include <user_pin.h>
#include <user_serial.h>
#include <user_sg90.h>
int main(void)
{
    OLED_Init(); // 初始化OLED
    gpio_pin_init(); // 初始化GPIO
    serial3_init(); // 先初始化串口3，因为串口2会调用串口3发送指令到语音模块
    serial2_init(); // 初始化串口2
    GY906_Init(); // 初始化红外传感器
    SG90_Init(); // 初始化舵机
    while (1) {
        float temp = 0;
        rt_mutex_take(temp_mutex, RT_WAITING_FOREVER);
        temp = SMBus_ReadTemp();
        rt_mutex_release(temp_mutex);
        OLED_Display(temp); // 显示温度
        rt_thread_mdelay(100);
    }
    return RT_EOK;
}
