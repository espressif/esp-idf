SPI Slave Half Duplex
=====================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The Half Duplex (HD) Mode is a special mode provided by ESP SPI Slave peripheral. Under this mode, the hardware provides more services than the Full Duplex (FD) Mode (the mode for general-purpose SPI transactions, see :doc:`spi_slave`). These services reduce the CPU load and the response time of SPI Slave. However, it is important to note that the communication format is determined by the hardware and is always in a half-duplex configuration, allowing only one-way data transfer at any given time. Hence, the mode is named Half Duplex Mode due to this characteristic.

When conducting an SPI transaction, transactions can be classified into several types based on the **command** phase of the transaction. Each transaction may consist of the following phases: command, address, dummy, and data. The command phase is mandatory, while the other phases may be determined by the command field. During the command, address, and dummy phases, the bus is always controlled by the master (usually the host), while the direction of the data phase depends on the command. The data phase can be either an input phase, where the master writes data to the slave (e.g., the host sends data to the slave), or an output phase, where the master reads data from the slave (e.g., the host receives data from the slave).

Protocol
^^^^^^^^

About the details of how master should communicate with the SPI Slave, see :doc:`/api-reference/protocols/esp_spi_slave_protocol`.

Through these different transactions, the slave provides these services to the master:

- A DMA channel for the master to write a great amount of data to the slave.
- A DMA channel for the master to read a great amount of data from the slave.
- Several general purpose registers, shared between the master and the slave.
- Several general purpose interrupts, for the master to interrupt the SW of the slave.

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

Driver Usage
------------

Slave Initialization
^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`spi_slave_hd_init` to initialize the SPI bus as well as the peripheral and the driver. The SPI Slave exclusively uses the SPI peripheral, pins of the bus before it is deinitialized, which means other devices are unable to use the above resources during initialization. Thus, to ensure SPI resources are correctly occupied and the connections work properly, most configurations of the slave should be done as soon as the slave is initialized.

The :cpp:type:`spi_bus_config_t` specifies how the bus should be initialized, while :cpp:type:`spi_slave_hd_slot_config_t` specifies how the SPI Slave driver should work.

Deinitialization (Optional)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`spi_slave_hd_deinit` to uninstall the driver. The resources, including the pins, SPI peripheral, internal memory used by the driver, and interrupt sources, are released by the ``deinit()`` function.

Send/Receive Data by DMA Channels
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To send data to the master through the sending DMA channel, the application should properly wrap the data in an :cpp:type:`spi_slave_hd_data_t` descriptor structure before calling :cpp:func:`spi_slave_hd_queue_trans` with the data descriptor and the channel argument of :cpp:enumerator:`SPI_SLAVE_CHAN_TX`. The pointers to descriptors are stored in the queue, and the data is sent to the master in the same order they are enqueued using :cpp:func:`spi_slave_hd_queue_trans`, upon receiving the master's ``Rd_DMA`` command.

The application should check the result of data sending by calling :cpp:func:`spi_slave_hd_get_trans_res` with the channel set as :cpp:enumerator:`SPI_SLAVE_CHAN_TX`. This function blocks until the transaction with the command ``Rd_DMA`` from the master successfully completes (or timeout). The ``out_trans`` argument of the function outputs the pointer of the data descriptor which is just finished, providing information about the sending.

Receiving data from the master through the receiving DMA channel is quite similar. The application calls :cpp:func:`spi_slave_hd_queue_trans` with proper data descriptor and the channel argument of :cpp:enumerator:`SPI_SLAVE_CHAN_RX`. And the application calls the :cpp:func:`spi_slave_hd_get_trans_res` later to get the descriptor to the receiving buffer before it handles the data in the receiving buffer.

.. note::

    This driver itself does not have an internal buffer for the data to send or just received. The application should provide data buffer for driver via data descriptors to send to the master, or to receive data from the master.

    The application has to properly keep the data descriptor as well as the buffer it points, after the descriptor is successfully sent into the driver internal queue by :cpp:func:`spi_slave_hd_queue_trans`, and before returned by :cpp:func:`spi_slave_hd_get_trans_res`. During this period, the hardware as well as the driver may read or write to the buffer and the descriptor when required at any time.

