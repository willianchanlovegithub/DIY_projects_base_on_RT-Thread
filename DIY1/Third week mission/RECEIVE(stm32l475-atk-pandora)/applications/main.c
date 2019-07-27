/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2019-07-23     WillianChan  DIY Demo2(Second week mission)
 * 2019-07-27     WillianChan  DIY Demo3(Third week mission)这是一个雏形版本！！！
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_spi.h"
#include "nrf24l01.h"
#include <dfs_posix.h> /* 当需要使用文件操作时，需要包含这个头文件 */

#define LED0_PIN            GET_PIN(E, 7)
#define NRF24L01_CE_PIN     GET_PIN(D, 4)
#define NRF24_IRQ_PIN       GET_PIN(D, 3)
#define NRF24L01_SPI_DEVICE "spi20"

uint8_t RxBuf_P0[32] = {0};
uint8_t RxBuf_P1[32] = {0};
uint8_t RxBuf_P2[32] = {0};
uint8_t RxBuf_P3[32] = {0};
uint8_t RxBuf_P4[32] = {0};
uint8_t RxBuf_P5[32] = {0};

static void nrf24l01_receive_entry(void *parameter)
{
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
        
        if (rx_pipe_num_choose())
        {
            rt_kprintf("Reveice Nothing!\n");
        }
    }
}

/* 这是一个雏形版本！！！ */
static void save_recv_data_entry(void *parameter)
{
    int fd_p0, fd_p1, fd_p2, fd_p3, fd_p4, fd_p5;
    
    while (1)
    {
        rt_thread_mdelay(100);
        
        /* 以创建和读写模式打开 /recv_data_p0.txt 文件，如果该文件不存在则创建该文件 */
        fd_p0 = open("/recv_data_p0.txt", O_WRONLY | O_CREAT);
        if (fd_p0 >= 0)
        {
            write(fd_p0, RxBuf_P0, sizeof(RxBuf_P0));
            close(fd_p0);
        }
        else
            rt_kprintf("RxBuf_P0 Write failed.\n");
        
        fd_p1 = open("/recv_data_p1.txt", O_WRONLY | O_CREAT);
        if (fd_p1 >= 0)
        {
            write(fd_p1, RxBuf_P1, sizeof(RxBuf_P1));
            close(fd_p1);
        }
        else
            rt_kprintf("RxBuf_P1 Write failed.\n");
        
        fd_p2 = open("/recv_data_p2.txt", O_WRONLY | O_CREAT);
        if (fd_p2 >= 0)
        {
            write(fd_p2, RxBuf_P2, sizeof(RxBuf_P2));
            close(fd_p2);
        }
        else
            rt_kprintf("RxBuf_P2 Write failed.\n");
        
        fd_p3 = open("/recv_data_p3.txt", O_WRONLY | O_CREAT);
        if (fd_p3 >= 0)
        {
            write(fd_p3, RxBuf_P3, sizeof(RxBuf_P3));
            close(fd_p3);
        }
        else
            rt_kprintf("RxBuf_P3 Write failed.\n");
        
        fd_p4 = open("/recv_data_p4.txt", O_WRONLY | O_CREAT);
        if (fd_p4 >= 0)
        {
            write(fd_p4, RxBuf_P4, sizeof(RxBuf_P4));
            close(fd_p4);
        }
        else
            rt_kprintf("RxBuf_P4 Write failed.\n");
        
        fd_p5 = open("/recv_data_p5.txt", O_WRONLY | O_CREAT);
        if (fd_p5 >= 0)
        {
            write(fd_p5, RxBuf_P5, sizeof(RxBuf_P5));
            close(fd_p5);
        }
        else
            rt_kprintf("RxBuf_P5 Write failed.\n");
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
    
    nrf24l01_thread  = rt_thread_create("nrfsend", nrf24l01_receive_entry, RT_NULL,
                                        1024, RT_THREAD_PRIORITY_MAX / 2, 20);
    if (nrf24l01_thread != RT_NULL)
    {
        rt_thread_startup(nrf24l01_thread);
    }
    
    /* 这是一个雏形版本！！！ */
    DFS_thread = rt_thread_create("savedata", save_recv_data_entry, RT_NULL,
                                        1024, RT_THREAD_PRIORITY_MAX / 2, 20);
    if (nrf24l01_thread != RT_NULL)
    {
        rt_thread_startup(DFS_thread);
    }

    led_thread = rt_thread_create("ledshine", led_shine_entry, RT_NULL,
                                  192, RT_THREAD_PRIORITY_MAX / 2, 20);
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
