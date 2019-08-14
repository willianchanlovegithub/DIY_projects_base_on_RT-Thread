/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2018-11-19     flybreak     add stm32f407-atk-explorer bsp
 * 2019-07-15     WillianCham  DIY Demo1(First week mission)
 * 2019-07-24     WillianChan  DIY Demo2 use mailbox and mempool(Second week mission)
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "sensor.h"
#include "sensor_dallas_ds18b20.h"
#include "drv_spi.h"
#include "nrf24l01.h"

#define LED0_PIN            GET_PIN(F, 9)
#define DS18B20_DATA_PIN    GET_PIN(G, 9)
#define NRF24L01_CE_PIN     GET_PIN(G, 6)
#define NRF24_IRQ_PIN       GET_PIN(G, 8)
#define NRF24L01_SPI_DEVICE "spi10"

#define MP_BLOCK_SIZE       RT_ALIGN(sizeof(struct tmp_msg), sizeof(intptr_t)) /* 为了字节对齐 */
#define MB_LEN              (4)
#define MP_LEN              MB_LEN

/* 这是要传输的数据结构定义 */
struct tmp_msg
{
    rt_tick_t timestamp;
    char str_value[8];
    int int_value;
    float float_value;
};
static rt_mailbox_t tmp_msg_mb;           /* 邮箱 */
static rt_mp_t tmp_msg_mp;                /* 内存池 */
static struct rt_sensor_data sensor_data; /* sensor结构体 */

static void read_temp_entry(void *parameter)
{
    struct tmp_msg *msg;
    rt_device_t dev = RT_NULL;
    rt_size_t res;

    dev = rt_device_find(parameter);
    if (dev == RT_NULL)
    {
        rt_kprintf("Can't find device:%s\n", parameter);
        return;
    }

    if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR) != RT_EOK)
    {
        rt_kprintf("open device failed!\n");
        return;
    }
    rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)100);

    while (1)
    {
        res = rt_device_read(dev, 0, &sensor_data, 1);
        if (res != 1)
        {
            rt_kprintf("read data failed!size is %d\n", res);
            rt_device_close(dev);
            return;
        }
        else
        {
            /* 申请一块内存 要是内存池满了 就挂起等待 */
            msg = rt_mp_alloc(tmp_msg_mp, RT_WAITING_FOREVER);
            msg->timestamp = sensor_data.timestamp;
            msg->int_value = sensor_data.data.temp;
            rt_mb_send(tmp_msg_mb, (rt_ubase_t)msg);
            msg = NULL;
        }
        rt_thread_mdelay(500);
    }
}

static void nrf24l01_send_entry(void *parameter)
{
    struct tmp_msg *msg;
    struct hal_nrf24l01_port_cfg halcfg;
    nrf24_cfg_t cfg;
    uint8_t rbuf[32 + 1] = {0};
    uint8_t tbuf[32] = {0};

    nrf24_default_param(&cfg);
    halcfg.ce_pin = NRF24L01_CE_PIN;
    halcfg.spi_device_name = NRF24L01_SPI_DEVICE;
    cfg.role = ROLE_PTX;
    cfg.ud = &halcfg;
    cfg.use_irq = 0;
    nrf24_init(&cfg);

    while (1)
    {
        rt_thread_mdelay(500);
        
        if (rt_mb_recv(tmp_msg_mb, (rt_ubase_t*)&msg, RT_WAITING_FOREVER) == RT_EOK)
        {
            if (msg->int_value >= 0)
            {
                rt_sprintf((char *)tbuf, "%d,+%3d.%d", msg->timestamp, msg->int_value / 10, msg->int_value % 10);
            }
            else
            {
                rt_sprintf((char *)tbuf, "%d,-%2d.%d", msg->timestamp, -msg->int_value / 10, -msg->int_value % 10);
            }
            rt_kputs((char *)tbuf);
            rt_kputs("\n");
            rt_mp_free(msg); /* 释放内存块 */
            msg = RT_NULL;   /* 请务必要做 */
        }
        if (nrf24_ptx_run(rbuf, tbuf, rt_strlen((char *)tbuf)) < 0)
        {
            rt_kputs("Send failed! >>> ");
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
    rt_thread_t ds18b20_thread, led_thread, nrf24l01_thread;
    
    tmp_msg_mb = rt_mb_create("temp_mb0", MB_LEN, RT_IPC_FLAG_FIFO);
    tmp_msg_mp = rt_mp_create("temp_mp0", MP_LEN, MP_BLOCK_SIZE);
    
    ds18b20_thread = rt_thread_create("18b20tem", read_temp_entry, "temp_ds18b20",
                                      640, RT_THREAD_PRIORITY_MAX / 2, 20);
    if (ds18b20_thread != RT_NULL)
    {
        rt_thread_startup(ds18b20_thread);
    }
    
    nrf24l01_thread  = rt_thread_create("nrfsend", nrf24l01_send_entry, RT_NULL,
                                        1024, RT_THREAD_PRIORITY_MAX / 2, 20);
    if (nrf24l01_thread != RT_NULL)
    {
        rt_thread_startup(nrf24l01_thread);
    }
    
    led_thread = rt_thread_create("ledshine", led_shine_entry, RT_NULL,
                                  192, RT_THREAD_PRIORITY_MAX / 2, 20);
    if (led_thread != RT_NULL)
    {
        rt_thread_startup(led_thread);
    }
    
    return RT_EOK;
}

static int rt_hw_ds18b20_port(void)
{
    struct rt_sensor_config cfg;
    
    cfg.intf.user_data = (void *)DS18B20_DATA_PIN;
    rt_hw_ds18b20_init("ds18b20", &cfg);
    
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_ds18b20_port);

static int rt_hw_nrf24l01_init(void)
{
    rt_hw_spi_device_attach("spi1", "spi10", GPIOG, GPIO_PIN_7);
    
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_nrf24l01_init);
