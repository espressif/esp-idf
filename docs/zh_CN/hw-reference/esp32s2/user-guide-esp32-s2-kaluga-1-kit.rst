==============================
ESP32-S2-Kaluga-1 套件 v1.3
==============================

:link_to_translation:`en:[English]`

更早版本：:doc:`user-guide-esp32-s2-kaluga-1-kit-v1.2`

ESP32-S2-Kaluga-1 v1.3 是一款来自乐鑫的开发套件，主要可用于以下目的：

- 展示 ESP32-S2 芯片的人机交互功能
- 为用户提供基于 ESP32-S2 的人机交互应用开发工具

ESP32-S2 的功能强大，应用场景非常丰富。对于初学者来说，可能的用例包括：

- **智能家居**：从最简单的智能照明、智能门锁、智能插座，到更复杂的视频流设备、安防摄像头、OTT 设备和家用电器等
- **电池供电设备**：Wi-Fi mesh 传感器网络、Wi-Fi 网络玩具、可穿戴设备、健康管理设备等
- **工业自动化设备**：无线控制与机器人技术、智能照明、HVAC 控制设备等
- **零售和餐饮业**：POS 机和服务机器人

.. Image of v1.2 is used as there are no visual changes

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-kit-v1.0-3d.png
    :align: center
    :width: 3452px
    :height: 1590px
    :scale: 20%
    :alt: ESP32-S2-Kaluga-1-Kit-Assembly
    :figclass: align-center

    ESP32-S2-Kaluga-1-Kit 概述（点击放大）


ESP32-S2-Kaluga-1 套件包括以下几个开发板：

- 主板：*ESP32-S2-Kaluga-1*
- 扩展板：

  - :doc:`user-guide-esp-lyrat-8311a_v1.3` - 音频播放器
  - :doc:`user-guide-esp-lyrap-toucha-v1.1` - 触摸板
  - :doc:`user-guide-esp-lyrap-lcd32-v1.2` - 3.2" LCD 屏
  - :doc:`user-guide-esp-lyrap-cam-v1.1` - 摄像头

由于 ESP32-S2 的管脚复用，部分扩展板的兼容性有所限制，具体请见 :ref:`user-guide-esp32-s2-kaluga-1-kit-ext-board-compatibility`。

本文档主要介绍 **ESP32-S2-Kaluga-1 主板** 及其与扩展板的交互。更多有关具体扩展板的信息，请点击相应的链接。

本指南包括：

- `快速入门`_：提供 ESP32-S2-Kaluga-1 的简要概述及必须了解的硬件和软件信息。
- `硬件参考`_：提供 ESP32-S2-Kaluga-1 的详细硬件信息。
- `硬件修订历史`_：提供该开发版的“修订历史”、“已知问题”以及此前版本开发板的用户指南链接。
- `相关文档`_：提供相关文档的链接。


快速入门
========


本节介绍如何开始使用 ESP32-S2-Kaluga-1，主要包括三大部分：首先，介绍一些关于 ESP32-S2-Kaluga-1 的基本信息，然后在  `应用程序开发`_ 章节介绍如何进行硬件初始化，最后介绍如何为 ESP32-S2-Kaluga-1 烧录固件。


概述
----

ESP32-S2-Kaluga-1 主板是整个套件的核心。该主板集成了 ESP32-S2-WROVER 模组，并配备连接至各个扩展板的连接器。ESP32-S2-Kaluga-1 是人机交互接口原型设计的关键工具。

ESP32-S2-Kaluga-1 主板配备了多个连接器，可用于连接相应扩展板：

- 扩展板连接器，用于连接 ESP-LyraT-8311A、ESP-LyraP-LCD32
- 摄像头连接器，用于连接 ESP-LyraP-CAM
- 触摸 FPC 连接器，用于连接 ESP-LyraP-TouchA
- LCD FPC 连接器（尚无可用官方配套扩展板）
- I2C FPC 连接器（尚无可用官方配套扩展板）

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32s2-kaluga-1-v1.3-3d.png
    :align: center
    :width: 3089px
    :height: 2335px
    :scale: 25%
    :alt: ESP32-S2-Kaluga-1
    :figclass: align-center

    ESP32-S2-Kaluga-1（点击放大）

所有四个扩展板都经过特别设计，以支持以下功能：

