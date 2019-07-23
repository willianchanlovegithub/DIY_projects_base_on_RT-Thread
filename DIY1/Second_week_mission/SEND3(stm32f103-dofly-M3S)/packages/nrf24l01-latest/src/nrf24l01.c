/*
 * Copyright (c) 2019, sogwyms@gmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-23     sogwms       the first version
 */

// note: 地址宽度:5
// note: ACTIVATE command

/* Includes --------------------------------------------------------------------------------*/
#include <rtthread.h>
#include "nrf24l01.h"

/* Exported types --------------------------------------------------------------------------*/
/* Exported constants ----------------------------------------------------------------------*/
/* Exported macro --------------------------------------------------------------------------*/ 
///<命令映射  
#define NRF24CMD_R_REG          0x00  // 读寄存器
#define NRF24CMD_W_REG          0x20  // 写寄存器
#define NRF24CMD_R_RX_PAYLOAD   0x61  // 读接收缓冲区
#define NRF24CMD_W_TX_PAYLOAD   0xA0  // 写发送缓冲区
#define NRF24CMD_FLUSH_TX       0xE1  // 清空发送FIFO
#define NRF24CMD_FLUSH_RX       0xE2  // 清空接收FIFO
#define NRF24CMD_REUSE_TX_PL    0xE3  // PTX模式下使用，重装载发送缓冲区
#define NRF24CMD_ACTIVATE       0x50  // 使能命令，后接数据 0x73
#define NRF24CMD_R_RX_PL_WID    0x60  // 读顶层接收FIFO大小
#define NRF24CMD_W_ACK_PAYLOAD  0xA8  // RX模式下使用，写应答发送缓冲区
///<寄存器映射
#define NRF24REG_CONFIG         0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define NRF24REG_EN_AA          0x01  // 自动应答功能设置
#define NRF24REG_EN_RXADDR      0x02  // 可用信道设置
#define NRF24REG_SETUP_AW       0x03  // 收发地址宽度设置
#define NRF24REG_SETUP_RETR     0x04  // 自动重发功能设置
#define NRF24REG_RF_CH          0x05  // 工作频率设置
#define NRF24REG_RF_SETUP       0x06  // 发射速率、功耗功能设置
#define NRF24REG_STATUS         0x07  // 状态寄存器
#define NRF24REG_OBSERVE_TX     0x08  // 发送监测功能
#define NRF24REG_RPD            0x09  // 接收功率检测           
#define NRF24REG_RX_ADDR_P0     0x0A  // 频道0接收数据地址
#define NRF24REG_RX_ADDR_P1     0x0B  // 频道1接收数据地址
#define NRF24REG_RX_ADDR_P2     0x0C  // 频道2接收数据地址
#define NRF24REG_RX_ADDR_P3     0x0D  // 频道3接收数据地址
#define NRF24REG_RX_ADDR_P4     0x0E  // 频道4接收数据地址
#define NRF24REG_RX_ADDR_P5     0x0F  // 频道5接收数据地址
#define NRF24REG_TX_ADDR        0x10  // 发送地址寄存器
#define NRF24REG_RX_PW_P0       0x11  // 接收频道0接收数据长度
#define NRF24REG_RX_PW_P1       0x12  // 接收频道1接收数据长度
#define NRF24REG_RX_PW_P2       0x13  // 接收频道2接收数据长度
#define NRF24REG_RX_PW_P3       0x14  // 接收频道3接收数据长度
#define NRF24REG_RX_PW_P4       0x15  // 接收频道4接收数据长度
#define NRF24REG_RX_PW_P5       0x16  // 接收频道5接收数据长度
#define NRF24REG_FIFO_STATUS    0x17  // FIFO栈入栈出状态寄存器设置
#define NRF24REG_DYNPD          0x1C  // 动态数据包长度
#define NRF24REG_FEATURE        0x1D  // 特点寄存器  
///<寄存器功能位掩码部分映射
//CONFIG
#define NRF24BITMASK_RX_DR      ((uint8_t)(1<<6))  // 接收完成中断使能位
#define NRF24BITMASK_TX_DS      ((uint8_t)(1<<5))  // 发送完成中断使能位
#define NRF24BITMASK_MAX_RT     ((uint8_t)(1<<4))  // 达最大重发次数中断使能位
#define NRF24BITMASK_EN_CRC     ((uint8_t)(1<<3))  // CRC使能位
#define NRF24BITMASK_CRCO       ((uint8_t)(1<<2))  // CRC编码方式 （1B or 2B）
#define NRF24BITMASK_PWR_UP     ((uint8_t)(1<<1))  // 上（掉）电
#define NRF24BITMASK_PRIM_RX    ((uint8_t)(1))     // PR（T）X
//SETUP_AW
#define NRF24BITMASK_AW         ((uint8_t)(0x03))  // RX/TX地址宽度
//SETUP_RETR
#define NRF24BITMASK_ARD        ((uint8_t)(0xF0))  // 重发延时
#define NRF24BITMASK_ARC        ((uint8_t)(0x0F))  // 重发最大次数
//RF_CH
#define NRF24BITMASK_RF_CH      ((uint8_t)(0x7F))  // 射频频道
//RF_SETUP
#define NRF24BITMASK_RF_DR      ((uint8_t)(1<<3))  // 空中速率
#define NRF24BITMASK_RF_PWR     ((uint8_t)(0x06))  // 发射功率
//STATUS
#define NRF24BITMASK_RX_DR      ((uint8_t)(1<<6))  // 接收完成标志位
#define NRF24BITMASK_TX_DS      ((uint8_t)(1<<5))  // 发送完成标志位
#define NRF24BITMASK_MAX_RT     ((uint8_t)(1<<4))  // 最大重发次数标志位
#define NRF24BITMASK_RX_P_NO    ((uint8_t)(0x0E))  // RX_FIFO状态标志区位
#define NRF24BITMASK_TX_FULL    ((uint8_t)(1))     // TX_FIFO满标志位
//OBSERVE_TX
#define NRF24BITMASK_PLOS_CNT   ((uint8_t)(0xF0))  // 丢包计数
#define NRF24BITMASK_ARC_CNT    ((uint8_t)(0x0F))  // 重发计数
//CD
#define NRF24BITMASK_CD         ((uint8_t)(1))     // 载波检测标志位
//通用掩码，RX_PW_P[0::5] 掩码相同
#define NRF24BITMASK_RX_PW_P_   ((uint8_t)(0x3F))  // 数据管道RX-Payload中的字节数
//FIFO_STATUS
#define NRF24BITMASK_TX_REUSE   ((uint8_t)(1<<6))  // 
#define NRF24BITMASK_TX_FULL2    ((uint8_t)(1<<5)) // 
#define NRF24BITMASK_TX_EMPTY   ((uint8_t)(1<<4))  // 
#define NRF24BITMASK_RX_RXFULL  ((uint8_t)(1<<1))  // 
#define NRF24BITMASK_RX_EMPTY   ((uint8_t)(1))     // 
//FEATURE
#define NRF24BITMASK_EN_DPL     ((uint8_t)(1<<2))  // 动态长度使能位
#define NRF24BITMASK_EN_ACK_PAY ((uint8_t)(1<<1))  // Payload with ACK 使能位
#define NRF24BITMASK_EN_DYN_ACK ((uint8_t)(1))     // W_TX_PAYLOAD_NOACK 命令使能位
//通用掩码，适用于多个寄存器： EN_AA, EN_RXADDR, DYNPD
#define NRF24BITMASK_PIPE_0     ((uint8_t)(1))     // 
#define NRF24BITMASK_PIPE_1     ((uint8_t)(1<<1))  // 
#define NRF24BITMASK_PIPE_2     ((uint8_t)(1<<2))  // 
#define NRF24BITMASK_PIPE_3     ((uint8_t)(1<<3))  // 
#define NRF24BITMASK_PIPE_4     ((uint8_t)(1<<4))  // 
#define NRF24BITMASK_PIPE_5     ((uint8_t)(1<<5))  // 

