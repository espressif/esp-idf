与 ESP32 创建串口连接 (传统 GNU Make)
=======================================
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

本章节介绍如何在 ESP32 和 PC 之间建立串口连接。

连接 ESP32 和 PC
--------------------

用 USB 线将 ESP32 开发板连接到 PC。如果设备驱动程序没有自动安装，确认 ESP32 开发板上的 USB 转串口芯片（或外部串口适配器）型号，在网上搜索驱动程序并进行安装。

以下是乐鑫 ESP32 开发板驱动程序的链接：

* ESP32-PICO-KIT 和 ESP32-DevKitC - `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_

* ESP32-WROVER-KIT 和 ESP32 Demo Board - `FTDI Virtual COM Port Drivers <http://www.ftdichip.com/Drivers/VCP.htm>`_

以上驱动仅用于参考。当您将上述 ESP32 开发板与 PC 连接时，对应驱动程序应该已经被打包在操作系统中并自动安装。

在 Windows 上查看端口
---------------------

检查 Windows 设备管理器中的 COM 端口列表。断开 ESP32 与 PC 的连接，然后重新连接，查看哪个端口从列表中消失，然后再次显示。

以下为 ESP32 DevKitC 和 ESP32 WROVER KIT 串口：

.. figure:: ../../_static/esp32-devkitc-in-device-manager.png
    :align: center
    :alt: USB to UART bridge of ESP32-DevKitC in Windows Device Manager
    :figclass: align-center

    设备管理器中 ESP32-DevKitC 的 USB 串口转换器

.. figure:: ../../_static/esp32-wrover-kit-in-device-manager.png
    :align: center
    :alt: Two USB Serial Ports of ESP-WROVER-KIT in Windows Device Manager
    :figclass: align-center

    Windows 设备管理器中的两个 USB-WROVER-KIT 串行端口

在 Linux 和 MacOS 上检查串口
-----------------------------

要查看 ESP32 开发板（或外部串口适配器）的串口设备名称，运行以下命令两次，第一次先拔下开发板或适配器，第二次插入开发板或适配器之后再运行命令，第二次运行指令后出现的端口即是 ESP32 对应的串口：

Linux ::

    ls /dev/tty*

MacOS ::

    ls /dev/cu.*


.. _linux-dialout-group-legacy:

在 Linux 添加用户到 ``dialout`` 
-----------------------------------

当前登录用户可以通过 USB 读写串口。在大多数 Linux 发行版中，这是通过以下命令将用户添加到 ``dialout`` 组来完成的::

    sudo usermod -a -G dialout $USER

在 Arch Linux 中，需要通过以下命令将用户添加到 ``uucp`` 组中::

    sudo usermod -a -G uucp $USER

重新登录以确保串行端口的读写权限被启用。


确认串口连接
------------------------

现在验证串口连接是可用的。您可以使用串口终端程序来执行此操作。在这个例子中，我们将使用 `PuTTY SSH Client <http://www.putty.org/>`_ ，它有 Windows 和 Linux 等平台的版本。您也可以使用其他串口程序并设置如下的通信参数。

运行终端，设置串口：波特率 = 115200，数据位 = 8，停止位 = 1，奇偶校验 = N。以下是设置串口和在 Windows 和 Linux 上传输参数（如 115200-8-1-N）的一些截屏示例。注意选择上述步骤中确认的串口进行设置。

.. figure:: ../../_static/putty-settings-windows.png
    :align: center
    :alt: Setting Serial Communication in PuTTY on Windows
    :figclass: align-center

    在 Windows 上的 PuTTY 设置串口传输。

.. figure:: ../../_static/putty-settings-linux.png
    :align: center
    :alt: Setting Serial Communication in PuTTY on Linux
    :figclass: align-center

    在 Linux 上的 PuTTY 设置串口传输。

在终端打开串口，检查是否有任何打印出来的日志。日志内容取决于加载到 ESP32 的应用程序。下图为 ESP32 的一个示例日志。

.. highlight:: none

::

    ets Jun  8 2016 00:22:57

    rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    ets Jun  8 2016 00:22:57

    rst:0x7 (TG0WDT_SYS_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    configsip: 0, SPIWP:0x00
    clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
    mode:DIO, clock div:2
    load:0x3fff0008,len:8
    load:0x3fff0010,len:3464
    load:0x40078000,len:7828
    load:0x40080000,len:252
    entry 0x40080034
    I (44) boot: ESP-IDF v2.0-rc1-401-gf9fba35 2nd stage bootloader
    I (45) boot: compile time 18:48:10

    ...

如果您看到一些清晰的日志，则表示串行连接正常，您可以继续安装，最后将应用程序上载到 ESP32。

.. note::

   对于某些串口接线配置，在 ESP32 启动并产生串行输出之前，需要在终端程序中禁用串行 RTS ＆ DTR 引脚。这取决于串口适配器硬件本身，大多数开发板（包括所有乐鑫开发板）没有这个问题。此问题仅存在于将 RTS ＆ DTR 引脚直接连接到 EN ＆ GPIO0 引脚上的情况。更多详细信息，参见 `esptool documentation`_。

.. note::

   验证通讯正常后关闭串口终端。下一步，我们将使用另一个应用程序来上传 ESP32。此应用程序在终端打开时将无法访问串口。

如您在安装用于 ESP32 开发的软件时，从 :ref:`get-started-connect-legacy` 小节跳转到了这里，请返回到 :ref:`get-started-configure-legacy` 小节继续阅读。

.. _esptool documentation: https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection#automatic-bootloader

