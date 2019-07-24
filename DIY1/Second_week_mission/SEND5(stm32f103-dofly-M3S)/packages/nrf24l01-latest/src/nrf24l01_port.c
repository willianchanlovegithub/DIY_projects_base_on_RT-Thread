/*
 * Copyright (c) 2019, sogwyms@gmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-23     sogwms       the first version
 */
#include "nrf24l01_port.h"
#include <rtdevice.h>
#include "drv_gpio.h"
#include "drv_spi.h"

static struct rt_spi_device *spi_dev_nrf;
static int nrf24l01_ce_pin;

static uint8_t send_then_recv(const uint8_t *pb_send, uint8_t len_send, uint8_t *pb_recv, uint8_t len_recv)
{
    rt_spi_send_then_recv(spi_dev_nrf, pb_send, len_send, pb_recv, len_recv);
    return 0;
}

static uint8_t send_then_send(const uint8_t *pb_send1, uint8_t len1, const uint8_t *pb_send2, uint8_t len2)
{
    rt_spi_send_then_send(spi_dev_nrf, pb_send1, len1, pb_send2, len2);
    return 0;
}

static uint8_t write(const uint8_t *pb, uint8_t len)
{
    rt_spi_send(spi_dev_nrf, pb, len);
    return len;
}

static void set_ce(void)
{
    rt_pin_write(nrf24l01_ce_pin, PIN_HIGH);
}

static void reset_ce(void)
{
    rt_pin_write(nrf24l01_ce_pin, PIN_LOW);
}

static int init(void *vp)
{
    struct rt_spi_configuration cfg;        
    struct hal_nrf24l01_port_cfg *halcfg = (struct hal_nrf24l01_port_cfg *)vp;

    nrf24l01_ce_pin = halcfg->ce_pin;

    spi_dev_nrf = (struct rt_spi_device *)rt_device_find(halcfg->spi_device_name);
    if (!spi_dev_nrf) {
        rt_kprintf("[nrf24l01 port]error can't find device");
    }

    cfg.data_width = 8;
    cfg.max_hz = 10 * 1000 * 1000;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MSB | RT_SPI_MODE_0;
    rt_spi_configure(spi_dev_nrf, &cfg);

    rt_pin_mode(nrf24l01_ce_pin, PIN_MODE_OUTPUT);
    reset_ce();

    return 0;
}

hal_nrf24l01_port_t hal_nrf24l01_port = {
    .send_then_recv = send_then_recv,
    .send_then_send = send_then_send,
    .write = write,
    .set_ce = set_ce,
    .reset_ce = reset_ce,
    .init = init
};
