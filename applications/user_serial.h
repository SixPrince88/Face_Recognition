/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-22     27198       the first version
 */
#ifndef APPLICATIONS_USER_SERIAL_H_
#define APPLICATIONS_USER_SERIAL_H_
#include <rtdevice.h>
#include <rtthread.h>
#include <stdio.h>
#define SAMPLE_UART3_NAME "uart3"
#define SAMPLE_UART2_NAME "uart2"
#define MAX_FRAME_LEN 8
#define SERIAL2_RB_SIZE 32
#define S2_RX_MAX_MSGS 4
#define S2_RX_MSG_SIZE 4
#define S3_RX_MAX_MSGS 8
#define S3_RX_MSG_SIZE 16

typedef struct frame_node {
    rt_slist_t list;
    rt_uint8_t data[MAX_FRAME_LEN];
    rt_size_t len;
} frame_node_t;

struct {
    rt_sem_t rx_sem;
    rt_mutex_t list_mutex;
    struct rt_ringbuffer rb;
    rt_uint8_t rb_storage[SERIAL2_RB_SIZE];
    rt_slist_t frame_list;
    enum { WAIT_HEAD,
        IN_FRAME } state;
    rt_uint8_t cur_frame[MAX_FRAME_LEN];
    rt_size_t cur_frame_len;
} serial2_rx;
extern rt_mailbox_t serial2_tx_mb;
extern rt_mq_t serial3_tx_mq;
int serial3_init(void);
int serial2_init(void);

#endif /* APPLICATIONS_USER_SERIAL_H_ */
