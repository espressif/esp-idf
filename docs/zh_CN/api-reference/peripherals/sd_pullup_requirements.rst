SD 上拉需求
===========

:link_to_translation:`en:[English]`

乐鑫硬件产品适用于多种场景，这些场景可能需要不同的管脚上拉状态。因此，要调整某些产品的特定管脚上拉状态，为 SD 总线提供所需的上拉电阻。

{IDF_TARGET_NAME} 使用 SPI 或 SDMMC 控制器与 SD 卡通信时，需要满足 SD 卡的上拉需求。当 SD 卡在 SPI 模式或 1 位 SD 模式下运行时，SD 总线的 CMD 和 DATA (DAT0 - DAT3) 线必须通过 10 kΩ 电阻上拉。SD 卡和 SDIO 设备也应在上述所有管脚（无论是否连接到主机）上有上拉电阻，以防设备进入错误状态。

.. only:: esp32

    默认情况下，如果 flash 电压为 3.3 V，则 MTDI strapping 管脚与 DAT2 线的上拉电阻不兼容。详情请参阅 :ref:`mtdi_strapping_pin`。

.. todo::

   Add a diagram of the bus lines and pullups

本文档涵盖以下章节内容：

- :ref:`compatibility_overview_espressif_hw_sdio` - 简要介绍乐鑫产品管脚的默认上拉状态与 SD 总线所需状态之间的兼容性
- :ref:`sdio_solutions`  - 解决兼容性问题的方案
- :ref:`related_info_sdio` - 其他相关信息


.. _compatibility_overview_espressif_hw_sdio:

兼容性概述
----------

本节概述了在使用安全数字输入输出 (SDIO) 时可能出现的兼容性问题。SD 总线需要连接上拉电阻，因此要妥善解决与主机设备及从机设备有关的兼容性问题。下面每个问题都有相应解决方案的链接，主机设备和从机设备的解决方案可能有所不同。


.. only:: esp32

    片上系统 (SoCs)
    ^^^^^^^^^^^^^^^

    - ESP32（除 D2WD 版本外，请参阅 `ESP32 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_）：

        - :ref:`sd_pull-up_no_pull-ups`
        - 带有 3.3 V flash 芯片的模组，请参阅 :ref:`strapping_conflicts_dat2`

    - ESP32-D2WD：

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`no_pull-up_on_gpio12`

.. only:: SOC_SDMMC_USE_GPIO_MATRIX

    片上系统 (SoCs)
    ^^^^^^^^^^^^^^^

    {IDF_TARGET_NAME} SDMMC 主机控制器允许 SD 接口信号使用所有 GPIO。但建议避免使用 strapping GPIO、带内部弱下拉的 GPIO 以及通常用于其他用途的 GPIO，以防出现冲突。

    .. only:: esp32s3

        - GPIO0（strapping 管脚）
        - GPIO45、GPIO46（strapping 管脚，内部弱下拉）
        - GPIO26 - GPIO32（通常用于 SPI flash 和 PSRAM）
        - GPIO33 - GPIO37（用于带有八线 SPI flash 或八线 PSRAM 的芯片和模组）
        - GPIO43、GPIO44（默认用于 UART0 的 GPIO）
        - GPIO19、GPIO20（默认用于 USB 的 GPIO）


.. only:: esp32

    系统级封装 (SIP)
    ^^^^^^^^^^^^^^^^

    - ESP32-PICO-D4：

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`


    模组
    ^^^^

    - ESP32-WROOM-32 系列模组，包括 ESP32-WROOM-32、ESP32-WROOM-32D、ESP32-WROOM-32U 和 ESP32-SOLO-1

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`

    - ESP32-WROVER 系列模组，包括 ESP32-WROVER 和 ESP32-WROVER-I

        - :ref:`sd_pull-up_no_pull-ups`

    - ESP32-WROVER-B 系列模组，包括 ESP32-WROVER-B 和 ESP32-WROVER-IB

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`


