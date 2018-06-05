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

In full duplex mode, the read and write phases are combined, and the SPI host reads and
writes data simultaneously. The total transaction length is decided by 
``command_bits + address_bits + trans_conf.length``, while the ``trans_conf.rx_length``
only determins length of data received into the buffer.

While in half duplex mode, the host have independent write and read phases. The length of write phase and read phase are
decided by ``trans_conf.length`` and ``trans_conf.rx_length`` respectively. 

The command and address phase are optional in that not every SPI device will need to be sent a command
and/or address. This is reflected in the device configuration: when the ``command_bits`` or ``address_bits``
fields are set to zero, no command or address phase is done.

Something similar is true for the read and write phase: not every transaction needs both data to be written
as well as data to be read. When ``rx_buffer`` is NULL (and SPI_USE_RXDATA) is not set) the read phase 
is skipped. When ``tx_buffer`` is NULL (and SPI_USE_TXDATA) is not set) the write phase is skipped.

GPIO matrix and native pins
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Most peripheral pins in ESP32 can directly connect to a GPIO, which is called *native pin*. When the peripherals are
required to work with other pins than the native pins, ESP32 use a *GPIO matrix* to realize this. If one of the pins is
not native, the driver automatically routes all the signals to the GPIO matrix, which works under 80MHz. The signals are
sampled and sent to peripherals or the GPIOs. 

When the GPIO matrix is used, signals cannot propogate to the peripherals over 40MHz, and the setup time of MISO is very
likely violated. Hence the clock frequency limitation is a little lower than the case without GPIO matrix.

Native pins for SPI controllers are as below:

+----------+------+------+
| Pin Name | HSPI | VSPI |
+          +------+------+
|          | GPIO Number |
+==========+======+======+
| CS0*     | 15   | 5    |
+----------+------+------+
| SCLK     | 14   | 18   |
+----------+------+------+
| MISO     | 12   | 19   |
+----------+------+------+
| MOSI     | 13   | 23   |
+----------+------+------+
| QUADWP   | 2    | 22   |
+----------+------+------+
| QUADHD   | 4    | 21   |
+----------+------+------+

note * Only the first device attaching to the bus can use CS0 pin.

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

Command and address phases
^^^^^^^^^^^^^^^^^^^^^^^^^^

During the command and address phases, ``cmd`` and ``addr`` field in the
``spi_transaction_t`` struct are sent to the bus, while nothing is read at the
same time. The default length of command and address phase are set in the
``spi_device_interface_config_t`` and by ``spi_bus_add_device``. When the the
flag ``SPI_TRANS_VARIABLE_CMD`` and ``SPI_TRANS_VARIABLE_ADDR`` are not set in
the ``spi_transaction_t``,the driver automatically set the length of these 
phases to the default value as set when the device is initialized respectively.

If the length of command and address phases needs to be variable, declare a
``spi_transaction_ext_t`` descriptor, set the flag ``SPI_TRANS_VARIABLE_CMD`` 
or/and ``SPI_TRANS_VARIABLE_ADDR`` in the ``flags`` of ``base`` member and 
configure the rest part of ``base`` as usual. Then the length of each phases 
will be ``command_bits`` and ``address_bits`` set in the ``spi_transaction_ext_t``.

Write and read phases
^^^^^^^^^^^^^^^^^^^^^

Normally, data to be transferred to or from a device will be read from or written to a chunk of memory
indicated by the ``rx_buffer`` and ``tx_buffer`` members of the transaction structure. 
When DMA is enabled for transfers, these buffers are highly recommended to meet the requirements as below:

  1. allocated in DMA-capable memory using ``pvPortMallocCaps(size, MALLOC_CAP_DMA)``;
  2. 32-bit aligned (start from the boundary and have length of multiples of 4 bytes).

If these requirements are not satisfied, efficiency of the transaction will suffer due to the allocation and 
memcpy of temporary buffers.

.. note::  Half duplex transactions with both read and write phases are not supported when using DMA. See
  :ref:`spi_known_issues` for details and workarounds.

Sometimes, the amount of data is very small making it less than optimal allocating a separate buffer
for it. If the data to be transferred is 32 bits or less, it can be stored in the transaction struct
itself. For transmitted data, use the ``tx_data`` member for this and set the ``SPI_USE_TXDATA`` flag
on the transmission. For received data, use ``rx_data`` and set ``SPI_USE_RXDATA``. In both cases, do
not touch the ``tx_buffer`` or ``rx_buffer`` members, because they use the same memory locations
as ``tx_data`` and ``rx_data``.

Speed and Timing Considerations
-------------------------------

Transferring speed
^^^^^^^^^^^^^^^^^^

