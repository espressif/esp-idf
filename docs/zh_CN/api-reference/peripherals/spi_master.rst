SPI 主机驱动程序
===================

:link_to_translation:`en:[English]`

SPI 主机驱动程序是一个软件程序，用于在 {IDF_TARGET_NAME} 的通用 SPI (GP-SPI) 外设工作在主控模式时，对其进行控制。

.. only:: esp32

    .. note::

        SPI1 不属于 GP-SPI。SPI 主机驱动程序也支持 SPI1。但在 SPI1 总线上使用 SPI 主机驱动程序存在诸多限制，请参阅 :ref:`spi_master_on_spi1_bus`。

有关 GP-SPI 硬件相关信息，请参考 **{IDF_TARGET_NAME} 技术参考手册** > **SPI 控制器** [`PDF <{IDF_TARGET_TRM_CN_URL}#spi>`__]。

术语
--------

下表为 SPI 主机驱动的相关术语。

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 术语
     - 定义
   * - 主机 (Host)
     - {IDF_TARGET_NAME} 内置的 SPI 控制器外设。用作 SPI 主机，在总线上发起 SPI 传输。
   * - 设备 (Device)
     - SPI 从机设备。一条 SPI 总线与一或多个设备连接。每个设备共享 MOSI、MISO 和 SCLK 信号，但只有当主机向设备的专属 CS 线发出信号时，设备才会在总线上处于激活状态。
   * - 总线 (Bus)
     - 信号总线，由连接到同一主机的所有设备共用。一般来说，每条总线包括以下线：MISO、MOSI、SCLK、一条或多条 CS 线，以及可选的 QUADWP 和 QUADHD。因此，除每个设备都有单独的 CS 线外，所有设备都连接在相同的线下。多个设备也可以菊花链的方式共享一条 CS 线。
   * - MOSI
     - 主机输出，从机输入，也写作 D。数据从主机发送至设备。在 Octal/OPI 模式下也表示为 data0 信号。
   * - MISO
     - 主机输入，从机输出，也写作 Q。数据从设备发送至主机。在 Octal/OPI 模式下也表示为 data1 信号。
   * - SCLK
     - 串行时钟。由主机产生的振荡信号，使数据位的传输保持同步。
   * - CS
     - 片选。允许主机选择连接到总线上的单个设备，以便发送或接收数据。
   * - QUADWP
     - 写保护信号。只用于 4 位 (qio/qout) 传输。在 Octal/OPI 模式下也表示为 data2 信号。
   * - QUADHD
     - 保持信号。只用于 4 位 (qio/qout) 传输。在 Octal/OPI 模式下也表示为 data3 信号。
   * - DATA4
     - 在 Octal/OPI 模式下表示为 data4 信号。
   * - DATA5
     - 在 Octal/OPI 模式下表示为 data5 信号。
   * - DATA6
     - 在 Octal/OPI 模式下表示为 data6 信号。
   * - DATA7
     - 在 Octal/OPI 模式下表示为 data7 信号。
   * - 断言 (Assertion)
     - 指激活一条线路的操作。
   * - 去断言 (De-assertion)
     - 指将线路恢复到非活动状态（回到空闲状态）的操作。
   * - 传输事务 (Transaction)
     - 即主机断言设备的 CS 线，向设备发送数据/从设备读取数据，接着去断言 CS 线的过程。传输事务为原子操作，不可打断。
   * - 发射沿 (Launch Edge)
     - 源寄存器将信号 **发射** 到线路上的时钟边沿。
   * - 锁存沿 (Latch Edge)
     - 目的寄存器 **锁存** 信号的时钟边沿。


主机驱动特性
---------------

SPI 主机驱动程序负责管理主机与设备间的通信，具有以下特性：

- 支持多线程环境使用
- 读写数据过程中 DMA 透明传输
- 同一信号总线上不同设备的数据可自动时分复用，请参阅 :ref:`spi_bus_lock`。

.. warning::

    SPI 主机驱动允许总线上连接多个设备（共享单个 {IDF_TARGET_NAME} SPI 外设）。每个设备仅由一个任务访问时，驱动程序线程安全。反之，若多个任务尝试访问同一 SPI 设备，则驱动程序 **非线程安全**。此时，建议执行以下任一操作：

    - 重构应用程序，确保每个 SPI 外设在同一时间仅由一个任务访问。使用 :cpp:member:`spi_bus_config_t::isr_cpu_id` 将 SPI ISR 注册到与 SPI 外设相关任务相同的内核，以确保线程安全。
    - 使用 :c:macro:`xSemaphoreCreateMutex` 为共享设备添加互斥锁。

.. toctree::
   :hidden:

   SPI 特性 <spi_features>

SPI 传输事务
--------------------

SPI 总线传输事务由五个阶段构成，详见下表（任意阶段均可跳过）。

{IDF_TARGET_ADDR_LEN:default="32", esp32="64"}

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 阶段名称
     - 描述
   * - 命令阶段 (Command)
     - 在此阶段，主机向总线发送命令字段，长度为 0-16 位。
   * - 地址阶段 (Address)
     - 在此阶段，主机向总线发送地址字段，长度为 0-{IDF_TARGET_ADDR_LEN} 位。
   * - Dummy 阶段
     - 此阶段可自行配置，用于适配时序要求。
   * - 写入阶段 (Write)
     - 此阶段主机向设备传输数据，这些数据在紧随命令阶段（可选）和地址阶段（可选）之后。从电平的角度来看，数据与命令没有区别。
   * - 读取阶段 (Read)
     - 此阶段主机读取设备数据。

