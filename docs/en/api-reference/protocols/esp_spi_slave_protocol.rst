ESP SPI Slave HD (Half Duplex) Mode Protocol
============================================

:link_to_translation:`zh_CN:[中文]`

.. only:: esp32

    .. warning::

        ESP32 does not support this feature.

.. _esp_spi_slave_caps:

SPI Slave Capabilities of Espressif Chips
-----------------------------------------

+------------------+-------+----------+----------+----------+----------+----------+----------+----------+
|                  | ESP32 | ESP32-S2 | ESP32-C3 | ESP32-S3 | ESP32-C2 | ESP32-C6 | ESP32-H2 | ESP32-P4 |
+------------------+-------+----------+----------+----------+----------+----------+----------+----------+
| SPI Slave HD     | N     | Y (v2)   | Y (v2)   | Y (v2)   | Y (v2)   | Y (v2)   | Y (v2)   | Y (v2)   |
+------------------+-------+----------+----------+----------+----------+----------+----------+----------+
| Tohost intr      |       | N        | N        | N        | N        | N        | N        | N        |
+------------------+-------+----------+----------+----------+----------+----------+----------+----------+
| Frhost intr      |       | 2 \*     | 2 \*     | 2 \*     | 2 \*     | 2 \*     | 2 \*     | 2 \*     |
+------------------+-------+----------+----------+----------+----------+----------+----------+----------+
| TX DMA           |       | Y        | Y        | Y        | Y        | Y        | Y        | Y        |
+------------------+-------+----------+----------+----------+----------+----------+----------+----------+
| RX DMA           |       | Y        | Y        | Y        | Y        | Y        | Y        | Y        |
+------------------+-------+----------+----------+----------+----------+----------+----------+----------+
| Shared registers |       | 72       | 64       | 64       | 64       | 64       | 64       | 64       |
+------------------+-------+----------+----------+----------+----------+----------+----------+----------+

Introduction
------------

In the half duplex mode, the master has to use the protocol defined by the slave to communicate with the slave. Each transaction may consist of the following phases (listed by the order they should exist):

- Command: 8-bit, master to slave

    This phase determines the rest phases of the transactions. See :ref:`spi_slave_hd_supported_cmds`.

- Address: 8-bit, master to slave, optional

    For some commands (WRBUF, RDBUF), this phase specifies the address of the shared register to write to/read from. For other commands with this phase, they are meaningless but still have to exist in the transaction.

.. only:: esp32s2

    - Dummy: 8-bit (for 1-bit mode) or 4-bit (for 2/4-bit mode), floating, optional

        This phase is the turnaround time between the master and the slave on the bus, and also provides enough time for the slave to prepare the data to send to the master.

.. only:: not esp32s2

    - Dummy: 8-bit, floating, optional

        This phase is the turnaround time between the master and the slave on the bus, and also provides enough time for the slave to prepare the data to send to the master.

- Data: variable length, the direction is also determined by the command.

    This may be a data OUT phase, in which the direction is slave to master, or a data IN phase, in which the direction is master to slave.

The **direction** means which side (master or slave) controls the MOSI, MISO, WP, and HD pins.

Data IO Modes
-------------

In some IO modes, more data wires can be used to send the data. As a result, the SPI clock cycles required for the same amount of data will be less than in the 1-bit mode. For example, in QIO mode, address and data (IN and OUT) should be sent on all 4 data wires (MOSI, MISO, WP, and HD). Here are the modes supported by the ESP32-S2 SPI slave and the wire number (WN) used in corresponding modes.

+-------+------------+------------+--------------+---------+
| Mode  | Command WN | Address WN | Dummy cycles | Data WN |
+=======+============+============+==============+=========+
| 1-bit | 1          | 1          | 1            | 1       |
+-------+------------+------------+--------------+---------+
| DOUT  | 1          | 1          | 4            | 2       |
+-------+------------+------------+--------------+---------+
| DIO   | 1          | 2          | 4            | 2       |
+-------+------------+------------+--------------+---------+
| QOUT  | 1          | 1          | 4            | 4       |
+-------+------------+------------+--------------+---------+
| QIO   | 1          | 4          | 4            | 4       |
+-------+------------+------------+--------------+---------+
| QPI   | 4          | 4          | 4            | 4       |
+-------+------------+------------+--------------+---------+

