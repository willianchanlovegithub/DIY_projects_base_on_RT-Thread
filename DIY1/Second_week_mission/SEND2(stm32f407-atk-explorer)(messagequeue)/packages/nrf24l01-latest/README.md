# nRF24L01

## 1、介绍

这是一个 RT-Thread 的软件包，该软件包提供了 nRF24L01 模块的驱动。

nRF24L01 是由 NORDIC 生产的工作在 2.4GHz~2.5GHz 的 ISM 频段的单片无线收发器芯片。

> 更多关于 nRF24L01 的信息请参阅 [_features.md_](/docs/features.md) 或 _数据手册_

### 1.1 目录结构

| 名称 | 说明 |
| ---- | ---- |
| docs  | 文档 |
| examples | 有关使用该驱动的样例代码 |
| src  | 源代码目录 |

### 1.2 许可证

nRF24L01 package 遵循 Apache license v2.0 许可，详见 `LICENSE` 文件。

### 1.3 依赖

- RT-Thread PIN 设备
- RT-Thread SPI 设备

## 2、获取软件包

使用 nRF24L01 package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers --->
        [*] nRF24L01: Single-chip 2.4GHz wireless transceiver.
```

本驱动同时提供了四个样例程序来使用，使用样例程序需要在包管理器中选中(四选一)，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers 
        [*] nRF24L01: Single-chip 2.4GHz wireless transceiver.
            [*] Using sample --->
                Sample (xxx)
```

_**注意：要使样例程序正常运行还需要调整 examples/sample.h 文件中的宏定义为实际的硬件连接**_

最后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3、使用 nRF24L01

_**使用前务必查看 [user_guide.md](/docs/user_guide.md)**_

1. 首先进行初始化，假设为轮询 PRX，示例如下:

```c
    /*该示例仅为最简配置展示，实际使用中应结合具体情况调整*/
    #include "nrf24l01.h"

    #define NRF24L01_CE_PIN         GET_PIN(A, 3)
    #define NRF24L01_SPI_DEVICE     "spi10"

    struct hal_nrf24l01_port_cfg halcfg;
    nrf24_cfg_t cfg;

    nrf24_default_param(&cfg);
    // hardware
    halcfg.ce_pin = NRF24L01_CE_PIN;
    halcfg.spi_device_name = NRF24L01_SPI_DEVICE;
    cfg.ud = &halcfg;
    // radio
    cfg.role = ROLE_PRX;    // PRX
    cfg.use_irq = 0;        // False
    nrf24_init(&cfg);
```

2. 然后调用对应的 API，假设为轮询 PRX，示例如下:

```c
    /*该示例仅供参考*/
    uint8_t rbuf[32 + 1];
    uint8_t tbuf[32];
    int rlen;
    uint32_t cnt = 0;
    
    while (1) {
        // polling cycle
        rt_thread_mdelay(5);

        rlen = nrf24_prx_cycle(rbuf, tbuf, rt_strlen((char *)tbuf));
        if (rlen > 0) {       // received data (also indicating that the previous frame of data was sent complete)
            rbuf[rlen] = '\0';
            rt_kputs((char *)rbuf);
            rt_sprintf((char *)tbuf, "i-am-PRX:%dth\r\n", cnt);
            cnt++;
        }
        else {  // no data
            ;
        }
    }
```

更多的示例可以参考 /examples 下的样例程序

## 4、注意事项

无

## 5、联系方式

- 维护：sogwyms@gmail.com
- 主页：https://github.com/sogwms/nrf24l01