.. todo::

   Add a package diagram.


传输事务属性由总线配置结构体 :cpp:type:`spi_bus_config_t`、设备配置结构体 :cpp:type:`spi_device_interface_config_t` 和传输事务配置结构体 :cpp:type:`spi_transaction_t` 共同决定。

一个 SPI 主机可以发送全双工传输事务，此时读取和写入阶段同步进行。传输事务总长度取决于以下结构体成员长度总和：

- :cpp:member:`spi_device_interface_config_t::command_bits`
- :cpp:member:`spi_device_interface_config_t::address_bits`
- :cpp:member:`spi_transaction_t::length`

而 :cpp:member:`spi_transaction_t::rxlength` 则决定了接收到的数据包长度。

在半双工传输事务中，读取和写入阶段独立进行（一次一个方向）。写入和读取阶段的长度由 :cpp:member:`spi_transaction_t::length` 和 :cpp:member:`spi_transaction_t::rxlength` 分别决定。

并非每个 SPI 设备都要求命令和/或地址，因此命令阶段和地址阶段为可选项。这反映在设备的配置中：如果 :cpp:member:`spi_device_interface_config_t::command_bits` 和/或 :cpp:member:`spi_device_interface_config_t::address_bits` 被设置为零，则不会唤起命令或地址阶段。

并非每个传输事务都需要写入和读取数据，因此读取和写入阶段也是可选项。如果将 :cpp:member:`spi_transaction_t::rx_buffer` 设置为 ``NULL``，且未设置 :c:macro:`SPI_TRANS_USE_RXDATA`，读取阶段将被跳过。如果将 :cpp:member:`spi_transaction_t::tx_buffer` 设置为 ``NULL``，且未设置 :c:macro:`SPI_TRANS_USE_TXDATA`，写入阶段将被跳过。

主机驱动程序支持两种类型的传输事务：中断传输事务和轮询传输事务。用户可以选择在不同设备上使用不同的传输事务类型。若设备需要同时使用两种传输事务类型，请参阅 :ref:`mixed_transactions`。


.. _interrupt_transactions:

中断传输事务
^^^^^^^^^^^^^^^^^^^^^^

中断传输事务将阻塞传输事务程序，直至传输事务完成，以使 CPU 运行其他任务程序。

应用任务中可以将多个传输事务加入到队列中，驱动程序将在中断服务程序 (ISR) 中自动逐一发送队列中的数据。在所有传输事务完成以前，任务可切换到其他程序中。


.. _polling_transactions:

轮询传输事务
^^^^^^^^^^^^^^^^^^^^

轮询传输事务不依赖于中断，程序将不断轮询 SPI 主机的状态位，直到传输事务完成。

所有执行中断传输事务的任务都可能被队列阻塞。在此情况下，用户需要等待 ISR 和传输事务传输完成。轮询传输事务节省了原本用于队列处理和上下文切换的时间，减少了传输事务持续时间。传输事务类型的缺点是在这些事务进行期间，CPU 将被占用而处于忙碌状态。

传输事务完成后，:cpp:func:`spi_device_polling_end` 程序需要至少 1 µs 的时间来解除阻塞其他任务。此处强烈建议调用函数 :cpp:func:`spi_device_acquire_bus` 和 :cpp:func:`spi_device_release_bus` 来打包一系列轮询传输事务以避免开销。详情请参阅 :ref:`bus_acquiring`。

.. _transaction-line-mode:

传输线模式配置
^^^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_NAME} 支持的线路模式如下。要使用这些模式，请在结构体 :cpp:type:`spi_transaction_t` 中设置 ``flags``，如 ``传输事务标志信号`` 一栏所示。要检查相应的 IO 管脚是否被设置，请在 :cpp:type:`spi_bus_config_t` 中设置 ``flags``，如 ``总线 IO 设置标志信号`` 一栏所示。


.. only:: not SOC_SPI_SUPPORT_OCT

   .. list-table::
       :widths: 30 30 30 30 50 50
       :header-rows: 1

       * - 模式
         - 命令位宽
         - 地址位宽
         - 数据位宽
         - 传输事务标志信号
         - 总线 IO 设置标志信号
       * - 普通 SPI 模式
         - 1
         - 1
         - 1
         - 0
         - 0
       * - 双线输出模式
         - 1
         - 1
         - 2
         - SPI_TRANS_MODE_DIO
         - SPICOMMON_BUSFLAG_DUAL
       * - 双线 I/O 模式
         - 1
         - 2
         - 2
         - SPI_TRANS_MODE_DIO
           SPI_TRANS_MULTILINE_ADDR
         - SPICOMMON_BUSFLAG_DUAL
       * - 四线输出模式
         - 1
         - 1
         - 4
         - SPI_TRANS_MODE_QIO
         - SPICOMMON_BUSFLAG_QUAD
       * - 四线 I/O 模式
         - 1
         - 4
         - 4
         - SPI_TRANS_MODE_QIO
           SPI_TRANS_MULTILINE_ADDR
         - SPICOMMON_BUSFLAG_QUAD

