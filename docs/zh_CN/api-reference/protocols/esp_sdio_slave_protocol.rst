ESP SDIO 主从机通信
========================

:link_to_translation:`en:[English]`

本文档介绍了 ESP 数字输入输出 (Secure Digital Input and Output，SDIO) 从机设备的初始化过程，并提供 ESP SDIO 从机协议的详细信息。该协议为非标准协议，允许 SDIO 主机与 ESP SDIO 从机进行通信。

创建 ESP SDIO 从机协议是为了实现 SDIO 主机和从机之间的通信。这是因为 SDIO 规范只说明了如何访问卡的自定义区（向功能 1-7 发送 CMD52 和 CMD53），却没有说明底层的硬件实现过程。

.. _esp_sdio_slave_caps:

乐鑫芯片的 SDIO 从机功能
-----------------------------------

{IDF_TARGET_NAME} 芯片的 SDIO 从机提供以下服务：

.. list-table::
   :widths: 70 30
   :header-rows: 1

   * - 服务
     - {IDF_TARGET_NAME}
   * - SDIO 从机
     - Y
   * - :ref:`Tohost intr <esp_sdio_slave_interrupts>`
     - 8
   * - :ref:`Frhost intr <esp_sdio_slave_interrupts>`
     - 8
   * - :ref:`TX DMA <esp_sdio_slave_send_fifo>`
     - Y
   * - :ref:`RX DMA <esp_sdio_slave_rcv_fifo>`
     - Y
   * - :ref:`共享寄存器 <esp_sdio_slave_shared_registers>`
     - 56\*

\* 未包括中断寄存器


.. _esp_slave_init:

初始化 ESP SDIO 从机
-------------------------

主机需按照标准的 SDIO 初始化流程，对 {IDF_TARGET_NAME} SDIO 从机进行初始化（参考 `SDIO Simplified Specification <https://www.sdcard.org/downloads/pls/>`_ 3.1.2 章节）。下文将 SDIO 从机简称为 SD/SDIO 卡。以下是 ESP SDIO 从机初始化流程的一个简单示例：

1. SDIO 复位

    CMD52（写入 0x6 = 0x8）

2. SD 复位

    CMD0

3. 检查 IO 卡（可选）

    CMD8

4.  发送 SDIO op cond 指令，等待 IO 卡就绪

    CMD5 arg = 0x00000000

    CMD5 arg = 0x00ff8000（根据以上响应进行轮询，直到 IO 卡就绪）

    **示例：**

        首次 CMD5 (arg = 0x00000000) 后  R4 的 arg 为 0xXXFFFF00。

        不断发送 CMD5 arg = 0x00FFFF00，直到 R4 显示卡已就绪 (arg 31 = 1) 为止。

5. 设置地址

    CMD3

6. 选择卡

    CMD7（根据 CMD3 响应设置 arg）

    **示例：**

        CMD3 后 R6 的 arg 为 0x0001xxxx。

        CMD7 的 arg 应为 0x00010000。

7. 选择 4-bit 模式（可选）

    CMD52（写入 0x07 = 0x02）

8. 启用 Func1

    CMD52（写入 0x02 = 0x02）

9.  启用 SDIO 中断（使用中断线 (DAT1) 时必要）

    CMD52（写入 0x04 = 0x03）

10. 设置 Func0 块大小（可选，默认为 512 (0x200) 字节）

     CMD52/53（读取 0x10 ~ 0x11）

     CMD52/53（写入 0x10 = 0x00）

     CMD52/53（写入 0x11 = 0x02）

     CMD52/53（再次读取 0x10 ~ 0x11, 检查最终写入的值）

11. 设置 Func1 块大小（可选，默认为 512 (0x200) 字节）

     CMD52/53（读取 0x110 ~ 0x111）

     CMD52/53（写入 0x110 = 0x00）

     CMD52/53（写入 0x111 = 0x02）

     CMD52/53（再次读取 0x110 ~ 0x111, 检查最终写入的值）


.. _esp_slave_protocol_layer:

ESP SDIO 从机协议
--------------------------

ESP SDIO 从机协议基于 SDIO 规范的 I/O 读/写命令（即 CMD52 和 CMD53）创建。该协议提供以下服务：

- 发送 FIFO 和接收 FIFO
- 52 个主从机共享的 8 位读写寄存器（要了解详细信息，请参考 *{IDF_TARGET_NAME} 技术参考手册* > *SDIO 从机控制器* > *寄存器列表* > SDIO SLC Host 寄存器 [`PDF <{IDF_TARGET_TRM_CN_URL}#sdioslave-reg-summ>`__]）
- 16 个通用中断源，其中 8 个从主机到从机的中断源，8 个自从机到主机的中断源。

开始通信前，主机需启用从机的 I/O Function 1，访问从机的寄存器，如下所示。

代码示例：:example:`peripherals/sdio`

ESP32 SDIO 用作主机与 ESP32 SDIO 从机通信时，协议中涉及的逻辑由 :doc:`/api-reference/protocols/esp_serial_slave_link` 组件实现。

