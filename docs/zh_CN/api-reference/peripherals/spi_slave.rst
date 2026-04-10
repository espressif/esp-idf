SPI 从机驱动程序
================

:link_to_translation:`en:[English]`

SPI 从机驱动程序控制在 {IDF_TARGET_NAME} 中作为从机的 GP-SPI 外设。

有关 GP-SPI 硬件相关信息，请参考 **{IDF_TARGET_NAME} 技术参考手册** > **SPI 控制器** [`PDF <{IDF_TARGET_TRM_CN_URL}#spi>`__]。

术语
-----------

下表为 SPI 从机驱动的相关术语。

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 术语
     - 定义
   * - 主机 (Host)
     - {IDF_TARGET_NAME} 外部的 SPI 控制器外设。用作 SPI 主机，在总线上发起 SPI 传输。
   * - 从机设备 (Device)
     - SPI 从机设备（通用 SPI 控制器）。每个从机设备共享 MOSI、MISO 和 SCLK 信号，但只有当主机向从机设备的专属 CS 线发出信号时，从机设备才会在总线上处于激活状态。
   * - 总线 (Bus)
     - 信号总线，由连接到同一主机的所有从机设备共用。一般来说，一条总线包括以下线路：MISO、MOSI、SCLK、一条或多条 CS 线，以及可选的 QUADWP 和 QUADHD。每个从机设备都有单独的 CS 线，除此之外，所有从机设备都连接在相同的线路下。如果以菊花链的方式连接，几个从机设备也可以共享一条 CS 线。
   * - MISO
     - 主机输入，从机输出，也写作 Q。数据从从机设备发送至主机。
   * - MOSI
     - 主机输出，从机输入，也写作 D。数据从主机发送至从机设备。
   * - SCLK
     - 串行时钟。由主机产生的振荡信号，使数据位的传输保持同步。
   * - CS
     - 片选。允许主机选择连接到总线上的单个从机设备，以便发送或接收数据。
   * - QUADWP
     - 写保护信号。只用于 4 位 (qio/qout) 传输。
   * - QUADHD
     - 保持信号。只用于 4 位 (qio/qout) 传输。
   * - 断言 (Assertion)
     - 指激活一条线的操作。反之，将线路恢复到非活动状态（回到空闲状态）的操作则称为 **去断言**。
   * - 传输事务 (Transaction)
     - 即主机断言从机设备的 CS 线，向从机设备传输数据，接着去断言 CS 线的过程。传输事务为原子操作，不可打断。
   * - 发射沿 (Launch Edge)
     - 源寄存器将信号 **发射** 到线路上的时钟边沿。
   * - 锁存沿 (Latch Edge)
     - 目的寄存器 **锁存** 信号的时钟边沿。


驱动程序的功能
---------------

{IDF_TARGET_MAX_DATA_BUF:default="64", esp32s2="72"}

SPI 从机驱动程序允许将 SPI 外设作为全双工设备使用。驱动程序可以发送/接收长度不超过 {IDF_TARGET_MAX_DATA_BUF} 字节的传输事务，或者利用 DMA 来发送/接收更长的传输事务。然而，存在一些与 DMA 有关的 :ref:`已知问题 <spi_dma_known_issues>`。

SPI 从机驱动程序支持将 SPI ISR 注册至指定 CPU 内核。如果多个任务同时尝试访问一个 SPI 设备，建议重构应用程序，以使每个 SPI 外设一次只由一个任务访问。此外，请使用 :cpp:member:`spi_bus_config_t::isr_cpu_id` 将 SPI ISR 注册至与 SPI 外设相关任务相同的内核，确保线程安全。

