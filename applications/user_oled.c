/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-21     Thread       the first version
 */
#define LOG_TAG "oled"
#define DBG_LVL LOG_LVL_DBG
#include <ulog.h>
#include "user_oled.h"
#include "stdio.h"
#include "string.h"
static u8g2_t u8g2; // 初始化u8g2
// 初始化显示配置
int OLED_Init(void)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_rtthread);
    u8x8_SetPin(u8g2_GetU8x8(&u8g2), U8X8_PIN_I2C_CLOCK, OLED_I2C_PIN_SCL);
    u8x8_SetPin(u8g2_GetU8x8(&u8g2), U8X8_PIN_I2C_DATA, OLED_I2C_PIN_SDA);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_spleen8x16_me);
    u8g2_DrawStr(&u8g2, 0, 16, "Face_Recognition");
    u8g2_DrawStr(&u8g2, 0, 32, "Temp:       ");
    LOG_D("OLED init successful\n");
    return RT_EOK;
}
// 显示温度值
void OLED_Display(float temp)
{
    rt_uint8_t str_buf[32] = { 0 };
    sprintf((char*)str_buf, "%0.2f", temp);
    u8g2_DrawStr(&u8g2, 48, 32, (char*)str_buf);
    u8g2_SendBuffer(&u8g2);
}