/* Exported variables ----------------------------------------------------------------------*/
static uint16_t l_error_count = 0;
extern hal_nrf24l01_port_t hal_nrf24l01_port;

/* Exported functions ----------------------------------------------------------------------*/  



static uint8_t _read_reg(uint8_t reg)
{
    uint8_t temp, rtmp = 0;

    temp = NRF24CMD_R_REG | reg;
    hal_nrf24l01_port.send_then_recv(&temp, 1, &rtmp, 1);

    return rtmp;
}

static void _write_reg(uint8_t reg, uint8_t data)
{
    uint8_t temp[2];

    temp[0] = NRF24CMD_W_REG | reg;
    temp[1] = data;
    hal_nrf24l01_port.write(&temp[0], 2);
}

/**
 * @brief 置位寄存器的部分位
 * @param[in] reg: 寄存器地址
 * @param[in] mask: 位掩码. eg: 0x81 标识置位第七位和第零位
 */
static void _set_reg_bits(uint8_t reg, uint8_t mask)
{
    uint8_t temp;

    temp = _read_reg(reg);
    temp |= mask;
    _write_reg(reg, temp);
}

static void _reset_reg_bits(uint8_t reg, uint8_t mask)
{
    uint8_t temp;

    temp = _read_reg(reg);
    temp &= ~mask;
    _write_reg(reg, temp);
}

