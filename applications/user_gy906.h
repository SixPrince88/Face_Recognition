#ifndef __GY906_H
#define __GY906_H

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>

// #include "gpio.h"
#include "drv_common.h"
#include <rtdbg.h>
#include <rtdevice.h>
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

// #define SCK_Pin GPIO_PIN_6
// #define SCK_GPIO_Port GPIOB
// #define SDA_Pin GPIO_PIN_7
// #define SDA_GPIO_Port GPIOB

#define SCK_GPIO GET_PIN(B, 6)
#define SDA_GPIO GET_PIN(B, 7)

int GY906_Init(void);
void SMBus_StartBit(void);
void SMBus_StopBit(void);
void SMBus_SendBit(u8);
u8 SMBus_SendByte(u8);
u8 SMBus_ReceiveBit(void);
u8 SMBus_ReceiveByte(u8);
void SMBus_Delay(u32);
void SMBus_Init(void);
u16 SMBus_ReadMemory(u8, u8);
u8 PEC_Calculation(u8*);
float SMBus_ReadTemp(void);
void Coarse_delay_us(uint32_t us);
extern rt_mutex_t temp_mutex;
#endif