.. only:: esp32

    .. _sdio_dev_kits:

    开发板
    ^^^^^^

    - ESP32-PICO-KIT，包括 PICO-KIT v4.1、v4.0 和 v3

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`
        - :ref:`gpio2_strapping_pin`

    - ESP32-DevKitC，包括 ESP32-DevKitC v4 和 v2

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`
        - :ref:`gpio2_strapping_pin`

    - ESP-WROVER-KIT

        - 提供所需的上拉电阻
        - :ref:`pull-up_conflicts_on_gpio13` （v4.1、v3、v2 和 v1）
        - :ref:`strapping_conflicts_dat2` （v4.1、v2 和 v1）
        - :ref:`gpio2_strapping_pin` （v2 和 v1）

        通过查看安装在开发板上的模组，可以确定所使用的 ESP32-WROVER-KIT 的版本：

        - ESP32-WROVER-B 对应 v4.1 版本
        - ESP32-WROVER 对应 v3 版本
        - ESP32-WROOM-32 对应 v1 和 v2 版本

    - ESP32-LyraTD-MSC

        - 提供所需的上拉电阻
        - :ref:`strapping_conflicts_dat2`

    - ESP32-LyraT

        - 提供所需的上拉电阻
        - :ref:`pull-up_conflicts_on_gpio13`

.. only:: esp32s3

    .. _sdio_dev_kits:

    开发板
    ^^^^^^

    - ESP32-S3-DevKitC-1

        - :ref:`sd_pull-up_no_pull-ups`

    - ESP32-S3-USB-OTG

        - 该开发板可在单线和四线 SD 模式或 SPI 模式下使用。
        - 在 GPIO 33-38 上提供所需的上拉电阻。

    - ESP32-S3-EYE

        - 该开发板仅限于单线 SD 模式下使用。
        - 在 GPIO 38-40 上提供所需的上拉电阻。

.. only:: SOC_SDIO_SLAVE_SUPPORTED

    非乐鑫主机
    ^^^^^^^^^^

    请确保 SDIO 主机为所有 SD 总线信号提供必要的上拉电阻。


.. _sdio_solutions:

解决方案
--------

.. _sd_pull-up_no_pull-ups:

无上拉电阻
^^^^^^^^^^

.. only:: esp32 or esp32s3

    如果所使用的开发板没有上拉电阻，可以执行以下操作：

    - 如果主机设备和从机设备在不同的开发板上，将其中一块开发板换为带有上拉电阻的开发板。如需了解哪些乐鑫开发板带有上拉电阻，请参阅 :ref:`sdio_dev_kits`。
    - 用一个 10 kΩ 的电阻将每个需要上拉的管脚连接到 VDD，从而附加外部上拉电阻。

.. only:: not esp32 and not esp32s3

    如果所使用的开发板没有上拉电阻，可以执行以下操作：

    - 如果主机设备和从机设备在不同的开发板上，将其中一块开发板换为带有上拉电阻的开发板。
    - 用一个 10 kΩ 的电阻将每个需要上拉的管脚连接到 VDD，从而附加外部上拉电阻。