.. only:: SOC_SPI_SUPPORT_SLEEP_RETENTION

    睡眠保留
    ^^^^^^^^

    {IDF_TARGET_NAME} 支持在进入 **Light Sleep** 之前保留 SPI 寄存器中的内容，并在唤醒后恢复。即程序不需要在 **Light Sleep** 唤醒后重新配置 SPI。

    该特性可以通过置位配置中的 :c:macro:`SPICOMMON_BUSFLAG_SLP_ALLOW_PD` 标志位启用。启用后驱动允许系统在 Light Sleep 时对 SPI 掉电，同时保存寄存器配置。它可以帮助降低轻度睡眠时的功耗，但需要花费一些额外的存储来保存寄存器的配置。

    注意在 Slave 角色下，不支持在所有传输（发送和接收）未完成时进入睡眠，否则将会出错。

SPI 传输事务
----------------

主机断言 CS 线并在 SCLK 线上发出时钟脉冲时，一次全双工 SPI 传输事务就此开始。每个时钟脉冲都意味着通过 MOSI 线从主机转移一个数据位到从机设备上，并同时通过 MISO 线返回一个数据位。传输事务结束后，主机去断言 CS 线。

传输事务的属性由作为从机设备的 SPI 外设的配置结构体 :cpp:type:`spi_slave_interface_config_t` 和传输事务配置结构体 :cpp:type:`spi_slave_transaction_t` 决定。

由于并非每次传输事务都需要写入和读取数据，可以选择配置 :cpp:type:`spi_transaction_t` 为仅 TX、仅 RX 或同时 TX 和 RX 传输事务。如果将 :cpp:member:`spi_slave_transaction_t::rx_buffer` 设置为 ``NULL``，读取阶段将被跳过。与之类似，如果将 :cpp:member:`spi_slave_transaction_t::tx_buffer` 设置为 ``NULL``，则写入阶段将被跳过。

.. note::

    主机应在从机设备准备好接收数据之后再进行传输事务。建议使用另外一个 GPIO 管脚作为握手信号来同步设备。更多细节，请参阅 :ref:`transaction_interval`。

.. only:: SOC_PSRAM_DMA_CAPABLE

    使用 PSRAM 的传输
    ^^^^^^^^^^^^^^^^^^

    SPI Slave 驱动程序支持使用 PSRAM 进行传输。直接传入 PSRAM 地址作为 :cpp:member:`spi_slave_transaction_t::tx_buffer` 或 :cpp:member:`spi_slave_transaction_t::rx_buffer` 即可。对于 rx_buffer ，其地址和传输长度有对齐要求，使用 :cpp:func:`heap_caps_malloc` 分配内存可以自动处理对齐要求。对于不能控制的内存，也可以使用 :c:macro:`SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO` 标志位，驱动会自动从 PSRAM 重新分配满足要求的内存。

    请注意该功能共享 MSPI 总线带宽（总线频率 * 总线位宽），因此主机对该设备的传输带宽应小于 PSRAM 带宽，否则 **可能会丢失传输数据**，此时 ``spi_slave_transmit`` 函数将会返回 :c:macro:`ESP_ERR_INVALID_STATE` 错误。

使用驱动程序
------------

- 调用函数  :cpp:func:`spi_slave_initialize`，将 SPI 外设初始化为从机设备。请确保在 `bus_config` 中设置正确的 I/O 管脚，并将未使用的信号设置为 ``-1``。

.. only:: esp32

    如果传输事务的数据大于 32 字节，需要将参数 ``dma_chan`` 分别设置为 ``1`` 或 ``2`` 以使能 DMA 通道 1 或通道 2，否则应将 ``dma_chan`` 设为 ``0``。

.. only:: esp32s2

    如果传输事务的数据大于 32 字节，需要在主机上设置参数 ``dma_chan`` 以使能 DMA 通道。若数据小于 32 字节，则应将 ``dma_chan`` 设为 ``0``。

- 传输事务开始前，需用要求的事务参数填充一个或多个 :cpp:type:`spi_slave_transaction_t` 结构体。可以通过调用函数 :cpp:func:`spi_slave_queue_trans` 来将所有传输事务排进队列，并在稍后使用函数 :cpp:func:`spi_slave_get_trans_result` 查询结果；也可以将所有请求输入 :cpp:func:`spi_slave_transmit` 中单独处理。主机上的传输事务完成前，后两个函数将被阻塞，以便发送并接收队列中的数据。