static void _write_reg_bits(uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t temp, tidx;

    for (tidx = 0; tidx < 8; tidx++)
    {
        if (mask & (1 << tidx))
            break;
    }
    temp = ~mask & _read_reg(reg);
    temp |= mask & (value << tidx);
    _write_reg(reg, temp);
}

static void send_activate_command(void)
{
    uint8_t temp[2] = {
        NRF24CMD_ACTIVATE,
        0x73};
    hal_nrf24l01_port.write(temp, 2);
}

static void set_address_width5(void)
{
    _write_reg(NRF24REG_SETUP_AW, 0x3);
}

static void set_tx_rp0_address5(const uint8_t *pb)
{
    uint8_t temp;

    temp = NRF24CMD_W_REG | NRF24REG_RX_ADDR_P0;
    hal_nrf24l01_port.send_then_send(&temp, 1, pb, 5);
    temp = NRF24CMD_W_REG | NRF24REG_TX_ADDR;
    hal_nrf24l01_port.send_then_send(&temp, 1, pb, 5);
}

static void set_rf_channel(uint8_t channel)
{
    _write_reg(NRF24REG_RF_CH, channel & 0x7F);
}

static void set_air_data_rate(nrf24_adr_et adr)
{
    if (adr == ADR_1Mbps)
    {
        _reset_reg_bits(NRF24REG_RF_SETUP, NRF24BITMASK_RF_DR);
    }
    else if (adr == ADR_2Mbps)
    {
        _set_reg_bits(NRF24REG_RF_SETUP, NRF24BITMASK_RF_DR);
    }
}

static void set_rf_power(nrf24_power_et pa)
{
    if ((pa == RF_POWER_0dBm) ||
        (pa == RF_POWER_N6dBm) ||
        (pa == RF_POWER_N12dBm) ||
        (pa == RF_POWER_N18dBm))
    {
        _write_reg_bits(NRF24REG_RF_SETUP, NRF24BITMASK_RF_PWR, pa);
    }
}

static void _set_auto_retransmit_delay(uint8_t ard)
{
    _write_reg_bits(NRF24REG_SETUP_RETR, NRF24BITMASK_ARD, ard);
}

static void _set_auto_retransmit_count(uint8_t arc)
{
    _write_reg_bits(NRF24REG_SETUP_RETR, NRF24BITMASK_ARC, arc);
}

static void set_crc(nrf24_crc_et crc)
{
    if (crc == CRC_0_BYTE)
    {
        _reset_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_EN_CRC);
    }
    else
    {
        if (crc == CRC_1_BYTE)
        {
            _set_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_EN_CRC);
            _reset_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_CRCO);
        }
        else if (crc == CRC_2_BYTE)
        {
            _set_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_EN_CRC);
            _set_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_CRCO);
        }
    }
}

static void set_esb_param(nrf24_esb_t *pt)
{
    // avoid ARD equals 250us
    if (pt->ard == 0)
        pt->ard += 1;
    _set_auto_retransmit_delay(pt->ard);
    _set_auto_retransmit_count(pt->arc);
}

// bit: RX_DR, TX_DS, MAX_RT
static void reset_status(uint8_t bitmask)
{
    bitmask |= _read_reg(NRF24REG_STATUS);
    _write_reg(NRF24REG_STATUS, bitmask);
}

static void reset_observe_tx(void)
{
    _write_reg(NRF24REG_OBSERVE_TX, 0);
}

