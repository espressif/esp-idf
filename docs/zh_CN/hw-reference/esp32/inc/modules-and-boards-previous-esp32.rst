.. _esp-modules-and-boards-previous_esp32:

模组
====

到目前为止没有模组停止更新或停产。


开发板
======

要查看最新的开发板，请点击 :ref:`esp-modules-and-boards`。

.. _esp-modules-and-boards-esp32-pico-kit-v4:

ESP32-PICO-KIT V4
-----------------

乐鑫最小的 ESP32 系列开发板，集成所有直接连接到 PC 的 USB 端口必需组件，采用插入迷你面包板的排针。该开发板搭载集成了 4 MB flash、晶振、滤波电容器和射频匹配网络的 ESP32-PICO-D4 模组，仅需在 20 x 52 毫米的 PCB 板上安装少量外部组件，包括天线、LDO、USB 至 UART 桥接器、复位按键和下载按键。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v4.jpeg
    :align: center
    :alt: ESP32-PICO-KIT V4
    :width: 50%

    ESP32-PICO-KIT V4

和 ESP32-PICO-KIT V3 相比，V4 版本改进了布局，将引出的管脚数量从 20 个减少至 17 个，可以更好地与迷你面包板匹配。

相关文档
^^^^^^^^

* :doc:`esp32/get-started-pico-kit`
* `ESP32-PICO-KIT V4 原理图 <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v4_schematic.pdf>`_ (PDF)
* `ESP32-PICO-D4 技术规格书 <http://espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_cn.pdf>`_ (PDF)

.. _esp-modules-and-boards-esp32-pico-kit-v3:

ESP32-PICO-KIT V3
-----------------

第一款搭载乐鑫 ESP32-PICO-D4 模组的迷你开发板。该开发板包含用于烧录和调试的 USB 端口，以及两排可以插入面包板的排针，共计 20 个。ESP32-PICO-D4 模组尺寸为 7 x 7 毫米，体积小，除了双核 CPU 之外还集成了 4 MB flash、晶振和天线，仅需再安装几个外部组件。模组和所有组件最终装配到 20 x 52 毫米的 PCB 板上，构成开发板。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v3.jpeg
    :align: center
    :alt: ESP32-PICO-KIT V3
    :width: 50%

    ESP32-PICO-KIT V3

相关文档
^^^^^^^^

* :doc:`esp32/get-started-pico-kit-v3`
* `ESP32-PICO-KIT V3 原理图 <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v3_schematic.pdf>`_ (PDF)
* `ESP32-PICO-D4 技术规格书 <http://espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_cn.pdf>`_ (PDF)


.. _esp-modules-and-boards-esp32-devkitc-v2:

ESP32 Core Board V2 / ESP32 DevKitC
-----------------------------------

搭载了 ESP-WROOM-32 模组的小型便捷开发板，板上管脚引出，外围器件少。该款开发板包含 USB 转串口（也是开发板的电源接口）、复位按键和下载按键。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-core-board-v2.png
    :align: center
    :alt: ESP32 Core Board V2 / ESP32 DevKitC
    :width: 50%

    ESP32 Core Board V2 / ESP32 DevKitC

相关文档
^^^^^^^^