- （可选）启用/禁用驱动程序功能：从机驱动程序支持在程序初始化后通过调用 :cpp:func:`spi_slave_disable` / :cpp:func:`spi_slave_enable` 来禁用/启用驱动程序，以便能够更改时钟或电源配置或休眠以节省电量。默认情况下，驱动程序在初始化后为“启用”状态。

- （可选）如需卸载 SPI 从机驱动程序，请调用 :cpp:func:`spi_slave_free`。


传输事务数据和主/从机长度不匹配
---------------------------------------------------

通常，通过从机设备进行传输的数据会被读取或写入到由 :cpp:member:`spi_slave_transaction_t::rx_buffer` 和 :cpp:member:`spi_slave_transaction_t::tx_buffer` 指示的大块内存中。可以配置 SPI 驱动程序，使用 DMA 进行传输。在这种情况下，则必须使用 ``pvPortMallocCaps(size, MALLOC_CAP_DMA)`` 将缓存区分配到具备 DMA 功能的内存中。

驱动程序可以读取或写入缓存区的数据量取决于 :cpp:member:`spi_slave_transaction_t::length`，但其并不会定义一次 SPI 传输的实际长度。传输事务的长度由主机的时钟线和 CS 线决定，且只有在传输事务完成后，才能从 :cpp:member:`spi_slave_transaction_t::trans_len` 中读取实际长度。

如果传输长度超过缓存区长度，则只有在 :cpp:member:`spi_slave_transaction_t::length` 中指定的初始比特数会被发送和接收。此时， :cpp:member:`spi_slave_transaction_t::trans_len` 被设置为 :cpp:member:`spi_slave_transaction_t::length` 而非实际传输事务长度。若需满足实际传输事务长度的要求，请将 :cpp:member:`spi_slave_transaction_t::length` 设置为大于 :cpp:member:`spi_slave_transaction_t::trans_len` 预期最大值的值。如果传输长度短于缓存区长度，则只传输与缓存区长度相等的数据。

GPIO 交换矩阵和 IO_MUX
----------------------

.. only:: esp32

    {IDF_TARGET_NAME} 的大多数外设信号都直接连接到其专用的 IO_MUX 管脚。不过，也可以使用 GPIO 交换矩阵，将信号路由到任何可用的其他管脚。

    如果通过 GPIO 交换矩阵路由了至少一个信号，则所有信号都将通过 GPIO 交换矩阵路由。如果已经配置过驱动程序，所有 SPI 信号都已路由到专用 IO_MUX 管脚，或者没有连接到任何管脚，那么 GPIO 交换矩阵将被绕过。

    GPIO 交换矩阵提高了信号传输的灵活性，但也增大了 MISO 信号的输入延迟，导致违反 MISO 设置时间的可能性更高。如需 SPI 高速运行，请使用专用的 IO_MUX 管脚。

    .. note::

        更多有关 MISO 输入延迟对最大时钟频率影响的细节，请参阅 :ref:`timing_considerations`。

    下表列出了 SPI 总线的 IO_MUX 管脚。

    .. list-table::
       :widths: 40 30 30
       :header-rows: 1

       * - 管脚名称
         - GPIO 编号 (SPI2)
         - GPIO 编号 (SPI3)
       * - CS0*
         - 15
         - 5
       * - SCLK
         - 14
         - 18
       * - MISO
         - 12
         - 19
       * - MOSI
         - 13
         - 23
       * - QUADWP
         - 2
         - 22
       * - QUADHD
         - 4
         - 21