//[note] the NRF24CMD_R_RX_PL_WID command doesn't work?!
static uint8_t get_top_rxfifo_width(void)
{
    uint8_t temp = NRF24CMD_R_RX_PL_WID;

    hal_nrf24l01_port.send_then_recv(&temp, 1, &temp, 1);
    return temp;
}

// [note] enable activate-command befor enable_dpl
// pipe range: 0 ~ 5
// will also enable ENAA_PX to
static void enable_dpl_and_ackpayload(uint8_t pipe)
{
    if (pipe > 5)
        return;

    _set_reg_bits(NRF24REG_FEATURE, NRF24BITMASK_EN_ACK_PAY);
    _set_reg_bits(NRF24REG_FEATURE, NRF24BITMASK_EN_DPL);
    _set_reg_bits(NRF24REG_EN_AA, (1 << pipe));
    _set_reg_bits(NRF24REG_DYNPD, (1 << pipe));
}

static void enabled_irq(uint8_t bitmask)
{
    if (!((bitmask == NRF24BITMASK_RX_DR) || (bitmask == NRF24BITMASK_TX_DS) || (bitmask == NRF24BITMASK_MAX_RT)))
        return;

    _reset_reg_bits(NRF24REG_CONFIG, bitmask);
}

static void disable_irq(uint8_t bitmask)
{
    if (!((bitmask == NRF24BITMASK_RX_DR) || (bitmask == NRF24BITMASK_TX_DS) || (bitmask == NRF24BITMASK_MAX_RT)))
        return;

    _set_reg_bits(NRF24REG_CONFIG, bitmask);
}

static void write_tx_payload(const uint8_t *pb, uint8_t len)
{
    uint8_t temp = NRF24CMD_W_TX_PAYLOAD;

    hal_nrf24l01_port.send_then_send(&temp, 1, pb, len);
}

static void write_ack_payload(uint8_t pipe, const uint8_t *pb, uint8_t len)
{
    uint8_t temp;

    if (pipe > 5)
        return;

    temp = NRF24CMD_W_ACK_PAYLOAD | pipe;
    hal_nrf24l01_port.send_then_send(&temp, 1, pb, len);
}

static void read_rxpayload(uint8_t *pb, uint8_t len)
{
    uint8_t tcmd;

    if ((len > 32) || (len == 0))
        return;

    tcmd = NRF24CMD_R_RX_PAYLOAD;
    hal_nrf24l01_port.send_then_recv(&tcmd, 1, pb, len);
}

static void flush_tx_fifo(void)
{
    uint8_t temp = NRF24CMD_FLUSH_TX;

    hal_nrf24l01_port.write(&temp, 1);
}

static void flush_rx_fifo(void)
{
    uint8_t temp = NRF24CMD_FLUSH_RX;

    hal_nrf24l01_port.write(&temp, 1);
}

// note: will clear the error-counter
uint16_t nrf24_get_errcnt(void)
{
    uint16_t temp = l_error_count;
    l_error_count = 0;
    
    return temp;
}

void nrf24_power_up(void)
{
    _set_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_PWR_UP);
}

void nrf24_power_down(void)
{
    _reset_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_PWR_UP);
}

/**
 * @brief PTX(ROLE) run
 * @param[in] pb_tx: pointer of tx-buffer
 * @param[out] pb_rx: pointer of rx-buffer
 * @param tlen: size of pb_tx (by bytes)
 * @return Zero indicates sent complete but no data received;
 *         Negative number indicates error;
 *         Other indicates the number of bytes of received data, and indicates that sent is complete 
 * 
 * @attention Send data and then received data 
 */
int nrf24_ptx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen)
{
    uint8_t sta, trycnt = 0, rlen = 0;

    if (tlen > 32)
        return -6;

    write_tx_payload(pb_tx, tlen);

    hal_nrf24l01_port.set_ce();

    do
    {
        sta = _read_reg(NRF24REG_STATUS);
        if (sta & NRF24BITMASK_MAX_RT)
        {
            // send failed; try again
            reset_status(NRF24BITMASK_MAX_RT);

            if (++trycnt > 6)
            {
                hal_nrf24l01_port.reset_ce();
                flush_tx_fifo();
                reset_status(NRF24BITMASK_MAX_RT);

                l_error_count++;

                return -1;
            }
        }
    } while (!(sta & NRF24BITMASK_TX_DS));
    reset_status(NRF24BITMASK_TX_DS);

    if (sta & NRF24BITMASK_RX_DR)
    {
        reset_status(NRF24BITMASK_RX_DR);
        rlen = get_top_rxfifo_width();
        read_rxpayload(pb_rx, rlen);
    }

    hal_nrf24l01_port.reset_ce();

    return rlen;
}