.. only:: esp32

    .. _pull-up_conflicts_on_gpio13:

    GPIO13 上拉冲突
    ^^^^^^^^^^^^^^^

    如果所用设备的 DAT3 没有正确上拉，可以参考下列操作：

    - 使用 1 位 SD 模式并将设备的 DAT3 连接到 VDD
    - 使用 SPI 模式
    - 对 GPIO13 管脚执行以下操作之一：
        - 移除下拉电阻
        - 附加小于 5 kΩ 的上拉电阻（建议使用 2 kΩ 的电阻）
        - 在 1 位 SD 模式下，通过主机、或是通过 VDD 上的 3.3 V 电压，将管脚拉高或驱动到高电平。


    .. _strapping_conflicts_dat2:

    Bootstrap 与 SDIO 在 DAT2 上的冲突
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    ESP32 的启动 strapping 需求与 SDIO 协议之间存在冲突。详情请参阅 :ref:`mtdi_strapping_pin`。

    要解决此冲突，可参考以下选项：

    1.（推荐）烧录选择 flash 电压的 eFuse。该操作会将内部调节器的输出电压永久配置为 3.3 V，并且 GPIO12 将不再用作 strapping 管脚。烧录完成后，将上拉电阻连接到 GPIO12。

    .. warning::

        烧录 eFuse 是不可逆的！上述问题清单可能已经过时，因此请确保要烧录的模组内嵌 3.3 V flash 芯片，可前往 https://www.espressif.com/ 进行确认。如果在带有 1.8 V flash 芯片的模组上烧录 3.3 V eFuse，模组将停止工作。

    如果确定要烧录 eFuse，请转到 ESP-IDF 目录下，通过 ``espefuse.py`` 工具运行以下命令：

    .. code-block:: bash

        components/esptool_py/esptool/espefuse.py set_flash_voltage 3.3V

    此命令将烧录 ``XPD_SDIO_TIEH``、``XPD_SDIO_FORCE`` 和 ``XPD_SDIO_REG`` eFuse。当以上三个 eFuse 被烧录为 1 时，内部 VDD_SDIO flash 电压调节器将被永久设置为 3.3 V。如果烧录成功，将看到以下日志：

    .. code-block:: bash

        espefuse.py v2.6
        Connecting....

        Enable internal flash voltage regulator (VDD_SDIO) to 3.3 V.
        The following eFuses are burned: XPD_SDIO_FORCE, XPD_SDIO_REG, XPD_SDIO_TIEH.
        This is an irreversible operation.
        Type 'BURN' (all capitals) to continue.
        BURN
        VDD_SDIO setting complete.

    要检查 eFuse 的状态，请运行：

    .. code-block:: none

        idf.py efuse-summary

    如果运行自动烧录脚本，最好使用独立的 eFuse 工具 ``espefuse.py``。此工具还具有 ``--do-not-confirm`` 选项，可在无需确认的情况下烧录 eFuse。

    有关烧录 eFuse 的更多信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** [`PDF <{IDF_TARGET_TRM_CN_URL}#efuse>`__]。

    2. **如果使用 1 位 SD 模式或 SPI 模式**，请断开 DAT2 管脚并确保其被拉高。可以采取下列两个选项之一：

    - 使主机的 DAT2 管脚悬空，并将从机设备的 DAT2 直接连接到 VDD。
    - 对于从机设备，构建具有 ``SDIO_SLAVE_FLAG_DAT2_DISABLED`` 选项的固件并重新烧录设备，此选项可避免从机设备检测 DAT2 线。请注意，CCCR 寄存器将不再支持 4 位 SD 模式，但主机并不知晓这一点，因此必须在主机端禁用 4 位 SD 模式。


    .. _no_pull-up_on_gpio12:

    GPIO12 无上拉电阻
    ^^^^^^^^^^^^^^^^^

    所用模组与 SDIO 协议兼容，只需通过 10 kΩ 电阻将 GPIO12 连接到 VDD 即可。


    .. _gpio2_strapping_pin:

    下载模式不起效（次要问题）
    ^^^^^^^^^^^^^^^^^^^^^^^^^^

    当 GPIO2 管脚根据 SD 上拉需求被拉高时，将无法进入下载模式，因为 GPIO2 是一个 strapping 管脚，下载模式下必须被拉低。

    可参考以下解决方案：

    - 对于需要用跳线短接 GPIO0 和 GPIO2 管脚的开发板，跳线连接完成后，自动复位电路会在进入下载模式前将 GPIO2 与 GPIO0 一起拉低。
    - 对于 GPIO2 管脚上连有组件（例如下拉电阻和/或 LED）的开发板，请查看开发板的原理图，确认连接到 GPIO2 上组件的具体情况。

        - **LED** 大多数情况下不会影响操作。
        - **下拉电阻** 可能会干扰 DAT0 信号，必须移除。

    如果上述解决方案无效，请检查主机或从机设备是否带有会影响 GPIO2 的上拉电阻，如有，将其移除。