There're two factors limiting the transferring speed: (1) The transaction interval, (2) The SPI clock frequency used.
When large transactions are used, the clock frequency determines the transferring speed; while the interval effects the
speed a lot if small transactions are used.

    1. Transaction interval: The interval mainly comes from the cost of FreeRTOS queues and the time switching between
       tasks and the ISR. It also takes time for the software to setup spi peripheral registers as well as copy data to
       FIFOs, or setup DMA links. Depending on whether the DMA is used, the interval of an one-byte transaction is around 
       25us typically. 
            
            1.  The CPU is blocked and switched to other tasks when the
                transaction is in flight. This save the cpu time but increase the interval. 
            2.  When the DMA is enabled, it needs about 2us per transaction to setup the linked list. When the master is
                transferring, it automatically read data from the linked list. If the DMA is not enabled,
                CPU has to write/read each byte to/from the FIFO by itself. Usually this is faster than 2us, but the
                transaction length is limited to 32 bytes for both write and read.
       
       Typical transaction interval with one byte data is as below:

       +-----------------------+---------+
       | Transaction Time (us) | Typical |
       +=======================+=========+
       | DMA                   | 24      | 
       +-----------------------+---------+
       | No DMA                | 22      |
       +-----------------------+---------+

    2. SPI clock frequency: Each byte transferred takes 8 times of the clock period *8/fspi*. If the clock frequency is
       too high, some functions may be limited to use. See :ref:`timing_considerations`.

For a normal transaction, the overall cost is *20+8n/Fspi[MHz]* [us] for n bytes tranferred
in one transaction. Hence the transferring speed is : *n/(20+8n/Fspi)*. Example of transferring speed under 8MHz
clock speed:

+-----------+----------------------+--------------------+------------+-------------+
| Frequency | Transaction Interval | Transaction Length | Total Time | Total Speed |
|           |                      |                    |            |             |
| [MHz]     | [us]                 | [bytes]            | [us]       | [kBps]      |
+===========+======================+====================+============+=============+
| 8         | 25                   | 1                  | 26         | 38.5        |
+-----------+----------------------+--------------------+------------+-------------+
| 8         | 25                   | 8                  | 33         | 242.4       |
+-----------+----------------------+--------------------+------------+-------------+
| 8         | 25                   | 16                 | 41         | 490.2       |
+-----------+----------------------+--------------------+------------+-------------+
| 8         | 25                   | 64                 | 89         | 719.1       |
+-----------+----------------------+--------------------+------------+-------------+
| 8         | 25                   | 128                | 153        | 836.6       |
+-----------+----------------------+--------------------+------------+-------------+

When the length of transaction is short, the cost of transaction interval is really high. Please try to squash data
into one transaction if possible to get higher transfer speed.

.. _timing_considerations:

Timing considerations
^^^^^^^^^^^^^^^^^^^^^
Due to the input delay of MISO pin, ESP32 SPI master cannot read data at very high speed. The frequency allowed is
rather low when the GPIO matrix is used. Currently only frequency not greater than 8.8MHz is fully supported. When the
frequency is higher, you have to use the native pins or the *dummy bit workaround*.

.. _dummy_bit_workaround:

**Dummy bit workaround:** We can insert dummy clocks (during which the host does not read data) before the read phase
actually begins. The slave still sees the dummy clocks and gives out data, but the host does not read until the read
phase. This compensates the lack of setup time of MISO required by the host, allowing the host reading at higher
frequency.

The maximum frequency (in MHz) host can read (or read and write) under different conditions is as below:

+-------------+-------------+-----------+-----------------------------+
| Frequency Limit           | Dummy Bits| Comments                    | 
+-------------+-------------+ Used      +                             +
| GPIO matrix | Native pins | By Driver |                             |
+=============+=============+===========+=============================+
| 8.8         | N.M.        | 0         |                             |
+-------------+-------------+-----------+-----------------------------+
| N.M.        | N.M.        | 1         | Half Duplex, no DMA allowed |
+-------------+-------------+-----------+                             +
| N.M.        | N.M.        | 2         |                             |
+-------------+-------------+-----------+-----------------------------+

N.M.: Not Measured Yet.

And if the host only writes, the *dummy bit workaround* is not used and the frequency limit is as below:

+-------------+----------------------+
| GPIO matrix | Native pins          |
+=============+======================+
| 40          | 80                   |
+-------------+----------------------+

Thread Safety
-------------

The SPI driver API is thread safe when multiple SPI devices on the same bus are accessed from different tasks. However, the driver is not thread safe if the same SPI device is accessed from multiple tasks.

In this case, it is recommended to either refactor your application so only a single task accesses each SPI device, or to add mutex locking around access of the shared device.

.. _spi_known_issues:

Known Issues
------------

1. Half duplex mode is not compatible with DMA when both writing and reading phases exist.

   If such transactions are required, you have to use one of the alternative solutions:

   1. use full-duplex mode instead.
   2. disable the DMA by setting the last parameter to 0 in bus initialization function just as below:
      ``ret=spi_bus_initialize(VSPI_HOST, &buscfg, 0);``  

      this may prohibit you from transmitting and receiving data longer than 32 bytes.
   3. try to use command and address field to replace the write phase.

2. Full duplex mode is not compatible with the *dummy bit workaround*, hence the frequency is limited. See :ref:`dummy
   bit speed-up workaround <dummy_bit_workaround>`.


Application Example
-------------------
 
Display graphics on the 320x240 LCD of WROVER-Kits: :example:`peripherals/spi_master`.


API Reference - SPI Common
--------------------------

.. include:: /_build/inc/spi_common.inc


API Reference - SPI Master
--------------------------

.. include:: /_build/inc/spi_master.inc

