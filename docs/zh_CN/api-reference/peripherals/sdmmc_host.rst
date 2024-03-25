SDMMC 主机驱动
=================

:link_to_translation:`en:[English]`

概述
--------

{IDF_TARGET_NAME} 的 SDMMC 主机外设共有两个卡槽，用于插入 SD 卡、连接 SDIO 设备或连接 eMMC 芯片，每个卡槽均可单独使用。

.. only:: esp32

    - 卡槽 0 (:c:macro:`SDMMC_HOST_SLOT_0`) 为 8 位卡槽，使用 PIN MUX 中的 ``HS1_*`` 信号。
    - 卡槽 1 (:c:macro:`SDMMC_HOST_SLOT_1`) 为 4 位卡槽，使用 PIN MUX 中的 ``HS2_*`` 信号。

    这些卡槽通过 IO MUX 连接到 {IDF_TARGET_NAME} 的 GPIO，其管脚映射如下表所示。

    .. list-table::
        :header-rows: 1
        :widths: 20 40 40
        :align: center

        * - 信号
          - 卡槽 0
          - 卡槽 1
        * - CMD
          - GPIO11
          - GPIO15
        * - CLK
          - GPIO6
          - GPIO14
        * - D0
          - GPIO7
          - GPIO2
        * - D1
          - GPIO8
          - GPIO4
        * - D2
          - GPIO9
          - GPIO12
        * - D3
          - GPIO10
          - GPIO13
        * - D4
          - GPIO16
          -
        * - D5
          - GPIO17
          -
        * - D6
          - GPIO5
          -
        * - D7
          - GPIO18
          -
        * - CD
          - 来自 GPIO 交换矩阵的任意输入
          - 来自 GPIO 交换矩阵的任意输入
        * - WP
          - 来自 GPIO 交换矩阵的任意输入
          - 来自 GPIO 交换矩阵的任意输入

    可以通过 GPIO 交换矩阵将卡检测 (CD) 和写保护 (WP) 信号路由到任意管脚。为了保留这些管脚，需要先配置 :cpp:class:`sdmmc_slot_config_t` 结构体的 ``cd`` 和 ``wp``，再调用 :cpp:func:`sdmmc_host_init_slot`。注意，使用 SDIO 卡时，不建议指定 CD 管脚，因为 ESP32 中的 CD 信号也可能触发 SDIO 从机设备中断。

    .. warning::

        卡槽 0 使用的管脚 (``HS1_*``) 既用于连接主机上的 SD 卡插槽，也用于连接 ESP32-WROOM 和 ESP32-WROVER 模组中的 SPI flash 芯片，但这些管脚不能同时用于 SD 卡与 SPI flash。如需使用卡槽 0，请将 SPI flash 连接到其他管脚，并根据需要配置 eFuse。


.. only:: SOC_SDMMC_USE_GPIO_MATRIX

    卡槽 :c:macro:`SDMMC_HOST_SLOT_0` 和 :c:macro:`SDMMC_HOST_SLOT_1` 都支持 1、4、8 线的 SD 接口，这些卡槽通过 GPIO 交换矩阵连接到 {IDF_TARGET_NAME} 的 GPIO，即每个 SD 卡信号都可以使用任意 GPIO 连接。

.. only:: esp32p4

    - 卡槽 :c:macro:`SDMMC_HOST_SLOT_1` 通过GPIO矩阵连接。这意味着任何GPIO都可以用于每个SD卡信号。它适用于非UHS-I用途。
    - 卡槽 :c:macro:`SDMMC_HOST_SLOT_0` 专用于UHS-I模式，驱动程序中尚不支持该模式。

    目前SDMMC主机需要为IO电平提供外部电压参考。如果您自行购买ESP32P4芯片并计划使用SDMMC外设，请参阅 :ref:'wr-ctrl' 。

支持的速率模式
---------------------

SDMMC 主机驱动支持以下速率模式：

- 默认速率 (20 MHz)：对于 SD 卡，支持 1 线或 4 线传输；对于 3.3 V eMMC，支持 1 线、4 线或 8 线传输。
- 高速模式 (40 MHz)：对于 SD 卡，支持 1 线或 4 线传输；对于 3.3 V eMMC，支持 1 线、4 线或 8 线传输。
- 高速 DDR 模式 (40 MHz)：对于 3.3 V eMMC，支持 4 线传输。

当前尚不支持的速率模式：

- 高速 DDR 模式：不支持 8 线 eMMC 传输
- UHS-I 1.8 V 模式：不支持 4 线 SD 卡传输


使用 SDMMC 主机驱动
---------------------------

在大多数应用程序中，只有下列函数会被直接调用：

- :cpp:func:`sdmmc_host_init`
- :cpp:func:`sdmmc_host_init_slot`
- :cpp:func:`sdmmc_host_deinit`

其他函数将通过 :cpp:class:`sdmmc_host_t` 结构体中的函数指针由 SD/MMC 协议层调用，例如：

- :cpp:func:`sdmmc_host_set_bus_width`
- :cpp:func:`sdmmc_host_set_card_clk`
- :cpp:func:`sdmmc_host_do_transaction`


配置总线宽度和频率
-----------------------------------

