# 第一周任务清单

- 正确读取 ds18b20 温度数据
- 了解 RT-Thread 的 sensor 框架，并将 ds18b20 对接到 sensor 框架上
- 了解线程的使用，创建一个线程，在线程中读取温度数据，并通过FinSH控制台实时打印出来

## 备注

- `stm32f407-atk-explorer` 文件夹里面的工程就是第一周的任务

- 是基于正点原子的探索者开发板的 BSP 制作的，手头有这款开发板的可以直接打开工程编译下载即可。使用其他开发板的，如果已经有 BSP 支持的，可以直接使用对应的 BSP 来做，注意需要在 ENV 工具中开启 sensor 框架，并将 `stm32f407-atk-explorer\board\ports` 文件夹中的 `sensor_dallas_ds18b20.c` 文件引入到自己的工程当中即可。其余开发板，可以参考该 Demo 程序，照葫芦画瓢。

## 结果

FinSH控制台输出采集到的温度数据信息，如下图所示：

![board](figures/result.jpg)