* 触摸板控制
    * 带有 6 个触摸按钮
    * 支持最大 5 mm 亚克力板
    * 支持湿手操作
    * 支持防水功能。ESP32-S2 可以配置为在多个触摸板同时被水复盖时自动禁用所有触摸板功能，并在去除水滴后重新启用触摸板

* 音频播放
    * 连接扬声器，以播放音频
    * 配合触控板使用，可控制音频播放和调节音量

* LCD 显示屏
    * LCD 接口（8 位并行 RGB、8080 和 6800 接口)

* 摄像头图像采集
    * 支持 OV2640 和 OV3660 摄像头模块
    * 8-bit DVP 图像传感器接口（ESP32-S2 还支持 16 位 DVP 图像传感器，但需要您自行进行二次开发）
    * 支持高达 40 MHz 时钟频率
    * 优化 DMA 传输带宽，便于传输高分辨率图像


组件描述
--------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-v1.3-layout-front.png
    :align: center
    :width: 934px
    :height: 645px
    :scale: 70%
    :alt: ESP32-S2-Kaluga-1 - 正面
    :figclass: align-center

    ESP32-S2-Kaluga-1 - 正面（点击放大）

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-v1.3-layout-back.png
    :align: center
    :width: 934px
    :height: 600px
    :scale: 70%
    :alt: ESP32-S2-Kaluga-1 - 反面
    :figclass: align-center

    ESP32-S2-Kaluga-1 - 反面（点击放大）


下表将从左边的 ESP32-S2 模组开始，以顺时针顺序介绍上图中的主要组件。

**保留** 表示该功能可用，但当前版本的套件并未启用该功能。


.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 描述
   * - ESP32-S2-WROVER 模组
     - 集成 ESP32-S2 芯片，可提供 Wi-Fi 连接、数据处理和灵活的数据存储功能。
   * - 4.3" LCD FPC 连接器
     - （保留）可使用 FPC 线连接 4.3" LCD 扩展板。
   * - ESP Prog 连接器
     - （保留）用于连接乐鑫固件烧录设备 (ESP-Prog)。
   * - JTAG 开关
     - 切换到 ON 方向，启用 ESP32-S2 和 FT2232 之间的连接。此时，可通过 USB-UART/JTAG 端口进行 JTAG 调试，详见 :doc:`../../api-guides/jtag-debugging/index`。
   * - 引出管脚排针 2
     - ESP32-S2-WROVER 模组的部分 GPIO 直接引出至该开发板（详见开发板上的标记）。
   * - USB-to-UART/JTAG 桥接器
     - FT2232 适配器开发板，允许在 USB 端口使用 UART/JTAG 协议通信。
   * - 摄像头连接器
     - 用于连接摄像头扩展板，比如 ESP-LyraP-CAM。
   * - 扩展板连接器
     - 用于连接带有配套连接器的扩展板。
   * - Reset 复位按钮
     - 用于重启系统。
   * - Boot 按钮
     - 按下 **Boot** 键并保持，同时按一下 **Reset** 键，进入“固件下载”模式，通过串口下载固件。
   * - USB-UART/JTAG 端口
     - PC 和 ESP32-S2 模组之间的通信接口（UART 或 JTAG）。
   * - USB 电源端口
     - 为开发板供电。
   * - 电池端口
     - 2 针连接器，用于连接外部电池。
   * - 电源 LED 指示灯
     - 当 USB 电源或外部电源供电电压正常，则 LED 亮起。
   * - 电源开关
     - 打开可为系统供电。
   * - RGB 跳线
     - 如需使用 RGB LED，需在该位置增加一个跳线。
   * - RGB LED 指示灯
     - 可编程 RGB LED 指示灯，受控于 GPIO45。在使用前需要安装 RGB 跳线。
   * - 调压器
     - 5 V 转 3.3 V 调压器。
   * - I2C FPC 连接器
     - （保留）可通过 FPC 线连接其他 I2C 扩展板。
   * - 引出管脚排针 1
     - ESP32-S2-WROVER 模组的部分 GPIO 直接引出至该开发板（详见开发板上的标记）。
   * - 触摸 FPC 连接器
     - 可通过 FPC 线连接 ESP-LyraP-TouchA 扩展板。
   * - 触摸开关
     - 切换到 OFF 方向，配置 GPIO1 到 GPIO14 连接触摸传感器；切换到 ON 方向，配置 GPIO1 到 GPIO14 用于其他目的。
   * - 3.2" LCD FPC 连接器
     - 可通过 FPC 线连接 3.2" LCD 扩展板，比如 ESP-LyraP-LCD32。


