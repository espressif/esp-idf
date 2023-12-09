SD SPI 主机驱动程序
=====================

:link_to_translation:`en:[English]`

概述
--------

SD SPI 主机驱动程序支持使用 SPI 主控驱动程序与一或多张 SD 卡通信，SPI 主控驱动程序则利用 SPI 主机实现功能。每张 SD 卡都通过一个 SD SPI 设备访问，相应设备以 SD SPI 设备句柄 :cpp:type:`sdspi_dev_handle_t` 表示。调用 :cpp:func:`sdspi_host_init_device` 将设备连接到 SPI 总线上时会返回所需 SPI 设备句柄。注意，在使用 SPI 总线前，需要先通过 :cpp:func:`spi_bus_initialize` 初始化总线。

.. only:: esp32

    SD SPI 主机驱动程序的命名模式参照 :doc:`sdmmc_host`，因为二者本身及支持的 API 都非常相似。

    SD SPI 驱动程序使用 SPI 模式访问 SD 卡，此时的吞吐量较低，但可以更自由地选择管脚。通过 GPIO 矩阵，SPI 外设的信号可以路由到 {IDF_TARGET_NAME} 上的任意管脚。反之，若使用 SDMMC 主机驱动程序（请参阅 :doc:`sdmmc_host`）通过 SD 1 位/4 位模式访问 SD 卡，则可以实现更高的吞吐量，但需要使用专用的 IO_MUX 管脚路由信号。

SD SPI 主机驱动程序基于 :doc:`spi_master` 实现。借助 SPI 主控驱动程序，SD 卡及其他 SPI 设备可以共享同一 SPI 总线。SPI 主机驱动程序将处理来自不同任务的独占访问。

SD SPI 驱动程序使用受软件控制的 CS 信号。

使用方法
----------

首先，使用宏 :c:macro:`SDSPI_DEVICE_CONFIG_DEFAULT` 初始化结构体 :cpp:type:`sdspi_device_config_t`，该结构体用于初始化 SD SPI 设备。该宏还会填充默认的管脚映射，与 SDMMC 主机驱动的管脚映射相同。随后根据需要，修改结构体中的主机和管脚配置。然后调用 ``sdspi_host_init_device`` 初始化 SD SPI 设备，并将其连接到所属的总线上。

接着，使用宏 :c:macro:`SDSPI_HOST_DEFAULT` 初始化结构体 :cpp:type:`sdmmc_host_t`，该结构体用于存储上层（SD/SDIO/MMC 驱动）的状态和配置信息。将结构体中的 ``slot`` 参数设置为从 ``sdspi_host_init_device`` 返回的 SD SPI 设备的 SD SPI 句柄。使用 :cpp:type:`sdmmc_host_t` 调用 ``sdmmc_card_init``，检测并初始化 SD 卡。

初始化完成后，即可使用 SD/SDIO/MMC 驱动函数访问 SD 卡。

其他细节
-------------

通常，大多数应用程序仅使用驱动程序的以下 API 函数：

- :cpp:func:`sdspi_host_init`
- :cpp:func:`sdspi_host_init_device`
- :cpp:func:`sdspi_host_remove_device`
- :cpp:func:`sdspi_host_deinit`

对于其他函数，大多由协议层的 SD/SDIO/MMC 驱动程序通过 :cpp:type:`sdmmc_host_t` 结构体中的函数指针使用。更多详情，请参阅 :doc:`../storage/sdmmc`。

.. note::

    由于 SPI 驱动程序的限制，SD 卡在通过 SPI 总线与主设备进行数据传输和通信时，速度不超过 :c:macro:`SDMMC_FREQ_DEFAULT`。

.. warning::

    在 SD 卡之间以及其他 SPI 设备间共享 SPI 总线时，存在部分限制，详情请参阅 :doc:`sdspi_share`。

.. todo

..    The SD SPI API reference could use more detail such as:
..    - Configuration. What are some key points of concern regarding slot configuration.
..    - Which function/how is a transaction done?
..    - Are there code snippets or corresponding application examples?

相关文档
--------------

.. toctree::
   :maxdepth: 1

   sdspi_share


API 参考
-------------

.. include-build-file:: inc/sdspi_host.inc