.. only:: not esp32

    {IDF_TARGET_SPI2_IOMUX_PIN_CS:default="N/A",   esp32s2="10", esp32s3="10", esp32c2="10", esp32c3="10", esp32c6="16", esp32h2="1", esp32p4="7" , esp32c5="10", esp32c61="8", esp32h21="12"}
    {IDF_TARGET_SPI2_IOMUX_PIN_CLK:default="N/A",  esp32s2="12", esp32s3="12", esp32c2="6",  esp32c3="6",  esp32c6="6",  esp32h2="4", esp32p4="9" , esp32c5="6",  esp32c61="6", esp32h21="2"}
    {IDF_TARGET_SPI2_IOMUX_PIN_MOSI:default="N/A", esp32s2="11"  esp32s3="11", esp32c2="7"   esp32c3="7",  esp32c6="7",  esp32h2="5", esp32p4="8" , esp32c5="7",  esp32c61="7", esp32h21="3"}
    {IDF_TARGET_SPI2_IOMUX_PIN_MISO:default="N/A", esp32s2="13"  esp32s3="13", esp32c2="2"   esp32c3="2",  esp32c6="2",  esp32h2="0", esp32p4="10", esp32c5="2",  esp32c61="2", esp32h21="4"}
    {IDF_TARGET_SPI2_IOMUX_PIN_HD:default="N/A",   esp32s2="9"   esp32s3="9",  esp32c2="4"   esp32c3="4",  esp32c6="4",  esp32h2="3", esp32p4="6" , esp32c5="4",  esp32c61="3", esp32h21="1"}
    {IDF_TARGET_SPI2_IOMUX_PIN_WP:default="N/A",   esp32s2="14"  esp32s3="14", esp32c2="5"   esp32c3="5",  esp32c6="5",  esp32h2="2", esp32p4="11", esp32c5="5",  esp32c61="4", esp32h21="0"}

    {IDF_TARGET_NAME} 的大多数外设信号都直接连接到其专用的 IO_MUX 管脚。不过，也可以使用 GPIO 交换矩阵，将信号路由到任何可用的其他管脚。如果通过 GPIO 交换矩阵路由了至少一个信号，则所有信号都将通过 GPIO 交换矩阵路由。

    当 SPI 主机频率配置为 80 MHz 或更低时，则通过 GPIO 交换矩阵或 IO_MUX 路由 SPI 管脚效果相同。

    下表列出了 SPI 总线的 IO_MUX 管脚。

    .. list-table::
       :widths: 40 30
       :header-rows: 1

       * - 管脚名称
         - GPIO 编号 (SPI2)
       * - CS0
         - {IDF_TARGET_SPI2_IOMUX_PIN_CS}
       * - SCLK
         - {IDF_TARGET_SPI2_IOMUX_PIN_CLK}
       * - MISO
         - {IDF_TARGET_SPI2_IOMUX_PIN_MISO}
       * - MOSI
         - {IDF_TARGET_SPI2_IOMUX_PIN_MOSI}
       * - QUADWP
         - {IDF_TARGET_SPI2_IOMUX_PIN_WP}
       * - QUADHD
         - {IDF_TARGET_SPI2_IOMUX_PIN_HD}


速度与时钟
-------------------------------

.. _transaction_interval:

传输事务间隔
^^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_NAME} 的 SPI 从机外设是由 CPU 控制的通用从机设备。与专用的从机相比，在内嵌 CPU 的 SPI 从机设备中，预定义寄存器的数量有限，所有的传输事务都必须由 CPU 处理。也就是说，传输和响应并不是实时的，且可能存在明显的延迟。

解决方案为，首先使用函数 :cpp:func:`spi_slave_queue_trans`，然后使用 :cpp:func:`spi_slave_get_trans_result`，来代替 :cpp:func:`spi_slave_transmit`。由此一来，可使从机设备的响应速度提高一倍。

也可以配置一个 GPIO 管脚，当从机设备开始新一次传输事务前，它将通过该管脚向主机发出信号。示例代码存放在 :example:`peripherals/spi_slave` 目录下。