/**
 * @brief PRX(ROLE) cycle
 * @param[out] pb_rx: pointer of rx-buffer
 * @param[in] pb_tx: pointer of tx-buffer
 * @param tlen: size of pb_tx (by bytes)
 * @return Zero indicates no data received;
 *         Negative number indicates error;          
 *         Other indicates the number of bytes of received data, and indicates that sent is complete 
 * 
 * @attention Receive data and then send data
 */
int nrf24_prx_cycle(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen)
{
    uint8_t sta, rlen = 0;

    sta = _read_reg(NRF24REG_FIFO_STATUS);
    if (!(sta & NRF24BITMASK_RX_EMPTY))
    {
        rlen = get_top_rxfifo_width();
        read_rxpayload(pb_rx, rlen);
        // flush_rx_fifo();
        if ((tlen > 0) && (tlen <= 32))
        {
            write_ack_payload(0, pb_tx, tlen);
        }
    }

    return rlen;
}

void nrf24_prx_write_txbuffer(const uint8_t *pb, uint8_t len)
{
    if (len > 32)
        return;
    write_ack_payload(0, pb, len);
}

/**
 * Please refer to nrf24_ptx_run
 */
int nrf24_irq_ptx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen, void (*waitirq)(void))
{
    int rlen = 0;
    uint8_t sta;

    if (tlen > 32)
        return -6;

    write_tx_payload(pb_tx, tlen);
    hal_nrf24l01_port.set_ce();
    (*waitirq)();
    sta = _read_reg(NRF24REG_STATUS);
    if (sta & NRF24BITMASK_TX_DS)
    {
        reset_status(NRF24BITMASK_TX_DS);
        if (sta & NRF24BITMASK_RX_DR)
        {
            reset_status(NRF24BITMASK_RX_DR);
            rlen = get_top_rxfifo_width();
            read_rxpayload(pb_rx, rlen);
        }
    }
    else if (sta & NRF24BITMASK_MAX_RT)
    {
        hal_nrf24l01_port.reset_ce();
        flush_tx_fifo();
        reset_status(NRF24BITMASK_MAX_RT);
        
        l_error_count++;

        rlen = -1;
    }
    else
    {
        // shouldn't run to here
        rlen = -2;
    }

    hal_nrf24l01_port.reset_ce();

    return rlen;
}

/**
 * Please refer to nrf24_prx_cycle
 */
int nrf24_irq_prx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen, void (*waitirq)(void))
{
    int rlen = 0;

    (*waitirq)();
    reset_status(NRF24BITMASK_RX_DR | NRF24BITMASK_TX_DS);
    rlen = nrf24_prx_cycle(pb_rx, pb_tx, tlen);

    return rlen;
}

void nrf24_default_param(nrf24_cfg_t *pt)
{
    pt->power = RF_POWER_0dBm;
    pt->esb.ard = 5;        // (5+1)*250 = 1500us
    pt->esb.arc = 6;        // up to 6 times
    pt->crc = CRC_2_BYTE;   // crc; fcs is two bytes
    pt->adr = ADR_1Mbps;    // air data rate 1Mbps
    pt->channel = 6;        // rf channel 6

    pt->address[4] = 0xC2;  // address is 0xC2C2C2C2C3
    pt->address[3] = 0xC2;
    pt->address[2] = 0xC2;
    pt->address[1] = 0xC2;
    pt->address[0] = 0xC3;
}

/**
 * @return success returns 0; failure returns negative number
 */
