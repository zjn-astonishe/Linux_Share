- [1. 前言导引](#1-前言导引)
- [2. Linux中I2C驱动架构总览](#2-linux中i2c驱动架构总览)
  - [2.1. I2C核心](#21-i2c核心)
  - [2.2. I2C总线驱动](#22-i2c总线驱动)
  - [2.3. I2C设备驱动](#23-i2c设备驱动)
- [3. Linux中I2C驱动框架代码结构总览](#3-linux中i2c驱动框架代码结构总览)
  - [3.1. 关键文件路径](#31-关键文件路径)
    - [3.1.1. 核心层和总线驱动](#311-核心层和总线驱动)
    - [3.1.2. 设备驱动(以gt1x触摸屏为例)](#312-设备驱动以gt1x触摸屏为例)
    - [3.1.3. 设备树文件(以TB-RK3568X为例)](#313-设备树文件以tb-rk3568x为例)
    - [3.1.4. 重要的头文件](#314-重要的头文件)
  - [3.2. 关键数据结构](#32-关键数据结构)
    - [3.2.1. 定义](#321-定义)
    - [3.2.2. 关联](#322-关联)
- [4. Linux中I2C驱动框架代码流程分析](#4-linux中i2c驱动框架代码流程分析)
- [5. 结语](#5-结语)
- [6. 参考资料](#6-参考资料)

# 1. 前言导引

I2C总线是Philips公司开发的一种简单、双向二线制同步串行总线，只需要两根线即可传送信息。I2C结合了SPI和UART的优点，可以像SPI一样将多个从机连接到单个主机，也可以使用多个主机控制一个或多个从机。

由于本文重点是梳理Linux中I2C驱动框架，所以对于I2C的通信协议、物理总线等基础内容不做过多赘述，读者可自行百度了解。

接下来，本文将从I2C驱动架构总览、I2C驱动框架代码结构总览、I2C驱动框架代码流程分析三个部分对I2C驱动框架进行梳理。

- I2C驱动架构总览主要介绍Linux中I2C驱动的整体架构，讲解各个组成部分的功能和相互联系。
- I2C驱动框架代码结构总览主要介绍Linux中I2C驱动框架的重要文件路径和关键数据结构。
- I2C驱动框架代码流程分析主要介绍Linux中I2C驱动框架的实现细节。

# 2. Linux中I2C驱动架构总览

下图展示了Linux中I2C驱动架构的基本架构：

![Linux中I2C驱动架构](https://github.com/zjn-astonishe/Linux_Share/blob/master/Image/image/Linux%E8%AE%BE%E5%A4%87%E9%A9%B1%E5%8A%A8%E5%BC%80%E5%8F%91%E8%AF%A6%E8%A7%A3/Linux%E5%86%85%E6%A0%B8%E9%87%8CI2C%E5%AD%90%E7%B3%BB%E7%BB%9F%E8%BD%AF%E4%BB%B6%E6%A1%86%E6%9E%B6%E5%9B%BE.png?raw=true)

对于南向开发而言，只需关注架构的内核空间部分。在《Linux设备驱动开发详解》一书第15章《Linux I2C核心、总线与设备驱动》中，将Linux内核里的I2C子系统分为核心、总线驱动和设备驱动三部分。

## 2.1. I2C核心

I2C核心提供了I2C总线驱动和I2C设备驱动注册和注销的方法，I2C通信方法上层的与具体适配器无关的代码，以及探测设备、检测设备地址的上层代码。I2C总线驱动和设备驱动之间依赖于I2C核心作为纽带。

## 2.2. I2C总线驱动

I2C总线驱动是对soc中I2C控制器的软件实现(i2c_algorithm)。提供I2C控制器与从设备间完成数据通信的能力(i2c_adapter)。对应软件架构图中硬件抽象层部分和硬件实现控制层。

## 2.3. I2C设备驱动

I2C设备驱动(客户驱动)是对I2C从设备的软件实现。对应软件架构图中的驱动层。

# 3. Linux中I2C驱动框架代码结构总览

## 3.1. 关键文件路径
### 3.1.1. 核心层和总线驱动

<pre>
~/Documents/OpenHarmony/out/kernel/src_tmp/linux-5.10/drivers/i2c/
├── algos               // i2c_algorithm相关，通信算法
├── busses              // i2c_adapter相关，已经编写好的各种向i2c核心层注册的适配器，与I2C总线驱动相关
├── muxes
├── i2c-boardinfo.c     // i2c静态声明i2c设备的文件，设备树出现后已经不太使用。
├── i2c-core-acpi.c     // 以下i2c-core-*.c对应老版本的i2c-core.c，对应I2C核心，由内核开发者实现的，与硬件无关的代码。主要为其他各部分提供操作接口，在其内部通过结构体里面的函数指针调用硬件相关信息，即结构体里面函数指针的函数在设备加载的时候初始化。
├── i2c-core-base.c
├── i2c-core.h
├── i2c-core-of.c
├── i2c-core-slave.c
├── i2c-core-smbus.c
├── i2c-dev.c           // 为i2c_adapter实现了设备文件功能，只是提供了通用的read()、write()和ioctl()等接口，供应用层直接控制I2C控制器访问I2C设备的存储空间或寄存器。
├── i2c-mux.c
├── i2c-slave-eeprom.c
├── i2c-slave-testunit.c
├── i2c-smbus.c         // 实现smbus协议的扩展文件
├── i2c-stub.c
├── Kconfig
└── Makefile
</pre>

### 3.1.2. 设备驱动(以gt1x触摸屏为例)
<pre>
~/Documents/OpenHarmony/out/kernel/src_tmp/linux-5.10/drivers/input/touchscreen/gt1x/
├── gt1x.c              // gt1x触摸屏设备驱动主要代码位置
├── gt1x_cfg.h
├── gt1x_extents.c
├── gt1x_firmware.h
├── gt1x_generic.c      // gt1x触摸屏设备驱动主要代码位置
├── gt1x_generic.h
├── gt1x.h
├── gt1x_tools.c
├── gt1x_update.c
├── GT5688_Config_20170713_1080_1920.cfg
└── Makefile
</pre>

### 3.1.3. 设备树文件(以TB-RK3568X为例)

<pre>
~/Documents/OpenHarmony/out/kernel/src_tmp/linux-5.10/arch/arm64/boot/dts/rockchip/
├── ...
├── rk3568-toybrick-x0-linux.dts
├── rk3568.dtsi
├── rk3568-linux.dtsi
├── rk3568-toybrick-mipi-tx0-beiqicloud.dtsi
├── ...
</pre>

### 3.1.4. 重要的头文件

<pre>
~/Documents/OpenHarmony/out/kernel/src_tmp/linux-5.10/include/linux/
├── ...
├── device.h
├── ...
├── i2c.h
├── of.h
├── of_device.h
├── ...
</pre>

## 3.2. 关键数据结构

### 3.2.1. 定义

数据结构|文件路径|描述
:-:|:-:|:-:|
`i2c_adapter`|`/drivers/i2c/busses/i2c-core-base.c`|用于识别物理I2Cs总线以及访问该总线所需的访问算法
`i2c_algorithm`|`/include/linux/i2c.h`|I2C通信方法
`i2c_msg`|`/usr/include/linux/i2c.h`|i2c_algorithm中通信函数的基本单位
`i2c_driver`|`/include/linux/i2c.h`|I2C设备驱动
`i2c_client`|`/include/linux/i2c.h`|I2C从机设备


### 3.2.2. 关联

下图解释了上述结构体之间的关联。在I2C设备驱动注册的过程中，会调用i2c_driver的匹配函数`match()`与i2c_client(在i2c_adapter注册过程中解析设备树信息生成)进行匹配，匹配成功则调用`probe()`函数完成驱动注册的收尾工作。设备驱动可通过i2c_adapter中提供的i2c_algorithm，构造i2c_msg与I2C设备通信，完成控制操作。

![关联](https://github.com/zjn-astonishe/Linux_Share/blob/master/Image/image/Linux%E8%AE%BE%E5%A4%87%E9%A9%B1%E5%8A%A8%E5%BC%80%E5%8F%91%E8%AF%A6%E8%A7%A3/%E5%85%B3%E9%94%AE%E6%95%B0%E6%8D%AE%E7%BB%93%E6%9E%84%E4%B9%8B%E9%97%B4%E5%85%B3%E8%81%94.png?raw=true)

# 4. Linux中I2C驱动框架代码流程分析

# 5. 结语

# 6. 参考资料