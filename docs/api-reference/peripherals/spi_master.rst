SPI Master driver
=================

Overview
--------

The ESP32 has four SPI peripheral devices, called SPI0, SPI1, HSPI and VSPI. SPI0 is entirely dedicated to
the flash cache the ESP32 uses to map the SPI flash device it is connected to into memory. SPI1 is
connected to the same hardware lines as SPI0 and is used to write to the flash chip. HSPI and VSPI
are free to use. SPI1, HSPI and VSPI all have three chip select lines, allowing them to drive up to
three SPI devices each as a master.

The spi_master driver
^^^^^^^^^^^^^^^^^^^^^

The spi_master driver allows easy communicating with SPI slave devices, even in a multithreaded environment.
It fully transparently handles DMA transfers to read and write data and automatically takes care of 
multiplexing between different SPI slaves on the same master

Terminology
^^^^^^^^^^^

The spi_master driver uses the following terms:

* Host: The SPI peripheral inside the ESP32 initiating the SPI transmissions. One of SPI, HSPI or VSPI. (For 
  now, only HSPI or VSPI are actually supported in the driver; it will support all 3 peripherals 
  somewhere in the future.)
* Bus: The SPI bus, common to all SPI devices connected to one host. In general the bus consists of the
  miso, mosi, sclk and optionally quadwp and quadhd signals. The SPI slaves are connected to these 
  signals in parallel.

  - miso - Also known as q, this is the input of the serial stream into the ESP32

  - mosi - Also known as d, this is the output of the serial stream from the ESP32

  - sclk - Clock signal. Each data bit is clocked out or in on the positive or negative edge of this signal

  - quadwp - Write Protect signal. Only used for 4-bit (qio/qout) transactions.

  - quadhd - Hold signal. Only used for 4-bit (qio/qout) transactions.

* Device: A SPI slave. Each SPI slave has its own chip select (CS) line, which is made active when
  a transmission to/from the SPI slave occurs.
* Transaction: One instance of CS going active, data transfer from and/or to a device happening, and
  CS going inactive again. Transactions are atomic, as in they will never be interrupted by another
  transaction.


SPI transactions
^^^^^^^^^^^^^^^^

A transaction on the SPI bus consists of five phases, any of which may be skipped:

* The command phase. In this phase, a command (0-16 bit) is clocked out.
* The address phase. In this phase, an address (0-64 bit) is clocked out.
* The write phase. The master sends data to the slave.
* The dummy phase. The phase is configurable, used to meet the timing requirements.
* The read phase. The slave sends data to the master.

In full duplex, the read and write phases are combined, causing the SPI host to read and
write data simultaneously. The total transaction length is decided by 
``dev_conf.command_bits + dev_conf.address_bits + trans_conf.length``, while the ``trans_conf.rx_length``
only determins length of data received into the buffer.

In half duplex, the length of write phase and read phase are decided by ``trans_conf.length`` and 
``trans_conf.rx_length`` respectively. ** Note that a half duplex transaction with both a read and 
write phase is not supported when using DMA. ** If such transaction is needed, you have to use one 
of the alternative solutions:
  1. use full-duplex mode instead.
  2. disable the DMA by set the last parameter to 0 in bus initialization function just as belows:
     ``ret=spi_bus_initialize(VSPI_HOST, &buscfg, 0);``  

     this may prohibit you from transmitting and receiving data longer than 32 bytes.
  3. try to use command and address field to replace the write phase.

The command and address phase are optional in that not every SPI device will need to be sent a command
and/or address. This is reflected in the device configuration: when the ``command_bits`` or ``address_bits``
fields are set to zero, no command or address phase is done.

Something similar is true for the read and write phase: not every transaction needs both data to be written
as well as data to be read. When ``rx_buffer`` is NULL (and SPI_USE_RXDATA) is not set) the read phase 
is skipped. When ``tx_buffer`` is NULL (and SPI_USE_TXDATA) is not set) the write phase is skipped.

Using the spi_master driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Initialize a SPI bus by calling ``spi_bus_initialize``. Make sure to set the correct IO pins in
  the ``bus_config`` struct. Take care to set signals that are not needed to -1.

- Tell the driver about a SPI slave device connected to the bus by calling spi_bus_add_device. 
  Make sure to configure any timing requirements the device has in the ``dev_config`` structure.
  You should now have a handle for the device, to be used when sending it a transaction.

- To interact with the device, fill one or more spi_transaction_t structure with any transaction 
  parameters you need. Either queue all transactions by calling ``spi_device_queue_trans``, later
  quering the result using ``spi_device_get_trans_result``, or handle all requests synchroneously
  by feeding them into ``spi_device_transmit``.

- Optional: to unload the driver for a device, call ``spi_bus_remove_device`` with the device
  handle as an argument

- Optional: to remove the driver for a bus, make sure no more drivers are attached and call 
  ``spi_bus_free``.


Transaction data
^^^^^^^^^^^^^^^^

Normally, data to be transferred to or from a device will be read from or written to a chunk of memory
indicated by the ``rx_buffer`` and ``tx_buffer`` members of the transaction structure. 
When DMA is enabled for transfers, these buffers are highly recommended to meet the requirements as belows:
  1. allocated in DMA-capable memory using ``pvPortMallocCaps(size, MALLOC_CAP_DMA)``;
  2. 32-bit aligned (start from the boundary and have length of multiples of 4 bytes).
If these requirements are not satisfied, efficiency of the transaction will suffer due to the allocation and 
memcpy of temporary buffers.

Sometimes, the amount of data is very small making it less than optimal allocating a separate buffer
for it. If the data to be transferred is 32 bits or less, it can be stored in the transaction struct
itself. For transmitted data, use the ``tx_data`` member for this and set the ``SPI_USE_TXDATA`` flag
on the transmission. For received data, use ``rx_data`` and set ``SPI_USE_RXDATA``. In both cases, do
not touch the ``tx_buffer`` or ``rx_buffer`` members, because they use the same memory locations
as ``tx_data`` and ``rx_data``.

Application Example
-------------------
 
Display graphics on the 320x240 LCD of WROVER-Kits: :example:`peripherals/spi_master`.


API Reference - SPI Common
--------------------------

.. include:: /_build/inc/spi_common.inc


API Reference - SPI Master
--------------------------

.. include:: /_build/inc/spi_master.inc