int nrf24_init(nrf24_cfg_t *pt)
{
    if ((pt->role != ROLE_PTX) && (pt->role != ROLE_PRX))
    {
        rt_kprintf("[nrf24-warning]: unknown ROLE\r\n");
        _reset_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_PWR_UP);
        return -1;
    }

    hal_nrf24l01_port.init(pt->ud);

    send_activate_command(); // it doesn't work?

    enable_dpl_and_ackpayload(0);
    set_address_width5();
    set_tx_rp0_address5(pt->address);
    set_rf_power(pt->power);
    set_rf_channel(pt->channel);
    set_air_data_rate(pt->adr);
    set_crc(pt->crc);
    set_esb_param(&pt->esb);

    if (pt->use_irq) {
        //enable all irq
        enabled_irq(NRF24BITMASK_RX_DR | NRF24BITMASK_TX_DS | NRF24BITMASK_MAX_RT);
    }
    else {
        //disable all irq
        disable_irq(NRF24BITMASK_RX_DR | NRF24BITMASK_TX_DS | NRF24BITMASK_MAX_RT);
    }

    flush_rx_fifo();
    flush_tx_fifo();

    reset_status(NRF24BITMASK_RX_DR | NRF24BITMASK_TX_DS | NRF24BITMASK_MAX_RT);
    reset_observe_tx();

    if (pt->role == ROLE_PTX)
    {
        _set_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_PWR_UP);
        _reset_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_PRIM_RX);
    }
    else if (pt->role == ROLE_PRX)
    {
        _set_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_PWR_UP);
        _set_reg_bits(NRF24REG_CONFIG, NRF24BITMASK_PRIM_RX);
        hal_nrf24l01_port.set_ce();
    }
    else
    {
        // never run to here
        ;
    }

    return 0;
}

#if defined(NRF24_USING_INFO_REPORT)

void _nrf24_report_config_reg(uint8_t data)
{
    if (data & NRF24BITMASK_PRIM_RX)
        rt_kprintf("PRX mode\r\n");
    else
        rt_kprintf("PTX mode\r\n");
    if (data & NRF24BITMASK_EN_CRC)
    {
        rt_kprintf("crc opened. FCS: ");
        if (data & NRF24BITMASK_CRCO)
            rt_kprintf("2bytes\r\n");
        else
            rt_kprintf("1byte\r\n");
    }
    else
        rt_kprintf("crc closed\r\n");
    if (!(data & (NRF24BITMASK_RX_DR | NRF24BITMASK_TX_DS | NRF24BITMASK_MAX_RT)))
    {
        if (!(data & NRF24BITMASK_RX_DR))
            rt_kprintf("RX irq; ");
        if (!(data & NRF24BITMASK_TX_DS))
            rt_kprintf("TX irq; ");
        if (!(data & NRF24BITMASK_MAX_RT))
            rt_kprintf("MAX_RT irq; ");
        rt_kprintf("opened\r\n");
    }
    else
    {
        rt_kprintf("all irq closed\r\n");
    }
    if (data & NRF24BITMASK_PWR_UP)
        rt_kprintf("power up now\r\n");
    else
        rt_kprintf("power down now\r\n");
}

