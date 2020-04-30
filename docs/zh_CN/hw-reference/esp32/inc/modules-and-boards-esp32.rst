.. _esp-wroom-solo-wrover-modules:

模组
====

ESP32 系列模组集成了晶振、天线匹配电路等重要组件，可直接集成到终端产品中。如果再结合一些其他组件，例如编程接口、自举电阻和排针，您就可以体验 ESP32 的强大功能了。

下表总结了上述系列模组的主要特点，详细信息见后文。

=====================  ==============  ============  ============  ======  =================
 模组                   芯片             Flash (MB)    PSRAM (MB)    天线     尺寸 (mm)       
=====================  ==============  ============  ============  ======  =================
 ESP32-WROOM-32        ESP32-D0WDQ6    4             –             MIFA    18 × 25.5 × 3.1 
 ESP32-WROOM-32D       ESP32-D0WD      4、8 或 16     –             MIFA    18 × 25.5 × 3.1 
 ESP32-WROOM-32U       ESP32-D0WD      4、8 或 16     –             U.FL    18 × 19.2 × 3.1 
 ESP32-SOLO-1          ESP32-S0WD      4             –             MIFA    18 × 25.5 × 3.1 
 ESP32-WROVER (PCB)    ESP32-D0WDQ6    4             8             MIFA    18 × 31.4 × 3.3 
 ESP32-WROVER (IPEX)   ESP32-D0WDQ6    4             8             U.FL    18 × 31.4 × 3.3 
 ESP32-WROVER-B        ESP32-D0WD      4、8 或 16     8             MIFA    18 × 31.4 × 3.3 
 ESP32-WROVER-IB       ESP32-D0WD      4、8 或 16     8             U.FL    18 × 31.4 × 3.3 
=====================  ==============  ============  ============  ======  =================


* ESP32-**D**.. 代表双核芯片，ESP32-**S**.. 代表单核芯片；
* MIFA - 蛇形倒 F 天线；
* U.FL - U.FL/IPEX 天线连接器；
* ESP32-WROOM-32x、ESP32-WROVER-B 和 ESP32-WROVER-IB 模组默认内置 4 MB flash，客户可定制 8 MB 和 16 MB flash，详情见 `乐鑫产品订购信息`_ (PDF) 和《`ESP32 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_》(PDF)；
* 最初发布的 ESP32-WROVER 模组内置 4 MB PSRAM；
* *ESP-WROOM-32* 是 *ESP32-WROOM-32* 的曾用名。

.. _esp-modules-and-boards-esp32-wroom-32:

ESP32-WROOM-32 模组
-------------------

ESP32-WROOM-32 模组是 WROOM/WROVER 系列最先发布的模组，内置 ESP32-D0WDQ6 芯片，是一款基础且常用的 ESP32 模组。

有关该模组的详细信息，请查看 :ref:`esp-wroom-solo-wrover-modules` 中的表格和`乐鑫产品订购信息`_。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wroom-32-front-back.jpg
    :align: center
    :alt: ESP32-WROOM-32 module (front and back)
    :width: 45%

    ESP32-WROOM-32 模组（正反面图）

相关文档
^^^^^^^^

* 《`ESP32-WROOM-32 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_cn.pdf>`_》(PDF)；
* `ESP32-WROOM-32 参考设计 <https://www.espressif.com/zh-hans/support/download/documents?keys=ESP32-WROOM-32+%E5%8F%82%E8%80%83%E8%AE%BE%E8%AE%A1>`_，包括原理图（由 OrCAD Capture 绘制）、PCB 布局（由 Mentor PADS 绘制）、GERBER 文件和 BOM 清单。

.. _esp-modules-and-boards-esp32-wroom-32d-and-u:

ESP32-WROOM-32D/ESP32-WROOM-32U 模组
------------------------------------

两款模组均集成了 ESP32-D0WD 芯片，与 :ref:`esp-modules-and-boards-esp32-wroom-32` 集成的 ESP32-D0WDQ6 相比，ESP32-D0WD 芯片的封装更小，在 PCB 上占用的面积更小。

有关这两款模组的详细信息，请查看 :ref:`esp-wroom-solo-wrover-modules` 中的表格和 `乐鑫产品订购信息`_。

ESP32-WROOM-32U 是整个 WROOM/WROVER 模组系列中最小的模组。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wroom-32d-front-back.jpg
    :align: center
    :alt: ESP32-WROOM-32D module (front and back)
    :width: 45%

    ESP32-WROOM-32D 模组（正反面图）

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wroom-32u-front-back.jpg
    :align: center
    :alt: ESP32-WROOM-32U module (front and back)
    :width: 45%

    ESP32-WROOM-32U 模组（正反面图）

相关文档
^^^^^^^^

* 《`ESP32-WROOM-32D/ESP32-WROOM-32U 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_cn.pdf>`_》(PDF)


