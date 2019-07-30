# 使用前必读

## 必要了解（关于 nRF24L01）

- nRF24L01 是一个无线通信模块，其通信方式属于**半双工**

- nRF24L01 通信以包为单位，一个包最多携带 32 字节的数据

- nRF24L01 中有增强性突发模式，该模式可实现自动应答、重传等机制而无需 MCU 的干预. 在该模式下有两个角色：Primary Transmitter（PTX）和 Primary Receiver（PRX）. _（本驱动向上层提供的API即基于此模式）_

- 要使 nRF24L01 模块间通信必须匹配关键设置：速率、地址、频率(频道)、CRC 等

### PTX、PRX 通信过程**简要说明**

1. PTX 方发送数据
2. PRX 方接收到数据
3. PRX 方回应一个 ACK，该 ACK 可以携带数据（发送给 PTX 方的）
4. PTX 方接收到 ACK (可能包含 PRX 发送来的数据)

从上述通信过程可以看出：_PTX 属于主动方，PRX 属于被动方。如果 PTX 方不发送数据则也无法接收数据，同时 PRX 方即不能发送也无法接收到数据。_

## 驱动 (配置、API)

- 当前驱动仅使用通道0.
- 目前不支持多实例

### 核心

使用前必须配置好，配置项有两处：

1. 底层硬件

```c
struct hal_nrf24l01_port_cfg{
    int ce_pin;
    char *spi_device_name;
};
```

- ce_pin 为 PIN设备下的引脚号
- spi_device_name 为 SPI设备的名称 （_**此意味着使用驱动前必须挂载好SPI设备**_）

2. Radio

**强烈建议**先调用函数 void nrf24_default_param(nrf24_cfg_t *pt); 设置默认值，然后根据需要重设对应项。当然此不是必须的，你也可以手动设置所有项

```c
typedef struct
{
    nrf24_esb_t esb;

    nrf24_role_et role;
    nrf24_power_et power;
    nrf24_adr_et adr;
    nrf24_crc_et crc;
    uint8_t address[5];
    uint8_t channel;    //range: 0 ~ 125

    uint8_t use_irq;
    void *ud;
} nrf24_cfg_t;
```

- esb 为重传机制相关、如不了解忽略即可
- role：角色选择 (PTX、PRX)
- power：功率选择
- adr：空中速率(1Mbps、2Mbps)
- crc：crc长度选择(1byte、2bytes)
- address：5字节地址 (address[0]为最低字节)
- channel：频率选择(0 : 125 对应 2.4GHz : 2.525GHz)
- use_irq：是否使用中断
- ud：用来传递对底层的配置 struct hal_nrf24l01_port_cfg

在配置并初始化完成后即可调用 API 进行通信了。相关的API有四个：

- 轮询式的 PTX <-> nrf24_ptx_run
- 中断式的 PTX <-> nrf24_irq_ptx_run
- 轮询式的 PRX <-> nrf24_prx_cycle
- 中断式的 PRX <-> nrf24_irq_prx_run

对于一个 nRF24L01 模块只能从中选取一个与配置（role：PTX 或 PRX）符合的使用. 详细的关于如何使用该 API 建议查看 [API 说明](/docs/api.md) 或者 [样例程序](/examples)

### 辅助功能

驱动提供了两项辅助调试的功能，默认关闭，可在 `menuconfig` 中打开：

```
RT-Thread online packages  --->
    peripheral libraries and drivers  --->
        [*] nRF24L01: Single-chip 2.4GHz wireless transceiver.  --->
            [*] Using info report
            [*] Using shell cmd
```

- Using info report

在 `menuconfig` 里选中该选项会使能宏 `NRF24_USING_INFO_REPORT`，使能该宏会启用 void nrf24_report(void); 该函数会读取 nRF24L01 几乎所有的寄存器，然后进行解读并输出解读信息

- Using shell cmd

在 `menuconfig` 里选中该选项会使能宏 `NRF24_USING_SHELL_CMD`，使能该宏会添加一 msh 命令 nrf24，使用该命令可以对 nRF24L01 进行初始化、读写寄存器等

