SPI Slave Half Duplex
=====================

Introduction
------------

The half duplex (HD) mode is a special mode provided by ESP SPI Slave peripheral. Under this mode, the hardware provides more services than the full duplex (FD) mode (the mode for general purpose SPI transactions, see :doc:`spi_slave`). These services reduce the CPU load and the response time of SPI Slave, but the communication format is determined by the hardware. The communication format is always half duplex, so comes the name of Half Duplex Mode.

There are several different types of transactions, determined by the *command* phase of the transaction. Each transaction may consist of the following phases: command, address, dummy, data. The command phase is mandatory, while the other fields may be determined by the command field. During the command, address, dummy phases, the bus is always controlled by the master, while the direction of the data phase depends on the command. The data phase can be either an in phase, for the master to write data to the slave; or an out phase, for the master to read data from the slave.

About the details of how master should communicate with the SPI Slave, see :doc:`/api-reference/protocols/esp_spi_slave_protocol`.

By these different transactions, the slave provide these services to the master:

- A DMA channel for the master to write a great amount of data to the slave.
- A DMA channel for the master to read a great amount of data from the slave.
- Several general purpose registers, shard between the master and the slave.
- Several general purpose interrupts, for the master to interrupt the SW of slave.

Terminology
-----------

- Transaction
- Channel
- Sending
- Receiving
- Data Descriptor

Driver Feature
--------------

- Transaction read/write by master in segments

- Queues for data to send and received

Driver usage
------------

Slave initialization
^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`spi_slave_hd_init` to initialize the SPI bus as well as the peripheral and the driver. The SPI slave will exclusively use the SPI peripheral, pins of the bus before it's deinitialized. Most configurations of the slave should be done as soon as the slave is being initialized.

The :cpp:type:`spi_bus_config_t` specifies how the bus should be initialized, while :cpp:type:`spi_slave_hd_slot_config_t` specifies how the SPI Slave driver should work.

Deinitialization (optional)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`spi_slave_hd_deinit` to uninstall the driver. The resources, including the pins, SPI peripheral, internal memory used by the driver, interrupt sources, will be released by the deinit function.

Send/Receive Data by DMA Channels
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To send data to the master through the sending DMA channel, the application should properly wrap the data to send by a :cpp:type:`spi_slave_hd_data_t` descriptor structure before calling :cpp:func:`spi_slave_hd_queue_trans` with the data descriptor, and the channel argument of :cpp:enumerator:`SPI_SLAVE_CHAN_TX`. The pointers to descriptors are stored in the queue, and the data will be send to the master upon master's RDDMA command in the same order they are put into the queue by :cpp:func:`spi_slave_hd_queue_trans`.

The application should check the result of data sending by calling :cpp:func:`spi_slave_hd_get_trans_res` with the channel set as :cpp:enumerator:`SPI_SLAVE_CHAN_TX`. This function will block until the transaction with command RDDMA from master successfully completes (or timeout). The ``out_trans`` argument of the function will output the pointer of the data descriptor which is just finished.

Receiving data from the master through the receiving DMA channel is quite similar. The application calls :cpp:func:`spi_slave_hd_queue_trans` with proper data descriptor and the channel argument of :cpp:enumerator:`SPI_SLAVE_CHAN_RX`. And the application calls the :cpp:func:`spi_slave_hd_get_trans_res` later to get the descriptor to the receiving buffer, before it handles the data in the receiving buffer.

.. note::
    This driver itself doesn't have internal buffer for the data to send, or just received. The application should provide data descriptors for the data buffer to send to master, or to receive data from the master.

    The application will have to properly keep the data descriptor as well as the buffer it points to, after the descriptor is successfully sent into the driver internal queue by :cpp:func:`spi_slave_hd_queue_trans`, and before returned by :cpp:func:`spi_slave_hd_get_trans_res`. During this period, the hardware as well as the driver may read or write to the buffer and the descriptor when required at any time.

