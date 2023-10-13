Communication with ESP SDIO Slave
=================================

:link_to_translation:`zh_CN:[中文]`

This document describes the process of initialization of an ESP SDIO Slave device and then provides details on the ESP SDIO Slave protocol - a non-standard protocol that allows an SDIO Host to communicate with an ESP SDIO slave.

The ESP SDIO Slave protocol was created to implement the communication between SDIO host and slave, because the SDIO specification only shows how to access the custom region of a card (by sending CMD52 and CMD53 to functions 1-7) without any details regarding the underlying hardware implementation.

.. _esp_sdio_slave_caps:

SDIO Slave Capabilities of Espressif Chips
------------------------------------------

The services provided by the SDIO Slave peripheral of the {IDF_TARGET_NAME} chip are listed in the table below:

.. list-table::
   :widths: 70 30
   :header-rows: 1

   * - Services
     - {IDF_TARGET_NAME}
   * - SDIO slave
     - Y
   * - :ref:`Tohost intr <esp_sdio_slave_interrupts>`
     - 8
   * - :ref:`Frhost intr <esp_sdio_slave_interrupts>`
     - 8
   * - :ref:`TX DMA <esp_sdio_slave_send_fifo>`
     - Y
   * - :ref:`RX DMA <esp_sdio_slave_rcv_fifo>`
     - Y
   * - :ref:`Shared registers <esp_sdio_slave_shared_registers>`
     - 56\*

\* Not including the interrupt registers


.. _esp_slave_init:

ESP SDIO Slave Initialization
-----------------------------

The host should initialize the {IDF_TARGET_NAME} SDIO slave according to the standard SDIO initialization process (Section 3.1.2 of `SDIO Simplified Specification <https://www.sdcard.org/downloads/pls/>`_). In this specification as well as below, the SDIO slave is called an SDIO/IO card. Here is a brief example of an ESP SDIO Slave initialization process:

1. SDIO reset

    CMD52 (Write 0x6 = 0x8)

2. SD reset

    CMD0

3. Check whether IO card (optional)

    CMD8

4. Send SDIO op cond and wait for card ready

    CMD5 arg = 0x00000000

    CMD5 arg = 0x00ff8000 (according to the response above, poll until ready)

    **Example:**

        Arg of R4 after first CMD5 (arg = 0x00000000) is 0xXXFFFF00.

        Keep sending CMD5 with arg = 0x00FFFF00 until the R4 shows card ready (arg bit 31 = 1).

5. Set address

    CMD3

6. Select card

    CMD7 (arg address according to CMD3 response)

    **Example:**

        Arg of R6 after CMD3 is 0x0001xxxx.

        Arg of CMD7 should be 0x00010000.

7. Select 4-bit mode (optional)

    CMD52 (Write 0x07 = 0x02)

8. Enable func1

    CMD52 (Write 0x02 = 0x02)

9. Enable SDIO interrupt (required if interrupt line (DAT1) is used)

    CMD52 (Write 0x04 = 0x03)

10. Set Func0 blocksize (optional, default value is 512 (0x200))

     CMD52/53 (Read 0x10 ~ 0x11)

     CMD52/53 (Write 0x10 = 0x00)

     CMD52/53 (Write 0x11 = 0x02)

     CMD52/53 (Read 0x10 ~ 0x11, read to check the final value)

11. Set Func1 blocksize (optional, default value is 512 (0x200))

     CMD52/53 (Read 0x110 ~ 0x111)

     CMD52/53 (Write 0x110 = 0x00)

     CMD52/53 (Write 0x111 = 0x02)

     CMD52/53 (Read 0x110 ~ 0x111, read to check the final value)


.. _esp_slave_protocol_layer:

ESP SDIO Slave Protocol
-----------------------

The ESP SDIO Slave protocol is based on the SDIO Specification's I/O Read/Write commands, i.e., CMD52 and CMD53. The protocol offers the following services:

- Sending FIFO and receiving FIFO
- 52 8-bit R/W registers shared by host and slave (For details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *SDIO Slave Controller* > *Register Summary* > SDIO SLC Host registers [`PDF <{IDF_TARGET_TRM_EN_URL}#sdioslave-reg-summ>`__])
- 16 general purpose interrupt sources, 8 from host to slave and 8 from slave to host

To begin communication, the host needs to enable the I/O Function 1 in the slave and access its registers as described below.

Check the code example: :example:`peripherals/sdio`

The :doc:`ESP Serial Slave Link </api-reference/protocols/esp_serial_slave_link>` component implements the logic of this protocol for ESP32 SDIO Host when communicating with an ESP32 SDIO slave.

.. _esp_sdio_slave_shared_registers:

Slave Register Table
^^^^^^^^^^^^^^^^^^^^

