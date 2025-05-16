SD/SDIO/MMC 驱动程序
=========================

:link_to_translation:`en:[English]`

概述
--------

SD/SDIO/MMC 驱动支持 SD 存储器、SDIO 卡和 eMMC 芯片。这是一个协议层驱动 (:component_file:`sdmmc/include/sdmmc_cmd.h`)，可以与以下驱动配合使用：

.. list::
    :SOC_SDMMC_HOST_SUPPORTED: - SDMMC 主机驱动 (:component_file:`esp_driver_sdmmc/legacy/include/driver/sdmmc_host.h`)，详情请参阅 :doc:`SDMMC Host API <../peripherals/sdmmc_host>`。
    :SOC_GPSPI_SUPPORTED: - SDSPI 主机驱动 (:component_file:`esp_driver_sdspi/include/driver/sdspi_host.h`)，详情请参阅 :doc:`SD SPI Host API <../peripherals/sdspi_host>`。

协议层与主机层
^^^^^^^^^^^^^^

本文中的 SDMMC 协议层能处理 SD 协议的具体细节，例如卡初始化流程和各种数据传输命令流程。该协议层通过 :cpp:class:`sdmmc_host_t` 结构体与主机通信。该结构体包含指向主机各种功能的指针。

主机驱动通过支持以下功能来实现协议驱动：

- 发送命令至从设备
- 接收和发送数据
- 处理总线错误

.. blockdiag:: /../_static/diagrams/sd/sd_arch.diag
    :scale: 100%
    :caption: SD 主机端组件架构
    :align: center


应用示例
-------------------

.. list::

    :SOC_SDMMC_HOST_SUPPORTED: - :example:`storage/sd_card/sdmmc` 演示了如何通过 SDMMC 接口操作使用 FatFS 文件系统格式化的 SD 卡。

    :SOC_SDMMC_HOST_SUPPORTED: - :example:`storage/emmc` 演示了如何通过 SDMMC 接口操作使用 FatFS 文件系统格式化的 eMMC 芯片。

    :SOC_GPSPI_SUPPORTED: - :example:`storage/sd_card/sdspi` 演示了如何通过 SPI 接口操作使用 FatFS 文件系统格式化的 SD 卡。

协议层 API
------------------

协议层具备 :cpp:class:`sdmmc_host_t` 结构体，此结构体描述了 SD/MMC 主机驱动，列出了其功能，并提供指向驱动程序函数的指针。协议层将卡信息储存于 :cpp:class:`sdmmc_card_t` 结构体中。向 SD/MMC 主机发送命令时，协议层使用 :cpp:class:`sdmmc_command_t` 结构体来描述命令、参数、预期返回值和需传输的数据（如有）。

用于 SD 存储卡的 API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list::
    :SOC_SDMMC_HOST_SUPPORTED: - 初始化 SDMMC 主机，请调用主机驱动函数，例如 :cpp:func:`sdmmc_host_init` 和 :cpp:func:`sdmmc_host_init_slot`。
    :SOC_GPSPI_SUPPORTED: - 初始化 SDSPI 主机，请调用主机驱动函数，例如 :cpp:func:`sdspi_host_init` 和 :cpp:func:`sdspi_host_init_slot`。
    - 初始化卡，请调用 :cpp:func:`sdmmc_card_init`，并将参数 ``host`` （主机驱动信息）和参数 ``card`` （指向 :cpp:class:`sdmmc_card_t` 结构体的指针）传递给此函数。函数运行结束后，将会向 :cpp:class:`sdmmc_card_t` 结构体填充该卡的信息。
    - 读取或写入卡的扇区，请分别调用 :cpp:func:`sdmmc_read_sectors` 和 :cpp:func:`sdmmc_write_sectors`，并将参数 ``card`` （指向卡信息结构的指针）传递给函数。

    - 如果不再使用该卡，请调用主机驱动函数，例如 ``sdmmc_host_deinit`` 或 ``sdspi_host_deinit``，以禁用SDMMC 主机外设或 SDSPI 主机外设，并释放驱动程序分配的资源。

