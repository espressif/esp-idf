SPI Flash 和片外 SPI RAM 配置
=============================

:link_to_translation:`en:[English]`

本文档提供配置 SPI flash 和片外 SPI RAM 的指南，还详细说明了支持的频率和模式组合，以及错误处理。

术语表
------

.. list-table::
    :header-rows: 1
    :widths: 20 80
    :align: center

    * - 术语
      - 定义
    * - **SPI**
      - 串行外设接口
    * - **MSPI**
      - 存储器 SPI 外设，专用于存储器的 SPI 外设
    * - **SDR**
      - 单倍数据传输速率 (SDR)，也被称为 STR（单次传输速率）
    * - **DDR**
      - 双倍数据传输速率 (DDR)，也被称为 DTR（双次传输速率）
    * - **行模式**
      - 在 SPI 事务的数据阶段，用来传输数据的信号数量。例如，4 位模式下，数据阶段的速度为每个时钟周期内加载 4 位数据。
    * - **FxRx**
      - F 代表 flash，R 代表 PSRAM，x 代表行模式。例如，F4R4 指的是具有四线 flash 和四线 PSRAM 的 {IDF_TARGET_NAME}。

.. note::

    在 {IDF_TARGET_NAME}上，MSPI 代表 SPI0/1，SPI0 和 SPI1 共享一个公共 SPI 总线。主 flash 和 PSRAM 连接到 MSPI 外设，CPU 通过 Cache 访问它们。


.. _flash-psram-configuration:

如何配置 Flash 和 PSRAM
-----------------------

运行 ``idf.py menuconfig``，打开配置菜单。

配置 Flash
^^^^^^^^^^

在 ``Serial flasher config`` 菜单下，可以找到 flash 相关的配置。

1. 选择在板上使用的 flash 类型。如果是八线 flash，请选择 :ref:`CONFIG_ESPTOOLPY_OCT_FLASH`；如果是四线 flash，则不必选择此配置。
2. 选择 flash 的行模式。在 :ref:`CONFIG_ESPTOOLPY_FLASHMODE` 中选择行模式，线模式越高，SPI 速度越快。有关行模式的术语，请参阅上述术语表。
3. 选择 flash 的采样模式。在 :ref:`CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE` 中选择采样模式，DDR 模式比 SDR 模式速度更快。有关 SDR 和 DDR 模式的术语，请参阅上述术语表。
4. 选择 flash 的速度。在 :ref:`CONFIG_ESPTOOLPY_FLASHFREQ` 中选择 flash 频率。
5. 选择 flash 的大小。在 :ref:`CONFIG_ESPTOOLPY_FLASHSIZE` 中选择 flash 的大小，以兆字节为单位。

配置 PSRAM
^^^^^^^^^^

要启动 PSRAM，请在 ``Component config / Hardware Settings`` 菜单下启用 :ref:`CONFIG_SPIRAM`。在 ``SPI RAM config`` 菜单下可以看到所有与 PSRAM 相关的配置。

1. 选择在板上使用的 PSRAM 类型。在 :ref:`CONFIG_SPIRAM_MODE` 中可以选择四线或八线 PSRAM。
2. 选择 PSRAM 的速度。在 :ref:`CONFIG_SPIRAM_SPEED` 中选择 PSRAM 的频率。

.. note::

    应根据实际硬件选择 flash 和 PSRAM 的配置 1。

    如果要重置上述配置：

    - flash 和 PSRAM 共享相同的内部时钟。
    - 四线 flash 仅支持 STR 模式。八线 flash 在 OPI 模式下可能支持 STR/DTR 模式中的一种或两种，具体取决于 flash 的型号和供应商。
    - 四线 PSRAM 仅支持 STR 模式，而八线 PSRAM 仅支持 DTR 模式。

    因此，在选择 flash 的配置 2、3、4 以及 PSRAM 的配置 2 时，应留意上述限制。更多信息请参阅 :ref:`所有支持的模式和速度 <flash-psram-combination>`。

.. note::

    如果配有八线 flash 的开发板在第二阶段引导加载程序之前复位，请参考 :ref:`错误处理章节 <flash-psram-error>`。


.. _flash-psram-combination:

所有支持的模式和速度
--------------------

.. note::

    在 MSPI DDR 模式下，数据在正边沿和负边沿都会被采样。例如，将 flash 设置为 80 MHz，DDR 模式，则 flash 的最终速度为 160 MHz，比直接将 flash 设置为 120 MHz，STR 模式更快。

.. important::

    120 MHz DDR 模式为实验性功能，仅在启用下述选项时才能实现：

     - :ref:`CONFIG_IDF_EXPERIMENTAL_FEATURES`

    通过上述步骤，就能看到 120 MHz 的选项。

    风险：

    如果芯片在某个温度下上电，当温度上升或下降超过 20 摄氏度后，访问 PSRAM/flash 或是从 PSRAM/flash 获取数据的操作将随机崩溃，而 flash 访问的崩溃将导致程序崩溃。

    请注意，20 摄氏度并不是一个完全准确的数字，这个值在不同芯片间可能会有所不同。