.. only:: SOC_SPI_SUPPORT_OCT

    .. list-table::
       :widths: 30 40 40 40 50 50
       :header-rows: 1

       * - 模式
         - 命令位宽
         - 地址位宽
         - 数据位宽
         - 传输事务标志信号
         - 总线 IO 设置标志信号
       * - 普通 SPI 模式
         - 1
         - 1
         - 1
         - 0
         - 0
       * - 双线输出模式
         - 1
         - 1
         - 2
         - SPI_TRANS_MODE_DIO
         - SPICOMMON_BUSFLAG_DUAL
       * - 双线 I/O 模式
         - 1
         - 2
         - 2
         - SPI_TRANS_MODE_DIO
           SPI_TRANS_MULTILINE_ADDR
         - SPICOMMON_BUSFLAG_DUAL
       * - 四线输出模式
         - 1
         - 1
         - 4
         - SPI_TRANS_MODE_QIO
         - SPICOMMON_BUSFLAG_QUAD
       * - 四线 I/O 模式
         - 1
         - 4
         - 4
         - SPI_TRANS_MODE_QIO
           SPI_TRANS_MULTILINE_ADDR
         - SPICOMMON_BUSFLAG_QUAD
       * - 八线输出模式
         - 1
         - 1
         - 8
         - SPI_TRANS_MODE_OCT
         - SPICOMMON_BUSFLAG_OCTAL
       * - OPI 模式
         - 8
         - 8
         - 8
         - SPI_TRANS_MODE_OCT
           SPI_TRANS_MULTILINE_ADDR
           SPI_TRANS_MULTILINE_CMD
         - SPICOMMON_BUSFLAG_OCTAL

命令阶段和地址阶段
^^^^^^^^^^^^^^^^^^^^^^^^^^

在命令阶段和地址阶段，:cpp:member:`spi_transaction_t::cmd` 和 :cpp:member:`spi_transaction_t::addr` 将被发送到总线，该过程中无数据读取。命令阶段和地址阶段的默认长度通过调用 :cpp:func:`spi_bus_add_device` 在 :cpp:type:`spi_device_interface_config_t` 中设置。如果 :cpp:member:`spi_transaction_t::flags` 中的标志信号 :c:macro:`SPI_TRANS_VARIABLE_CMD` 和 :c:macro:`SPI_TRANS_VARIABLE_ADDR` 未设置，则驱动程序将在设备初始化期间自动将这些阶段的长度设置为默认值。

如需更改命令阶段和地址阶段的长度，可通过以下步骤实现：声明结构体 :cpp:type:`spi_transaction_ext_t`，在 :cpp:member:`spi_transaction_ext_t::base` 中设置标志信号 :c:macro:`SPI_TRANS_VARIABLE_CMD` 和/或 :c:macro:`SPI_TRANS_VARIABLE_ADDR`，随后按正常步骤完成余下配置。这样一来，各阶段的长度将等于结构体 :cpp:type:`spi_transaction_ext_t` 中设置的 :cpp:member:`spi_transaction_ext_t::command_bits` 和 :cpp:member:`spi_transaction_ext_t::address_bits` 长度。

如果需要命令阶段和地址阶段的线数与数据阶段保持一致，则应在结构体 :cpp:type:`spi_transaction_t` 中将 ``SPI_TRANS_MULTILINE_CMD`` 和/或 ``SPI_TRANS_MULTILINE_ADDR`` 设置进该结构体的 ``flags`` 成员变量。请参阅 :ref:`transaction-line-mode`。


写入和读取阶段
^^^^^^^^^^^^^^^^^^^^^

一般而言，需要传输到设备或由设备读取的数据将由 :cpp:member:`spi_transaction_t::rx_buffer` 和 :cpp:member:`spi_transaction_t::tx_buffer` 指向的内存块中读取或写入。如果传输时启用了 DMA，则缓冲区应：

  1. 申请支持 DMA 的内存。具体操作请参阅 :ref:`支持 DMA 的内存 <dma-capable-memory>`。
  2. 32 位对齐（从 32 位边界开始，长度为 4 字节的倍数）。

若未满足以上要求，传输事务效率将受到临时缓冲区分配和复制的影响。

如果使用多条数据线传输，请在结构体 :cpp:type:`spi_device_interface_config_t` 中的 ``flags`` 设置 ``SPI_DEVICE_HALFDUPLEX`` 标志信号。结构体 :cpp:type:`spi_transaction_t` 中的 ``flags`` 应按照 :ref:`transaction-line-mode` 中的描述设置。

.. only:: esp32

    .. note::

        当启用 DMA 时，不支持同时具有读取阶段和写入阶段的半双工传输事务。有关细节和解决方法，请参阅 :ref:`spi_known_issues`。

.. only:: not SOC_SPI_HD_BOTH_INOUT_SUPPORTED

    .. note::

        不支持同时具有读取和写入阶段的半双工传输事务。请使用全双工模式。

.. _bus_acquiring:

获取总线
^^^^^^^^^^^^^

若需连续发送专门的 SPI 传输事务以提高效率，可采用获取总线的方式。获取总线后，与其他设备间的传输事务（包括轮询传输事务或中断传输事务）将处于待处理状态，直到总线被释放。要获取和释放总线，请调用函数 :cpp:func:`spi_device_acquire_bus` 和 :cpp:func:`spi_device_release_bus`。


使用驱动程序
-----------------