.. only:: not SOC_SDMMC_HOST_SUPPORTED

    eMMC 芯片支持
    ^^^^^^^^^^^^^^^^

    {IDF_TARGET_NAME} 没有 SDMMC 主机控制器，只能使用 SPI 协议与卡通信。然而，eMMC 芯片不能通过 SPI 使用。因此，无法在 {IDF_TARGET_NAME} 上使用 eMMC 芯片。

.. only:: SOC_SDMMC_HOST_SUPPORTED

    用于 eMMC 芯片的 API
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    从协议层的角度而言，eMMC 存储芯片与 SD 存储卡相同。尽管 eMMC 是芯片，不具备卡的外形，但由于协议相似 (`sdmmc_card_t`, `sdmmc_card_init`)，用于 SD 卡的一些概念同样适用于 eMMC 芯片。注意，eMMC 芯片不可通过 SPI 使用，因此它与 SD SPI 主机驱动不兼容。

    如需初始化 eMMC 内存并执行读/写操作，请参照上一章节 SD 卡操作步骤。


    用于 SDIO 卡的 API
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    SDIO 卡初始化和检测过程与 SD 存储卡相同，唯一的区别是 SDIO 模式下数据传输命令不同。

    在卡初始化和卡检测（通过运行 :cpp:func:`sdmmc_card_init`）期间，驱动仅配置 IO 卡如下寄存器：

    1. I/O 中止 (0x06) 寄存器：在该寄存器中设置 RES 位可重置卡的 IO 部分；
    2. 总线接口控制 (0x07) 寄存器：如果主机和插槽配置中启用 4 线模式，则驱动程序会尝试在该寄存器中设置总线宽度字段。如果字段设置成功，则从机支持 4 线模式，主机也切换至 4 线模式；
    3. 高速 (0x13) 寄存器：如果主机配置中启用高速模式，则该寄存器的 SHS 位会被设置。

    注意，驱动程序不会在 (1) I/O 使能寄存器和 Int 使能寄存器，及 (2) I/O 块大小中，设置任何位。应用程序可通过调用 :cpp:func:`sdmmc_io_write_byte` 来设置相关位。

    如需卡配置或传输数据，请根据具体情况，选择下表函数：

    .. list-table::
       :widths: 55 25 20
       :header-rows: 1

       * - 操作
         - 函数读取
         - 函数写入
       * - 使用 IO_RW_DIRECT (CMD52) 读写单个字节
         - :cpp:func:`sdmmc_io_read_byte`
         - :cpp:func:`sdmmc_io_write_byte`
       * - 使用 IO_RW_EXTENDED (CMD53) 的字节模式读写多个字节
         - :cpp:func:`sdmmc_io_read_bytes`
         - :cpp:func:`sdmmc_io_write_bytes`
       * - 块模式下，使用 IO_RW_EXTENDED (CMD53) 读写数据块
         - :cpp:func:`sdmmc_io_read_blocks`
         - :cpp:func:`sdmmc_io_write_blocks`

    使用 :cpp:func:`sdmmc_io_enable_int` 函数，应用程序可启用 SDIO 中断。在单线模式下使用 SDIO 时，还需要连接 D1 线来启用 SDIO 中断。

    如果需要应用程序保持等待直至发生 SDIO 中断，请使用 :cpp:func:`sdmmc_io_wait_int` 函数。

    .. only:: esp32

        如果需要与 ESP32 的 SDIO 从设备通信，请使用 ESSL 组件（ESP 串行从设备链接）。请参阅 :doc:`/api-reference/protocols/esp_serial_slave_link` 和示例 :example:`peripherals/sdio/host`。

    复合卡（存储 + IO）
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    该驱动程序不支持 SD 复合卡，复合卡会被视为 IO 卡。


线程安全
^^^^^^^^^^^^^

多数应用程序仅需在一个任务中使用协议层。因此，协议层在 :cpp:class:`sdmmc_card_t` 结构体或在访问 SDMMC 或 SD SPI 主机驱动程序时不使用任何类型的锁。这种锁通常在较高层级实现，例如文件系统驱动程序。


API 参考
-------------

.. include-build-file:: inc/sdmmc_cmd.inc

.. include-build-file:: inc/sdmmc_types.inc