.. note::

    PSRAM 在 120M 运行时需要相位点校准算法。相位点设置与启动时的温度有关。当芯片运行期间温度大幅上升（下降）时，PSRAM 可能会出现读写错误。为解决这一问题，可以使能 :ref:`CONFIG_SPIRAM_TIMING_TUNING_POINT_VIA_TEMPERATURE_SENSOR`，根据温度值动态调整 PSRAM 相位点。这将创建一个任务，每隔 :ref:`CONFIG_SPIRAM_TIMING_MEASURE_TEMPERATURE_INTERVAL_SECOND` 秒测量一次温度，并相应调整 PSRAM 相位点。

F8R8 硬件
^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 20 30 20 30
    :align: center

    * - 组别
      - Flash 模式
      - 组别
      - PSRAM 模式
    * - A
      - 120 MHz DDR
      - A
      - 120 MHz DDR
    * - A
      - 120 MHz SDR
      - A
      -
    * - B
      - 80 MHz DDR
      - B
      - 80 MHz DDR
    * - C
      - 80 MHz SDR
      - C
      - 40 MHz DDR
    * - C
      - 40 MHz DDR
      - C
      -
    * - C
      - < 40 MHz
      - C
      -
    * - D
      -
      - D
      - 禁用

1. 组别 A 中的 flash 模式与组别 A/D 中的 PSRAM 模式配对。
2. 组别 B/C 中的 flash 模式与组别 B/C/D 中的 PSRAM 模式配对。


F4R8 硬件
^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 20 30 20 30
    :align: center

    * - 组别
      - Flash 模式
      - 组别
      - PSRAM 模式
    * - A
      - 120 MHz SDR
      - A
      - 120 MHz DDR
    * - B
      - 80 MHz SDR
      - B
      - 80 MHz DDR
    * - C
      - 40 MHz SDR
      - C
      - 40 MHz DDR
    * - C
      - 20 MHz SDR
      - C
      -
    * - D
      -
      - D
      - 禁用

1. 组别 A 中的 flash 模式与组别 A/D 中的 PSRAM 模式配对。
2. 组别 B/C 中的 flash 模式与组别 B/C/D 中的 PSRAM 模式配对。


F4R4 硬件
^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 20 30 20 30
    :align: center

    * - 组别
      - Flash 模式
      - 组别
      - PSRAM 模式
    * - A
      - 120 MHz
      - A
      - 120 MHz
    * - B
      - 80 MHz
      - B
      - 80 MHz
    * - C
      - 40 MHz
      - C
      - 40 MHz
    * - C
      - 20 MHz
      - C
      -
    * - D
      -
      - D
      - disable

1. 组别 A 中的 flash 模式 与组别 A/C/D 的 PSRAM 模式配对。
2. 组别 B 中的 flash 模式 与组别 B/C/D 的 PSRAM 模式配对。
3. 组别 C 中的 flash 模式 与组别 A/B/C/D 的 PSRAM 模式配对。


.. _flash-psram-error:

错误处理
--------

1. 如果配有八线 flash 的开发板在第二阶段引导加载程序之前复位:

    .. code-block:: c

        ESP-ROM:esp32s3-20210327
        Build:Mar 27 2021
        rst:0x7 (TG0WDT_SYS_RST),boot:0x18 (SPI_FAST_FLASH_BOOT)
        Saved PC:0x400454d5
        SPIWP:0xee
        mode:DOUT, clock div:1
        load:0x3fcd0108,len:0x171c
        ets_loader.c 78

   这可能意味着必要的 efuse 未得到正确烧录。请使用命令 ``espefuse.py summary``，检查芯片的 eFuse 位。

   ROM 引导加载程序可通过 eFuse 位 ``FLASH_TYPE`` 将 flash 复位为默认模式（SPI 模式）。如果未烧录此位，且 flash 处于 OPI 模式，则 ROM 引导加载程序可能无法从 flash 中读取并加载以下图像。

2. 如果启用 :ref:`CONFIG_ESPTOOLPY_OCT_FLASH` 后出现如下错误日志：

    .. code-block:: c

        Octal Flash option selected, but EFUSE not configured!

   这意味着：

   - 要么当前正在使用配有四线 flash 的开发板，
   - 要么当前正在使用带有八线 flash 的板，但未烧录 eFuse 位 ``FLASH_TYPE``。乐鑫保证在制造模组时烧录此位，但如果模组由其他公司制造，则可能遇到上述情况。


以下是烧录 eFuse 位的方法：

.. code-block:: shell

    idf.py -p PORT efuse-burn --do-not-confirm FLASH_TYPE 1

.. note::

    此步骤不可逆，请确保使用配有八线 flash 的开发板。
