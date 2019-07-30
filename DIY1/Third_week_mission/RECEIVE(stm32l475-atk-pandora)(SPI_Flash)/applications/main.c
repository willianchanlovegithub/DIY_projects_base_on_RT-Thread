/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2019-07-23     WillianChan  DIY Demo2(Second week mission)
 * 2019-07-30     WillianChan  DIY Demo3(Third week mission)
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
#define WRITE_EVENT         (0x01U << 0)  /* 感兴趣的事件 */

static rt_event_t recvdata_event;         /* 事件集 */
static struct rt_ringbuffer *recvdatabuf; /* ringbuffer */

struct recvdata
{
    int timestamp;
    float temperature;
};

static void nrf24l01_receive_entry(void *parameter)
{
    struct recvdata buf;
    static char str_data[64];
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
        if (!rx_pipe_num_choose())
        {
            if(sscanf((char *)RxBuf_P0, "%d,+%f", &buf.timestamp, &buf.temperature) != 2)
            {
                if(sscanf((char *)RxBuf_P0, "%d,-%f", &buf.timestamp, &buf.temperature) != 2)
                {
                    continue;
                }
                buf.temperature = -buf.temperature;
            }
            sprintf(str_data, "%d,%f\n",buf.timestamp, buf.temperature);
            
            rt_ringbuffer_put(recvdatabuf, (rt_uint8_t *)str_data, strlen(str_data));
            rt_event_send(recvdata_event, WRITE_EVENT);
        }
        
        rt_thread_mdelay(30);
    }
}

static void save_recv_data_entry(void *parameter)
{
    FILE *recvdata_p0 = RT_NULL;
    rt_uint32_t set;
    static int writebuffer[1024];
    rt_size_t size;
    while (1)
    {
        if (rt_event_recv(recvdata_event, WRITE_EVENT, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &set) != RT_EOK)
        {
            continue;
        }
        
        do
        {
            if (rt_event_recv(recvdata_event, WRITE_EVENT, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, rt_tick_from_millisecond(1000), &set) == RT_EOK)
            {
                if (rt_ringbuffer_data_len(recvdatabuf) > (4096 / 2))
                {
                    recvdata_p0 = fopen("recvdata_p0.csv", "ab+");
                    
                    if (recvdata_p0 != RT_NULL)
                    {
                        while(rt_ringbuffer_data_len(recvdatabuf))
                        {
                            size = rt_ringbuffer_get(recvdatabuf, (rt_uint8_t *)writebuffer, (4096 / 2));
                            fwrite(writebuffer, 1, size, recvdata_p0);
                        }
                        fclose(recvdata_p0);
                    }
                }
                continue;
            }
            recvdata_p0 = fopen("recvdata_p0.csv", "ab+");

            if (recvdata_p0 != RT_NULL)
            {
                while(rt_ringbuffer_data_len(recvdatabuf))
                {
                    size = rt_ringbuffer_get(recvdatabuf, (rt_uint8_t *)writebuffer, (4096 / 2));
                    fwrite(writebuffer, 1, size, recvdata_p0);
                }
                fclose(recvdata_p0);
            }
            
        } while(0);
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
    
    recvdata_event = rt_event_create("temp_evt0", RT_IPC_FLAG_FIFO);
    RT_ASSERT(recvdata_event);
    recvdatabuf = rt_ringbuffer_create(4096); /* ringbuffer的大小是4KB */
    RT_ASSERT(recvdatabuf);
    
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
