/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-24     Thread       the first version
 */
#define LOG_TAG "sg90"
#define DBG_LVL LOG_LVL_DBG
#include <ulog.h>
#include <user_serial.h>
#include <user_sg90.h>
struct rt_device_pwm* pwm_dev; /* PWM设备句柄 */
static rt_timer_t door_timer = RT_NULL;
static rt_bool_t door_is_open = RT_FALSE;
rt_sem_t sg90_sem = RT_NULL;
static void open_door(void)
{
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, PWM_PERIOD, PWM_PULSE_OPEN);
    if (rt_mq_send(serial3_tx_mq, "Door opened", sizeof("Door opened")) != RT_EOK) {
        LOG_E("Send to serial3 queue failed");
    }
}
static void close_door(void)
{
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, PWM_PERIOD, PWM_PULSE_CLOSE);
    if (rt_mq_send(serial3_tx_mq, "Door closed", sizeof("Door closed")) != RT_EOK) {
        LOG_E("Send to serial3 queue failed");
    }
}
static void door_timeout(void* parameter)
{
    if (door_is_open) {
        close_door();
        door_is_open = RT_FALSE;
        rt_kprintf("Door closed\n");
    }
}
static void sg90_control_entry(void* parameter)
{
    while (1) {
        rt_sem_take(sg90_sem, RT_WAITING_FOREVER);
        if (!door_is_open) {
            open_door();
            door_is_open = RT_TRUE;
            rt_kprintf("Door opened\n");
        }
        // 重置定时器（先停止后启动实现重置）
        rt_timer_stop(door_timer);
        rt_timer_start(door_timer);
    }
}
int SG90_Init(void)
{
    rt_err_t ret = RT_EOK;
    /* 查找设备 */
    pwm_dev = (struct rt_device_pwm*)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL) {
        LOG_E("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return RT_ERROR;
    }
    /* 设置PWM周期和脉冲宽度默认值 */
    ret = rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, PWM_PERIOD, PWM_PULSE_CLOSE);
    if (ret != RT_EOK) {
        LOG_E("SG90 set period or pulse failed!");
        return RT_ERROR;
    } else {
        LOG_D("SG90 set period or pulse successful\n");
    }
    /* 使能设备 */
    ret = rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
    if (ret != RT_EOK) {
        LOG_E("SG90 enable failed!");
        return RT_ERROR;
    }
    /* 创建事件集 */
    sg90_sem = rt_sem_create("sg90_sem", 0, RT_IPC_FLAG_FIFO);
    if (sg90_sem == RT_NULL) {
        LOG_E("Create sg90_sem failed!");
        return RT_ERROR;
    }
    /*创建门控制定时器(单次触发)*/
    door_timer = rt_timer_create("door_tmr", door_timeout, RT_NULL, 7000, RT_TIMER_FLAG_ONE_SHOT);
    if (door_timer == RT_NULL) {
        LOG_E("Create door timer failed!");
        return RT_ERROR;
    }
    /* 创建舵机控制线程 */
    rt_thread_t sg90_thread = rt_thread_create("sg90_ctrl", sg90_control_entry, RT_NULL, 512, 24, 10);
    if (!sg90_thread) {
        LOG_E("Create SG90 thread failed!");
        return RT_ERROR;
    } else {
        rt_thread_startup(sg90_thread);
    }
    LOG_D("SG90 init successful\n");
    return RT_EOK;
}