Please note that, when using this driver for data transfer, the buffer does not have to be fully sent or filled before it is terminated. For example, in the segment transaction mode, the master has to send ``CMD7`` to terminate a ``Wr_DMA`` transaction or send ``CMD8`` to terminate an ``Rd_DMA`` transaction (in segments), no matter whether the send (receive) buffer is used up (full) or not.

.. _spi_slave_hd_data_arguments:

Using Data Descriptor with Customized User Arguments
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes you may have initiator (sending data descriptor) and closure (handling returned descriptors) functions in different places. When you get the returned data descriptor in the closure, you may need some extra information when handling the finished data descriptor. For example, you may want to know which round it is for the returned descriptor when you send the same piece of data several times.

Set the ``arg`` member in the data descriptor to a variable indicating the transaction by force casting, or point it to a structure that wraps all the information you may need when handling the sending/receiving data. Then you can get what you need in your closure.

.. _spi_slave_hd_callbacks:

Using Callbacks
^^^^^^^^^^^^^^^

.. note::

    These callbacks are called in the ISR, so the required operations need to be processed quickly and returned as soon as possible to ensure that the system is functioning properly. You may need to be very careful to write the code in the ISR.

    Since the interrupt handling is executed concurrently with the application, long delays or blocking may cause the system to respond slower or lead to unpredictable behavior. Therefore, when writing callback functions, avoid using operations that may cause delays or blocking, e.g., waiting, sleeping, resource locking, etc.

The :cpp:type:`spi_slave_hd_callback_config_t` member in the :cpp:type:`spi_slave_hd_slot_config_t` configuration structure passed when initializing the SPI Slave HD driver, allows you to have callbacks for each event you may concern.

The corresponding interrupt for each callback that is not **NULL** is enabled, so that the callbacks can be called immediately when the events happen. You do not need to provide callbacks for the unconcerned events.

The ``arg`` member in the configuration structure can help you pass some context to the callback or indicate the specific SPI Slave instance when using the same callbacks for multiple SPI Slave peripherals. You can set the arg member to a variable that indicates the SPI Slave instance by performing a forced type casting or point it to a context structure. All the callbacks are called with this ``arg`` argument you set when the callbacks are initialized.

There are two other arguments: the ``event`` and the ``awoken``.

    - The ``event`` passes the information of the current event to the callback. The :cpp:type:`spi_slave_hd_event_t` type contains the information of the event, for example, event type, the data descriptor just finished (The :ref:`data argument <spi_slave_hd_data_arguments>` is very useful in this case!).
    - The ``awoken`` argument serves as an output parameter. It informs the ISR that tasks have been awakened after the callback function, and the ISR should call `portYIELD_FROM_ISR()` to schedule these tasks. Simply pass the ``awoken`` argument to all FreeRTOS APIs that may unblock tasks, and the value of ``awoken`` will be returned to the ISR.

Writing/Reading Shared Registers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`spi_slave_hd_write_buffer` to write the shared buffer, and :cpp:func:`spi_slave_hd_read_buffer` to read the shared buffer.

.. note::

    On {IDF_TARGET_NAME}, the shared registers are read/written in words by the application but read/written in bytes by the master. There is no guarantee four continuous bytes read from the master are from the same word written by the slave's application. It is also possible that if the slave reads a word while the master is writing bytes of the word, the slave may get one word with half of them just written by the master, and the other half has not been written into.

    The master can confirm that the word is not in transition by reading the word twice and comparing the values.

    For the slave, it is more difficult to ensure the word is not in transition because the process of master writing four bytes can be very long (32 SPI clocks). You can put some CRC in the last (largest address) byte of a word so that when the byte is written, the word is sure to be all written.

    Due to the conflicts that may be among read/write from SW (worse if there are multi-cores) and master, it is suggested that a word is only used in one direction (only written by the master or only written by the slave).

Receiving General Purpose Interrupts from the Master
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When the master sends ``CMD8``, ``CMD9`` or ``CMDA``, the slave corresponding is triggered. Currently the ``CMD8`` is permanently used to indicate the termination of ``Rd_DMA`` segments. To receive general-purpose interrupts, register callbacks for ``CMD9`` and ``CMDA`` when the slave is initialized, see :ref:`spi_slave_hd_callbacks`.


Application Example
-------------------

The code example for Device/Host communication can be found in the :example:`peripherals/spi_slave_hd` directory of ESP-IDF examples.


API Reference
-------------

.. include-build-file:: inc/spi_slave_hd.inc