.. _esp-modules-and-boards-esp32-solo-1:

ESP32-SOLO-1 模组
-----------------

ESP32-SOLO-1 模组是 ESP32-WROOM-32D 模组的简化版本，内置一个 ESP32 单核芯片，支持高达 160 MHz 的时钟频率。

有关此模组的详细信息，请查看 :ref:`esp-wroom-solo-wrover-modules` 中的表格和 `乐鑫产品订购信息`_。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-solo-1-front-back.jpg
    :align: center
    :alt: ESP32-SOLO-1 module (front and back)
    :width: 45%

    ESP32-SOLO-1 模组（正反面图）

相关文档
^^^^^^^^

* 《`ESP32-SOLO-1 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-solo-1c_datasheet_cn.pdf>`__》(PDF)

.. _esp-modules-and-boards-esp32-wrover:

ESP32-WROVER 系列模组
---------------------

ESP32-WROVER 系列模组在 ESP32-WROOM-32x 模组的基础上进行了一些修改，其中包含一些功能升级，并新增 8 MB SPI PSRAM（伪静态 RAM）。

有关该模组的详细信息，请查看 :ref:`esp-wroom-solo-wrover-modules` 中的表格和 `乐鑫产品订购信息`_。

* **ESP32-WROVER (PCB)** 模组和 **ESP32-WROVER (IPEX)** 模组内置 1.8 V PSRAM，支持 144 MHz 时钟频率。
* **ESP32-WROVER-B** 模组和 **ESP32-WROVER-IB** 模组内置 3.3 V PSRAM，支持 133 MHz 时钟频率。

下图为配备有 PCB 天线的 ESP32-WROVER 模组：

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wrover.jpg
    :align: center
    :alt: ESP32-WROVER module (front and back)
    :width: 40%

    ESP32-WROVER 模组（正反面图）

相关文档
^^^^^^^^

* 《`ESP32-WROVER 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32_wrover_datasheet_cn.pdf>`__》(PDF)
* 《`ESP32-WROVER-B 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-wrover-b_datasheet_cn.pdf>`__》(PDF)
* 《`ESP-PSRAM64 & ESP-PSRAM64H 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp-psram64_esp-psram64h_datasheet_cn.pdf>`__》(PDF)
* `ESP32-WROVER 参考设计 <https://www.espressif.com/zh-hans/support/download/documents?keys=ESP32-WROVER+%E5%8F%82%E8%80%83%E8%AE%BE%E8%AE%A1>`_ 包含原理图（由 OrCAD Capture 绘制）、PCB 布局（由 Mentor PADS 绘制）、GERBER 文件和 BOM 清单。

ESP32-PICO-D4 模组
------------------

ESP32-PICO-D4 模组是一款 SiP 模组，无缝集成了所有外设，包括：

- 4 MB flash
- 晶振
- 滤波电容
- RF 匹配电路

有关该模组的详细信息，请查看 `乐鑫产品订购信息`_。

相关文档
^^^^^^^^

* 《`ESP32-PICO-D4 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_cn.pdf>`__》(PDF)

ESP32 开发板
============

ESP32 系列开发板功能各异，具体有以下不同点：

- 访问的 ESP32 GPIO 管脚不同；
- 接口不同，包括 USB 和 JTAG；
- 外设不同，包括 TouchPad、LCD 显示屏、SD 卡槽和相机模组排母等。

.. _esp-modules-and-boards-esp32-pico-kit:

ESP32-PICO-KIT V4.1 开发板
--------------------------
ESP32-PICO-KIT V4.1 开发板是基于 ESP32 的最小开发板，内置连接至电脑 USB 端口所需的所有组件，同时设有排针，可直接将此开发板插接于迷你面包板。

ESP32-PICO-KIT V4.1 开发板集成了 `ESP32-PICO-D4 模组`_，只需在 PCB (20 x 52 mm) 上添加少许外部组件即可构成一个功能齐全的开发板。这部分外部组件包括天线、LDO、USB 至 UART 桥接器、一个重置按钮和一个固件下载模式激活按钮。


.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v4.1.jpg
    :align: center
    :alt: ESP32-PICO-KIT V4.1 board
    :width: 50%

    ESP32-PICO-KIT V4.1 开发板

与 ESP32-PICO-KIT V4 相比，ESP32-PICO-KIT V4.1 开发板支持 CP2102N USB 至 UART 桥接器，可提供高达 3 Mbps 的传输速率。

相关文档
^^^^^^^^

