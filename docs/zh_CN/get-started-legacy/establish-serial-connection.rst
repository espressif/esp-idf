与 ESP32 创建串口连接（传统 GNU Make 系统）
================================================
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

本章节主要介绍如何创建 ESP32 和 PC 之间的串口连接。


连接 ESP32 和 PC
--------------------

用 USB 线将 ESP32 开发板连接到 PC。如果设备驱动程序没有自动安装，请先确认 ESP32 开发板上的 USB 转串口芯片（或外部转串口适配器）型号，然后在网上搜索驱动程序，并进行手动安装。

以下是乐鑫 ESP32 等开发板驱动程序的链接：


.. csv-table::
    :header: 开发板, USB 驱动, 备注
    :widths: 40, 20, 40

    :ref:`ESP32-DevKitC <esp-modules-and-boards-esp32-devkitc>`,  `CP210x`_
    `ESP32-LyraT <https://www.espressif.com/en/products/hardware/esp32-lyrat>`_, `CP210x`_
    `ESP32-LyraTD-MSC <https://www.espressif.com/en/products/hardware/esp32-lyratd-msc>`_, `CP210x`_
    :ref:`ESP32-PICO-KIT <esp-modules-and-boards-esp32-pico-kit>`, `CP210x`_
    :ref:`ESP-WROVER-KIT <esp-modules-and-boards-esp-wrover-kit>`, `FTDI`_
    :ref:`ESP32 Demo 板 <esp-modules-and-boards-esp32-demo-board>`, `FTDI`_
    `ESP-Prog`_, `FTDI`_, 编程板 (w/o ESP32)
    `ESP32-MeshKit-Sense <https://github.com/espressif/esp-iot-solution/blob/master/documents/evaluation_boards/ESP32-MeshKit-Sense_guide_en.md#esp32-meshkit-sense-hardware-design-guidelines>`_, n/a, 搭配 `ESP-Prog`_ 使用
    `ESP32-Sense Kit <https://github.com/espressif/esp-iot-solution/blob/master/documents/evaluation_boards/esp32_sense_kit_guide_en.md#guide-for-esp32-sense-development-kit>`_, n/a, 搭配 `ESP-Prog`_ 使用

.. _CP210x: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers
.. _FTDI: http://www.ftdichip.com/Drivers/VCP.htm
.. _ESP-Prog: https://github.com/espressif/esp-iot-solution/blob/master/documents/evaluation_boards/ESP-Prog_guide_en.md#introduction-to-the-esp-prog-board

* CP210x: `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_
* FTDI: `FTDI Virtual COM Port Drivers <http://www.ftdichip.com/Drivers/VCP.htm>`_

以上驱动仅用作参考。正常情况下，当上述任一 ESP32 开发板与 PC 连接时，打包在操作系统中的驱动程序将会开始自动安装。


查看端口（Windows 用户）
---------------------------

检查 Windows 设备管理器中的 COM 端口列表。断开 ESP32 与 PC 的连接，然后重连。查看从列表中消失后再次出现的是哪个端口。

以下为 ESP32 DevKitC 和 ESP32 WROVER KIT 串口：

.. figure:: ../../_static/esp32-devkitc-in-device-manager.png
    :align: center
    :alt: Windows 设备管理器中 ESP32-DevKitC 的 USB 至 UART 桥
    :figclass: align-center

    Windows 设备管理器中 ESP32-DevKitC 的 USB 至 UART 桥

.. figure:: ../../_static/esp32-wrover-kit-in-device-manager.png
    :align: center
    :alt: Windows 设备管理器中 ESP-WROVER-KIT 的两个 USB 串行端口
    :figclass: align-center

    Windows 设备管理器中 ESP-WROVER-KIT 的两个 USB 串行端口


查看端口（Linux 和 MacOS 用户）
----------------------------------

查看 ESP32 开发板（或外部转串口适配器）的串口设备名称，请运行两次下述命令。首先，断开开发板或适配器，第一次运行命令；然后，连接开发板或适配器，第二次运行命令。其中，第二次运行命令后出现的端口即是 ESP32 对应的串口：

Linux ::

    ls /dev/tty*

MacOS ::

    ls /dev/cu.*

.. _linux-dialout-group-legacy:

将用户增加至 Linux 的 ``dialout`` 组
-------------------------------------

当前登录用户应当拥有通过 USB 对串口进行读写的权限。在多数 Linux 版本中，您都可以通过以下命令，将用户添加到 ``dialout`` 组，来获取读写权限::

    sudo usermod -a -G dialout $USER

在 Arch Linux 中，需要通过以下命令将用户添加到 ``uucp`` 组中::

    sudo usermod -a -G uucp $USER

请重新登录，确保串口读写权限可以生效。


确认串口连接
-------------

现在，请使用串口终端程序，验证串口连接是否可用。在本示例中，我们将使用 `PuTTY SSH Client <http://www.putty.org/>`_ 进行验证。该工具同时适用于 Windows 和 Linux 操作系统。您也可以使用其他串口程序，设置通信参数如下。

运行终端，配置串口：波特率 = 115200，数据位 = 8，停止位 = 1，奇偶校验 = N。在 Windows 和 Linux 中配置串口和通信参数（如 115200-8-1-N）的截图如下。注意，这里一定要选择在上述步骤中确认的串口进行配置。

.. figure:: ../../_static/putty-settings-windows.png
    :align: center
    :alt: 在 Windows 操作系统中使用 PuTTY 设置串口通信参数
    :figclass: align-center

    在 Windows 操作系统中使用 PuTTY 设置串口通信参数

.. figure:: ../../_static/putty-settings-linux.png
    :align: center
    :alt: 在 Linux 操作系统中使用 PuTTY 设置串口通信参数
    :figclass: align-center

    在 Linux 操作系统中使用 PuTTY 设置串口通信参数


然后，请在终端打开串口，查看 ESP32 是否有任何打印，具体打印内容取决于加载至 ESP32 的程序。ESP32 打印示例

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

如果打印出的日志是可读的（而不是乱码），则表示串口连接正常。此时，您可以继续进行安装，并最终将应用程序下载到 ESP32。

.. note::

    在某些串口接线方式下，在 ESP32 启动并开始打印串口日志前，需要在终端程序中禁用串口 RTS ＆ DTR 引脚。该问题仅存在于将 RTS ＆ DTR 引脚直接连接到 EN ＆ GPIO0 引脚上的情况，绝大多数开发板（包括乐鑫所有的开发板）都没有这个问题。更多详细信息，参见 `esptool 文档`_。

.. note::

    验证完成后，请关闭串口终端。我们将在后续步骤中向 ESP32 下载新的应用程序，如果未关闭终端，则该应用程序将无法访问串口。

如果您是在安装 ESP32 软件的过程中从 :ref:`get-started-connect-legacy` 章节跳转至此，请返回 :ref:`get-started-configure-legacy` 章节。


.. _esptool 文档: https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection#automatic-bootloader

