/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2019-07-23     WillianChan  DIY Demo2(Second week mission)
 * 2019-07-27     WillianChan  DIY Demo3(Third week mission)
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdio.h>
#include <string.h>
#include "drv_spi.h"
#include "nrf24l01.h"

#define LED0_PIN            GET_PIN(E, 7)
#define NRF24L01_CE_PIN     GET_PIN(D, 4)
#define NRF24_IRQ_PIN       GET_PIN(D, 3)
#define NRF24L01_SPI_DEVICE "spi20"

#define MP_BLOCK_SIZE       RT_ALIGN(sizeof(struct tmp_msg), sizeof(intptr_t)) /* 为了字节对齐 */
#define MB_LEN              (4)
#define MP_LEN              MB_LEN

/* 这是要传输的数据结构定义 */
struct tmp_msg
{
    char str_value[32];
};
static rt_mailbox_t tmp_msg_mb; /* 邮箱 */
static rt_mp_t tmp_msg_mp;      /* 内存池 */

static void nrf24l01_receive_entry(void *parameter)
{
    struct tmp_msg *msg;
    struct hal_nrf24l01_port_cfg halcfg;
    nrf24_cfg_t cfg;

    nrf24_default_param(&cfg);
    halcfg.ce_pin = NRF24L01_CE_PIN;
    halcfg.spi_device_name = NRF24L01_SPI_DEVICE;
    cfg.role = ROLE_PRX;
    cfg.ud = &halcfg;
    cfg.use_irq = 0;
    nrf24_init(&cfg);

    while (1)
    {
        rt_thread_mdelay(90);
        
        if (!rx_pipe_num_choose())
        {
            /* 申请一块内存 要是内存池满了 就挂起等待 */
            msg = rt_mp_alloc(tmp_msg_mp, RT_WAITING_FOREVER);
            rt_strncpy(msg->str_value, (char *)RxBuf_P0, sizeof msg->str_value);
            rt_mb_send(tmp_msg_mb, (rt_ubase_t)msg);
            msg = NULL;
        }
    }
}

static void save_recv_data_entry(void *parameter)
{
    FILE *recvdata_p0 = RT_NULL;
    struct tmp_msg *msg;
    
    while (1)
    {
        rt_thread_mdelay(100);
        
        if (rt_mb_recv(tmp_msg_mb, (rt_ubase_t *)&msg, RT_WAITING_FOREVER) == RT_EOK)
        {
            recvdata_p0 = fopen("recvdata_p0.txt", "a+");
            if (recvdata_p0 != RT_NULL)
            {
                fputs(msg->str_value, recvdata_p0);
                fputs("\n", recvdata_p0);
                fclose(recvdata_p0);
            }
            rt_mp_free(msg); /* 释放内存块 */
            msg = RT_NULL;   /* 请务必要做 */
        }
    }
}

static void led_shine_entry(void *parameter)
{
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    
    while(1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}

int main(void)
{
    rt_thread_t nrf24l01_thread, led_thread, DFS_thread;
    
    tmp_msg_mb = rt_mb_create("temp_mb0", MB_LEN, RT_IPC_FLAG_FIFO);
    tmp_msg_mp = rt_mp_create("temp_mp0", MP_LEN, MP_BLOCK_SIZE);
    
    nrf24l01_thread  = rt_thread_create("nrfrecv", nrf24l01_receive_entry, RT_NULL,
                                        1024, RT_THREAD_PRIORITY_MAX / 2, 20);
    if (nrf24l01_thread != RT_NULL)
    {
        rt_thread_startup(nrf24l01_thread);
    }
    
    DFS_thread = rt_thread_create("DFSsave", save_recv_data_entry, RT_NULL,
                                  1024, RT_THREAD_PRIORITY_MAX / 2 - 1, 20);
    if (nrf24l01_thread != RT_NULL)
    {
        rt_thread_startup(DFS_thread);
    }

    led_thread = rt_thread_create("ledshine", led_shine_entry, RT_NULL,
                                  192, RT_THREAD_PRIORITY_MAX - 2, 20);
    if (led_thread != RT_NULL)
    {
        rt_thread_startup(led_thread);
    }

    return RT_EOK;
}

static int rt_hw_nrf24l01_init(void)
{
    rt_hw_spi_device_attach("spi2", "spi20", GPIOD, GPIO_PIN_5);
    
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_nrf24l01_init);