* :doc:`esp32/get-started-devkitc-v2`
* `ESP32 DevKitC V2 原理图 <https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf>`__ (PDF)
* `CP210x USB 至 UART 桥接器 VCP 驱动 <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_

.. _esp-modules-and-boards-esp-wrover-kit-v3:

ESP-WROVER-KIT V3
-----------------

ESP-WROVER-KIT V3 开发板上装有用于编程的 USB 转双串口和用于调试的 JTAG 接口，由 USB 接口或 5 毫米标准电源插座供电。电源通过跳线选择，有单独的开关。该款开发板上还有 MicroSD 卡槽、3.2” SPI LCD 屏幕、连接摄像头的专用排针、用于诊断的 RGB 二极管和内部 RTC 在低功耗模式下可操作的 32.768 kHz 晶振。

和此前版本的 ESP-WROVER-KIT 开发板一样，V3 版本可以搭载 :ref:`esp-modules-and-boards-esp32-wroom-32` 或 :ref:`esp-modules-and-boards-esp32-wrover`。

这是第一款默认搭载 :ref:`esp-modules-and-boards-esp32-wrover` 的 ESP-WROVER-KIT。V3 版本还针对自举引脚的信号调节和联锁做了少量设计变动。此外，V3 版本还在 WROVER/WROOM 模组和 VDD33 之间加了一个 0 欧电阻 (R166)，可以在测量电流时直接移除或替换成分流电阻，便于分析 ESP32 不同操作模式下的功耗。变动的设计在原理图中用绿色圈出。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v3.jpg
    :align: center
    :alt: ESP-WROVER-KIT V3
    :width: 90%

    ESP-WROVER-KIT V3

摄像头排针变为摄像头排母，开发板组焊层为哑光黑色。图片中的开发板搭载的是 :ref:`esp-modules-and-boards-esp32-wrover`。

相关文档
^^^^^^^^

* :doc:`esp32/get-started-wrover-kit-v3`
* `ESP-WROVER-KIT V3 原理图 <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-3.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI 虚拟 COM 端口驱动`_

.. _esp-modules-and-boards-esp-wrover-kit-v2:

ESP-WROVER-KIT V2
-----------------

ESP32 DevKitJ V1 的升级版，根据 DevKitJ 使用中发现的问题优化了设计，比如增加对 SD 卡的支持。该款开发板默认搭载 ESP-WROOM-32 模组。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v2.jpg
    :align: center
    :alt: ESP-WROVER-KIT V2
    :width: 90%

    ESP-WROVER-KIT V2

和此前的版本有所不同，这一版开发板采用黑色抛光和摄像头排针。

相关文档
^^^^^^^^

* :doc:`esp32/get-started-wrover-kit-v2`
* `ESP-WROVER-KIT V2 原理图 <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-2.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI 虚拟 COM 端口驱动`_

.. _esp-modules-and-boards-esp-wrover-kit-v1:

ESP-WROVER-KIT V1 / ESP32 DevKitJ V1
------------------------------------

ESP-WROVER-KIT 开发板初版，板上搭载 ESP-WROOM-32 模组。

ESP-WROVER-KIT 开发板上装有用于编程的 USB 转双串口和用于调试的 JTAG 接口，由 USB 接口或 5 毫米标准电源插座供电。电源通过跳线选择，有单独的开关。该款开发板上还有 MicroSD 卡槽、3.2” SPI LCD 屏幕、连接摄像头的专用排针、用于诊断的 RGB 二极管和内部 RTC 在低功耗模式下可操作的 32.768 kHz 晶振。

ESP-WROVER-KIT 所有版本均适配 ESP-WROOM-32 或 ESP32-WROVER 模组。


.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-devkitj-v1.jpg
    :align: center
    :alt: ESP-WROVER-KIT V1 / ESP32 DevKitJ V1
    :width: 90%

    ESP-WROVER-KIT V1 / ESP32 DevKitJ V1

该款开发板有红色阻焊层。

相关文档
^^^^^^^^

* `ESP-WROVER-KIT V1 原理图 <https://dl.espressif.com/dl/schematics/ESP32-DevKitJ-v1_sch.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI 虚拟 COM 端口驱动`_

.. _esp-modules-and-boards-esp32-demo-board:

ESP32 Demo Board V2
-------------------

首款功能齐全的评估板，板上装有排针、DIP 开关、USB 转串口、复位和启动模式按键、电源开关、10 个触控板和连接 LCD 屏幕的专用排针。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-demo-board-v2.jpg
    :align: center
    :alt: ESP32 Demo Board V2

    ESP32 Demo Board V2

该款开发板已停产。

相关文档
^^^^^^^^

* `ESP32 Demo Board V2 Schematic <https://dl.espressif.com/dl/schematics/ESP32-Demo-Board-V2_sch.pdf>`__ (PDF)
* `FTDI 虚拟 COM 端口驱动`_