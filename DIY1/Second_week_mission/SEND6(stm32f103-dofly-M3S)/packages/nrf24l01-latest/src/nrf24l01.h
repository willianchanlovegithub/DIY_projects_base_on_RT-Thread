/*
 * Copyright (c) 2019, sogwyms@gmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-23     sogwms       the first version
 */
#ifndef __NRF24L01_H__
#define __NRF24L01_H__

/* Includes --------------------------------------------------------------------------------*/
#include "nrf24l01_port.h"
/* Exported types --------------------------------------------------------------------------*/
typedef enum
{
    ROLE_PTX = 0xF0,
    ROLE_PRX = 0xF1,
} nrf24_role_et;

typedef enum
{
    MODE_POWER_DOWN,
    MODE_STANDBY,
    MODE_TX,
    MODE_RX,
} nrf24_mode_et;

typedef enum
{
    CRC_0_BYTE = 0xF0,
    CRC_1_BYTE = 0,
    CRC_2_BYTE = 1,
} nrf24_crc_et;

typedef enum
{
    RF_POWER_N18dBm = 0,
    RF_POWER_N12dBm = 0x1,
    RF_POWER_N6dBm  = 0x2,
    RF_POWER_0dBm   = 0x3,
} nrf24_power_et;

typedef enum
{
    ADR_1Mbps = 0,
    ADR_2Mbps = 1,
} nrf24_adr_et;

typedef struct
{
    uint8_t ard : 4;    // meaning: (ard+1)*250us; value '0' isn't recommended
    uint8_t arc : 4;
} nrf24_esb_t;

typedef struct
{
    nrf24_esb_t esb;

    nrf24_role_et role;
    nrf24_power_et power;
    nrf24_adr_et adr;
    nrf24_crc_et crc;
    uint8_t address[5];
    uint8_t channel;    //range: 0 ~ 127

    uint8_t use_irq;
    void *ud;
} nrf24_cfg_t;

/* Exported constants ----------------------------------------------------------------------*/
/* Exported macro --------------------------------------------------------------------------*/ 
/* Exported variables ----------------------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------------------*/  
extern void nrf24_default_param(nrf24_cfg_t *pt);
extern int nrf24_init(nrf24_cfg_t *pt);
extern int nrf24_ptx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen);
extern int nrf24_prx_cycle(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen);
extern void nrf24_prx_write_txbuffer(const uint8_t *pb, uint8_t len);
extern int nrf24_irq_ptx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen, void(*waitirq)(void));
extern int nrf24_irq_prx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen, void(*waitirq)(void));
extern uint16_t nrf24_get_errcnt(void);
extern void nrf24_power_up(void);
extern void nrf24_power_down(void);
#ifdef NRF24_USING_INFO_REPORT
void nrf24_report(void);
#endif // NRF24_USING_INFO_REPORT

#endif // __NRF24L01_H__