应用程序开发
------------

ESP32-S2-Kaluga-1 上电前，请首先确认开发板完好无损。


硬件准备
^^^^^^^^

- ESP32-S2-Kaluga-1
- 两根 USB 2.0 电缆（标准 A 转 Micro-B)

    - 电源选项
    - 用于 UART/JTAG 通信

- PC（Windows、Linux 或 macOS）
- 您选择的任何扩展板


硬件设置
^^^^^^^^

1. 连接您选择的扩展板（更多信息，请见对应拓展板的用户指南)
2. 插入两根 USB 电缆
3. 打开 **电源开关** 时，**电源 LED 指示灯** 应点亮。


.. _user-guide-esp32-s2-kaluga-1-kit-software-setup:

软件设置
^^^^^^^^

请前往 :doc:`../../get-started/index`，在 :ref:`get-started-step-by-step` 一节查看如何快速设置开发环境。

您还可以点击 `这里 <https://github.com/espressif/esp-dev-kits/tree/master/esp32-s2-kaluga-1>`_，获取有关 ESP32-S2-Kaluga-1 套件编程指南与应用示例的更多内容。


内容和包装
----------

零售订单
^^^^^^^^

每一个零售 ESP32-S2-Kaluga-1 开发套件均有独立包装。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32s2-kaluga-1-kit-v1.3-package-3d.png
    :align: center
    :alt: ESP32-S2-Kaluga-1 - 包装
    :figclass: align-center

    ESP32-S2-Kaluga-1 - 包装


内含以下部分：

- 主板
    - ESP32-S2-Kaluga-1
- 扩展板：
    - ESP-LyraT-8311A
    - ESP-LyraP-CAM
    - ESP-LyraP-TouchA
    - ESP-LyraP-LCD32
- 连接器
    - 20 针 FPC 线（用于连接 ESP32-S2-Kaluga-1 主板至 ESP-LyraP-TouchA 扩展板）
- 紧固件
    - 安装螺栓 (x 8)
    - 螺丝 (x 4)
    - 螺母 (x 4)

零售购买，请前往 https://www.espressif.com/zh-hans/contact-us/get-samples。


批发订单
^^^^^^^^

ESP32-S2-Kaluga-1 开发套件的批发包装为纸板箱。

批量订货，请参考 `乐鑫产品订购信息 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`_ (PDF)。


硬件参考
========


功能框图
--------

.. Image of v1.2 is used as there are no visual changes

ESP32-S2-Kaluga-1 的主要组件和连接方式如下图所示。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-v1.2-block-diagram.png
    :align: center
    :alt: ESP32-S2-Kaluga-1 功能框图
    :figclass: align-center

    ESP32-S2-Kaluga-1 功能框图


电源选项
--------

开发板可任一选用以下四种供电方式：

- Micro USB 端口供电（默认）
- 通过 2 针电池连接器使用外部电池供电
- 5V / GND 管脚供电
- 3V3 / GND 管脚供电


.. _user-guide-esp32-s2-kaluga-1-kit-ext-board-compatibility:

扩展板的兼容性
--------------

如需同时使用多块扩展板，请首先查看以下兼容性信息：