使用 :cpp:class:`sdmmc_host_t` 和 :cpp:class:`sdmmc_slot_config_t` 的默认初始化配置，即 :c:macro:`SDMMC_HOST_DEFAULT` 和 :c:macro:`SDMMC_SLOT_CONFIG_DEFAULT` 时，SDMMC 主机驱动会尝试以当前卡所支持的最大总线宽度进行通信（SD 卡为 4 线，eMMC 为 8 线），并使用 20 MHz 的通信频率。

在支持 40 MHz 频率通信的设计中，可以调整 :cpp:class:`sdmmc_host_t` 结构体中的 ``max_freq_khz`` 字段，提升总线频率：

.. code-block::

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

如需选择标准速率以外的特定频率，请根据所使用的 SD 接口（SDMMC 或 SDSPI）确定适当频率范围，并选择其中的任意值。然而，实际的时钟频率会由底层驱动程序计算，可能与你所需的值不同。

使用 SDMMC 接口时，``max_freq_khz`` 即频率上限，因此最终的频率值应始终低于该上限。而使用 SDSPI 接口时，驱动程序会提供最接近的适配频率，因此该值可以大于、等于或小于 ``max_freq_khz``。

请配置 :cpp:class:`sdmmc_slot_config_t` 的 ``width`` 字段，配置总线宽度。例如，配置 1 线模式的代码如下：

.. code-block::

    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    slot.width = 1;

.. only:: SOC_SDMMC_USE_GPIO_MATRIX

    配置 GPIO
    -----------------

    通过配置结构体 :cpp:class:`sdmmc_slot_config_t`，{IDF_TARGET_NAME} 的 SDMMC 主机可以根据需要，为每个信号配置任意的 GPIO 管脚。

    例如，使用以下代码，可以将 GPIO 1-6 分别用于 CLK、CMD、D0-D3 信号：

    .. code-block::

        sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
        slot.clk = GPIO_NUM_1;
        slot.cmd = GPIO_NUM_2;
        slot.d0 = GPIO_NUM_3;
        slot.d1 = GPIO_NUM_4;
        slot.d2 = GPIO_NUM_5;
        slot.d3 = GPIO_NUM_6;

    也可以配置 CD 和 WP 管脚。与配置其他信号的方法类似，你只需配置相同结构体的 ``cd`` 和 ``wp`` 参数：

    .. code-block::

        slot.cd = GPIO_NUM_7;
        slot.wp = GPIO_NUM_8;

    ``SDMMC_SLOT_CONFIG_DEFAULT`` 将 CD 和 WP 管脚都配置为 ``GPIO_NUM_NC``，表明默认情况下不会使用这两个管脚。

    通过上述方式初始化 :cpp:class:`sdmmc_slot_config_t` 结构体后，即可在调用 :cpp:func:`sdmmc_host_init_slot` 或其他任意高层函数（如 :cpp:func:`esp_vfs_fat_sdmmc_mount`）时使用该结构体。

.. only:: SOC_SDMMC_IO_POWER_EXTERNAL

    .. _pwr-ctrl:

    配置供电和参考电压
    ------------------

    {IDF_TARGET_NAME} SDMMC主机需要为IO电平提供外部电压参考 ，以支持高速设备， 驱动器将动态配置电压参考。您可以使用片上可编程LDO作为从机电源和电压参考 ，也可以提供正确的外部电源。

    - 要使用片上LDO ，请确保 VDDPST_5(sd_vref) 引脚连接到所选的片上LD通道 ，并调用 :cpp:func:'sd_pwr_ctrl_new_on_chip_ldo' 分配所选的LDO通道 ，然后将 'pwr_ctr_handle' 传递给 :cpp:class:'sdmmc_host_t::pwr_ctl_handle' 。
    - 要使用外部电源，请确保 VDDPST_5(sd_vref) 引脚已连接，然后按照 :cpp:class:'sd_pwr_ctrl_drv_t' 构造外部电源控制结构体，并将其传递给 :cpp:class:'sdmmc_host_t::pwr_ctr_handle'。

eMMC 芯片的 DDR 模式
-----------------------

默认情况下，如果满足以下条件，将使用 DDR 模式：

- 在 :cpp:class:`sdmmc_host_t` 结构体中将 SDMMC 主机频率配置为 :c:macro:`SDMMC_FREQ_HIGHSPEED`，且
- eMMC 芯片在其 CSD 寄存器中报告支持 DDR 模式

DDR 模式对信号完整性要求更高。如果要在保持 :c:macro:`SDMMC_FREQ_HIGHSPEED` 频率的同时禁用 DDR 模式，请在 :cpp:class:`sdmmc_host_t` 结构体的 :cpp:member:`sdmmc_host_t::flags` 字段中清除 :c:macro:`SDMMC_HOST_FLAG_DDR` 位：

.. code-block::

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    host.flags &= ~SDMMC_HOST_FLAG_DDR;


相关文档
--------

- :doc:`../storage/sdmmc`：介绍了实现协议层的高层驱动程序。
- :doc:`sdspi_host`：介绍了一种类似驱动，该驱动使用 SPI 控制器且受限于 SD 协议的 SPI 模式。
- :doc:`sd_pullup_requirements` 介绍了模组和开发套件上的上拉支持和兼容信息。


API 参考
-------------

.. include-build-file:: inc/sdmmc_host.inc