32-bit
""""""

- 0x044 (TOKEN_RDATA): in which bit 27-16 holds the number of the receiving buffer.
- 0x058 (INT_ST): holds the interrupt source bits from slave to host.
- 0x060 (PKT_LEN): holds the accumulated data length (in bytes) already read by host plus the data copied to the buffer but yet to be read.
- 0x0D4 (INT_CLR): write 1 to clear interrupt bits corresponding to INT_ST.
- 0x0DC (INT_ENA): mask bits for interrupts from slave to host.

8-bit
"""""

Shared general purpose registers:

- 0x06C-0x077: R/W registers 0-11 shared by slave and host.
- 0x07A-0x07B: R/W registers 14-15 shared by slave and host.
- 0x07E-0x07F: R/W registers 18-19 shared by slave and host.
- 0x088-0x08B: R/W registers 24-27 shared by slave and host.
- 0x09C-0x0BB: R/W registers 32-63 shared by slave and host.

Interrupt Registers:

- 0x08D (SLAVE_INT): bits for host to interrupt slave. auto clear.

FIFO (Sending and Receiving)
""""""""""""""""""""""""""""

0x090 - 0x1F7FF are reserved for FIFOs.

The address of CMD53 is related to the length requested to read from or write to the slave in a single transfer, as demonstrated by the equation below:

    *requested length = 0x1F800 - address*

The slave responds to data that has a length equal to the length field of CMD53. In cases where the data is longer than the **requested length**, the data will be zero filled (when sending) or discarded (when receiving). This includes both the block and the byte mode of CMD53.

.. note::

    The function number should be set to 1, and OP Code should be set to 1 (for CMD53).

    In order to achieve higher efficiency when accessing the FIFO by an arbitrary length, the block and byte modes of CMD53 can be used in combination. For example, given that the block size is set to 512 by default, you can write or get 1031 bytes of data from the FIFO by doing the following:

    1. Send CMD53 in block mode, block count = 2 (1024 bytes) to address 0x1F3F9 = 0x1F800 - **1031**.
    2. Then send CMD53 in byte mode, byte count = 8 (or 7 if your controller supports that) to address 0x1F7F9 = 0x1F800 - **7**.

.. _esp_sdio_slave_interrupts:

Interrupts
^^^^^^^^^^

SDIO interrupts are "level sensitive". For host interrupts, the slave sends an interrupt by pulling the DAT1 line down at a proper time. The host detects when the interrupt line is pulled down and reads the INT_ST register to determine the source of the interrupt. After that, the host can clear the interrupt bits by writing the ``INT_CLR`` register and process the interrupt. The host can also mask unneeded sources by clearing the bits in the INT_ENA register corresponding to the sources. If all the sources are cleared (or masked), the DAT1 line goes inactive.

On {IDF_TARGET_NAME}, the corresponding ``host_int`` bits are: bit 0 to bit 7.

For slave interrupts, the host sends a transfer to write the ``SLAVE_INT`` register. Once a bit is set to 1, the slave hardware and the driver will detect it and inform the application.

.. _esp_sdio_slave_rcv_fifo:

Receiving FIFO
^^^^^^^^^^^^^^

To write to the slave's receiving FIFO, the host should complete the following steps:

1. **Read the TOKEN1 field (bits 27-16) of the register TOKEN_RDATA (0x044)**. The buffer number remaining is TOKEN1 minus the number of buffers used by host.
2. **Make sure the buffer number is sufficient** (*buffer_size* x *buffer_num* is greater than the data to write, *buffer_size* is pre-defined between the host and the slave before the communication starts). Otherwise, keep returning to step 1 until the buffer size is sufficient.
3. **Write to the FIFO address with CMD53**. Note that the *requested length* should not exceed the length calculated at step 2, and the FIFO address is related to *requested length*.
4. **Calculate used buffers**. Note that a partially-used buffer at the tail is counted as used.

.. _esp_sdio_slave_send_fifo:

Sending FIFO
^^^^^^^^^^^^

To read the slave's sending FIFO, the host should complete the following steps:

1. **Wait for the interrupt line to become active** (optional, low by default).
2. **Read (poll) the interrupt bits in the INT_ST register** to monitor if new packets exist.
3. **If new packets are ready, read the PKT_LEN register**. Before reading the packets, determine the length of data to be read. As the host keeps the length of data already read from the slave, subtract this value from ``PKT_LEN``, the result will be the maximum length of data available for reading. If no data has been added to the sending FIFO yet, wait and poll until the slave is ready and update ``PKT_LEN``.
4. **Read from the FIFO using CMD53**. Note that the **requested length** should not be greater than calculated at Step 3, and the FIFO address is related to **requested length**.
5. **Update the read length**.
