通用异步接收器/发送器 (UART)
==================================================

{IDF_TARGET_UART_EXAMPLE_PORT:default = "UART_NUM_1", esp32 = "UART_NUM_2", esp32s3 = "UART_NUM_2"}

简介
--------

通用异步接收器/发送器 (UART) 属于一种硬件功能，通过使用 RS232、RS422、RS485 等常见异步串行通信接口来处理通信时序要求和数据帧。UART 是实现不同设备之间全双工或半双工数据交换的一种常用且经济的方式。

{IDF_TARGET_NAME} 芯片有 {IDF_TARGET_SOC_UART_HP_NUM} 个 UART 控制器（也称为端口），每个控制器都有一组相同的寄存器以简化编程并提高灵活性。

每个 UART 控制器可以独立配置波特率、数据位长度、位顺序、停止位位数、奇偶校验位等参数。所有具备完整功能的 UART 控制器都能与不同制造商的 UART 设备兼容，并且支持红外数据协会 (IrDA) 定义的标准协议。

.. only:: SOC_UART_LP_NUM

    此外，{IDF_TARGET_NAME} 芯片还有一个满足低功耗需求的 LP UART 控制器。LP UART 是原 UART 的功能剪裁版本。它只支持基础 UART 功能，不支持 IrDA 或 RS485 协议，并且只有一块较小的 RAM 存储空间。想要全面了解的 UART 及 LP UART 功能区别，请参考 **{IDF_TARGET_NAME} 技术参考手册** > UART 控制器 (UART) > 主要特性 [`PDF <{IDF_TARGET_TRM_EN_URL}#uart>`__]。

功能概述
-------------------

下文介绍了如何使用 UART 驱动程序的函数和数据类型在 {IDF_TARGET_NAME} 和其他 UART 设备之间建立通信。基本编程流程分为以下几个步骤：

1. :ref:`uart-api-setting-communication-parameters` - 设置波特率、数据位、停止位等
2. :ref:`uart-api-setting-communication-pins` - 分配连接设备的管脚
3. :ref:`uart-api-driver-installation` - 为 UART 驱动程序分配 {IDF_TARGET_NAME} 资源
4. :ref:`uart-api-running-uart-communication` - 发送/接收数据
5. :ref:`uart-api-using-interrupts` - 触发特定通信事件的中断
6. :ref:`uart-api-deleting-driver` - 如无需 UART 通信，则释放已分配的资源

步骤 1 到 3 为配置阶段，步骤 4 为 UART 运行阶段，步骤 5 和 6 为可选步骤。

UART 驱动程序函数通过 :cpp:type:`uart_port_t` 识别不同的 UART 控制器。调用以下所有函数均需此标识。


.. _uart-api-setting-communication-parameters:

设置通信参数
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

UART 通信参数可以在一个步骤中完成全部配置，也可以在多个步骤中单独配置。


一次性配置所有参数
""""""""""""""""""""""""""""""""

调用函数 :cpp:func:`uart_param_config` 并向其传递 :cpp:type:`uart_config_t` 结构体，:cpp:type:`uart_config_t` 结构体应包含所有必要的参数。请参考以下示例。

.. code-block:: c

    const uart_port_t uart_num = {IDF_TARGET_UART_EXAMPLE_PORT};
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

了解配置硬件流控模式的更多信息，请参考 :example:`peripherals/uart/uart_echo`。

分步依次配置每个参数
"""""""""""""""""""""""""""""""

调用下表中的专用函数，能够单独配置特定参数。如需重新配置某个参数，也可使用这些函数。

.. list-table:: 单独配置特定参数的函数
   :widths: 30 70
   :header-rows: 1

   * - 配置参数
     - 函数
   * - 波特率
     - :cpp:func:`uart_set_baudrate`
   * - 传输位
     - 调用 :cpp:func:`uart_set_word_length` 设置 :cpp:type:`uart_word_length_t`
   * - 奇偶控制
     - 调用 :cpp:type:`uart_parity_t` 设置 :cpp:func:`uart_set_parity`
   * - 停止位
     - 调用 :cpp:func:`uart_set_stop_bits` 设置 :cpp:type:`uart_stop_bits_t`
   * - 硬件流控模式
     - 调用 :cpp:func:`uart_set_hw_flow_ctrl` 设置 :cpp:type:`uart_hw_flowcontrol_t`
   * - 通信模式
     - 调用 :cpp:func:`uart_set_mode` 设置 :cpp:type:`uart_mode_t`