.. todo::

   Organize the Driver Usage into subsections that will reflect the general user experience of the users, e.g.,

   Configuration

   Add stuff about the configuration API here, the various options in configuration (e.g., configure for interrupt vs. polling), and optional configuration

   Transactions

   Describe how to execute a normal transaction (i.e., where data is larger than 32 bits). Describe how to configure between big and little-endian.

   - Add a sub-sub section on how to optimize when transmitting less than 32 bits
   - Add a sub-sub section on how to transmit mixed transactions to the same device


- 通过调用函数 :cpp:func:`spi_bus_initialize` 初始化 SPI 总线。确保在结构体 :cpp:type:`spi_bus_config_t` 中设置正确的 I/O 管脚。不需要的信号设置为 ``-1``。

- 通过调用函数 :cpp:func:`spi_bus_add_device` 注册连接到总线的设备。确保用参数 `dev_config` 配置设备可能需要的任何时序要求。完成上述操作后可获得设备句柄，以便在向设备发送传输事务时使用。

- 要与设备交互，请在一个或多个 :cpp:type:`spi_transaction_t` 结构体中填充所需的传输事务参数，随后使用轮询传输事务或中断传输事务发送这些结构体：

    - :ref:`中断传输事务 <interrupt_transactions>`
        调用函数 :cpp:func:`spi_device_queue_trans` 将传输事务添加到队列中，随后使用函数 :cpp:func:`spi_device_get_trans_result` 查询结果，或将所有请求输入 :cpp:func:`spi_device_transmit` 实现同步处理。

    - :ref:`轮询传输事务 <polling_transactions>`
        调用函数 :cpp:func:`spi_device_polling_transmit` 发送轮询传输事务。若有插入内容的需要，也可使用 :cpp:func:`spi_device_polling_start` 和 :cpp:func:`spi_device_polling_end` 发送传输事务。

- （可选）若要向设备发送背对背传输事务，请在发送传输事务前调用函数 :cpp:func:`spi_device_acquire_bus`，并在发送传输事务后调用函数 :cpp:func:`spi_device_release_bus`。

- （可选）要从总线上移除特定设备，请以设备句柄为参数调用函数 :cpp:func:`spi_bus_remove_device`。

- （可选）要复位（或重置）总线，请确保总线上未连接其他设备，并调用函数 :cpp:func:`spi_bus_free`。

SPI 主机驱动程序的示例代码存放在 ESP-IDF 示例项目的 :example:`peripherals/spi_master` 目录下。


传输数据小于 32 位的传输事务
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当传输事务数据等于或小于 32 位时，为数据分配一个缓冲区将是次优的选择。实际上，数据可以直接存储于传输事务结构体中。对已传输的数据，可通过调用函数 :cpp:member:`spi_transaction_t::tx_data` 并在传输时设置 :c:macro:`SPI_TRANS_USE_TXDATA` 标志信号来实现。对已接收的数据，可通过调用函数 :cpp:member:`spi_transaction_t::rx_data` 并设置 :c:macro:`SPI_TRANS_USE_RXDATA` 来实现。在这两种情况下，请勿修改 :cpp:member:`spi_transaction_t::tx_buffer` 或 :cpp:member:`spi_transaction_t::rx_buffer`，因为它们与 :cpp:member:`spi_transaction_t::tx_data` 和 :cpp:member:`spi_transaction_t::rx_data` 的内存位置相同。


非 ``uint8_t`` 的整数传输事务
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

SPI 主机逐字节地将数据读入和写入内存。默认情况下，数据优先以最高有效位 (MSB) 发送，极少数情况下会优先使用最低有效位 (LSB)。如果需要发送一个小于 8 位的值，这些位应以 MSB 优先的方式写入内存。

例如，如果需要发送 ``0b00010``，则应将其写成 ``uint8_t`` 变量，读取长度设置为 5 位。此时，设备仍然会收到 8 位数据，并另有 3 个“随机”位，所以读取过程必须准确。

此外，{IDF_TARGET_NAME} 属于小端芯片，即 ``uint16_t`` 和 ``uint32_t`` 变量的最低有效位存储在最小的地址。因此，如果 ``uint16_t`` 存储在内存中，则首先发送位 [7:0]，其次是位 [15:8]。

在某些情况下，要传输的数据大小与 ``uint8_t`` 数组不同，可使用以下宏将数据转换为可由 SPI 驱动直接发送的格式：

- 需传输的数据，使用 :c:macro:`SPI_SWAP_DATA_TX`
- 接收到的数据，使用 :c:macro:`SPI_SWAP_DATA_RX`


.. _mixed_transactions:

向同一设备发送混合传输事务的注意事项
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

为避免代码过于复杂，请一次只向单个设备发送一种类型的传输事务（中断或轮询）。如有需要，可交替发送中断传输事务和轮询传输事务，详见下文。

所有的轮询传输事务和中断传输事务完成后，方可发送轮询传输事务。

由于未完成的轮询传输事务将阻塞其他传输事务，请务必在 :cpp:func:`spi_device_polling_start` 之后调用函数 :cpp:func:`spi_device_polling_end` 以允许其他事务或其他设备使用总线。如果在轮询过程中无需切换到其他任务，可调用函数 :cpp:func:`spi_device_polling_transmit` 启动传输事务，则该传输事务将自动结束。

ISR 会干扰飞行中的轮询传输事务，以适应中断传输事务。在调用 :cpp:func:`spi_device_polling_start` 前，需确保所有发送到 ISR 的中断传输事务已经完成。为此可持续调用 :cpp:func:`spi_device_get_trans_result`，直至全部传输事务返回。

为更好地控制函数的调用顺序，只在单个任务中向同一设备发送混合传输事务。

.. only:: esp32

    .. _spi_master_on_spi1_bus:

    在 SPI1 总线上使用 SPI 主机驱动程序的注意事项
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. note::

        因具备 :ref:`spi_bus_lock` 特性，SPI 主机驱动程序可在 SPI1 总线上运行，但该过程十分棘手，且需要许多特殊处理。这是一个适合高级开发者的功能。

    要在 SPI1 总线上运行 SPI 主机驱动程序，需注意以下两个问题：

    1. 当驱动在 SPI1 上运行时，代码和数据应在内部存储器中。

       总线由设备、flash 中的数据（代码）缓存以及 PSRAM 共享。当其他驱动在 SPI1 总线上运行时，缓存应被禁用。此时，flash 上的数据（代码）以及 PSRAM 就不会在驱动获取 SPI 总线时被抓取。驱动可经由以下方式获取 SPI1 总线：

       - 在 :cpp:func:`spi_device_acquire_bus` 和 :cpp:func:`spi_device_release_bus` 之间显式获取总线。
       - 在 :cpp:func:`spi_device_polling_start` 和 :cpp:func:`spi_device_polling_end` 之间隐式获取总线（或在 :cpp:func:`spi_device_polling_transmit` 中）。

       上述时间内，所有的其他任务和大多数 ISR 将被禁用（见 :ref:`iram-safe-interrupt-handlers`）。当前任务所使用的应用程序代码和数据应放置于内存（DRAM 或 IRAM）中，或者已置于 ROM 中。访问外部存储器（flash 代码、flash 中的常量数据和 PSRAM 中的静态/堆积数据）将导致 ``缓存禁用情况下访问缓存内存区域`` 异常。关于 IRAM、DRAM、和 flash 缓存之间的区别，请参阅 :ref:`应用程序内存布局 <memory-layout>`。

       要将函数置入 IRAM，请选择：

       1. 将 ``IRAM_ATTR`` （包括 ``esp_attr.h``）添加到函数中，如：

              IRAM_ATTR void foo(void) { }

          函数内联时将跟随其调用者的段，且属性无法生效，使用 ``NOLINE_ATTR`` 可避免此类情况。注意，编译器可能会将部分代码转化为常量数据中的查找表，所以 ``noflash_text`` 并不安全。

       2. 或使用 ``linker.lf`` 中的 ``noflash`` 位置，请参阅 :doc:`../../api-guides/linker-script-generation`。注意，编译器可能会将部分代码转化为常量数据中的查找表，所以 ``noflash_text`` 并不安全。

       请注意，优化水平可能会影响编译器的内联行为，或将一些代码转化为常量数据中的查找表等。

       要将数据置入 DRAM，请选择：

       1. 在数据定义中加入 ``DRAM_ATTR`` （包括 ``esp_attr.h``），如：

              DRAM_ATTR int g_foo = 3;

       2. 或使用 ``linker.lf`` 中的 ``noflash`` 位置，请参阅 :doc:`../../api-guides/linker-script-generation`。

    查看更多示例，请参阅 :example:`peripherals/spi_master/hd_eeprom`。


GPIO 矩阵与 IO_MUX 管脚
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    ESP32 的多数外设信号都直接与之专用的 IO_MUX 管脚连接，但这些信号也可以通过 GPIO 交换矩阵传输到任何其他可用的管脚。只要有一个信号通过 GPIO 交换矩阵传输，那么所有的信号都将通由其传输。

    GPIO 交换矩阵提高了信号传输的灵活性，但也存在以下缺陷：

    - 加大了 MISO 信号的输入延迟，增加了 MISO 设置时序违反的可能性。如果需要高速运行 SPI，请使用专用的 IO_MUX 管脚。
    - 只能传输最大时钟频率为 40 MHz 的信号，而使用 IO_MUX 管脚时可以传输的最大时钟频率为 80 MHz。

    .. note::

        了解有关 MISO 输入延迟对最大时钟频率影响的更多信息，请参阅 :ref:`timing_considerations`。

    SPI 总线的 IO_MUX 管脚如下表所示。

    .. list-table::
       :widths: 40 20 20
       :header-rows: 1

       * - 管脚名称
         - GPIO 编号 (SPI2)
         - GPIO 编号 (SPI3)
       * - CS0 [1]_
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

    {IDF_TARGET_SPI2_IOMUX_PIN_CS:default="N/A",   esp32s2="10", esp32s3="10", esp32c2="10", esp32c3="10", esp32c6="16", esp32h2="1", esp32p4="7",  esp32c5="12"}
    {IDF_TARGET_SPI2_IOMUX_PIN_CLK:default="N/A",  esp32s2="12", esp32s3="12", esp32c2="6",  esp32c3="6",  esp32c6="6",  esp32h2="4", esp32p4="9",  esp32c5="6"}
    {IDF_TARGET_SPI2_IOMUX_PIN_MOSI:default="N/A", esp32s2="11"  esp32s3="11", esp32c2="7"   esp32c3="7",  esp32c6="7",  esp32h2="5", esp32p4="8",  esp32c5="7"}
    {IDF_TARGET_SPI2_IOMUX_PIN_MISO:default="N/A", esp32s2="13"  esp32s3="13", esp32c2="2"   esp32c3="2",  esp32c6="2",  esp32h2="0", esp32p4="10", esp32c5="2"}
    {IDF_TARGET_SPI2_IOMUX_PIN_HD:default="N/A",   esp32s2="9"   esp32s3="9",  esp32c2="4"   esp32c3="4",  esp32c6="4",  esp32h2="3", esp32p4="6",  esp32c5="4"}
    {IDF_TARGET_SPI2_IOMUX_PIN_WP:default="N/A",   esp32s2="14"  esp32s3="14", esp32c2="5"   esp32c3="5",  esp32c6="5",  esp32h2="2", esp32p4="11", esp32c5="5"}

    芯片的大多数外围信号都与之专用的 IO_MUX 管脚连接，但这些信号也可以通过较不直接的 GPIO 矩阵路由到任何其他可用的管脚。只要有一个信号是通过 GPIO 矩阵路由的，那么所有的信号都将通过它路由。

    当 SPI 主机被设置为 80 MHz 或更低的频率时，通过 GPIO 矩阵路由 SPI 管脚的行为将与通过 IOMUX 路由相同。

    SPI 总线的 IO_MUX 管脚如下表所示。

    .. list-table::
       :widths: 40 30
       :header-rows: 1

       * - 管脚名称
         - GPIO 编号 (SPI2)
       * - CS0 [1]_
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

