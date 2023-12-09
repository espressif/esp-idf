SDIO 卡从机驱动程序
===========================

:link_to_translation:`en:[English]`

概述
--------

.. only:: esp32

    如下表所示，ESP32 SDIO 卡的主机与从机外设共享两组管脚。SPI0 总线通常会占用第一组管脚，该总线负责运行代码，与 SPI flash 相连。因此，在 SDIO 主机不使用第二组管脚时，SDIO 从机驱动程序只能在第二组管脚上运行。

SDIO 从机支持以下三种运行模式：SPI、1 位 SD 和 4 位 SD。从机设备可以根据接口上的信号确定当前模式，并相应地配置自身以适配该模式。随后，从机驱动程序可以与从机设备进行通信，正确处理命令和数据传输。根据 SDIO 规范，无论是在 1 位 SD、4 位 SD 还是 SPI 模式下，CMD 和 DAT0-3 信号线都应设置为高电平。

连接方式
^^^^^^^^^^^

.. only:: esp32

    .. list-table::
        :header-rows: 1
        :widths: 25 25 25 25
        :align: center

        * - 管脚名称
          - SPI 模式的对应管脚
          - GPIO 编号（卡槽 1）
          - GPIO 编号（卡槽 2）
        * - CLK
          - SCLK
          - 6
          - 14
        * - CMD
          - MOSI
          - 11
          - 15
        * - DAT0
          - MISO
          - 7
          - 2
        * - DAT1
          - 中断
          - 8
          - 4
        * - DAT2
          - N.C.（拉高）
          - 9
          - 12
        * - DAT3
          - #CS
          - 10
          - 13

.. only:: esp32c6

    .. list-table::
        :header-rows: 1
        :widths: 30 40 30
        :align: center

        * - 管脚名称
          - SPI 模式的对应管脚
          - GPIO 编号
        * - CLK
          - SCLK
          - 19
        * - CMD
          - MOSI
          - 18
        * - DAT0
          - MISO
          - 20
        * - DAT1
          - 中断
          - 21
        * - DAT2
          - N.C.（拉高）
          - 22
        * - DAT3
          - #CS
          - 23

- 1 位 SD 模式：连接 CLK、CMD、DAT0 和 DAT1 管脚并接地。
- 4 位 SD 模式：连接所有管脚并接地。
- SPI 模式：连接 SCLK、MOSI、MISO、中断、#CS 管脚并接地。

.. note::

  请确保使用 10 KOhm - 90 KOhm 的上拉电阻将 SDIO 卡的 CMD 和数据线 DAT0-DAT3 配置为上拉，包括在 1 位模式或 SPI 模式下。大多数官方模组内部并未提供此类上拉电阻，使用官方开发板时，请参阅 :ref:`compatibility_overview_espressif_hw_sdio`，确认所用开发板是否配置此类上拉电阻。

.. only:: esp32

    .. note::

      多数官方模组的 strapping 管脚与 SDIO 从机功能配置存在冲突。若在内置 3.3 V flash 的 ESP32 模组上进行首次开发，需要在开发之前进行 eFuse 烧录，以调整模组的管脚配置，使其与 SDIO 功能兼容。请参阅 :ref:`compatibility_overview_espressif_hw_sdio`，了解具体配置方法。

        以下是内置 3.3 V flash 的模组/开发板列表：

        - 模组：除 ESP32-WROVER、ESP32-WROVER-I、ESP32-S3-WROOM-2 外的所有模组，模组列表见 `模组概览 <https://www.espressif.com/zh-hans/products/modules>`__
        - 开发板：ESP32-PICO-KIT、ESP32-DevKitC（最高版本为 v4）、ESP32-WROVER-KIT（v4.1 [也称 ESP32-WROVER-KIT-VB]、v2、v1 [也称 DevKitJ v1]）

        通过开发板上模组的型号可以判断 ESP32-WROVER-KIT 的版本：v4.1 使用 ESP32-WROVER-B 模组，v3 使用 ESP32-WROVER 模组，v2 和 v1 使用 ESP32-WROOM-32 模组。

要了解有关上拉电阻的更多技术细节，请参阅 :doc:`sd_pullup_requirements`。

.. toctree::
    :hidden:

    sd_pullup_requirements

主机可以配置 DAT3 管脚为高电平并发送 CMD0 命令，将从机初始化为 SD 模式；或配置 CS 管脚为低电平并发送 CMD0 命令，将从机初始化为 SPI 模式。CS 管脚与 DAT3 管脚相同。

初始化完成后，主机可以发送 CMD52 命令，将数据写入 CCCR 寄存器 0x07，启用 4 位 SD 模式。所有总线检测均由从机外设处理。

主机与从机的通信必须通过 ESP 从机特定协议进行。

通过 CMD52 和 CMD53 命令，从机驱动程序基于 Function 1 提供了以下三种服务：

(1) 发送和接收 FIFO
(2) 主机和从机共享的 52 个 8 位 读写寄存器
(3) 16 个中断源（8 个从主机到从机，8 个从从机到主机）

术语
^^^^^^^^^^^

SDIO 从机驱动程序的相关术语如下：

- 传输 (transfer)：传输始终由主机发出的命令符启动，可能包含一个响应和多个数据块。{IDF_TARGET_NAME} SDIO 从机驱动程序的核心机制是通过传输进行数据交换和通信。
- 发送 (sending)：从从机到主机的传输。
- 接收 (receiving)：从主机到从机的传输。

.. note::

  在 **{IDF_TARGET_NAME} 技术参考手册** > **SDIO 从机控制器** [`PDF <{IDF_TARGET_TRM_CN_URL}#sdioslave>`__] 中，寄存器从主机的角度进行命名和定义。即，``RX`` 寄存器指的是发送寄存器，``TX`` 寄存器指的是接收寄存器。 我们在驱动程序中不再使用 ``TX`` 或 ``RX``，以避免产生歧义。

- FIFO：在 Function 1 内的特定地址，可以通过使用 CMD53 命令读写大量数据。该地址与在单个传输中请求从从机读取或写入的长度相关：**请求长度** = 0x1F800 – 地址。
- 所有权 (ownership)：拥有缓冲区的所有权时，驱动程序可以随机读写该缓冲区（通常通过 DMA 实现）。在所有权返回给应用程序前，应用程序不应读取/写入该缓冲区。如果应用程序从缓冲区中读取数据时，接收驱动程序拥有缓冲区的所有权，可能会读取到随机数据；如果应用程序向缓冲区写入数据时，发送驱动程序拥有缓冲区的所有权，发送的数据可能会损坏。
- 请求长度 (requested length)：一次传输中的请求长度，由 FIFO 地址确定。
- 传输长度 (transfer length)：一次传输中的请求长度，由 CMD53 字节/块计数字段确定。

.. note::

  请求长度不同于传输长度。在 {IDF_TARGET_NAME} SDIO DMA 中，操作基于 **请求长度** 而非 **传输长度**，即 DMA 控制器会根据 **请求长度** 处理数据传输，确保只传输 **请求长度** 范围内的数据。**传输长度** 必须等于或长于 **请求长度**，并将剩余部分在发送时填充为 0，或在接受时丢弃。

- 接收缓冲区大小 (receiving buffer size)：通信开始前，主机与从机间会预先定义缓冲区大小。初始化过程中，从机应用程序必须通过结构体 ``sdio_slave_config_t`` 中的 ``recv_buffer_size`` 设置缓冲区大小。
- 中断 (interrupts)：{IDF_TARGET_NAME} SDIO 从机支持两个方向的中断，即由主机到从机（以下称从机中断）以及由从机到主机（以下称主机中断）。更多详情，请参阅 :ref:`interrupts`。
- 寄存器 (registers)：通过 CMD52 或 CMD53 命令在 Function 1 中访问的特定地址。

与 ESP SDIO 从机通信
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在使用主机初始化 SDIO 从机时，应遵循标准 SDIO 初始化流程（请参阅 `SDIO 简化规范 <https://www.sdcard.org/downloads/pls/pdf/?p=PartE1_SDIO_Simplified_Specification_Ver3.00.jpg&f=PartE1_SDIO_Simplified_Specification_Ver3.00.pdf&e=EN_SSE1>`_ 的第 3.1.2 节），简化版流程可参考 :ref:`esp_slave_init`。

此外，在通过 CMD52/CMD53 访问到 Function 1 这一机制的基础上，还存在一个仅适用于 {IDF_TARGET_NAME} 的上层通信协议。该特定通信协议中，主机和从机通过 CMD52/CMD53 命令进行数据交换和通信。更多详情，请参阅 :ref:`esp_slave_protocol_layer`。

组件 :doc:`/api-reference/protocols/esp_serial_slave_link` 也支持 {IDF_TARGET_NAME} 主机与 {IDF_TARGET_NAME} SDIO 从机通信。在开发主机应用程序时，请参阅 :example:`peripherals/sdio` 中的示例。


.. _interrupts:

中断
^^^^^^^^^^

为了方便通信，SDIO 从机驱动程序中既存在由主机到从机的中断信号，也存在由从机到主机的中断信号。

从机中断
""""""""""""""""

主机可以通过在寄存器 0x08D 中写入任意一个位来向从机发起中断。一旦置位了寄存器中的任意一位，就会产生一个中断，促使 SDIO 从机驱动调用特定回调函数，该回调函数由 ``sdio_slave_config_t`` 结构体中的 ``slave_intr_cb`` 定义。

.. note::

  该回调函数在中断服务例程中调用，请勿在其中使用任何延迟、循环或可能阻塞的函数，如互斥锁。

类似前述情况，还有一组备选函数可供使用。可以调用 ``sdio_slave_wait_int`` 在一定时间内等待中断，或调用 ``sdio_slave_clear_int`` 清除来自主机的中断。回调函数可以与等待函数完美配合。

主机中断
"""""""""""""""

从机可以在特定时间通过中断线向主机发起中断，这个中断是电平触发的。主机检测到中断线电平拉低时，它可以读取从机中断状态寄存器，以查看中断源。主机可以清除特定的中断位，或选择禁用中断源。在清除或禁用所有中断源前，中断线会保持激活状态。

在 SDIO 从机驱动程序中，还存在一些专用中断源和通用中断源，详情请参阅 ``sdio_slave_hostint_t``。

共享寄存器
^^^^^^^^^^^^^^^^

在主机与从机之间共有 52 个共享的 8 位读写寄存器，用于在主机和从机之间共享信息。通过 ``sdio_slave_read_reg`` 和 ``sdio_slave_write_reg``，从机可以随时读取或写入寄存器。主机可以通过 CMD52 或 CMD53 访问（读写）这些寄存器。

接收 FIFO
^^^^^^^^^^^^^^

准备向从机发送数据包时，主机需要读取从机的缓冲区数据数量，判定从机是否准备好接收数据。

为了支持接收来自主机的数据，应用程序需按照以下步骤，将缓冲区加载到从机驱动程序中：

1. 调用 ``sdio_slave_recv_register_buf`` 注册缓冲区，并获取已注册缓冲区的句柄。驱动程序会为链接到硬件的链表描述符所需的缓冲区分配内存。这些缓冲区的大小应与接收缓冲区大小相等。
2. 将缓冲区句柄传递给 ``sdio_slave_recv_load_buf``，将缓冲区加载到驱动程序中。
3. 调用 ``sdio_slave_recv`` 或 ``sdio_slave_recv_packet`` 获取接收到的数据。如果需要采取非阻塞式调用，可以设置 ``wait`` 为 0。

   这两个 API 的区别在于，``sdio_slave_recv_packet`` 会提供更多有关数据包的信息，数据包可以由多个缓冲区组成。

   当此 API 返回 ``ESP_ERR_NOT_FINISHED`` 时，应循环调用此 API，直到返回值为 ``ESP_OK``。此时，在主机发送的数据包中，包含了所有与 ``ESP_ERR_NOT_FINISHED`` 一起返回的连续缓冲区，以及与 ``ESP_OK`` 一起返回的最后一个缓冲区。

   调用 ``sdio_slave_recv_get_buf`` 获取所接收数据的地址，以及每个缓冲区实际接收到的长度。数据包的长度是数据包中所有缓冲区接收长度的总和。

   如果主机发送的数据始终小于接收缓冲区的大小，或者数据包的边界（例如，数据只是一个字节流）无关紧要，则可以使用更简单的 ``sdio_slave_recv``。

4. 调用 ``sdio_recv_load_buf``，将经过处理的缓冲区句柄再次传递给驱动程序。

.. note::

  为减少复制数据的开销，驱动程序本身不具有任何内部缓冲区；应用程序有责任及时提供新的缓冲区，DMA 会自动将接收到的数据存储到缓冲区中。