* :doc:`../../hw-reference/esp32/get-started-pico-kit`
* `ESP32-PICO-KIT V4.1 原理图 <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v4.1_schematic.pdf>`_ (PDF)
* `ESP32-PICO-KIT 参考设计 <https://www.espressif.com/zh-hans/support/download/documents?keys=ESP32-PICO-KIT+%E5%8F%82%E8%80%83%E8%AE%BE%E8%AE%A1>`_，包含原理图（由 OrCAD Capture 绘制）、PCB 布局（由 Mentor PADS 绘制）、GERBER 文件和 BOM 清单。
* 《`ESP32-PICO-D4 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_cn.pdf>`_》(PDF)

较早版本开发板
^^^^^^^^^^^^^^

* :ref:`esp-modules-and-boards-esp32-pico-kit-v4`
* :ref:`esp-modules-and-boards-esp32-pico-kit-v3`


.. _esp-modules-and-boards-esp32-devkitc:

ESP32 DevKitC V4 开发板
-----------------------

ESP32 DevKitC V4 开发板是一款小巧实用的开发板，具备以下特色功能：

- 集成了 :ref:`esp-modules-and-boards-esp32-wroom-32`
- USB 转串口编程接口同时可为开发板供电
- 设有排针
- 设有重置按钮和固件下载模式激活按钮
- 以及其他组件

与较早版本的 :ref:`esp-modules-and-boards-esp32-devkitc-v2` 相比，ESP32 DevKitC V4 开发板集成了 :ref:`esp-modules-and-boards-esp32-wrover` （而非 ESP32-WROOM-32 模组），同时内置 CP2102N 芯片，支持更高波特率。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-devkitc-v4-front.jpg
    :align: center
    :alt: ESP32 DevKitC V4 board
    :width: 50%

    ESP32 DevKitC V4 开发板

相关文档
^^^^^^^^

* :doc:`../../hw-reference/esp32/get-started-devkitc`
* `ESP32-DevKitC 原理图 <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf>`_ (PDF)
* `ESP32-DevKitC 参考设计 <https://www.espressif.com/zh-hans/support/download/documents?keys=ESP32-DevKitC+%E5%8F%82%E8%80%83%E8%AE%BE%E8%AE%A1>`_，包含原理图（由 OrCAD Capture 绘制）、PCB 布局（由 Mentor PADS 绘制）、GERBER 文件和 BOM 清单。
* `CP210x USB 至 UART 桥 VCP 驱动器 <https://cn.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_

较早版本开发板
^^^^^^^^^^^^^^

* :ref:`esp-modules-and-boards-esp32-devkitc-v2`


.. _esp-modules-and-boards-esp-wrover-kit:

ESP-WROVER-KIT V4.1 开发板
--------------------------

ESP-WROVER-KIT V4.1 开发板具备以下特色功能：

- USB 转双串口转换器（用于后续编程）
- JTAG 调试接口
- MicroSD 卡槽
- 3.2” SPI LCD 显示屏
- 相机模组排母
- RGB 发光二极管
- 支持 32.768 kHz 晶振输入用于 RTC 及低功耗模式操作

ESP-WROVER-KIT V4.1 开发板支持 USB 供电或标准的 5 毫米电源插座供电，可使用跳线选择电源，或使用独立的开关控制电源。

ESP-WROVER-KIT V4.1 开发板集成了 ESP-WROVER-B 模组，该模组集成了 8 MB PSRAM，方便用户灵活扩展存储空间，增强数据处理能力。ESP-WROVER-KIT V4.1 开发板还可以集成 :ref:`esp-wroom-solo-wrover-modules` 中所述的 ESP 其他版本模组。

与 :ref:`esp-modules-and-boards-esp-wrover-kit-v3` 相比，ESP-WROVER-KIT V4.1 开发板在设计上有以下改动：

- JP8、JP11 和 JP13 合并成了一个 JP2。
- USB 连接器的固定脚改为直插式，并移至板子右下角。
- R61 已变更为零欧姆电阻。
- 基于测试结果和采购选择，部分组件已由功能对等组件替代，例如 EN 和 Boot 按钮。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v4.1-front.jpg
   :align: center
   :alt: ESP-WROVER-KIT V4.1 board
   :width: 90%

   ESP-WROVER-KIT V4.1 开发板

上图所示开发板集成了 ESP32-WROVER-B 模组。

相关文档
^^^^^^^^

* :doc:`../../hw-reference/esp32/get-started-wrover-kit`
* `ESP-WROVER-KIT V4.1 原理图 <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_V4_1.pdf>`__ (PDF)
* :doc:`../../../api-guides/jtag-debugging/index`
* `FTDI 虚拟 COM 端口驱动`_

较早版本开发板
^^^^^^^^^^^^^^

* :ref:`esp-modules-and-boards-esp-wrover-kit-v3`
* :ref:`esp-modules-and-boards-esp-wrover-kit-v2`
* :ref:`esp-modules-and-boards-esp-wrover-kit-v1`



.. _FTDI 虚拟 COM 端口驱动: https://www.ftdichip.com/Drivers/VCP.htm
.. _乐鑫产品订购信息: https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf
