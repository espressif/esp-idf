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
multiplexing between different SPI slaves on the same master.

.. note::

    **Notes about thread safety**

    The SPI driver API is thread safe when multiple SPI devices on the same bus are accessed from different tasks. However, the driver is not thread safe if the same SPI device is accessed from multiple tasks.

    In this case, it is recommended to either refactor your application so only a single task accesses each SPI device, or to add mutex locking around access of the shared device.


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
as well as data to be read. When ``rx_buffer`` is NULL (and SPI_TRANS_USE_RXDATA) is not set) the read phase
is skipped. When ``tx_buffer`` is NULL (and SPI_TRANS_USE_TXDATA) is not set) the write phase is skipped.

The driver offers two different kinds of transactions: the interrupt
transactions and the polling transactions. Each device can choose one kind of
transaction to send. See :ref:`mixed_transactions` if your device do require
both kinds of transactions.

.. _interrupt_transactions:

Interrupt transactions
""""""""""""""""""""""""

The interrupt transactions use an interrupt-driven logic when the
transactions are in-flight. The routine will get blocked, allowing the CPU to
run other tasks, while it is waiting for a transaction to be finished.

Interrupt transactions can be queued into a device, the driver automatically
send them one-by-one in the ISR. A task can queue several transactions, and
then do something else before the transactions are finished.

.. _polling_transactions:

Polling transactions
""""""""""""""""""""

The polling transactions don't rely on the interrupt, the routine keeps polling
the status bit of the SPI peripheral until the transaction is done.

All the tasks that do interrupt transactions may get blocked by the queue, at
which point they need to wait for the ISR to run twice before the transaction
is done. Polling transactions save the time spent on queue handling and
context switching, resulting in a smaller transaction interval smaller. The
disadvantage is that the the CPU is busy while these transactions are in
flight.

The ``spi_device_polling_end`` routine spends at least 1us overhead to
unblock other tasks when the transaction is done. It is strongly recommended
to wrap a series of polling transactions inside of ``spi_device_acquire_bus``
and ``spi_device_release_bus`` to avoid the overhead. (See
:ref:`bus_acquiring`)

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

.. _bus_acquiring:

Bus acquiring
^^^^^^^^^^^^^

Sometimes you may want to send spi transactions exclusively, continuously, to
make it as fast as possible. You may use ``spi_device_acquire_bus`` and
``spi_device_release_bus`` to realize this. When the bus is acquired,
transactions to other devices (no matter polling or interrupt) are pending
until the bus is released.

Using the spi_master driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Initialize a SPI bus by calling ``spi_bus_initialize``. Make sure to set the correct IO pins in
  the ``bus_config`` struct. Take care to set signals that are not needed to -1.

- Tell the driver about a SPI slave device connected to the bus by calling spi_bus_add_device.
  Make sure to configure any timing requirements the device has in the ``dev_config`` structure.
  You should now have a handle for the device, to be used when sending it a transaction.

- To interact with the device, fill one or more spi_transaction_t structure with any transaction
  parameters you need. Then send them either in a polling way or the interrupt way:

    - :ref:`Interrupt <interrupt_transactions>`
        Either queue all transactions by calling ``spi_device_queue_trans``,
        and at a later time query the result using
        ``spi_device_get_trans_result``, or handle all requests
        synchroneously by feeding them into ``spi_device_transmit``.

    - :ref:`Polling <polling_transactions>`
        Call the ``spi_device_polling_transmit`` to send polling
        transactions. Alternatively, you can send a polling transaction by
        ``spi_device_polling_start`` and ``spi_device_polling_end`` if you
        want to insert something between them.

- Optional: to do back-to-back transactions to a device, call
  ``spi_device_acquire_bus`` before and ``spi_device_release_bus`` after the
  transactions.

- Optional: to unload the driver for a device, call ``spi_bus_remove_device`` with the device
  handle as an argument

- Optional: to remove the driver for a bus, make sure no more drivers are attached and call
  ``spi_bus_free``.