.. only:: esp32

    .. _related_info_sdio:

    相关信息
    --------

    .. _mtdi_strapping_pin:

    MTDI strapping 管脚
    ^^^^^^^^^^^^^^^^^^^

    MTDI (GPIO12) 为 strapping 管脚，用于选择内部调节器 (VDD_SDIO) 的输出电压，为 flash 芯片供电。该管脚带有内部下拉电阻，因此若处于未连接状态，MTDI 管脚将在启动时读取低电平，选择默认的 3.3 V 操作。

    除 ESP32-WROVER-B 外的所有 ESP32-WROVER 模组都使用 1.8 V flash，并在 GPIO12 上有内部上拉电阻。使用 3.3 V flash 的其他模组在 GPIO12 管脚上没有上拉电阻，并在芯片内部 GPIO12 管脚默认使能弱下拉。

    为 GPIO12 添加上拉电阻以进行 SD 卡操作时，请考虑以下事项：

    - 在不使用内部调节器 (VDD_SDIO) 为 flash 供电的开发板上，GPIO12 可以被拉高。
    - 在使用 1.8 V flash 芯片的开发板上，复位时 GPIO12 需要被拉高，这与 SD 卡操作完全兼容。
    - 在使用内部调节器和 3.3 V flash 芯片的开发板上，复位时 GPIO12 必须被拉低，这与 SD 卡操作不兼容。有关乐鑫开发板与 SD 卡操作之间的兼容性问题，详情请参阅 :ref:`compatibility_overview_espressif_hw_sdio`。


    内部上拉和 strapping 需求
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    使用外部电阻始终是更优选择。乐鑫产品中的内部弱上拉和下拉电阻启用后可以代替外部电阻，但此解决方案并不能保证可靠的 SDIO 通信。

    话虽如此，有关内部上拉和 strapping 需求的信息仍有一些参考价值。乐鑫硬件产品不同管脚（CMD 和 DATA 等）的内部弱上拉和弱下拉电阻的默认连接情况不同。下表展示了 CMD 和 DATA 管脚的默认上拉和下拉状态。

    表中使用了以下缩写：

    - **WPU**：SoC 内部的弱上拉电阻
    - **WPD**：SoC 内部的弱下拉电阻
    - **PU**：位于乐鑫模组内部、SoC 外部的上拉电阻

    .. list-table:: CMD 和 DATA 管脚的默认上拉和下拉状态
       :widths: 25 25 25 25
       :header-rows: 1

       * - GPIO 编号
         - 管脚名
         - 启动状态
         - strapping 需求
       * - **15**
         - CMD
         - WPU
         -
       * - **2**
         - DAT0
         - WPD
         - 下载模式要求低电平
       * - **4**
         - DAT1
         - WPD
         -
       * - **12**
         - DAT2
         - 使用 1.8 V flash 时为 PU；3.3 V flash 时为 WPD
         - 使用 1.8 V flash 时要求高电平；3.3 V flash 时要求低电平
       * - **13**
         - DAT3
         - WPU
         -

.. only:: not esp32

    .. _related_info_sdio:

    相关信息
    --------

    内部上拉和 strapping 需求
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    使用外部电阻始终是更优选择。乐鑫产品中的内部弱上拉和下拉电阻启用后可以代替外部电阻，但此解决方案并不能保证可靠的 SDIO 通信。

    通常不建议将 strapping 管脚重复用于 SDIO 功能。SD 和 strapping 管脚的上拉、下拉需求可能会相互冲突。有关 {IDF_TARGET} strapping 管脚的详细信息，请参阅技术规格书。