发送 FIFO
^^^^^^^^^^^^

每当从机要发送数据时，它会触发一个中断，并由主机请求数据包长度。发送模式有两种：

- 数据流模式 (stream mode)：在此模式下，当缓冲区加载到驱动程序中时，无论之前的数据包是否已经发送，该缓冲区的长度会计入主机在传入通信中请求的数据包长度中。换句话说，即使之前还有未发送的数据包，新加载的缓冲区长度也会包括在主机请求的数据包长度中。这样，主机可以在一次传输中获取多个缓冲区的数据。
- 数据包模式 (packet mode)：在此模式下，数据包长度逐个更新，且仅在前一个数据包发送时更新。此时，主机在一次传输中只能获取一个缓冲区的数据。

.. note::

  为减少复制数据的开销，驱动程序本身没有内部缓冲区，DMA 直接从应用程序提供的缓冲区中获取数据。发送完成前，应用程序不应该访问缓冲区，以确保数据传输的正确性。

结构体 ``sdio_slave_config_t`` 中的 ``sending_mode`` 可以设置发送模式，``send_queue_size`` 可以设置缓冲区数量。缓冲区大小均限制在 4092 字节内。尽管在流模式下，一次传输可以发送多个缓冲区，但每个缓冲区在队列中仍然计为一个。

应用程序可以调用 ``sdio_slave_transmit`` 函数发送数据包。此时，函数在传输完成后返回，因此队列并未完全占用。若需要更高效率，应用程序可以改用以下函数：

1. 将缓冲区信息（地址、长度以及表示缓冲区的 ``arg`` 参数）传递给 ``sdio_slave_send_queue``。

   - 如果需要采用非阻塞调用，请设置 ``wait`` 为 0。
   - 如果 ``wait`` 并未设置为 ``portMAX_DELAY`` （等待直到缓冲区传输完成），应用程序应检查返回结果，确认数据是否已放入队列中，或是否已丢弃。

2. 调用 ``sdio_slave_send_get_finished`` 来获取并处理已完成的传输。在缓冲区 ``sdio_slave_send_get_finished`` 返回前不应修改缓冲区。这意味着缓冲区实际上发送给了主机，而非在队列中等待。

要使用队列参数中 ``arg`` ，可以采用以下几种方法：

    1. 直接将 ``arg`` 指向一个动态分配的缓冲区，并在传输完成后使用 ``arg`` 释放该缓冲区。
    2. 在传输结构体中封装传输信息，并将 ``arg`` 指向该结构体。使用该结构体还可以执行更多操作，例如::

          typedef struct {
              uint8_t* buffer;
              size_t   size;
              int      id;
          }sdio_transfer_t;

          //发送传输：
          sdio_transfer_t trans = {
              .buffer = ADDRESS_TO_SEND,
              .size = 8,
              .id = 3,  //第 3 个传输
          };
          sdio_slave_send_queue(trans.buffer, trans.size, &trans, portMAX_DELAY);

          //… 在此还可能发送更多传输

          //处理完成的传输：
          sdio_transfer_t* arg = NULL;
          sdio_slave_send_get_finished((void**)&arg, portMAX_DELAY);
          ESP_LOGI("tag", "(%d) successfully send %d bytes of %p", arg->id, arg->size, arg->buffer);
          some_post_callback(arg); //执行更多操作

    3. 用于该驱动程序的接收部分，将 ``arg`` 指向该缓冲区的接收缓冲区句柄。这样，可以在发送数据时直接使用该缓冲区来接收数据::

           uint8_t buffer[256]={1,2,3,4,5,6,7,8};
           sdio_slave_buf_handle_t handle = sdio_slave_recv_register_buf(buffer);
           sdio_slave_send_queue(buffer, 8, handle, portMAX_DELAY);

           //… 在此还可能发送更多传输

           //加载已完成的传输，准备接收
           sdio_slave_buf_handle_t handle = NULL;
           sdio_slave_send_get_finished((void**)&handle, portMAX_DELAY);
           sdio_slave_recv_load_buf(handle);

       更多详情，请参阅 :example:`peripherals/sdio`。


应用示例
-------------------

从机/主机通信的相关应用示例请参阅 :example:`peripherals/sdio`。

API 参考
-------------

.. include-build-file:: inc/sdio_slave_types.inc
.. include-build-file:: inc/sdio_slave.inc