Normally, which mode is used is determined by the command sent by the master (See :ref:`spi_slave_hd_supported_cmds`), except the QPI mode.

QPI Mode
^^^^^^^^

The QPI mode is a special state of the SPI Slave. The master can send the ENQPI command to put the slave into the QPI mode state. In the QPI mode, the command is also sent in 4-bit, thus it is not compatible with the normal modes. The master should only send QPI commands when the slave is in QPI mode. To exit from the QPI mode, master can send the EXQPI command.

.. _spi_slave_hd_supported_cmds:

Supported Commands
------------------

.. note::
    The command name is in a master-oriented direction. For example, WRBUF means master writes the buffer of slave.

+----------+---------------------+---------+----------+----------------------------------------------------------+
| Name     | Description         | Command | Address  | Data                                                     |
+==========+=====================+=========+==========+==========================================================+
| WRBUF    | Write buffer        | 0x01    | Buf addr | master to slave, no longer than buffer size              |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| RDBUF    | Read buffer         | 0x02    | Buf addr | slave to master, no longer than buffer size              |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| WRDMA    | Write DMA           | 0x03    | 8 bits   | master to slave, no longer than length provided by slave |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| RDDMA    | Read DMA            | 0x04    | 8 bits   | slave to master, no longer than length provided by slave |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| SEG_DONE | Segments done       | 0x05    | -        | -                                                        |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| ENQPI    | Enter QPI mode      | 0x06    | -        | -                                                        |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| WR_DONE  | Write segments done | 0x07    | -        | -                                                        |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| CMD8     | Interrupt           | 0x08    | -        | -                                                        |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| CMD9     | Interrupt           | 0x09    | -        | -                                                        |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| CMDA     | Interrupt           | 0x0A    | -        | -                                                        |
+----------+---------------------+---------+----------+----------------------------------------------------------+
| EXQPI    | Exit QPI mode       | 0xDD    | -        | -                                                        |
+----------+---------------------+---------+----------+----------------------------------------------------------+

Moreover, WRBUF, RDBUF, WRDMA, and RDDMA commands have their 2-bit and 4-bit version. To do transactions in 2-bit or 4-bit mode, send the original command ORed by the corresponding command mask below. For example, command 0xA1 means WRBUF in QIO mode.

+-------+------+
| Mode  | Mask |
+=======+======+
| 1-bit | 0x00 |
+-------+------+
| DOUT  | 0x10 |
+-------+------+
| DIO   | 0x50 |
+-------+------+
| QOUT  | 0x20 |
+-------+------+
| QIO   | 0xA0 |
+-------+------+
| QPI   | 0xA0 |
+-------+------+

Segment Transaction Mode
------------------------

Segment transaction mode is the only mode supported by the SPI Slave HD driver for now. In this mode, for a transaction the slave loads onto the DMA, the master is allowed to read or write in segments. In this way, the master does not have to prepare a large buffer as the size of data provided by the slave. After the master finishes reading/writing a buffer, it has to send the corresponding termination command to the slave as a synchronization signal. The slave driver will update new data (if exist) onto the DMA upon seeing the termination command.

The termination command is WR_DONE (0x07) for WRDMA and CMD8 (0x08) for RDDMA.

Here is an example for the flow the master read data from the slave DMA:

1. The slave loads 4092 bytes of data onto the RDDMA.
2. The master do seven RDDMA transactions, each of them is 512 bytes long, and reads the first 3584 bytes from the slave.
3. The master do the last RDDMA transaction of 512 bytes (equal, longer, or shorter than the total length loaded by the slave are all allowed). The first 508 bytes are valid data from the slave, while the last 4 bytes are meaningless bytes.
4. The master sends CMD8 to the slave.
5. The slave loads another 4092 bytes of data onto the RDDMA.
6. The master can start new reading transactions after it sends the CMD8.
