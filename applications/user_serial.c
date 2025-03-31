/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-22     27198       the first version
 */
#define LOG_TAG "serial"
#define DBG_LVL LOG_LVL_DBG
#include <ulog.h>
#include <user_gy906.h>
#include <user_serial.h>
#include <user_sg90.h>
static rt_device_t serial2 = RT_NULL;
static rt_device_t serial3 = RT_NULL;
rt_mailbox_t serial2_tx_mb = RT_NULL;
rt_mq_t serial3_tx_mq = RT_NULL;
static void serial2_process_frame(const rt_uint8_t* data, rt_size_t len)
{
    if (len < 1) {
        return;
    }
    rt_uint8_t ASR_cmd[S3_RX_MSG_SIZE] = { 0 };
    float temp = 0;
    switch (data[0]) {
    case 0x05:
        if (len >= 2 && data[1] == 0x00)
            strncpy((char*)ASR_cmd, "Face added", sizeof("Face added"));
        break;
    case 0x02:
        if (len >= 2) {
            rt_mutex_take(temp_mutex, RT_WAITING_FOREVER);
            temp = SMBus_ReadTemp();
            rt_mutex_release(temp_mutex);
            if (temp < 30) {
                snprintf((char*)ASR_cmd, sizeof("Face:Mr.%d"), "Face:Mr.%d", (int)data[1] + 1);
                rt_sem_release(sg90_sem);
            }else{
                strncpy((char*)ASR_cmd, "Temp high", sizeof("Temp high"));
            }
        }
        break;
    case 0x01:
        if (len >= 2 && data[1] == 0x00)
            strncpy((char*)ASR_cmd, "Face deleted", sizeof("Face deleted"));
        break;
    }
    if (ASR_cmd[0] != '\0') {
        if (rt_mq_send(serial3_tx_mq, ASR_cmd, sizeof(ASR_cmd)) != RT_EOK) {
            LOG_E("Send to serial3 queue failed");
        }
    }
}
static rt_err_t serial2_input(rt_device_t dev, rt_size_t size)
{
    rt_uint8_t ch = 0;
    while (rt_device_read(dev, -1, &ch, 1) == 1) {
        rt_ringbuffer_put_force(&serial2_rx.rb, &ch, 1);
    }
    rt_sem_release(serial2_rx.rx_sem);
    return RT_EOK;
}
static void serial2_rx_thread_entry(void* parameter)
{
    rt_uint8_t data = 0;
    while (1) {
        rt_sem_take(serial2_rx.rx_sem, RT_WAITING_FOREVER);
        /* 处理环形缓冲区数据 */
        while (rt_ringbuffer_getchar(&serial2_rx.rb, &data) == 1) {
            switch (serial2_rx.state) {
            case WAIT_HEAD:
                if (data == 0xAF) {
                    serial2_rx.state = IN_FRAME;
                    serial2_rx.cur_frame_len = 0;
                }
                break;
            case IN_FRAME:
                if (data == 0xFA) {
                    /* 存入链表 */
                    if (serial2_rx.cur_frame_len > 0) {
                        frame_node_t* node = rt_malloc(sizeof(frame_node_t));
                        if (node) {
                            node->len = serial2_rx.cur_frame_len;
                            rt_memcpy(node->data, serial2_rx.cur_frame, node->len);
                            rt_mutex_take(serial2_rx.list_mutex, RT_WAITING_FOREVER);
                            rt_slist_append(&serial2_rx.frame_list, &node->list);
                            rt_mutex_release(serial2_rx.list_mutex);
                        }
                    }
                    serial2_rx.state = WAIT_HEAD;
                    serial2_rx.cur_frame_len = 0;
                } else {
                    if (serial2_rx.cur_frame_len < MAX_FRAME_LEN) {
                        serial2_rx.cur_frame[serial2_rx.cur_frame_len++] = data;
                    } else {
                        /* 超长丢弃 */
                        serial2_rx.state = WAIT_HEAD;
                        serial2_rx.cur_frame_len = 0;
                    }
                }
                break;
            }
        }
        /* 解析链表数据 */
        rt_mutex_take(serial2_rx.list_mutex, RT_WAITING_FOREVER);
        rt_slist_t* node = RT_NULL;
        while ((node = rt_slist_first(&serial2_rx.frame_list)) != RT_NULL) {
            /* 从链表移除并处理 */
            rt_slist_remove(&serial2_rx.frame_list, node);
            frame_node_t* frame_node = rt_container_of(node, frame_node_t, list);
            serial2_process_frame(frame_node->data, frame_node->len);
            rt_free(frame_node);
        }
        rt_mutex_release(serial2_rx.list_mutex);
    }
}
static void serial2_tx_thread_entry(void* parameter)
{
    rt_uint32_t addr = 0;
    while (1) {
        /* 从消息队列接收数据 */
        if (rt_mb_recv(serial2_tx_mb, &addr, RT_WAITING_FOREVER) == RT_EOK) {
            const rt_uint8_t* data = (const rt_uint8_t*)addr;
            rt_device_write(serial2, 0, data, 4);
        }
    }
}
int serial2_init(void)
{
    /* 初始化硬件 */
    serial2 = rt_device_find(SAMPLE_UART2_NAME);
    if (!serial2) {
        LOG_E("Find %s failed!", SAMPLE_UART2_NAME);
        return RT_ERROR;
    }
    /* 初始化数据结构 */
    rt_ringbuffer_init(&serial2_rx.rb, serial2_rx.rb_storage, SERIAL2_RB_SIZE);
    rt_slist_init(&serial2_rx.frame_list);
    serial2_rx.state = WAIT_HEAD;
    serial2_rx.cur_frame_len = 0;
    serial2_rx.rx_sem = rt_sem_create("rx_sem", 0, RT_IPC_FLAG_FIFO);
    if (!serial2_rx.rx_sem) {
        LOG_E("Create rx_sem failed!");
        return RT_ERROR;
    }
    serial2_rx.list_mutex = rt_mutex_create("list_mutex", RT_IPC_FLAG_FIFO);
    if (!serial2_rx.list_mutex) {
        LOG_E("Create list_mutex failed!");
        return RT_ERROR;
    }
    serial2_tx_mb = rt_mb_create("s2_tx_mb", 4, RT_IPC_FLAG_FIFO);
    if (!serial2_tx_mb) {
        LOG_E("Create serial2 tx mb failed!");
        return RT_ERROR;
    }
    /* 配置串口设备 */
    rt_device_open(serial2, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial2, serial2_input);
    /* 创建处理线程 */
    rt_thread_t thread = RT_NULL;
    thread = rt_thread_create("serial2_rx", serial2_rx_thread_entry, RT_NULL, 1024, 20, 10);
    if (!thread) {
        LOG_E("Create serial2 thread failed!");
        return RT_ERROR;
    } else {
        rt_thread_startup(thread);
    }
    /* 创建串口发送线程 */
    thread = rt_thread_create("serial2_tx", serial2_tx_thread_entry, RT_NULL, 512, 21, 10);
    if (!thread) {
        LOG_E("Create serial2 tx thread failed!");
        return RT_ERROR;
    } else {
        rt_thread_startup(thread);
    }
    LOG_D("Serial2 init successful\n");
    return RT_EOK;
}
static void serial3_thread_entry(void* parameter)
{
    rt_uint8_t data[S3_RX_MSG_SIZE] = { 0 };
    rt_size_t recv_len;
    while (1) {
        /* 从消息队列接收数据（支持不定长） */
        recv_len = rt_mq_recv(serial3_tx_mq, data, sizeof(data), RT_WAITING_FOREVER);
        if (recv_len > 0) {
            /* 计算实际有效数据长度 */
            rt_size_t actual_len = strlen((char*)data) + 1;
            /* 发送到串口3 */
            rt_device_write(serial3, 0, data, actual_len);
            LOG_D("Send to serial3: %s", data);
        } else {
            LOG_E("Receive from queue failed");
        }
    }
}
int serial3_init(void)
{
    serial3 = rt_device_find(SAMPLE_UART3_NAME);
    if (!serial3) {
        LOG_E("find %s failed!\n", SAMPLE_UART3_NAME);
        return RT_ERROR;
    }
    rt_device_open(serial3, RT_DEVICE_FLAG_INT_RX);
    /* 创建消息队列（每个消息32字节，队列容量16条） */
    serial3_tx_mq = rt_mq_create("s3_tx_mq", S3_RX_MSG_SIZE, S3_RX_MAX_MSGS, RT_IPC_FLAG_FIFO);
    if (!serial3_tx_mq) {
        LOG_E("create serial3 mq failed\n");
        return RT_ERROR;
    } else {
        LOG_D("create serial3 mq successful\n");
    }
    /* 创建串口3发送线程 */
    rt_thread_t serial3_tx_thread = rt_thread_create("serial3_tx", serial3_thread_entry, RT_NULL, 1024, 21, 10);
    if (!serial3_tx_thread) {
        LOG_E("create serial3 tx thread failed\n");
        return RT_ERROR;
    } else {
        rt_thread_startup(serial3_tx_thread);
    }
    LOG_D("serial3 init successful\n");
    return RT_EOK;
}