表中每个函数都可使用 ``_get_`` 对应项来查看当前设置值。例如，查看当前波特率值，请调用 :cpp:func:`uart_get_baudrate`。


.. _uart-api-setting-communication-pins:

设置通信管脚
^^^^^^^^^^^^^^^^^^^^^^^^^^

通信参数设置完成后，可以配置其他 UART 设备连接的 GPIO 管脚。调用函数 :cpp:func:`uart_set_pin`，指定配置 Tx、Rx、RTS 和 CTS 信号的 GPIO 管脚编号。如要为特定信号保留当前分配的管脚编号，可传递宏 :c:macro:`UART_PIN_NO_CHANGE`。

请为不使用的管脚都指定为宏 :c:macro:`UART_PIN_NO_CHANGE`。

.. code-block:: c

  // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
  ESP_ERROR_CHECK(uart_set_pin({IDF_TARGET_UART_EXAMPLE_PORT}, 4, 5, 18, 19));

.. _uart-api-driver-installation:

安装驱动程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

通信管脚设置完成后，请调用 :cpp:func:`uart_driver_install` 安装驱动程序并指定以下参数：

- Tx 环形缓冲区的大小
- Rx 环形缓冲区的大小
- 事件队列句柄和大小
- 分配中断的标志

该函数将为 UART 驱动程序分配所需的内部资源。

.. code-block:: c

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install({IDF_TARGET_UART_EXAMPLE_PORT}, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));

此步骤完成后，可连接外部 UART 设备检查通信。


.. _uart-api-running-uart-communication:

运行 UART 通信
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

串行通信由每个 UART 控制器的有限状态机 (FSM) 控制。

发送数据的过程分为以下步骤：

1. 将数据写入 Tx FIFO 缓冲区
2. FSM 序列化数据
3. FSM 发送数据

接收数据的过程类似，只是步骤相反：

1. FSM 处理且并行化传入的串行流
2. FSM 将数据写入 Rx FIFO 缓冲区
3. 从 Rx FIFO 缓冲区读取数据

因此，应用程序仅会通过 :cpp:func:`uart_write_bytes` 和 :cpp:func:`uart_read_bytes` 从特定缓冲区写入或读取数据，其余工作由 FSM 完成。


发送数据
""""""""""""""""""""

发送数据准备好后，调用函数 :cpp:func:`uart_write_bytes`，并向其传递数据缓冲区的地址和数据长度。该函数会立即或在有足够可用空间时将数据复制到 Tx 环形缓冲区，随后退出。当 Tx FIFO 缓冲区中有可用空间时，中断服务例程 (ISR) 会在后台将数据从 Tx 环形缓冲区移动到 Tx FIFO 缓冲区。调用函数请参考以下代码。

.. code-block:: c

    // Write data to UART.
    char* test_str = "This is a test string.\n";
    uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));

函数 :cpp:func:`uart_write_bytes_with_break` 与 :cpp:func:`uart_write_bytes` 类似，但在传输结束时会添加串行中断信号。“串行中断信号”意味着 Tx 线保持低电平的时间长于一个数据帧。

.. code-block:: c

    // Write data to UART, end with a break signal.
    uart_write_bytes_with_break(uart_num, "test break\n",strlen("test break\n"), 100);

能够将数据写入 Tx FIFO 缓冲区的另一函数是 :cpp:func:`uart_tx_chars`。 与 :cpp:func:`uart_write_bytes` 不同，此函数在没有可用空间之前不会阻塞。相反，它将写入所有可以立即放入硬件 Tx FIFO 的数据，然后返回写入的字节数。

“配套”函数 :cpp:func:`uart_wait_tx_done` 用于监听 Tx FIFO 缓冲区的状态，并在缓冲区为空时返回。

.. code-block:: c

    // Wait for packet to be sent
    const uart_port_t uart_num = {IDF_TARGET_UART_EXAMPLE_PORT};
    ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 100)); // wait timeout is 100 RTOS ticks (TickType_t)