.. list-table::
   :widths: 20 15 33 32
   :header-rows: 1

   * - 扩展板组合
     - 复用接口或管脚
     - 无法运行原因
     - 解决方案
   * - 8311A v1.3 + CAM v1.1
     - I2S 控制器
     - ESP32-S2 仅有 1 个 I2S 接口，但这两个开发板均需使用 ESP32-S2 的 I2S 接口进行通信（ESP-LyraT-8311A 使用标准模式；ESP-LyraP-CAM 使用 Camera 协议）。
     - 采用分时复用；或另外选择一款可以通过其他 GPIOs 或 DAC 连接的音频扩展板。
   * - TouchA v1.1 + LCD32 v1.2
     - IO11、IO6
     - 由于管脚 IO11 复用，导致无法触发触摸动作；ESP-LyraP-LCD32 则由于其 BLCT 管脚已与 IO6 断开，因此不受影响。
     - 不要初始化 ESP-LyraP-TouchA 扩展板的 IO11 (NETWORK) 管脚；或者配置 ESP-LyraP-LCD32 扩展板的 BLCT 管脚为 `-1` （相当于不使用 BLCT）。
   * - 8311A v1.3 + LCD32 v1.2
     - IO6
     - 配置 ESP-LyraP-LCD32 扩展板的 BK 管脚为 `-1` （相当于不使用 BK）。
     - ESP32-S2-Kaluga-1 的 BLCT 管脚将从 IO6 断开。
   * - TouchA v1.1 + 8311A v1.3
     - ESP-LyraT-8311A 的 BT_ADC 管脚 
     - ESP-LyraT-8311A 在初始化 6 个按钮时需要使用 BT_ADC 管脚，而 ESP-LyraP-TouchA 在完成触摸动作时也需要使用 BT_ADC 管脚。
     - 如需使用 ESP-LyraT-8311A 的 6 个按钮，则不要初始化 ESP-LyraP-TouchA 的 IO6 (PHOTO) 管脚。
   * - TouchA v1.1 + CAM v1.1
     - IO1、IO2、IO3
     - 由于管脚复用无法同时使用。
     - 不要初始化 ESP-LyraP-TouchA 的 IO1 (VOL_UP)、IO2 (PLAY) 和 IO3 (VOL_DOWN)。
   * - TouchA v1.1 + LCD32 v1.2 + CAM v1.1
     - IO1、IO2、IO3、IO11
     - 由于管脚复用无法同时使用。
     - 不要初始化 ESP-LyraP-TouchA 的 IO1 (VOL_UP)、IO2 (PLAY)、IO3 (VOL_DOWN) 和 IO11 (NETWORK)。
   * - TouchA v1.1 + LCD32 v1.2 + 8311A v1.3
     - IO6、IO11
     - 如果使用 ESP-LyraT-8311A 的 BT_ADC 管脚初始化开发板的 6 个按钮，其他扩展板则无法使用 IO6 和 IO11。
     - 不要初始化 ESP-LyraP-TouchA 的 IO11 (NETWORK)。此外，如果需要使用 BT_ADC，则不要初始化 IO6 (PHOTO)。


硬件修订历史
============

ESP32-S2-Kaluga-1 Kit v1.3
--------------------------

* 以下管脚已重新分配，以解决固件烧录问题：

  * Camera D2：GPIO36
  * Camera D3：GPIO37
  * AU_I2S1_SDI：GPIO34
  * AU_WAKE_INT：GPIO46

* RGB 已移动至开发板边缘
* 所有 dip 开关均移动至开发板的反面，从而便利用户操作


ESP32-S2-Kaluga-1 Kit v1.2
--------------------------

:doc:`首次发布 <user-guide-esp32-s2-kaluga-1-kit-v1.2>`


相关文档
========

.. toctree::
    :hidden:

    user-guide-esp32-s2-kaluga-1-kit-v1.2.rst
    user-guide-esp-lyrap-cam-v1.1
    user-guide-esp-lyrap-lcd32-v1.2
    user-guide-esp-lyrat-8311a_v1.3

- `ESP32-S2-WROVER 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_cn.pdf>`_ (PDF)
- `乐鑫产品订购信息 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`_ (PDF)
- :doc:`../../api-guides/jtag-debugging/index`

- `ESP32-S2-Kaluga-1 原理图 <https://dl.espressif.com/dl/schematics/ESP32-S2-Kaluga-1_V1_3_SCH_20200526A.pdf>`_ (PDF)
- `ESP32-S2-Kaluga-1 PCB 布局图 <https://dl.espressif.com/dl/schematics/ESP32-S2-Kaluga-1_V1_3_PCB_20200526AE.pdf>`_ (PDF)
- `ESP32-S2-Kaluga-1 管脚映射 <https://dl.espressif.com/dl/schematics/ESP32-S2-Kaluga-1_V1.3-Pin-Mapping-v0.1.xlsx>`_ (Excel)

有关本开发板的更多设计文档，请联系我们的商务部门 sales@espressif.com。


    