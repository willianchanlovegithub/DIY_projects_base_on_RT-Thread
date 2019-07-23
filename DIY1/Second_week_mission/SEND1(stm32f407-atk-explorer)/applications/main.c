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
 * 2019-07-18     WillianChan  DIY Demo2(Second week mission)
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

static void read_temp_entry(void *parameter)
{
    rt_device_t dev = RT_NULL;
    struct rt_sensor_data sensor_data;
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
            if (sensor_data.data.temp >= 0)
            {
                rt_kprintf("temp:%3d.%dC, timestamp:%5d\n",
                       sensor_data.data.temp / 10,
                       sensor_data.data.temp % 10,
                       sensor_data.timestamp);
            }
            else
            {
                rt_kprintf("temp:-%2d.%dC, timestamp:%5d\n",
                       abs(sensor_data.data.temp / 10),
                       abs(sensor_data.data.temp % 10),
                       sensor_data.timestamp);
            }
        }
        rt_thread_mdelay(100);
    }
}

static void nrf24l01_send_entry(void *parameter)
{
    struct hal_nrf24l01_port_cfg halcfg;
    nrf24_cfg_t cfg;
    int rlen;
    uint8_t rbuf[32 + 1];
    uint8_t tbuf[32] = "\nStart Sending\n";
    uint32_t cnt = 0;

    nrf24_default_param(&cfg);
    halcfg.ce_pin = NRF24L01_CE_PIN;
    halcfg.spi_device_name = NRF24L01_SPI_DEVICE;
    cfg.role = ROLE_PTX;
    cfg.ud = &halcfg;
    cfg.use_irq = 0;
    nrf24_init(&cfg);

    while (1)
    {
        rt_thread_mdelay(100);
        
        rlen = nrf24_ptx_run(rbuf, tbuf, rt_strlen((char *)tbuf));
        if (rlen >= 0)
        {
            rt_sprintf((char *)tbuf, "i-am-PTX_1:%dth\r\n", cnt++);
        }
        else
        {
            rt_kputs("Send data failed\r\n");            
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
