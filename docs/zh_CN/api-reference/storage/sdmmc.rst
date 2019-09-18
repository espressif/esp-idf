SD/SDIO/MMC 驱动程序
=========================

:link_to_translation:`en:[English]`

概述
--------

SD/SDIO/MMC 驱动是一种基于 SDMMC 和 SD SPI 主机驱动的协议级驱动程序，目前已支持 SD 存储器、SDIO 卡和 eMMC 芯片。

SDMMC 主机驱动和 SD SPI 主机驱动（:component:`driver/include/driver/sdmmc_host.h`）为以下功能提供 API：

- 发送命令至从设备
- 接收和发送数据
- 处理总线错误

初始化函数及配置函数：

- 如需初始化和配置 SDMMC 主机，请参阅 :doc:`SDMMC 主机 API <../peripherals/sdmmc_host>`
- 如需初始化和配置 SD SPI 主机，请参阅 :doc:`SD SPI 主机 API <../peripherals/sdspi_host>`

本文档中所述的 SDMMC 协议层仅处理 SD 协议相关事项，例如卡初始化和数据传输命令。

协议层通过 :cpp:class:`sdmmc_host_t` 结构体和主机协同工作，该结构体包含指向主机各类函数的指针。

应用示例
-------------------

ESP-IDF :example:`storage/sd_card` 目录下提供了 SDMMC 驱动与 FatFs 库组合使用的示例，演示了先初始化卡，然后使用 POSIX 和 C 库 API 向卡读写数据。请参考示例目录下 README.md 文件，查看更多详细信息。

协议层 API
------------------

协议层具备 :cpp:class:`sdmmc_host_t` 结构体，此结构体描述了 SD/MMC 主机驱动，列出了其功能，并提供指向驱动程序函数的指针。协议层将卡信息储存于 :cpp:class:`sdmmc_card_t` 结构体中。向 SD/MMC 主机发送命令时，协议层调用时需要一个 :cpp:class:`sdmmc_command_t` 结构体来描述命令、参数、预期返回值和需传输的数据（如有）。 

用于 SD 存储卡的 API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. 初始化主机，请调用主机驱动函数，例如 :cpp:func:`sdmmc_host_init` 和 :cpp:func:`sdmmc_host_init_slot`；
2. 初始化卡，请调用 :cpp:func:`sdmmc_card_init`，并将参数 ``host`` （即主机驱动信息）和参数 ``card`` （指向 :cpp:class:`sdmmc_card_t` 结构体的指针）传递给此函数。函数运行结束后，将会向 :cpp:class:`sdmmc_card_t` 结构体填充该卡的信息；
3. 读取或写入卡的扇区，请分别调用 :cpp:func:`sdmmc_read_sectors` 和 :cpp:func:`sdmmc_write_sectors`，并将参数 ``card`` （指向卡信息结构的指针）传递给函数； 
4. 如果不再使用该卡，请调用主机驱动函数，例如 :cpp:func:`sdmmc_host_deinit`，以禁用主机外设，并释放驱动程序分配的资源。

用于 eMMC 芯片的 API
^^^^^^^^^^^^^^^^^^^^^^^^^

从协议层的角度而言，eMMC 存储芯片与 SD 存储卡相同。尽管 eMMC 是芯片，不具备卡的外形，但由于协议相似 (`sdmmc_card_t`, `sdmmc_card_init`)，用于 SD 卡的一些概念同样适用于 eMMC 芯片。注意，eMMC 芯片不可通过 SPI 使用，因此它与 SD API 主机驱动不兼容。	

如需初始化 eMMC 内存并执行读/写操作，请参照上一章节 SD 卡操作步骤。

用于 SDIO 卡的 API
^^^^^^^^^^^^^^^^^^^^^^^^^

SDIO 卡初始化和检测过程与 SD 存储卡相同，唯一的区别是 SDIO 模式下数据传输命令不同。

在卡初始化和卡检测（通过运行 :cpp:func:`sdmmc_card_init`）期间，驱动仅配置 SDIO 卡如下寄存器：

1. I/O 中止 (0x06) 寄存器：在该寄存器中设置 RES 位可重置卡的 I/O 部分；
2. 总线接口控制 (0x07) 寄存器：如果主机和插槽配置中启用 4 线模式，则驱动程序会尝试在该寄存器中设置总线宽度字段。如果字段设置成功，则从机支持 4 线模式，主机也切换至 4 线模式；
3. 高速（0x13）寄存器：如果主机配置中启用高速模式，则会在该寄存器中设置 SHS 位。

注意，驱动程序不会在 (1) I/O 使能寄存器和 Int 使能寄存器，及 (2) I/O 块大小中，设置任何位。应用程序可通过调用 :cpp:func:`sdmmc_io_write_byte` 来设置相关位。

如需设置卡配置或传输数据，请根据您的具体情况选择下表中的函数：

=========================================================================  =================================  =================================
操作                                                                         读函数                             写函数                  
=========================================================================  =================================  =================================
使用 IO_RW_DIRECT (CMD52) 读写单个字节。                                   :cpp:func:`sdmmc_io_read_byte`     :cpp:func:`sdmmc_io_write_byte`
使用 IO_RW_EXTENDED (CMD53) 的字节模式读写多个字节。                       :cpp:func:`sdmmc_io_read_bytes`    :cpp:func:`sdmmc_io_write_bytes`
块模式下，使用 IO_RW_EXTENDED (CMD53) 读写数据块。                          :cpp:func:`sdmmc_io_read_blocks`   :cpp:func:`sdmmc_io_write_blocks`
=========================================================================  =================================  =================================

使用 :cpp:func:`sdmmc_io_enable_int` 函数，应用程序可启用 SDIO 中断。
 
在单线模式下使用 SDIO 时，还需要连接 D1 线来启用 SDIO 中断。

如果您需要应用程序保持等待直至发生 SDIO 中断，请使用 :cpp:func:`sdmmc_io_wait_int` 函数。


复合卡（存储 + SDIO）
^^^^^^^^^^^^^^^^^^^^^^^^^
 
该驱动程序不支持 SDIO 复合卡，复合卡会被视为 SDIO 卡。


线程安全
^^^^^^^^^^^^^

多数应用程序仅需在一个任务中使用协议层。因此，协议层在 :cpp:class:`sdmmc_card_t` 结构体或在访问 SDMMC 或 SD SPI 主机驱动程序时不使用任何类型的锁。这种锁通常在较高层级实现，例如文件系统驱动程序。

API 参考
-------------

.. include:: /_build/inc/sdmmc_cmd.inc

.. include:: /_build/inc/sdmmc_types.inc