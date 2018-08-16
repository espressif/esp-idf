ESP SDIO slave protocol
=======================

The protocol is based on Function 1 access by CMD52 and CMD53, offering 3 services: (1) sending and receiving FIFO, (2) 52 8-bit R/W
register shared by host and slave, (3) 8 general purpose interrupt sources from host to slave and 8 in the oppsite direction.

The host should access the registers below as described to communicate with slave.

Slave register table
--------------------

32-bit
^^^^^^^
- 0x044 (TOKEN_RDATA): in which bit 27-16 holds the receiving buffer number.
- 0x058 (INT_ST): holds the interrupt source bits from slave to host.
- 0x060 (PKT_LEN): holds the accumulated length (by byte) to be sent from slave to host.
- 0x0D4 (INT_CLR): write 1 to clear interrupt bits corresponding to INT_ST.
- 0x0DC (INT_ENA): mask bits for interrupts from slave to host.

8-bit
^^^^^
Shared general purpose registers:

- 0x06C-0x077: R/W registers 0-11 shared by slave and host.
- 0x07A-0x07B: R/W registers 14-15 shared by slave and host.
- 0x07E-0x07F: R/W registers 18-19 shared by slave and host.
- 0x088-0x08B: R/W registers 24-27 shared by slave and host.
- 0x09C-0x0BB: R/W registers 32-63 shared by slave and host.

Interrupt Registers:
- 0x08D (SLAVE_INT): bits for host to interrupt slave. auto clear.

FIFO (sending and receiving)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0x090 - 0x1F7FF are reserved for FIFOs.

The address of CMD53 is related to the length requested to read from/write to
the slave in a single transfer:

    *requested length = 0x1F800-address*

The slave will respond with the length according to the length field in
CMD53, with the data longer than *requested length* filled with 0 (sending)
or discard (receiving).

.. note:: This includes both the block and the byte mode of CMD53.

    The function number should be set to 1, OP Code should be set to 1 (for CMD53).

    It is allowed to use CMD53 mode combination of block+byte to get higher
    effeciency when accessing the FIFO by arbitrary length. E.g. The block
    size is set to 512 by default, you can write/get 1031 bytes of data
    to/from the FIFO by:

    1. Send CMD53 in block mode, block count=2 (1024 bytes) to address
       0x1F3F9=0x1F800-**1031**.
    2. Then send CMD53 in byte mode, byte count=8 (or 7 if your controller
       supports that) to address 0x1F7F9=0x1F800-**7**.

Interrupts
----------

For the host interrupts, the slave raise the interrupt by pulling DAT1 line down at a proper time (level sensitive).
The host detect this and read the INT_ST register to see the source. Then the host can clear it by writing the INT_CLR
register and do something with the interrupt. The host can also mask unneeded sources by clearing the bits in INT_ENA
register corresponding to the sources. If all the sources are cleared (or masked), the DAT1 line goes inactive.

``sdio_slave_hostint_t`` (:doc:`sdio_slave`) shows the bit definition corresponding to host interrupt sources.

For the slave interrupts, the host send transfers to write the SLAVE_INT register. Once a bit is written from 0 to 1,
the slave hardware and driver will detect it and inform the app.

Receiving FIFO
--------------

To write the receiving FIFO in the slave, host should work in the following steps:

1. Read the TOKEN1 field (bits 27-16) of TOKEN_RDATA (0x044) register. The buffer number remaining is TOKEN1 minus
   the number of buffers used by host.
2. Make sure the buffer number is sufficient (*buffer_size* * *buffer_num* is greater than data to write, *buffer_size*
   is pre-defined between the host and the slave before the communication starts). Or go back to step 1 until the buffer
   is enough.
3. Write to the FIFO address with CMD53. Note that the *requested length* should not be larger than calculated in step 2,
   and the FIFO address is related to *rquested length*.
4. Calculate used buffers, note that non-full buffer at the tail should be seen as one that is used.

Sending FIFO
------------

To read the sending FIFO in the slave, host should work in the following steps:

1. Wait for the interrupt line to be active (optional, low by default).
2. Read (poll) the interrupt bits in INT_ST register to see whether new packets exists.
3. If new packets are ready, reads the PKT_LEN reg. The data length to read from slave is PKT_LEN minuses the length
   that has been read from the host. If the PKT_LEN is not larger than used, wait and poll until the slave is ready and
   update the PKT_LEN.
4. Read from the FIFO with CMD53. Note that the *requested length* should not be larger than calculated in step3, and
   the FIFO address is related to *requested length*.
5. Recored read length.