.. _esp_sdio_slave_shared_registers:

从机寄存器列表
^^^^^^^^^^^^^^^^^^^^^^

32 位
""""""

- 0x044 (TOKEN_RDATA): 第 27-16 位为接收 buffer 的数量。
- 0x058 (INT_ST): 保存自从机到主机的中断源位。
- 0x060 (PKT_LEN): 保存主机已读取的累计数据长度（以字节为单位），和已复制到缓冲区但尚未读取的数据。
- 0x0D4 (INT_CLR): 写 1 清除 与 INT_ST 对应的中断位。
- 0x0DC (INT_ENA): 从机到主机的中断屏蔽位。

8 位
"""""

共享通用寄存器：

- 0x06C-0x077: 读/写寄存器 0-11，主机和从机都可读写。
- 0x07A-0x07B: 读/写寄存器 14-15，主机和从机都可读写。
- 0x07E-0x07F: 读/写寄存器 18-19，主机和从机都可读写。
- 0x088-0x08B: 读/写寄存器 24-27，主机和从机都可读写。
- 0x09C-0x0BB: 读/写寄存器 32-63，主机和从机都可读写。

中断寄存器：

- 0x08D (SLAVE_INT)：主机对从机的中断位，会自动清空。

FIFO（发送和接收）
""""""""""""""""""""""""""""

0x090 - 0x1F7FF 用作 FIFO。

CMD53 的地址与单次传输中从从机读取或写入从机的要求长度 (requested length) 有关。如下式所示：

    *要求长度 = 0x1F800 - 地址*

从机返回的数据长度等于 CMD53 中定义的长度。如果数据长度大于 *requested length*，多余的数据会在发送时自动补零，或在接收时被丢弃，这一情况同时适用于 CMD53 的块模式和字节模式。

.. note::

    将功能编号设置为 1，将 OP 也设置为 1（适用于 CMD53）。

        为了提高以任意长度访问 FIFO 时的效率，可以将 CMD53 的块模式和字节模式结合使用。例如，如果块大小默认设置为 512 字节，则可以通过以下操作从 FIFO 中写入或获取 1031 字节的数据：

    1. 在块模式下发送 CMD53，block count = 2（1024 字节）到 0x1F3F9 = 0x1F800 - **1031**。
    2. 然后在字节模式下发送 CMD53，byte count = 8（如果控制器支持也可为 7）到 0x1F7F9 = 0x1F800 - **7**。

.. _esp_sdio_slave_interrupts:

中断
^^^^

SDIO 中断属于电平敏感中断。对于主机中断，从机通过在适当时间拉低 DAT1 线的方式发送中断。当该中断线被拉低后，主机会检测到变化并读取 INT_ST 寄存器，确定中断源。然后，主机写入 INT_CLR 寄存器清除中断位，并处理中断。主机也可以清除 INT_ENA 寄存器中相应位以屏蔽不需要的源。如果所有源都已清除或已屏蔽，DAT1 线状态无效。

{IDF_TARGET_NAME} 上对应的 host_int 位：0 至 7。

对于从机中断，主机发起一次传输，将数据写入 SLAVE_INT 寄存器。一旦某位被写入 1，从机硬件和驱动程序就会检测到变化，通知应用程序。

.. _esp_sdio_slave_rcv_fifo:

接收 FIFO
^^^^^^^^^^^

要写入从机的接收 FIFO，主机应完成以下步骤：

1. **读取 TOKEN_RDATA (0x044) 寄存器的 TOKEN1 字段（27-16 位）。** 剩余的缓冲数量为 TOKEN1 减去主机使用的缓冲数量。
2. **确保有足够的 buffer**，（*buffer_size* x *buffer_num* 应大于要写入数据的长度，*buffer_size* 是主机和从机在开始通信前预定义的值）。如果 buffer 不够，重复步骤 1，直至满足要求为止。
3. **用 CMD53 写入 FIFO 地址**。注意， *requested length* 不应超过步骤 2 中计算出的长度，FIFO 地址与 *requested length* 有关。
4. **计算已使用的缓冲**。 注意，尾部的缓冲即使仅部分使用，也属于已使用的范围。

.. _esp_sdio_slave_send_fifo:

发送 FIFO
^^^^^^^^^^^

要读取从机的发送 FIFO，主机应完成以下步骤：

1. **等待中断线有效**。（可选，默认为低电平）
2. **读取（轮询）INT_ST 寄存器中的中断位**，以监控是否存在新数据包。
3. **如果新数据包已准备就绪，读取 PKT_LEN 寄存器**。在读取数据包之前，确定要读取数据的长度。由于主机会保留已从从机中读取的数据的长度，因此，要从 PKT_LEN 中减去该值，得到可读取的最大数据长度。如果发送 FIFO 中尚未写入过数据，则继续等待并轮询，直到从机准备就绪，然后更新 PKT_LEN。
4. **用 CMD53 从 FIFO 中读取数据**。注意， *要求长度* 应不大于步骤 3 中计算出的长度，FIFO 地址与 *要求长度* 相关。
5. **更新读取长度**。
