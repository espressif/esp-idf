SPI Slave driver
=================

Overview
--------

The ESP32 has four SPI peripheral devices, called SPI0, SPI1, HSPI and VSPI. SPI0 is entirely dedicated to
the flash cache the ESP32 uses to map the SPI flash device it is connected to into memory. SPI1 is
connected to the same hardware lines as SPI0 and is used to write to the flash chip. HSPI and VSPI
are free to use, and with the spi_slave driver, these can be used as a SPI slave, driven from a 
connected SPI master.

The spi_slave driver
^^^^^^^^^^^^^^^^^^^^^

The spi_slave driver allows using the HSPI and/or VSPI peripheral as a full-duplex SPI slave. It can make
use of DMA to send/receive transactions of arbitrary length.

Terminology
^^^^^^^^^^^

The spi_slave driver uses the following terms:

* Host: The SPI peripheral inside the ESP32 initiating the SPI transmissions. One of HSPI or VSPI. 
* Bus: The SPI bus, common to all SPI devices connected to a master. In general the bus consists of the
  miso, mosi, sclk and optionally quadwp and quadhd signals. The SPI slaves are connected to these 
  signals in parallel. Each  SPI slave is also connected to one CS signal.

  - miso - Also known as q, this is the output of the serial stream from the ESP32 to the SPI master

  - mosi - Also known as d, this is the output of the serial stream from the SPI master to the ESP32

  - sclk - Clock signal. Each data bit is clocked out or in on the positive or negative edge of this signal

  - cs - Chip Select. An active Chip Select delineates a single transaction to/from a slave.

* Transaction: One instance of CS going active, data transfer from and to a master happening, and
  CS going inactive again. Transactions are atomic, as in they will never be interrupted by another
  transaction.


SPI transactions
^^^^^^^^^^^^^^^^

A full-duplex SPI transaction starts with the master pulling CS low. After this happens, the master
starts sending out clock pulses on the CLK line: every clock pulse causes a data bit to be shifted from
the master to the slave on the MOSI line and vice versa on the MISO line. At the end of the transaction,
the master makes CS high again.

Using the spi_slave driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Initialize a SPI peripheral as a slave by calling ``spi_slave_initialize``. Make sure to set the 
  correct IO pins in the ``bus_config`` struct. Take care to set signals that are not needed to -1.
  A DMA channel (either 1 or 2) must be given if transactions will be larger than 32 bytes, if not
  the dma_chan parameter may be 0.

- To set up a transaction, fill one or more spi_transaction_t structure with any transaction 
  parameters you need. Either queue all transactions by calling ``spi_slave_queue_trans``, later
  quering the result using ``spi_slave_get_trans_result``, or handle all requests synchroneously
  by feeding them into ``spi_slave_transmit``. The latter two  functions will block until the 
  master has initiated and finished a transaction, causing the queued data to be sent and received.

- Optional: to unload the SPI slave driver, call ``spi_slave_free``.


Transaction data and master/slave length mismatches
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Normally, data to be transferred to or from a device will be read from or written to a chunk of memory
indicated by the ``rx_buffer`` and ``tx_buffer`` members of the transaction structure. The SPI driver
may decide to use DMA for transfers, so these buffers should be allocated in DMA-capable memory using 
``pvPortMallocCaps(size, MALLOC_CAP_DMA)``.

The amount of data written to the buffers is limited by the ``length`` member of the transaction structure:
the driver will never read/write more data than indicated there. The ``length`` cannot define the actual
length of the SPI transaction; this is determined by the master as it drives the clock and CS lines. In
case the length of the transmission is larger than the buffer length, only the start of the transmission
will be sent and received. In case the transmission length is shorter than the buffer length, only data up 
to the length of the buffer will be exchanged.

Warning: Due to a design peculiarity in the ESP32, if the amount of bytes sent by the master or the length 
of the transmission queues in the slave driver, in bytes, is not both larger than eight and dividable by 
four, the SPI hardware can fail to write the last one to seven bytes to the receive buffer.


Application Example
-------------------

Slave/master communication: :example:`peripherals/spi_slave`.

API Reference
-------------

.. include:: /_build/inc/spi_slave.inc

