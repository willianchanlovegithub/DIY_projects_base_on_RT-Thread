/*
 * Copyright (c) 2019, sogwyms@gmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-23     sogwms       the first version
 */
#ifndef __NRF24L01_PORT_H__
#define __NRF24L01_PORT_H__

#include <stdint.h>

typedef struct {
    uint8_t (*send_then_recv)(const uint8_t *pb_send, uint8_t len_send, uint8_t *pb_recv, uint8_t len_recv);
    uint8_t (*send_then_send)(const uint8_t *pb_send1, uint8_t len1, const uint8_t *pb_send2, uint8_t len2);
    uint8_t (*write)(const uint8_t *pb, uint8_t len);
    void (*set_ce)(void);
    void (*reset_ce)(void);
    int (*init)(void *vp);
} hal_nrf24l01_port_t;

struct hal_nrf24l01_port_cfg{
    int ce_pin;
    char *spi_device_name;
};

#endif // __NRF24L01_PORT_H__


