# DIY projects base on RT-Thread

一些基于 RT-Thread 开发的 DIY 项目

## DIY1——基于 RT-Thread 的分布式无线温度监控系统

### 第一周任务

- 正确读取 ds18b20 温度数据
- 了解 RT-Thread 的 sensor 框架，并将 ds18b20 对接到 sensor 框架上
- 了解线程的使用，创建一个线程，在线程中读取温度数据，并通过FinSH控制台实时打印出来

### 第二周任务（进行中）

- 通过 ENV 工具获取 nrf24l01软件包，并加载到 MDK 工程里面
- 了解多线程间的通信，了解 IPC 中邮箱和消息队列的特性，并能灵活使用，实现 ds18b20 线程与 nrf24l01 线程之间的数据通信
- 修改 nrf24l01 软件包，实现多点通信功能

### 第三周任务

- 了解 RT-Thread 文件系统，在接收节点中使用文件系统，存放来自发送节点发送过来的数据

### 第四周任务

- 接收节点根据上位机数据帧格式，通过串口发送温度数据给上位机，上位机能正确显示温度曲线
- 了解 RT-Thread 的 OneNet 云软件包、AT 组件，并使用它们实现将接收节点的数据通过 esp8266 wifi 模块上传至 OneNet 云端，云端要求能简单制作小应用实现实时温度远程监控

