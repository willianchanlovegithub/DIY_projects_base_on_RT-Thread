/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2019-07-23     WillianChan  DIY Demo2(Second week mission)
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_spi.h"
#include "nrf24l01.h"

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
        rt_thread_mdelay(500);
        
        if (!rx_pipe_num_choose())
        {
            rt_kprintf("nrf24l01 pipe0 data:%s\n", &RxBuf_P0);
            rt_kprintf("nrf24l01 pipe1 data:%s\n", &RxBuf_P1);
            rt_kprintf("nrf24l01 pipe2 data:%s\n", &RxBuf_P2);
            rt_kprintf("nrf24l01 pipe3 data:%s\n", &RxBuf_P3);
            rt_kprintf("nrf24l01 pipe4 data:%s\n", &RxBuf_P4);
            rt_kprintf("nrf24l01 pipe5 data:%s\n", &RxBuf_P5);
            rt_kputs("\n");
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
    rt_thread_t nrf24l01_thread, led_thread;
    
    nrf24l01_thread  = rt_thread_create("nrfsend", nrf24l01_receive_entry, RT_NULL,
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

static int rt_hw_nrf24l01_init(void)
{
    rt_hw_spi_device_attach("spi2", "spi20", GPIOD, GPIO_PIN_5);
    
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_nrf24l01_init);