.. [1] 只有连接到总线的第一台设备可以使用 CS0 管脚。

.. _speed_considerations:

传输速度的影响因素
--------------------------------

传输速度主要有以下三个限制因素

- 传输事务间隔时间
- SPI 时钟频率
- 缓存缺失的 SPI 函数，包括回调

影响大传输事务传输速度的主要参数是时钟频率。而多个小传输事务的传输速度主要由传输事务间隔时长决定。


传输事务持续时间
^^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_TRANS_TIME_INTR_DMA:default="N/A", esp32="28", esp32s2="23", esp32c3="28", esp32s3="26", esp32c2="42", esp32c6="34", esp32h2="58", esp32c5="27"}
{IDF_TARGET_TRANS_TIME_POLL_DMA:default="N/A", esp32="10", esp32s2="9",  esp32c3="10", esp32s3="11", esp32c2="17", esp32c6="17", esp32h2="28", esp32c5="16"}
{IDF_TARGET_TRANS_TIME_INTR_CPU:default="N/A", esp32="25", esp32s2="22", esp32c3="27", esp32s3="24", esp32c2="40", esp32c6="32", esp32h2="54", esp32c5="24"}
{IDF_TARGET_TRANS_TIME_POLL_CPU:default="N/A", esp32="8",  esp32s2="8",  esp32c3="9",  esp32s3="9",  esp32c2="15", esp32c6="15", esp32h2="24", esp32c5="13"}

传输事务持续时间包括设置 SPI 外设寄存器，将数据复制到 FIFO 或设置 DMA 链接，以及 SPI 传输事务时间。

中断传输事务允许附加额外的开销，以适应 FreeRTOS 队列的成本以及任务与 ISR 切换所需的时间。

对于 **中断传输事务**，CPU 可以在传输事务进行过程中切换到其他任务，这能够节约 CPU 时间，会延长传输事务持续时间，请参阅 :ref:`interrupt_transactions`。对于 **轮询传输事务**，它不会阻塞任务，但允许在传输事务进行时轮询。详情请参阅 :ref:`polling_transactions`。

如果 DMA 被启用，每个传输事务设置链接列表需要约 2 µs。当主机传输数据时，它会自动从链接列表中读取数据。如果不启用 DMA，CPU 必须自己从 FIFO 中写入和读取每个字节。这一过程时长通常不到 2 µs，但写入和读取的传输事务长度都被限制在 64 字节。

单个字节数据的典型传输事务持续时间如下。

- 使用 DMA 的中断传输事务：{IDF_TARGET_TRANS_TIME_INTR_DMA} µs。
- 使用 CPU 的中断传输事务：{IDF_TARGET_TRANS_TIME_INTR_CPU} µs。
- 使用 DMA 的轮询传输事务：{IDF_TARGET_TRANS_TIME_POLL_DMA} µs。
- 使用 CPU 的轮询传输事务：{IDF_TARGET_TRANS_TIME_POLL_CPU} µs。

请注意，以上数据测试时，:ref:`CONFIG_SPI_MASTER_ISR_IN_IRAM` 选项处于启用状态，SPI 传输事务相关的代码放置在 IRAM 中。若关闭此选项（例如为了节省 IRAM），可能影响传输事务持续时间。

SPI 时钟频率
^^^^^^^^^^^^^^^^^^^

GPSPI 外设的时钟源可以通过设置 :cpp:member:`spi_device_handle_t::cfg::clock_source` 选择，可用的时钟源请参阅 :cpp:type:`spi_clock_source_t`。

默认情况下，驱动程序将把 :cpp:member:`spi_device_handle_t::cfg::clock_source` 设置为 ``SPI_CLK_SRC_DEFAULT``。这往往代表 GPSPI 时钟源中的最高频率，在不同的芯片中这一数值会有所不同。