Please note that the buffer doesn't have to be fully sent or filled before it's terminated. For example, in the segment transaction mode, the master has to send CMD7 to terminate a WRDMA transaction, or send CMD8 to terminate a RDDMA transaction (in segments), no matter the send (receive) buffer is used up (full) or not.

.. _spi_slave_hd_data_arguments:

Using Data Arguments
^^^^^^^^^^^^^^^^^^^^

Sometimes you may have initiator (sending data descriptor) and closure (handling returned descriptors) functions in different places. When you get the returned data descriptor in the closure, you may need some extra information when handle the finished data descriptor. For example, you may want to know which round it is for the returned descriptor, when you send the same piece of data for several times.

Set the ``arg`` member in the data descriptor to an variable indicating the transaction (by force casting), or point it to a a structure which wraps all the information you may need when handling the sending/receiving data. Then you can get what you need in your closure.

.. _spi_slave_hd_callbacks:

Using callbacks
^^^^^^^^^^^^^^^

.. note::
    These callbacks are called in the ISR, so that they are fast enough. However, you may need to be very careful to write the code in the ISR. The callback should return as soon as possible. No delay or blocking operations are allowed.

The :cpp:type:`spi_slave_hd_intr_config_t` member in the :cpp:type:`spi_slave_hd_slot_config_t` configuration structure passed when initialize the SPI Slave HD driver, allows you having callbacks for each events you may concern.

The corresponding interrupt for each callbacks that is not *NULL* will enabled, so that the callbacks can be called immediately when the events happen. You don't need to provide callbacks for the unconcerned events.

The ``arg`` member in the configuration structure can help you pass some context to the callback, or indicate which SPI Slave instance when you are using the same callbacks for several SPI Slave peripherals. Set the ``arg`` member to an variable indicating the SPI Slave instance (by force casting), or point it to a context structure. All the callbacks will be called with this ``arg`` argument you set when the callbacks are initialized.

There are two other arguments: the ``event`` and the ``awoken``. The ``event`` passes the information of the current event to the callback. The :cpp:type:`spi_slave_hd_event_t` type contains the information of the event, for example, event type, the data descriptor just finished (The :ref:`data argument <spi_slave_hd_data_arguments>` will be very useful in this case!). The ``awoken`` argument is an output one, telling the ISR there are tasks are awoken after this callback, and the ISR should call `portYIELD_FROM_ISR()` to do task scheduling. Just pass the ``awoken`` argument to all FreeRTOS APIs which may unblock tasks, and the awoken will be returned to the ISR.

Writing/Reading Shared Registers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`spi_slave_hd_write_buffer` to write the shared buffer, and :cpp:func:`spi_slave_hd_read_buffer` to read the shared buffer.

.. note::
    On {IDF_TARGET_NAME}, the shared registers are read/written in words by the application, but read/written in bytes by the master. There's no guarantee four continuous bytes read from the master are from the same word written by the slave's application. It's also possible that if the slave reads a word while the master is writing bytes of the word, the slave may get one word with half of them just written by the master, and the other half hasn't been written into.

    The master can confirm that the word is not in transition by reading the word twice and comparing the values.

    For the slave, it will be more difficult to ensure the word is not in transition because the process of master writing four bytes can be very long (32 SPI clocks). You can put some CRC in the last (largest address) byte of a word so that when the byte is written, the word is sure to be all written.

    Due to the conflicts there may be among read/write from SW (worse if there are multi cores) and master, it is suggested that a word is only used in one direction (only written by master or only written by the slave).

Receiving General Purpose Interrupts From the Master
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When the master sends CMD 0x08, 0x09 or 0x0A, the slave corresponding will be triggered. Currently the CMD8 is permanently used to indicate the termination of RDDMA segments. To receiving general purpose interrupts, register callbacks for CMD 0x09 and 0x0A when the slave is initialized, see :ref:`spi_slave_hd_callbacks`.


Application Example
-------------------

The code example for Device/Host communication can be found in the :example:`peripherals/spi_slave_hd` directory of ESP-IDF examples.


API reference
-------------

.. include-build-file:: inc/spi_slave_hd.inc