接收数据
"""""""""""""""""

一旦 UART 接收了数据，并将其保存在 Rx FIFO 缓冲区中，就需要使用函数 :cpp:func:`uart_read_bytes` 检索数据。读取数据之前，调用 :cpp:func:`uart_get_buffered_data_len` 能够查看 Rx FIFO 缓冲区中可用的字节数。请参考以下示例。

.. code-block:: c

    // Read data from UART.
    const uart_port_t uart_num = {IDF_TARGET_UART_EXAMPLE_PORT};
    uint8_t data[128];
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    length = uart_read_bytes(uart_num, data, length, 100);

如果不再需要 Rx FIFO 缓冲区中的数据，可以调用 :cpp:func:`uart_flush` 清空缓冲区。


软件流控
""""""""""""""""""""""

如果硬件流控处于禁用状态，可使用函数 :cpp:func:`uart_set_rts` 和 :cpp:func:`uart_set_dtr` 分别手动设置 RTS 和 DTR 信号电平。


通信方式选择
"""""""""""""""""""""""""""""

UART 控制器支持多种通信模式，使用函数 :cpp:func:`uart_set_mode` 可以选择模式。选择特定模式后，UART 驱动程序将处理已连接 UART 设备的相应行为。例如，使用 RTS 线控制 RS485 驱动芯片，能够实现半双工 RS485 通信。

.. code-block:: bash

    // Setup UART in rs485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));


.. _uart-api-using-interrupts:

使用中断
^^^^^^^^^^^^^^^^^

根据特定的 UART 状态或检测到的错误，可以生成许多不同的中断。**{IDF_TARGET_NAME} 技术参考手册** > UART 控制器 (UART) > UART 中断 和 UHCI 中断 [`PDF <{IDF_TARGET_TRM_EN_URL}#uart>`__] 中提供了可用中断的完整列表。调用 :cpp:func:`uart_enable_intr_mask` 或 :cpp:func:`uart_disable_intr_mask` 能够分别启用或禁用特定中断。

调用 :cpp:func:`uart_driver_install` 函数可以安装驱动程序的内部中断处理程序，用以管理 Tx 和 Rx 环形缓冲区，并提供事件等高级 API 函数（见下文）。

API 提供了一种便利的方法来处理本文所讨论的特定中断，即用专用函数包装中断：

- **事件检测**：:cpp:type:`uart_event_type_t` 定义了多个事件，使用 FreeRTOS 队列功能能够将其报告给用户应用程序。调用 :ref:`uart-api-driver-installation` 中的 :cpp:func:`uart_driver_install` 函数，可以启用此功能，请参考 :example:`peripherals/uart/uart_events` 中使用事件检测的示例。

- **达到 FIFO 空间阈值或传输超时**：Tx 和 Rx FIFO 缓冲区在填充特定数量的字符和在发送或接收数据超时的情况下将会触发中断。如要使用此类中断，请执行以下操作：

    - 配置缓冲区长度和超时阈值：在结构体 :cpp:type:`uart_intr_config_t` 中输入相应阈值并调用 :cpp:func:`uart_intr_config`
    - 启用中断：调用函数 :cpp:func:`uart_enable_tx_intr` 和 :cpp:func:`uart_enable_rx_intr`
    - 禁用中断：调用函数 :cpp:func:`uart_disable_tx_intr` 或 :cpp:func:`uart_disable_rx_intr`

- **模式检测**：在检测到重复接收/发送同一字符的“模式”时触发中断，请参考示例 :example:`peripherals/uart/uart_events`。例如，模式检测可用于检测命令字符串末尾是否存在特定数量的相同字符（“模式”）。可以调用以下函数：

    - 配置并启用此中断：调用 :cpp:func:`uart_enable_pattern_det_baud_intr`
    - 禁用中断：调用 :cpp:func:`uart_disable_pattern_det_intr`


宏指令
^^^^^^^^^^^^

API 还定义了一些宏指令。例如，:c:macro:`UART_HW_FIFO_LEN` 定义了硬件 FIFO 缓冲区的长度，:c:macro:`UART_BITRATE_MAX` 定义了 UART 控制器支持的最大波特率。


.. _uart-api-deleting-driver:

删除驱动程序
^^^^^^^^^^^^^^^^^^^^^^^

如不再需要与 :cpp:func:`uart_driver_install` 建立通信，则可调用 :cpp:func:`uart_driver_delete` 删除驱动程序，释放已分配的资源。


RS485 特定通信模式简介
----------------------------------------------

.. note::

     下文将使用 ``[UART_REGISTER_NAME].[UART_FIELD_BIT]`` 指代 UART 寄存器字段/位。了解特定模式位的更多信息，请参考 **{IDF_TARGET_NAME} 技术参考手册** > UART 控制器 (UART) > 寄存器摘要 [`PDF <{IDF_TARGET_TRM_EN_URL}#uart-reg-summ>`__]。请搜索寄存器名称导航至寄存器描述，找到相应字段/位。

- ``UART_RS485_CONF_REG.UART_RS485_EN``：设置此位将启用 RS485 通信模式支持。
- ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN``：设置此位，发送器的输出信号将环回到接收器的输入信号。
- ``UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN``：设置此位，如果接收器繁忙，发送器仍将发送数据（由硬件自动解决冲突）。

{IDF_TARGET_NAME} 的 RS485 UART 硬件能够检测数据报传输期间的信号冲突，并在启用此中断时生成中断 ``UART_RS485_CLASH_INT``。术语冲突表示发送的数据报与另一端接收到的数据报不同。数据冲突通常与总线上其他活跃设备的存在有关，或者是由于总线错误而出现。

冲突检测功能允许在激活和触发中断时处理冲突。中断 ``UART_RS485_FRM_ERR_INT`` 和 ``UART_RS485_PARITY_ERR_INT`` 可与冲突检测功能一起使用，在 RS485 模式下分别控制帧错误和奇偶校验位错误。UART 驱动程序支持此功能，通过选择 :cpp:enumerator:`UART_MODE_RS485_APP_CTRL` 模式可以使用（参考函数 :cpp:func:`uart_set_mode`）。

冲突检测功能可与电路 A 和电路 C 一起使用（参考章节 `接口连接选项`_）。在使用电路 A 或 B 时，连接到总线驱动 DE 管脚的 RTS 管脚应由用户应用程序控制。调用函数 :cpp:func:`uart_get_collision_flag` 能够查看是否触发冲突检测标志。

{IDF_TARGET_NAME} UART 控制器本身不支持半双工通信，因其无法自动控制连接到 RS485 总线驱动 RE/DE 输入的 RTS 管脚。然而，半双工通信能够通过 UART 驱动程序对 RTS 管脚的软件控制来实现，调用 :cpp:func:`uart_set_mode` 并选择 :cpp:enumerator:`UART_MODE_RS485_HALF_DUPLEX` 模式能够启用这一功能。

主机开始向 Tx FIFO 缓冲区写入数据时，UART 驱动程序会自动置位 RTS 管脚（逻辑 1）；最后一位数据传输完成后，驱动程序就会取消置位 RTS 管脚（逻辑 0）。要使用此模式，软件必须禁用硬件流控功能。此模式适用于下文所有已用电路。


接口连接选项
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

本节提供了示例原理图来介绍 {IDF_TARGET_NAME} RS485 接口连接的基本内容。

.. note::

     - **下列原理图不一定包含所有必要元素**。

     - **模拟设备** ADM483 和 ADM2483 是 RS485 收发器的常见示例，**也可使用其他类似的收发器**。


电路 A：冲突检测电路
""""""""""""""""""""""""""""""""""""""

.. code-block:: none

         VCC ---------------+
                            |
                    +-------x-------+
         RXD <------| R             |
                    |              B|----------<> B
         TXD ------>| D    ADM483   |
 ESP                |               |     RS485 bus side
         RTS ------>| DE            |
                    |              A|----------<> A
               +----| /RE           |
               |    +-------x-------+
               |            |
              GND          GND

推荐这一电路，因为该电路较为简单，同时能够检测冲突。持续启用线路驱动中的接收器时，UART 将会监控 RS485 总线。启用 ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN`` 位时，UART 外设会执行回波抑制。


电路 B：无冲突检测的手动切换发射器/接收器
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


.. code-block:: none

         VCC ---------------+
                            |
                    +-------x-------+
         RXD <------| R             |
                    |              B|-----------<> B
         TXD ------>| D    ADM483   |
 ESP                |               |     RS485 bus side
         RTS --+--->| DE            |
               |    |              A|-----------<> A
               +----| /RE           |
                    +-------x-------+
                            |
                           GND

该电路无法检测冲突。置位 ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN`` 位时，电路将抑制硬件收到的空字节。这种情况下 ``UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN`` 位不适用。


电路 C：自动切换发射器/接收器
"""""""""""""""""""""""""""""""""""""""""""""""""""""""

.. code-block:: none

   VCC1 <-------------------+-----------+           +-------------------+----> VCC2
                 10K ____   |           |           |                   |
                +---|____|--+       +---x-----------x---+    10K ____   |
                |                   |                   |   +---|____|--+
  RX <----------+-------------------| RXD               |   |
                     10K ____       |                  A|---+---------------<> A (+)
                +-------|____|------| PV    ADM2483     |   |    ____  120
                |   ____            |                   |   +---|____|---+  RS485 bus side
        VCC1 <--+--|____|--+------->| DE                |                |
                10K        |        |                  B|---+------------+--<> B (-)
                        ---+    +-->| /RE               |   |    ____
           10K          |       |   |                   |   +---|____|---+
          ____       | /-C      +---| TXD               |    10K         |
  TX >---|____|--+_B_|/   NPN   |   |                   |                |
                     |\         |   +---x-----------x---+                |
                     | \-E      |       |           |                    |
                        |       |       |           |                    |
                       GND1    GND1    GND1        GND2                 GND2

这种电气隔离电路不需要用软件应用程序或驱动程序控制 RTS 管脚，因为电路能够自动控制收发器方向。但是在传输过程中，需要将 ``UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN`` 设置为 1 并将 ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN`` 设置为 0 来抑制空字节。此设置可以在任何 RS485 UART 模式下工作，包括 :cpp:enumerator:`UART_MODE_UART`。


应用示例
--------------------

下表列出了目录 :example:`peripherals/uart/` 下可用的代码示例。

.. list-table::
   :widths: 35 65
   :header-rows: 1

   * - 代码示例
     - 描述
   * - :example:`peripherals/uart/uart_echo`
     - 配置 UART 设置、安装 UART 驱动程序以及通过 UART1 接口读取/写入。
   * - :example:`peripherals/uart/uart_events`
     - 报告各种通信事件，使用模式检测中断。
   * - :example:`peripherals/uart/uart_async_rxtxtasks`
     - 通过同一 UART 在两个独立的 FreeRTOS 任务中发送和接收数据。
   * - :example:`peripherals/uart/uart_select`
     - 针对 UART 文件描述符使用同步 I/O 多路复用。
   * - :example:`peripherals/uart/uart_echo_rs485`
     - 设置 UART 驱动程序以半双工模式通过 RS485 接口进行通信。此示例与 :example:`peripherals/uart/uart_echo` 类似，但允许通过连接到 {IDF_TARGET_NAME} 管脚的 RS485 接口芯片进行通信。
   * - :example:`peripherals/uart/nmea0183_parser`
     - 解析通过 UART 外设从 GPS 收到的 NMEA0183 语句来获取 GPS 信息。


API 参考
---------------

.. include-build-file:: inc/uart.inc
.. include-build-file:: inc/uart_types.inc


GPIO 查找宏指令
^^^^^^^^^^^^^^^^^^^^^^^^^^^

UART 外设有供直接连接的专用 IO_MUX 管脚，但也可用非直接的 GPIO 矩阵将信号配置到其他管脚。如要直接连接，需要知道哪一管脚为 UART 通道的专用 IO_MUX 管脚。GPIO 查找宏简化了查找和分配 IO_MUX 管脚的过程，可根据 IO_MUX 管脚编号或所需 UART 通道名称选择一个宏，该宏将返回匹配的对应项。请查看下列示例。

.. note::

    如需较高的 UART 波特率（超过 40 MHz），即仅使用 IO_MUX 管脚时，可以使用此类宏。在其他情况下可以忽略这些宏，并使用 GPIO 矩阵为 UART 功能配置任一 GPIO 管脚。

1. :c:macro:`UART_NUM_2_TXD_DIRECT_GPIO_NUM` 返回 UART 通道 2 TXD 管脚的 IO_MUX 管脚编号（管脚 17）
2. :c:macro:`UART_GPIO19_DIRECT_CHANNEL` 在通过 IO_MUX 连接到 UART 外设时返回 GPIO 19 的 UART 编号（即 UART_NUM_0）
3. GPIO 19 在通过 IO_MUX 用作 UART CTS 管脚时，:c:macro:`UART_CTS_GPIO19_DIRECT_CHANNEL` 将返回 GPIO 19 的 UART 编号（即 UART_NUM_0）。该宏类似于上述宏，但指定了管脚功能，这也是 IO_MUX 分配的一部分。

.. include-build-file:: inc/uart_channel.inc