设备的实际时钟频率可能不完全等于所设置的数字，驱动会将其重新计算为与硬件兼容的最接近的数字，并且不超过时钟源的时钟频率。调用函数 :cpp:func:`spi_device_get_actual_freq` 以了解驱动计算的实际频率。

写入或读取阶段的理论最大传输速度可根据下表计算：

.. only:: not SOC_SPI_SUPPORT_OCT

    .. list-table::
       :widths: 40 30
       :header-rows: 1

       * - 写入/读取阶段的线宽
         - 速度 (Bps)
       * - 1-Line
         - *SPI 频率 / 8*
       * - 2-Line
         - *SPI 频率 / 4*
       * - 4-Line
         - *SPI 频率 / 2*

.. only:: SOC_SPI_SUPPORT_OCT

    .. list-table::
       :widths: 40 30
       :header-rows: 1

       * - 写入/读取阶段的线宽
         - 速度 (Bps)
       * - 1-Line
         - *SPI 频率 / 8*
       * - 2-Line
         - *SPI 频率 / 4*
       * - 4-Line
         - *SPI 频率 / 2*
       * - 8-Line
         - *SPI 频率*

其他阶段（命令阶段、地址阶段、Dummy 阶段）的传输速度计算与此类似。

.. only:: esp32

    时钟频率过高可能会限制部分功能的使用，请参阅 :ref:`timing_considerations`。

缓存缺失
^^^^^^^^^^

默认配置只将 ISR 置于 IRAM 中。其他 SPI 相关功能，包括驱动本身和回调都可能发生缓存缺失，需等待代码从 flash 中读取。为避免缓存缺失，可参考 :ref:`CONFIG_SPI_MASTER_IN_IRAM`，将整个 SPI 驱动置入 IRAM，并将整个回调及其 callee 函数一起置入 IRAM。

.. note::

    SPI 驱动是基于 FreeRTOS 的 API 实现的，在使用 :ref:`CONFIG_SPI_MASTER_IN_IRAM` 时，不得启用 :ref:`CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH`。

单个中断传输事务传输 n 字节的总成本为 **20+8n/Fspi[MHz]** [µs]，故传输速度为 **n/(20+8n/Fspi)**。8 MHz 时钟速度的传输速度见下表。

.. list-table::
   :widths: 30 45 40 30 30
   :header-rows: 1

   * - 频率 (MHz)
     - 传输事务间隔 (µs)
     - 传输事务长度 (bytes)
     - 传输时长 (µs)
     - 传输速度 (KBps)
   * - 8
     - 25
     - 1
     - 26
     - 38.5
   * - 8
     - 25
     - 8
     - 33
     - 242.4
   * - 8
     - 25
     - 16
     - 41
     - 490.2
   * - 8
     - 25
     - 64
     - 89
     - 719.1
   * - 8
     - 25
     - 128
     - 153
     - 836.6

传输事务长度较短时将提高传输事务间隔成本，因此应尽可能将几个短传输事务压缩成一个传输事务，以提升传输速度。

注意，ISR 在 flash 操作期间默认处于禁用状态。要在 flash 操作期间继续发送传输事务，请启用 :ref:`CONFIG_SPI_MASTER_ISR_IN_IRAM`，并在 :cpp:member:`spi_bus_config_t::intr_flags` 中设置 :c:macro:`ESP_INTR_FLAG_IRAM`。此时，flash 操作前列队的传输事务将由 ISR 并行处理。此外，每个设备的回调和它们的 ``callee`` 函数都应该在 IRAM 中，避免回调因缓存丢失而崩溃。详情请参阅 :ref:`iram-safe-interrupt-handlers`。


