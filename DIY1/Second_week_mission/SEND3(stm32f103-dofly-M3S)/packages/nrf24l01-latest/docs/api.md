# API 手册

_**返回值和参数等信息请直接查看源码，不在此赘述**_

## 初始化

***

```c
void nrf24_default_param(nrf24_cfg_t *pt);
```

功能：设置默认的参数

***

```c
int nrf24_init(nrf24_cfg_t *pt);
```

功能：依配置初始化底层硬件和 nRF24L01

## 通信

***

```c
int nrf24_ptx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen);
```

所属角色：PTX

功能：先发送数据，然后（若有数据的话）读取接收到的数据

***

```c
int nrf24_prx_cycle(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen);
```

所属角色：PRX

功能：先接收数据，若接收成功且 tlen 处于1到32之间，则写入下次接收数据时要发(回)送的数据

***

```c
void nrf24_prx_write_txbuffer(const uint8_t *pb, uint8_t len);
```

所属角色：PRX

功能：写入下次接收数据时要发(回)送的数据

注意：1 <= len <= 32

***

```c
int nrf24_irq_ptx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen, void(*waitirq)(void));
```

所属角色：PTX

功能：先发送数据，然后（若有数据的话）读取接收到的数据

***

```c
int nrf24_irq_prx_run(uint8_t *pb_rx, const uint8_t *pb_tx, uint8_t tlen, void(*waitirq)(void));
```

所属角色：PRX

功能：先接收数据，若接收成功且 tlen 处于1到32之间，则写入下次接收数据时要发(回)送的数据

## 其它

***

```c
uint16_t nrf24_get_errcnt(void);
```

功能：获取通信失败次数，同时会清零错误计数器

***

```c
void nrf24_power_up(void);
```

功能：设置 nRF24L01 上电，以便正常工作

***

```c
void nrf24_power_down(void);
```

功能：设置 nRF24L01 掉电，进入最低功耗模式（无法通信）
