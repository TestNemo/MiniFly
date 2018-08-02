# STM32F103 RT-Thread MiniFly

## 1. 简介

RT-Thread MiniFly 是一款开源飞控固件，基于正点原子MiniFly
STM32F103 是由意法半导体推出的Cortex-M3内核的高性能单片机
包括如下硬件特性：

| 硬件 | 描述 |
| -- | -- |
|芯片型号| STM32F10x全系列 |
|CPU| Cortex-M3 |
|主频| 48MHz-72MHz |

## 2. 编译说明


| 环境         | 说明                                                         |
| ------------ | ------------------------------------------------------------ |
| PC操作系统   | Linux/MacOS/Windows                                          |
| 编译器       | arm-none-eabi-gcc version 6.3.1 20170620 (release)/armcc/iar |
| 构建工具     | scons/mdk5/iar                                               |
| 依赖软件环境 | Env工具/(MDK或IAR或arm-none-eabi-gcc)/git/调试器驱动         |

1) 下载源码

```bash
    git clone https://github.com/RT-Thread/rt-thread.git
```

2) 配置工程并准备env

（Linux/Mac）

```bash
    cd rt-thread/bsp/stm32f10x
    scons --menuconfig
    source ~/.env/env.sh
    pkgs --upgrade
```

（Windows）

>在[RT-Thread官网][1]下载ENV工具包

3) 配置芯片型号

（Linux/Mac）

```bash
    scons --menuconfig
```

（Windows(ENV环境中)）

```bash
    menuconfig
```

在menuconfig页面配置并选择对应的芯片型号，若开发环境为MDK/IAR，则需要生成工程

4) 生成工程(Mac/Linux下请跳过此步骤)

（Windows IAR）

```bash
    SET RTT_CC=iar
    scons --target=iar -s
```

（Windows MDK5）

```bash
    scons --target=mdk5 -s
```

（Windows MDK4）

```bash
    scons --target=mdk4 -s
```


5) 编译

使用MDK或IAR请参见对应教程

（Windows arm-none-eabi-gcc）
使用以下指令设置gcc路径

```bash
    SET RTT_EXEC_PATH=[GCC路径]
```

（Linux/Mac arm-none-eabi-gcc）
使用以下指令设置gcc路径

```bash
    export RTT_EXEC_PATH=[GCC路径]
```

编译（WindowsLinux/Mac arm-none-eabi-gcc）

```bash
    scons -j4
```

出现下列信息即为编译成功

```bash
    LINK rtthread-stm32.elf
    arm-none-eabi-objcopy -O binary rtthread-stm32.elf rtthread.bin
    arm-none-eabi-size rtthread-stm32.elf
    text    data     bss     dec     hex filename
    46356     424    1520   48300    bcac rtthread-stm32.elf
    scons: done building targets.
```


如果编译正确无误，会产生rtthread-stm32.elf、rtthread.bin文件。其中rtthread.bin为二进制固件

## 3. 烧写及执行

烧写可以使用仿真器 ISP等多种方式 此处不再赘述

### 3.1 运行结果

如果编译 & 烧写无误，会在串口2*上看到RT-Thread的启动logo信息：

```bash
 \ | /
- RT -     Thread Operating System
 / | \     3.0.4 build May 15 2018
 2006 - 2018 Copyright by rt-thread team
msh />
```

*默认串口


## 4. 驱动支持情况及计划

| 驱动       | 支持情况 | 备注                         |
| ---------- | :------: | :--------------------------: |
| UART       | 支持     | UART1/2/3/4                    |
| GPIO       | 支持     | / |


### 4.1 IO在板级支持包中的映射情况

| IO号 | 板级包中的定义 |
| -- | -- |
| PA9 | USART1 TX |
| PA10 | USART1 RX |
| PA2 | USART2 TX |
| PA3 | USART2 RX |
| PB10 | USART3 TX |
| PB11 | USART3 RX |
| PC10 | UART4 TX |
| PC11 | UART4 RX |


## 6. 联系人信息

维护人:
[TestNemo][4] < [namao1985@163.com][5] >

  [1]: https://www.rt-thread.org/page/download.html
  [4]: https://github.com/TestNemo
  [5]: mailto:namao1985@163.com