void _nrf24_report_enaa_reg(uint8_t data)
{
    if (!(data & 0x3F))
    {
        rt_kprintf("all pipe AA closed");
        return;
    }

    if (data & NRF24BITMASK_PIPE_0)
        rt_kprintf("pipe0 ");
    if (data & NRF24BITMASK_PIPE_1)
        rt_kprintf("pipe1 ");
    if (data & NRF24BITMASK_PIPE_2)
        rt_kprintf("pipe2 ");
    if (data & NRF24BITMASK_PIPE_3)
        rt_kprintf("pipe3 ");
    if (data & NRF24BITMASK_PIPE_4)
        rt_kprintf("pipe4 ");
    if (data & NRF24BITMASK_PIPE_5)
        rt_kprintf("pipe5 ");
    rt_kprintf("AA opened\r\n");
}
void _nrf24_report_enrxaddr_reg(uint8_t data)
{
    if (!(data & 0x3F))
    {
        rt_kprintf("all rx-pipe closed");
        return;
    }

    if (data & NRF24BITMASK_PIPE_0)
        rt_kprintf("rx-pipe0 ");
    if (data & NRF24BITMASK_PIPE_1)
        rt_kprintf("rx-pipe1 ");
    if (data & NRF24BITMASK_PIPE_2)
        rt_kprintf("rx-pipe2 ");
    if (data & NRF24BITMASK_PIPE_3)
        rt_kprintf("rx-pipe3 ");
    if (data & NRF24BITMASK_PIPE_4)
        rt_kprintf("rx-pipe4 ");
    if (data & NRF24BITMASK_PIPE_5)
        rt_kprintf("rx-pipe5 ");
    rt_kprintf(" opened\r\n");
}
void _nrf24_report_setupaw_reg(uint8_t data)
{
    rt_kprintf("rx/tx address field width: ");
    switch (data & 0x3)
    {
    case 0:
        rt_kprintf("illegal\r\n");
        break;
    case 1:
        rt_kprintf("3bytes\r\n");
        break;
    case 2:
        rt_kprintf("4bytes\r\n");
        break;
    case 3:
        rt_kprintf("5bytes\r\n");
        break;
    }
}
void _nrf24_report_setupretr_reg(uint8_t data)
{
    rt_kprintf("auto retransmit delay: %dus\r\n", (((data & 0xF0) >> 4) + 1) * 250);
    rt_kprintf("auto retransmit count: up to %d\r\n", (data & 0x0F));
}
void _nrf24_report_rfch_reg(uint8_t data)
{
    rt_kprintf("rf channel: %d\r\n", data & 0x7F);
}
void _nrf24_report_rfsetup_reg(uint8_t data)
{
    rt_kprintf("air data rate: ");
    if (data & NRF24BITMASK_RF_DR)
        rt_kprintf("2Mbps\r\n");
    else
        rt_kprintf("1Mbsp\r\n");

    rt_kprintf("rf power: ");
    switch ((data & NRF24BITMASK_RF_PWR) >> 1)
    {
    case 0:
        rt_kprintf("-18dBm\r\n");
        break;
    case 1:
        rt_kprintf("-12dBm\r\n");
        break;
    case 2:
        rt_kprintf("-6dBm\r\n");
        break;
    case 3:
        rt_kprintf("0dBm\r\n");
        break;
    }
}
void _nrf24_report_status_reg(uint8_t data)
{
    rt_kprintf("status: ");
    if (data & NRF24BITMASK_RX_DR)
        rt_kprintf("new rx data; ");
    if (data & NRF24BITMASK_TX_DS)
        rt_kprintf("last tx ok; ");
    if (data & NRF24BITMASK_MAX_RT)
        rt_kprintf("max-rt error exist; ");

    if (data & NRF24BITMASK_TX_FULL)
        rt_kprintf("tx-fifo is full; ");
    else
        rt_kprintf("tx-fifo is not full; ");

    data = (data & NRF24BITMASK_RX_P_NO) >> 1;
    if (data > 5)
    {
        if (data == 7)
            rt_kprintf("rx-fifo empty; ");
        else
            rt_kprintf("rx-fifo not used?; ");
    }
    else
    {
        rt_kprintf("rx-fifo pipe: %d; ", data);
    }

    rt_kprintf("\r\n");
}
void _nrf24_report_observetx_reg(uint8_t data)
{
    rt_kprintf("lost packets count: %d\r\n", (data & NRF24BITMASK_PLOS_CNT) >> 4);
    rt_kprintf("retransmitted packets count: %d\r\n", data & NRF24BITMASK_ARC_CNT);
}

