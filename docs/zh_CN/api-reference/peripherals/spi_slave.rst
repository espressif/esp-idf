SPI 从机驱动程序
================

SPI 从机驱动程序控制在 {IDF_TARGET_NAME} 中作为从机的 SPI 外设。


{IDF_TARGET_NAME} 中 SPI 外设概述
-----------------------------------------------

.. only:: esp32 or esp32s2 or esp32s3

    {IDF_TARGET_NAME} 集成了 2 个通用的 SPI 控制器，可用作片外 SPI 主机驱动的从机节点。

    .. only:: esp32

        - SPI2，有时也称为 HSPI
        - SPI3，有时也称为 VSPI

    SPI2 和 SPI3 各自具有一个与之同名的独立总线信号。

.. only:: esp32c3 or esp32c2

    {IDF_TARGET_NAME} 集成了 1 个通用的 SPI 控制器，可用作片外 SPI 主机驱动的从机节点。该控制器为 SPI2，具有一个与之同名的独立总线信号。


术语
-----------

下表为 SPI 主机驱动的相关术语。

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
     - 指激活一条线的操作。反之，将线路恢复到非活动状态（回到空闲状态）的操作则称为 *去断言*。
   * - 传输事务 (Transaction)
     - 即主机断言从机设备的 CS 线，向从机设备传输数据，接着去断言 CS 线的过程。传输事务为原子操作，不可打断。
   * - 发射沿 (Launch Edge)
     - 源寄存器将信号 *发射* 到线路上的时钟边沿。
   * - 锁存沿 (Latch Edge)
     - 目的寄存器 *锁存* 信号的时钟边沿。


驱动程序的功能
---------------

{IDF_TARGET_MAX_DATA_BUF:default="64", esp32s2="72"}

SPI 从机驱动程序允许将 SPI 外设作为全双工设备使用。驱动程序可以发送/接收长度不超过 {IDF_TARGET_MAX_DATA_BUF} 字节的传输事务，或者利用 DMA 来发送/接收更长的传输事务。然而，存在一些与 DMA 有关的 :ref:`已知问题 <spi_dma_known_issues>`。


SPI 传输事务
----------------

主机断言 CS 线并在 SCLK 线上发出时钟脉冲时，一次全双工 SPI 传输事务就此开始。每个时钟脉冲都意味着通过 MOSI 线从主机转移一个数据位到从机设备上，并同时通过 MISO 线返回一个数据位。传输事务结束后，主机去断言 CS 线。

传输事务的属性由作为从机设备的 SPI 外设的配置结构体 :cpp:type:`spi_slave_interface_config_t` 和传输事务配置结构体 :cpp:type:`spi_slave_transaction_t` 决定。

由于并非每次传输事务都需要写入和读取数据，您可以选择配置 :cpp:type:`spi_transaction_t` 为仅 TX、仅 RX 或同时 TX 和 RX 传输事务。如果将 :cpp:member:`spi_slave_transaction_t::rx_buffer` 设置为 NULL，读取阶段将被跳过。如果将 :cpp:member:`spi_slave_transaction_t::tx_buffer` 设置为 NULL，则写入阶段将被跳过。

.. note::

    主机应在从机设备准备好接收数据之后再进行传输事务。建议使用另外一个 GPIO 管脚作为握手信号来同步设备。更多细节，请参阅 :ref:`transaction_interval`。


使用驱动程序
------------

- 调用函数 cpp:func:`spi_slave_initialize`，将 SPI 外设初始化为从机设备。请确保在 `bus_config` 中设置正确的 I/O 管脚，并将未使用的信号设置为 ``-1``。

.. only:: esp32

    如果传输事务的数据大于 32 字节，需要将参数 ``dma_chan`` 分别设置为 ``1`` 或 ``2`` 以使能 DMA 通道 1 或通道 2。若数据小于 32 字节，则应将 ``dma_chan`` 设为 ``0``。

.. only:: esp32s2

    如果传输事务的数据大于 32 字节，需要在主机上设置参数 ``dma_chan`` 以使能 DMA 通道。若数据小于 32 字节，则应将 ``dma_chan`` 设为 ``0``。

- 传输事务开始前，需用要求的事务参数填充一个或多个 :cpp:type:`spi_slave_transaction_t` 结构体。可以通过调用函数 :cpp:func:`spi_slave_queue_trans` 来将所有传输事务排进队列，并在稍后使用函数 :cpp:func:`spi_slave_get_trans_result` 查询结果；也可以将所有请求输入 :cpp:func:`spi_slave_transmit` 中单独处理。主机上的传输事务完成前，后两个函数将被阻塞，以便发送并接收队列中的数据。

-（可选）如需卸载 SPI 从机驱动程序，请调用 :cpp:func:`spi_slave_free`。