Tips
""""

1. Transactions with small amount of data:
    Sometimes, the amount of data is very small making it less than optimal allocating a separate buffer
    for it. If the data to be transferred is 32 bits or less, it can be stored in the transaction struct
    itself. For transmitted data, use the ``tx_data`` member for this and set the ``SPI_TRANS_USE_TXDATA`` flag
    on the transmission. For received data, use ``rx_data`` and set ``SPI_TRANS_USE_RXDATA``. In both cases, do
    not touch the ``tx_buffer`` or ``rx_buffer`` members, because they use the same memory locations
    as ``tx_data`` and ``rx_data``.

2. Transactions with integers other than uint8_t
    The SPI peripheral reads and writes the memory byte-by-byte. By default,
    the SPI works at MSB first mode, each bytes are sent or received from the
    MSB to the LSB. However, if you want to send data with length which is
    not multiples of 8 bits, unused bits are sent.

    E.g. you write ``uint8_t data = 0x15`` (00010101B), and set length to
    only 5 bits, the sent data is ``00010B`` rather than expected ``10101B``.

    Moreover, ESP32 is a little-endian chip whose lowest byte is stored at
    the very beginning address for uint16_t and uint32_t variables. Hence if
    a uint16_t is stored in the memory, it's bit 7 is first sent, then bit 6
    to 0, then comes its bit 15 to bit 8.

    To send data other than uint8_t arrays, macros ``SPI_SWAP_DATA_TX`` is
    provided to shift your data to the MSB and swap the MSB to the lowest
    address; while ``SPI_SWAP_DATA_RX`` can be used to swap received data
    from the MSB to it's correct place.

GPIO matrix and IOMUX
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Most peripheral signals in ESP32 can connect directly to a specific GPIO, which is called its IOMUX pin. When a
peripheral signal is routed to a pin other than its IOMUX pin, ESP32 uses the less direct GPIO matrix to make this
connection.

If the driver is configured with all SPI signals set to their specific IOMUX pins (or left unconnected), it will bypass
the GPIO matrix. If any SPI signal is configured to a pin other than its IOMUx pin, the driver will automatically route
all the signals via the GPIO Matrix. The GPIO matrix samples all signals at 80MHz and sends them between the GPIO and
the peripheral.

When the GPIO matrix is used, signals faster than 40MHz cannot propagate and the setup time of MISO is more easily
violated, since the input delay of MISO signal is increased. The maximum clock frequency with GPIO Matrix is 40MHz
or less, whereas using all IOMUX pins allows 80MHz.

.. note:: More details about influence of input delay on the maximum clock frequency, see :ref:`timing_considerations` below.

IOMUX pins for SPI controllers are as below:

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

.. _mixed_transactions:

Notes to send mixed transactions to the same device
"""""""""""""""""""""""""""""""""""""""""""""""""""

Though we suggest to send only one type (interrupt or polling) of
transactions to one device to reduce coding complexity, it is supported to
send both interrupt and polling transactions alternately. Notes below is to
help you do this.

The polling transactions should be started when all the other transactions
are finished, no matter they are polling or interrupt.

An unfinished polling transaction forbid other transactions from being sent.
Always call ``spi_device_polling_end`` after ``spi_device_polling_start`` to
allow other device using the bus, or allow other transactions to be started
to the same device. You can use ``spi_device_polling_transmit`` to simplify
this if you don't need to do something during your polling transaction.

An in-flight polling transaction would get disturbed by the ISR operation
caused by interrupt transactions. Always make sure all the interrupt
transactions sent to the ISR are finished before you call
``spi_device_polling_start``. To do that, you can call
``spi_device_get_trans_result`` until all the transactions are returned.

It is strongly recommended to send mixed transactions to the same device in
only one task to control the calling sequence of functions.

Speed and Timing Considerations
-------------------------------

.. _speed_considerations:

Transferring speed
^^^^^^^^^^^^^^^^^^