void _nrf24_report_fifostatus_reg(uint8_t data)
{
    if (data & NRF24BITMASK_TX_REUSE)
        rt_kprintf("tx-reuse opened\r\n");

    if (data & NRF24BITMASK_TX_FULL2)
        rt_kprintf("tx-fifo full\r\n");
    else if (data & NRF24BITMASK_TX_EMPTY)
        rt_kprintf("tx-fifo empty\r\n");
    else
        rt_kprintf("tx-fifo has some data\r\n");

    if (data & NRF24BITMASK_RX_RXFULL)
        rt_kprintf("rx-fifo full\r\n");
    else if (data & NRF24BITMASK_RX_EMPTY)
        rt_kprintf("rx-fifo empty\r\n");
    else
        rt_kprintf("rx-fifo has some data\r\n");
}
void _nrf24_report_dynpd_reg(uint8_t data)
{
    rt_kprintf("dynamic payload length enabled (pipe): ");
    if (!(data & 0x3F))
    {
        rt_kprintf("none\r\n");
        return;
    }

    if (data & NRF24BITMASK_PIPE_0)
        rt_kprintf("pipe0; ");
    if (data & NRF24BITMASK_PIPE_1)
        rt_kprintf("pipe1; ");
    if (data & NRF24BITMASK_PIPE_2)
        rt_kprintf("pipe2; ");
    if (data & NRF24BITMASK_PIPE_3)
        rt_kprintf("pipe3; ");
    if (data & NRF24BITMASK_PIPE_4)
        rt_kprintf("pipe4; ");
    if (data & NRF24BITMASK_PIPE_5)
        rt_kprintf("pipe5; ");
    rt_kprintf("\r\n");
}
void _nrf24_report_feature_reg(uint8_t data)
{
    rt_kprintf("feature enabled conditions: ");
    if (data & NRF24BITMASK_EN_DPL)
        rt_kprintf("dynamic payload length; ");
    if (data & NRF24BITMASK_EN_ACK_PAY)
        rt_kprintf("payload with ack; ");
    if (data & NRF24BITMASK_EN_DYN_ACK)
        rt_kprintf("W_TX_PAYLOAD_NOACK command; ");

    rt_kprintf("\r\n");
}
void nrf24_report(void)
{
    _nrf24_report_config_reg(_read_reg(NRF24REG_CONFIG));
    _nrf24_report_enaa_reg(_read_reg(NRF24REG_EN_AA));
    _nrf24_report_enrxaddr_reg(_read_reg(NRF24REG_EN_RXADDR));
    _nrf24_report_setupaw_reg(_read_reg(NRF24REG_SETUP_AW));
    _nrf24_report_setupretr_reg(_read_reg(NRF24REG_SETUP_RETR));
    _nrf24_report_rfch_reg(_read_reg(NRF24REG_RF_CH));
    _nrf24_report_rfsetup_reg(_read_reg(NRF24REG_RF_SETUP));
    _nrf24_report_status_reg(_read_reg(NRF24REG_STATUS));
    _nrf24_report_observetx_reg(_read_reg(NRF24REG_OBSERVE_TX));

    _nrf24_report_fifostatus_reg(_read_reg(NRF24REG_FIFO_STATUS));
    _nrf24_report_dynpd_reg(_read_reg(NRF24REG_DYNPD));
    _nrf24_report_feature_reg(_read_reg(NRF24REG_FEATURE));
}

#endif // NRF24_USING_INFO_REPORT

#ifdef NRF24_USING_SHELL_CMD
#include <stdlib.h>

static void nrf24(int argc, char **argv)
{
    if (argc < 2)
    {
        rt_kprintf("USAGE: nrf24 [OPTION]\r\n");
        rt_kprintf("[OPTION]:\r\n");
        rt_kprintf("          init [spiDevice]\r\n");
        rt_kprintf("          portinit [spiDevice]\r\n");
        rt_kprintf("          readreg [regAddr]\r\n");
        rt_kprintf("          writereg [regAddr] [data]\r\n");
#ifdef NRF24_USING_INFO_REPORT
        rt_kprintf("          report\r\n");
#endif // NRF24_USING_INFO_REPORT
        return;
    }    
#ifdef NRF24_USING_INFO_REPORT
    if (!rt_strcmp(argv[1], "report"))
    {
        nrf24_report();
    }
#endif // NRF24_USING_INFO_REPORT

    if (argc < 3)
    {
        return;
    }
    if (!rt_strcmp(argv[1], "readreg"))
    {
        uint8_t reg = atoi(argv[2]);
        rt_kprintf("reg:0x%x val: 0x%x\n", reg, _read_reg(reg));
    }
    if (!rt_strcmp(argv[1], "init"))
    {
        nrf24_cfg_t nrf24;
        nrf24_default_param(&nrf24);
        nrf24.role = ROLE_PRX;
        nrf24.ud = argv[2];
        nrf24_init(&nrf24);
    }
    else if (!rt_strcmp(argv[1], "portinit"))
    {
        hal_nrf24l01_port.init(argv[2]);
    }

    if (argc < 4)
    {
        return;
    }
    if (!rt_strcmp(argv[1], "writereg"))
    {
        if (!rt_strcmp(argv[1], "writereg"))
        {
            uint8_t reg = atoi(argv[2]);
            uint8_t data = atoi(argv[3]);
            _write_reg(reg, data);
        }
    }
}
MSH_CMD_EXPORT(nrf24, nrf24l01);

#endif // NRF24_USING_SHELL_CMD