.. only:: esp32

    .. _timing_considerations:

    时序影响因素
    ---------------------

    如图所示，SCLK 发射沿之后、信号被内部寄存器锁存之前，MISO 线存在延迟。因此，MISO 管脚的设置时间是 SPI 时钟速度的限制因素。当延迟过长时，设置松弛度 < 0，违反了设置时序要求，读取可能有误。

    .. image:: /../_static/spi_miso.png
        :scale: 40 %
        :align: center

    .. wavedrom:: /../_static/diagrams/spi/miso_timing_waveform.json

    最大有效频率取决于：

    - :cpp:member:`spi_device_interface_config_t::input_delay_ns` - SCLK 上的一个时钟周期开始后 MISO 总线上的最大数据有效时间
    - 是否使用了 IO_MUX 管脚或 GPIO 矩阵

    使用 GPIO 矩阵时，最大有效频率降至现有 **输入延迟** 的 33 ~ 77%。使用 IO_MUX 管脚或 **dummy 位** 可以保留较高的频率，调用函数 :cpp:func:`spi_get_freq_limit` 能够获取主机的最大读取频率。

    .. _dummy_bit_workaround:

    **Dummy 位**：在读取阶段开始之前，可以插入 Dummy 时钟，在此期间，主机不读取数据。设备能看到 Dummy 时钟并发出数据，但主机在读取阶段之前不会读取。这就弥补了主机 MISO 设置时间不足的问题，并使主机达到更高的读取频率。

    理想状态下，如果设备的速度足以使输入延迟控制在一个 APB 时钟周期 (12.5 ns) 以内，则在不同的条件下，主机读取（或读取和写入）的最大频率如下表所示：

    .. list-table::
       :widths: 30 30 40 50
       :header-rows: 1

       * - 频率限制 (MHz)
         - 频率限制 (MHz)
         - 驱动是否使用 Dummy 位
         - 备注
       * - GPIO 矩阵
         - IO_MUX 管脚
         -
         -
       * - 26.6
         - 80
         - 否
         -
       * - 40
         - --
         - 是
         - 半双工，禁用 DMA

    如果主机仅写入数据，可以通过设置 :cpp:member:`spi_device_interface_config_t::flags` 中的 ``SPI_DEVICE_NO_DUMMY`` 位来禁用 **Dummy 位** 和频率检查机制。禁用 **Dummy 位** 和频率检查机制后，即便在使用 GPIO 矩阵的情况下，输出频率也可达 80 MHz。

    :cpp:member:`spi_device_interface_config_t::flags`

    即使 :cpp:type:`spi_device_interface_config_t` 结构体中的 :cpp:member:`spi_device_interface_config_t::input_delay_ns` 设置为 0，SPI 主机驱动程序仍会运作，但设置精确值有助于：

    - 计算全双工传输事务的频率限制
    - 用半双工传输事务的 Dummy 位正确补偿时序

    可根据设备规格说明中 AC 特性章节的数据估计 SPI 时钟发射沿后的最大数据有效时间，或者使用示波器或逻辑分析仪测量该时间。

    请注意，实际 PCB 布局设计和过度负载可能会增加输入延迟。这意味着非最佳布线和/或总线上的负载电容很可能导致输入延迟值超过设备规格说明中给出的数值或在总线浮动时测量的数值。

    典型的延迟值如下表所示，以下数据由从机设备使用不同芯片测试所得：

    .. list-table::
       :widths: 40 20
       :header-rows: 1

       * - 设备
         - 输入延迟 (ns)
       * - 理想设备
         - 0
       * - 使用 IO_MUX 的 ESP32 从机设备
         - 50
       * - 使用 GPIO_MATRIX 的 ESP32 从机设备
         - 75

    MISO 路径延迟（有效时间）由从机的 **输入延迟** 与主机的 **GPIO 矩阵延迟** 组成。该延迟决定了频率限制，超过这个频率的全双工传输将无法如半双工交易中使用的 Dummy 位一样工作。该频率限制的计算方法为：

        *Freq limit [MHz] = 80 / (floor(MISO delay[ns]/12.5) + 1)*

    频率限制与输入延迟间的关系如图所示。如果主机使用 GPIO 矩阵，应在 MISO 延迟上额外增加两个 APB 时钟周期。

    .. image:: /../_static/spi_master_freq_tv.png

    不同设备在不同 **输入延迟** 时间下对应的频率限制如下表所示。

    当主机驱动为 IO_MUX (0 ns) 时：

    .. list-table::
       :widths: 20 40 40
       :header-rows: 1

       * - 输入延迟 (ns)
         - MISO 通道延迟 (ns)
         - 频率限制 (MHz)
       * - 0
         - 0
         - 80
       * - 50
         - 50
         - 16
       * - 75
         - 75
         - 11.43

    当主机驱动为 GPIO_MATRIX (25 ns) 时：

    .. list-table::
       :widths: 20 40 40
       :header-rows: 1

       * - 输入延迟 (ns)
         - MISO 通道延迟 (ns)
         - 频率限制 (MHz)
       * - 0
         - 25
         - 26.67
       * - 50
         - 75
         - 11.43
       * - 75
         - 100
         - 8.89


    .. _spi_known_issues:

    已知问题
    ------------

    1. 写入和读取阶段同时进行时，半双工传输事务与 DMA 不兼容。

        如需执行此类传输事务，可使用以下替代方案：

        1. 执行全双工传输事务。
        2. 将总线初始化函数的最后一个参数设置为 0 以禁用 DMA，即：
            ``ret=spi_bus_initialize(VSPI_HOST, &buscfg, 0);``

        此举可避免传输和接收超过 64 字节的数据。
        1. 尝试用命令和地址字段代替写入阶段。

    2. 全双工传输事务与 *Dummy 位* 不兼容，因此存在频率限制，请参阅 :ref:`Dummy 位加速方案 <dummy_bit_workaround>`。

    3. SPI 读取和写入阶段同时进行时，全双工模式和半双工模式下，:cpp:type:`spi_device_interface_config_t` 和 :cpp:type:`spi_transaction_ext_t` 中的 ``dummy_bits`` 均会失效。

    4. 全双工传输事务模式中，命令阶段和地址阶段与 ``cs_ena_pretrans`` 不兼容。


应用示例
-------------------

查看使用 SPI 主机驱动程序在半双工模式下读取/写入 AT93C46D EEPROM（8 位模式）的示例代码，请前往 ESP-IDF 示例的 :example:`peripherals/spi_master/hd_eeprom` 目录。

查看使用 SPI 主机驱动程序在全双工模式下驱动 LCD 屏幕（如 ST7789V 或 ILI9341）的示例代码，请前往 ESP-IDF 示例的 :example:`peripherals/spi_master/lcd` 目录。

API 参考 - SPI Common
--------------------------

.. include-build-file:: inc/spi_types.inc
.. include-build-file:: inc/spi_common.inc


API 参考 - SPI Master
--------------------------

.. include-build-file:: inc/spi_master.inc