There're three factors limiting the transferring speed: (1) The transaction interval, (2) The SPI clock frequency used.
(3) The cache miss of SPI functions including callbacks.
When large transactions are used, the clock frequency determines the transferring speed; while the interval effects the
speed a lot if small transactions are used.

    1. Transaction interval: It takes time for the software to setup spi
       peripheral registers as well as copy data to FIFOs, or setup DMA links.
       When the interrupt transactions are used, an extra overhead is appended,
       from the cost of FreeRTOS queues and the time switching between tasks and
       the ISR.

            1. For **interrupt transactions**, the CPU can switched to other
               tasks when the transaction is in flight. This save the cpu time
               but increase the interval (See :ref:`interrupt_transactions`).
               For
               **polling transactions**, it does not block the task but do
               polling when the transaction is in flight. (See
               :ref:`polling_transactions`).

            2.  When the DMA is enabled, it needs about 2us per transaction to setup the linked list. When the master is
                transferring, it automatically read data from the linked list. If the DMA is not enabled,
                CPU has to write/read each byte to/from the FIFO by itself. Usually this is faster than 2us, but the
                transaction length is limited to 64 bytes for both write and read.

       Typical transaction interval with one byte data is as below:

       +--------+----------------+--------------+
       |        | Typical Transaction Time (us) |
       +========+================+==============+
       |        | Interrupt      | Polling      |
       +--------+----------------+--------------+
       | DMA    | 24             | 8            |
       +--------+----------------+--------------+
       | No DMA | 22             | 7            |
       +--------+----------------+--------------+

    2. SPI clock frequency: Each byte transferred takes 8 times of the clock period *8/fspi*. If the clock frequency is
       too high, some functions may be limited to use. See :ref:`timing_considerations`.

    3. The cache miss: the default config puts only the ISR into the IRAM.
       Other SPI related functions including the driver itself and the callback
       may suffer from the cache miss and wait for some time while reading code
       from the flash. Select :ref:`CONFIG_SPI_MASTER_IN_IRAM` to put the whole
       SPI driver into IRAM, and put the entire callback(s) and its callee
       functions into IRAM to prevent this.

For an interrupt transaction, the overall cost is *20+8n/Fspi[MHz]* [us] for n bytes tranferred
in one transaction. Hence the transferring speed is : *n/(20+8n/Fspi)*. Example of transferring speed under 8MHz
clock speed:

+-----------+----------------------+--------------------+------------+-------------+
| Frequency | Transaction Interval | Transaction Length | Total Time | Total Speed |
|           |                      |                    |            |             |
| (MHz)     | (us)                 | (bytes)            | (us)       | (kBps)      |
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

BTW, the ISR is disabled during flash operation by default. To keep sending
transactions during flash operations, enable
:ref:`CONFIG_SPI_MASTER_ISR_IN_IRAM` and set :cpp:class:`ESP_INTR_FLAG_IRAM`
in the ``intr_flags`` member of :cpp:class:`spi_bus_config_t`. Then all the
transactions queued before the flash operations will be handled by the ISR
continuously during flash operation. Note that the callback of each devices,
and their callee functions, should be in the IRAM in this case, or your
callback will crash due to cache miss.

.. _timing_considerations:

Timing considerations
^^^^^^^^^^^^^^^^^^^^^

As shown in the figure below, there is a delay on the MISO signal after SCLK
launch edge and before it's latched by the internal register. As a result,
the MISO pin setup time is the limiting factor for SPI clock speed. When the
delay is too large, setup slack is < 0 and the setup timing requirement is
violated, leads to the failure of reading correctly.

.. image:: /../_static/spi_miso.png

.. wavedrom don't support rendering pdflatex till now(1.3.1), so we use the png here

.. image:: /../_static/miso_timing_waveform.png

The maximum frequency allowed is related to the *input delay* (maximum valid
time after SCLK on the MISO bus), as well as the usage of GPIO matrix. The
maximum frequency allowed is reduced to about 33~77% (related to existing
*input delay*) when the GPIO matrix is used. To work at higher frequency, you
have to use the IOMUX pins or the *dummy bit workaround*. You can get the
maximum reading frequency of the master by ``spi_get_freq_limit``.

.. _dummy_bit_workaround:

**Dummy bit workaround:** We can insert dummy clocks (during which the host does not read data) before the read phase
actually begins. The slave still sees the dummy clocks and gives out data, but the host does not read until the read
phase. This compensates the lack of setup time of MISO required by the host, allowing the host reading at higher
frequency.

In the ideal case (the slave is so fast that the input delay is shorter than an apb clock, 12.5ns), the maximum
frequency host can read (or read and write) under different conditions is as below:

+-------------+-------------+------------+-----------------------------+
| Frequency Limit (MHz)     | Dummy Bits | Comments                    |
+-------------+-------------+ Used       +                             +
| GPIO matrix | IOMUX pins  | By Driver  |                             |
+=============+=============+============+=============================+
| 26.6        | 80          | No         |                             |
+-------------+-------------+------------+-----------------------------+
| 40          | --          | Yes        | Half Duplex, no DMA allowed |
+-------------+-------------+------------+-----------------------------+

And if the host only writes, the *dummy bit workaround* is not used and the frequency limit is as below:

+-------------------+------------------+
| GPIO matrix (MHz) | IOMUX pins (MHz) |
+===================+==================+
| 40                | 80               |
+-------------------+------------------+

