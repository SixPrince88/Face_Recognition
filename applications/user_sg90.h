/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-24     Thread       the first version
 */
#ifndef APPLICATIONS_PWM_CFG_H_
#define APPLICATIONS_PWM_CFG_H_

#include <rtdbg.h>
#include <rtdevice.h>
#include <rtthread.h>

#define PWM_DEV_NAME "pwm3" /* PWM设备名称 */
#define PWM_DEV_CHANNEL 4 /* PWM通道 */
#define PWM_PERIOD 20000000 // 20ms周期
#define PWM_PULSE_OPEN 2500000 // 2.5ms脉宽对应180度
#define PWM_PULSE_CLOSE 500000 // 0.5ms脉宽对应0度
#define EVENT_TEMP_NORMAL (1 << 0) // 温度<30℃
#define EVENT_FACE_DETECT (1 << 1) // 检测到人脸
struct rt_device_pwm* pwm_dev; /* PWM设备句柄 */
extern rt_sem_t sg90_sem;
int SG90_Init(void);

#endif /* APPLICATIONS_PWM_CFG_H_ */