传输事务数据和主/从机长度不匹配
---------------------------------------------------

通常，通过从机设备进行传输的数据会被读取或写入到由 :cpp:member:`spi_slave_transaction_t::rx_buffer` 和 :cpp:member:`spi_slave_transaction_t::tx_buffer` 指示的大块内存中。可以配置 SPI 驱动程序，使用 DMA 进行传输。在这种情况下，则必须使用 ``pvPortMallocCaps(size, MALLOC_CAP_DMA)`` 将缓存区分配到具备 DMA 功能的内存中。

驱动程序可以读取或写入缓存区的数据量取决于 :cpp:member:`spi_slave_transaction_t::length`，但其并不会定义一次 SPI 传输的实际长度。传输事务的长度由主机的时钟线和 CS 线决定，且只有在传输事务完成后，才能从 :cpp:member:`spi_slave_transaction_t::trans_len` 中读取实际长度。

如果传输长度超过缓存区长度，则只有在 :cpp:member:`spi_slave_transaction_t::length` 中指定的初始比特数会被发送和接收。此时， :cpp:member:`spi_slave_transaction_t::trans_len` 被设置为 :cpp:member:`spi_slave_transaction_t::length` 而非实际传输事务长度。若需满足实际传输事务长度的要求，请将 :cpp:member:`spi_slave_transaction_t::length` 设置为大于 :cpp:member:`spi_slave_transaction_t::trans_len` 预期最大值的值。如果传输长度短于缓存区长度，则只传输与缓存区长度相等的数据。

.. only:: esp32

    GPIO 交换矩阵和 IO_MUX
    ----------------------

    {IDF_TARGET_NAME} 的大多数外设信号都直接连接到其专用的 IO_MUX 管脚。不过，也可以使用 GPIO 交换矩阵，将信号路由到任何可用的其他管脚。

    如果通过 GPIO 交换矩阵路由了至少一个信号，则所有信号都将通过 GPIO 交换矩阵路由。GPIO 交换矩阵以 80 MHz 的频率对所有信号进行采样，并在 GPIO 和外设之间进行传输。

    如果已经配置过驱动程序，所有的 SPI 信号都已路由到专用的 IO_MUX 管脚，或者根本没有连接到任何管脚，那么 GPIO 交换矩阵将被绕过。

    GPIO 交换矩阵提高了信号传输的灵活性，但也增大了 MISO 信号的输入延迟，导致违反 MISO 设置时间的可能性更高。如需 SPI 高速运行，请使用专用的 IO_MUX 管脚。

    .. note::

        更多有关 MISO 输入延迟对最大时钟频率影响的细节，请参阅 :ref:`timing_considerations`。

    下表列出了 SPI 总线的 IO_MUX 管脚。

    .. only:: esp32

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

    * 只有连接到总线上的第一个从机设备可以使用 CS0 管脚。


速度与时钟
-------------------------------

.. _transaction_interval:

传输事务间隔
^^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_NAME} 的 SPI 从机外设是由 CPU 控制的通用从机设备。与专用的从机相比，在内嵌 CPU 的 SPI 从机设备中，预定义寄存器的数量有限，所有的传输事务都必须由 CPU 处理。也就是说，传输和响应并不是实时的，且可能存在明显的延迟。

解决方案为，首先使用函数 :cpp:func:`spi_slave_queue_trans`，然后使用 :cpp:func:`spi_slave_get_trans_result`，来代替 :cpp:func:`spi_slave_transmit`。由此一来，可使从机设备的响应速度提高一倍。

您也可以配置一个 GPIO 管脚，当从机设备开始新一次传输事务前，它将通过该管脚向主机发出信号。示例代码存放在 :example:`peripherals/spi_slave` 目录下。


时钟频率要求
^^^^^^^^^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_MAX_FREQ:default="60", esp32="10", esp32s2="40"}

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
             - <11.4
           * - GPIO 交换矩阵
             - 68.75
             - <7.2

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

    如果启用 DMA，从机设备的发射沿会比正常时间提前半个 SPI 时钟周期，变为主机的实际锁存沿。在这种情况下，如果 GPIO 交换矩阵被绕过，数据采样的保持时间将是 68.75 ns，而非半个 SPI 时钟周期。如果使用了 GPIO 交换矩阵，保持时间将增加到 93.75 ns。主机应在锁存沿立即采样数据，或在 SPI 模式 1 或模式 3 中进行通信。如果您的主机无法满足上述时间要求，请在没有 DMA 的情况下初始化从机设备。


应用示例
-------------------

从机设备/主机通信的示例代码存放在 ESP-IDF 示例项目的 :example:`peripherals/spi_slave` 目录下。


API 参考
-------------

.. include-build-file:: inc/spi_slave.inc