The spi master driver can work even if the *input delay* in the ``spi_device_interface_config_t`` is set to 0.
However, setting a accurate value helps to: (1) calculate the frequency limit in full duplex mode, and (2) compensate
the timing correctly by dummy bits in half duplex mode. You may find the maximum data valid time after the launch edge
of SPI clocks in the AC characteristics chapter of the device specifications, or measure the time on a oscilloscope or
logic analyzer.

.. wavedrom don't support rendering pdflatex till now(1.3.1), so we use the png here

.. image:: /../_static/miso_timing_waveform_async.png

As shown in the figure above, the input delay is usually:

    *[input delay] = [sample delay] + [slave output delay]*

    1. The sample delay is the maximum random delay due to the
       asynchronization of SCLK and peripheral clock of the slave. It's usually
       1 slave peripheral clock if the clock is asynchronize with SCLK, or 0 if
       the slave just use the SCLK to latch the SCLK and launch MISO data. e.g.
       for ESP32 slaves, the delay is 12.5ns (1 apb clock), while it is reduced
       to 0 if the slave is in the same chip as the master.

    2. The slave output delay is the time for the MOSI to be stable after the
       launch edge. e.g. for ESP32 slaves, the output delay is 37.5ns (3 apb
       clocks) when IOMUX pins in the slave is used, or 62.5ns (5 apb clocks) if
       through the GPIO matrix.

Some typical delays are shown in the following table:

+--------------------+------------------+
| Device             | Input delay (ns) |
+====================+==================+
| Ideal device       |      0           |
+--------------------+------------------+
| ESP32 slave IOMUX* |      50          |
+--------------------+------------------+
| ESP32 slave GPIO*  |      75          |
+--------------------+------------------+
| ESP32 slave is on an independent      |
| chip, 12.5ns sample delay included.   |
+---------------------------------------+

The MISO path delay(tv), consists of slave *input delay* and master *GPIO matrix delay*, finally determines the
frequency limit, above which the full duplex mode will not work, or dummy bits are used in the half duplex mode. The
frequency limit is:

    *Freq limit[MHz] = 80 / (floor(MISO delay[ns]/12.5) + 1)*

The figure below shows the relations of frequency limit against the input delay. 2 extra apb clocks should be counted
into the MISO delay if the GPIO matrix in the master is used.

.. image:: /../_static/spi_master_freq_tv.png

Corresponding frequency limit for different devices with different *input delay* are shown in the following
table:

+--------+------------------+----------------------+-------------------+
| Master | Input delay (ns) | MISO path delay (ns) | Freq. limit (MHz) |
+========+==================+======================+===================+
| IOMUX  | 0                | 0                    | 80                |
+ (0ns)  +------------------+----------------------+-------------------+
|        | 50               | 50                   | 16                |
+        +------------------+----------------------+-------------------+
|        | 75               | 75                   | 11.43             |
+--------+------------------+----------------------+-------------------+
| GPIO   | 0                | 25                   | 26.67             |
+ (25ns) +------------------+----------------------+-------------------+
|        | 50               | 75                   | 11.43             |
+        +------------------+----------------------+-------------------+
|        | 75               | 100                  | 8.89              |
+--------+------------------+----------------------+-------------------+


.. _spi_known_issues:

Known Issues
------------

1. Half duplex mode is not compatible with DMA when both writing and reading phases exist.

   If such transactions are required, you have to use one of the alternative solutions:

   1. use full-duplex mode instead.
   2. disable the DMA by setting the last parameter to 0 in bus initialization function just as below:
      ``ret=spi_bus_initialize(VSPI_HOST, &buscfg, 0);``

      this may prohibit you from transmitting and receiving data longer than 64 bytes.
   3. try to use command and address field to replace the write phase.

2. Full duplex mode is not compatible with the *dummy bit workaround*, hence the frequency is limited. See :ref:`dummy
   bit speed-up workaround <dummy_bit_workaround>`.

3. ``cs_ena_pretrans`` is not compatible with command, address phases in full duplex mode.


Application Example
-------------------

Display graphics on the 320x240 LCD of WROVER-Kits: :example:`peripherals/spi_master`.


API Reference - SPI Common
--------------------------

.. include:: /_build/inc/spi_types.inc
.. include:: /_build/inc/spi_common.inc


API Reference - SPI Master
--------------------------

.. include:: /_build/inc/spi_master.inc

