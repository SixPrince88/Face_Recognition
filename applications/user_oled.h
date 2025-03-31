/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-21     Thread       the first version
 */
#ifndef APPLICATIONS_DISPLAY_CFG_H_
#define APPLICATIONS_DISPLAY_CFG_H_

#include "drv_common.h"
#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
#include <u8g2_port.h>
#define OLED_I2C_PIN_SCL GET_PIN(B, 6)
#define OLED_I2C_PIN_SDA GET_PIN(B, 7)

int OLED_Init(void);
void OLED_Display(float temp);
#endif /* APPLICATIONS_DISPLAY_CFG_H_ */
