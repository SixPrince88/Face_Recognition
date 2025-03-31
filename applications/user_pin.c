/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-22     27198       the first version
 */
#define LOG_TAG "gpio"
#define DBG_LVL LOG_LVL_DBG
#include <ulog.h>
#include <user_pin.h>
#include <user_serial.h>
static void key_add_face(void *args)
{
    static const rt_uint8_t add_cmd[4] = { 0xA1, 0x02, 0x00, 0x1A };
    if (rt_mb_send(serial2_tx_mb, (rt_uint32_t)add_cmd) != RT_EOK) {
        rt_kprintf("Send serial2 queue full!\n");
    } else {
        rt_kprintf("Send serial2 cmd:key add face\n");
    }
}
static void key_del_face(void *args)
{
    static const rt_uint8_t del_cmd[4] = { 0xA1, 0x01, 0x02, 0x1A };
    if (rt_mb_send(serial2_tx_mb, (rt_uint32_t)del_cmd) != RT_EOK) {
        rt_kprintf("Send serial2 queue full!\n");
    } else {
        rt_kprintf("Send serial2 cmd:key del face\n");
    }
}
void gpio_pin_init(void)
{
    /* 蜂鸣器引脚为输出模式 */
    rt_pin_mode(BEEP_PIN_NUM, PIN_MODE_OUTPUT);
    /* 默认低电平 */
    rt_pin_write(BEEP_PIN_NUM, PIN_LOW);
    /* 按键0引脚为输入模式 */
    rt_pin_mode(KEY0_PIN_NUM, PIN_MODE_INPUT_PULLUP);
    /* 绑定中断，下降沿模式，回调函数名为beep_on */
    rt_pin_attach_irq(KEY0_PIN_NUM, PIN_IRQ_MODE_FALLING, key_add_face, RT_NULL);
    /* 使能中断 */
    rt_pin_irq_enable(KEY0_PIN_NUM, PIN_IRQ_ENABLE);
    /* 按键1引脚为输入模式 */
    rt_pin_mode(KEY1_PIN_NUM, PIN_MODE_INPUT_PULLUP);
    /* 绑定中断，下降沿模式，回调函数名为beep_off */
    rt_pin_attach_irq(KEY1_PIN_NUM, PIN_IRQ_MODE_FALLING, key_del_face, RT_NULL);
    /* 使能中断 */
    rt_pin_irq_enable(KEY1_PIN_NUM, PIN_IRQ_ENABLE);
    LOG_D("gpio pin init successful\n");
}