时钟频率要求
^^^^^^^^^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_MAX_FREQ:default="60", esp32="10", esp32s2="40", esp32c6="40", esp32h2="32"}

SPI 从机的工作频率最高可达 {IDF_TARGET_MAX_FREQ} MHz。如果时钟频率过快或占空比不足 50%，数据就无法被正确识别或接收。

.. only:: esp32

    除此之外，在数据方面还有一些额外要求，以满足时间限制：

    - 读取 (MOSI):
        只有当数据在主机的发射沿准备好时，从机设备才能正确读取数据。此为大多数主机的默认情况。

    - 写入 (MISO):
        MISO 信号的输出延迟应短于半个时钟周期，以确保 MISO 线在下一个锁存沿之前保持稳定。鉴于时钟同步，不同情况下的输出延迟和频率限制如下。

        .. list-table::
           :widths: 30 40 40
           :header-rows: 1

           * - /
             - MISO 输出延迟 (ns)
             - 频率限制 (MHz)
           * - IO_MUX
             - 43.75
             - < 11.4
           * - GPIO 交换矩阵
             - 68.75
             - < 7.2

        注：
        1. 如果频率达到上限，会导致随机误差。
        2. 主机和设备之间的时钟不确定性 (12.5 ns) 已被考虑在内。
        3. 表中的输出延迟为理想情况（无负载）下的数据。如果 MISO 管脚负载较重，则输出延迟更长，且最大允许频率更低。

        例外情况：如果主机支持更多相关 MISO 设置时间的选项，例如，可以在下一个边沿锁存数据，或可以配置锁存时间，则频率限制的上限会更高。


.. _spi_dma_known_issues:

限制条件和已知问题
-----------------------------

1. 若启用了 DMA，则 RX 缓冲区应该以字对齐（从 32 位边界开始，字节长度为 4 的倍数）。否则，DMA 可能无法正确写入或无法实现边界对齐。若此项条件不满足，驱动程序将会报错。

   此外，主机写入字节长度应为 4 的倍数。长度不符合的数据将被丢弃。

.. only:: esp32

    2. 此外，使用 DMA 时需启用 SPI 模式 1 和模式 3。在 SPI 模式 0 和模式 2 下，为满足时序要求，MISO 信号必须提前半个时钟周期启动。新的时序如下：

    .. wavedrom:: /../_static/diagrams/spi/spi_slave_miso_dma.json

    如果启用 DMA，从机设备的发射沿会比正常时间提前半个 SPI 时钟周期，变为主机的实际锁存沿。在这种情况下，如果 GPIO 交换矩阵被绕过，数据采样的保持时间将是 68.75 ns，而非半个 SPI 时钟周期。如果使用了 GPIO 交换矩阵，保持时间将增加到 93.75 ns。主机应在锁存沿立即采样数据，或在 SPI 模式 1 或模式 3 中进行通信。如果主机无法满足上述时间要求，请在没有 DMA 的情况下初始化从机设备。

    3. ESP32 SPI Slave 在 CS 片选未使能时 **仍然** 在 MISO 引脚输出电平 0/1, 这可能导致总线上其他设备无法正确输出数据，解决方法为：

      1) 为 ESP32 SPI Slave 单独使用一条总线，不与其他设备共享总线。
      2) 在 ESP32 SPI MISO 引脚和总线之间增加缓冲器芯片，如 74HC125。

应用示例
-------------------

如需查看从机/主机通信的代码示例，请前往 :example:`peripherals/spi_slave_hd` 目录。

- :example: `peripherals/spi_slave/receiver` 演示了如何配置 SPI 从机以接收来自 SPI 主机的数据，并通过握手机制管理数据传输的准备状态。

- :example: `peripherals/spi_slave/sender` 演示了如何配置 SPI 主机将数据发送到 SPI 从机，并通过握手来确保数据传输的时机恰当。


API 参考
------------

.. include-build-file:: inc/spi_slave.inc
