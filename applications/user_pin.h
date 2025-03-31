/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-22     27198       the first version
 */
#ifndef APPLICATIONS_USER_PIN_H_
#define APPLICATIONS_USER_PIN_H_

#include <rtdevice.h>
#include <rtthread.h>
#include <rtdbg.h>
/* 引脚编号，通过查看设备驱动文件drv_gpio.c确定 */
#ifndef BEEP_PIN_NUM
#define BEEP_PIN_NUM 24 /* PB8 */
#endif
#ifndef KEY0_PIN_NUM
#define KEY0_PIN_NUM 66 /* PE2 */
#endif
#ifndef KEY1_PIN_NUM
#define KEY1_PIN_NUM 67 /* PE3 */
#endif

void gpio_pin_init(void);

#endif /* APPLICATIONS_USER_PIN_H_ */
