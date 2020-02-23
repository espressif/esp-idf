ESP Serial Slave Link
=====================

Overview
--------

Espressif provides several chips that can work as slaves. These slave devices rely on some
common buses, and have their own communication protocols over those buses. The `esp_serial_slave_link` component is
designed for the master to communicate with ESP slave devices through those protocols over the
bus drivers.

After an `esp_serial_slave_link` device is initialized properly, the application can use it to communicate with the ESP
slave devices conveniently.

For more details about ESP32 SDIO slave protocol, see document :doc:`/api-reference/peripherals/esp_slave_protocol`.

Terminology
-----------

- ESSL: Abbreviation for ESP Serial Slave Link, the component described by this document.

- Master: The device running the `esp_serial_slave_link` component.

- ESSL device: a virtual device on the master associated with an ESP slave device. The device
  context has the knowledge of the slave protocol above the bus, relying on some bus drivers to
  communicate with the slave.

- ESSL device handle: a handle to ESSL device context containing the configuration, status and
  data required by the ESSL component. The context stores the driver configurations,
  communication state, data shared by master and slave, etc.

  The context should be initialized before it is used, and get deinitialized if not used any more. The
  master application operates on the ESSL device through this handle.

- ESP slave: the slave device connected to the bus, which ESSL component is designed to
  communicate with.

- Bus: The bus over which the master and the slave communicate with each other.

- Slave protocol: The special communication protocol specified by Espressif HW/SW over the bus.

- TX buffer num: a counter, which is on the slave and can be read by the master, indicates the
  accumulated buffer numbers that the slave has loaded to the hardware to receive data from the
  master.

- RX data size: a counter, which is on the slave and can be read by the master, indicates the
  accumulated data size that the slave has loaded to the hardware to send to the master.

Services provided by ESP slave
------------------------------

There are some common services provided by the Espressif slaves:

1. Tohost Interrupts: The slave can inform the master about certain events by the interrupt line.

2. Frhost Interrupts: The master can inform the slave about certain events.

3. Tx FIFO (master to slave): the slave can send data in stream to the master. The SDIO slave can
   also indicate it has new data to send to master by the interrupt line.

   The slave updates the TX buffer num to inform the master how much data it can receive, and the
   master then read the TX buffer num, and take off the used buffer number to know how many buffers are remaining.

4. Rx FIFO (slave to master): the slave can receive data from the master in units of receiving
   buffers.

   The slave updates the RX data size to inform the master how much data it has prepared to
   send, and then the master read the data size, and take off the data length it has already received to know how many
   data is remaining.

5. Shared registers: the master can read some part of the registers on the slave, and also write
   these registers to let the slave read.


Initialization of ESP SDIO Slave Link
-------------------------------------

The ESP SDIO slave link (ESSL SDIO) devices relies on the sdmmc component. The ESSL device should
be initialized as below:

1. Initialize a sdmmc card (see :doc:` Document of SDMMC driver </api-reference/storage/sdmmc>`)
   structure.

2. Call :cpp:func:`sdmmc_card_init` to initialize the card.

3. Initialize the ESSL device with :cpp:type:`essl_sdio_config_t`. The `card` member should be
   the :cpp:type:`sdmmc_card_t` got in step 2, and the `recv_buffer_size` member should be filled
   correctly according to pre-negotiated value.

4. Call :cpp:func:`essl_init` to do initialization of the SDIO part.

5. Call :cpp:func:`essl_wait_for_ready` to wait for the slave to be ready.

APIs
----

After the initialization process above is performed, you can call the APIs below to make use of
the services provided by the slave:

Interrupts
^^^^^^^^^^

1. Call :cpp:func:`essl_get_intr_ena` to know which events will trigger the interrupts to the master.

2. Call :cpp:func:`essl_set_intr_ena` to set the events that will trigger interrupts to the master.

3. Call :cpp:func:`essl_wait_int` to wait until interrupt from the slave, or timeout.

4. When interrupt is triggered, call :cpp:func:`essl_get_intr` to know which events are active,
   and call :cpp:func:`essl_clear_intr` to clear them.

5. Call :cpp:func:`essl_send_slave_intr` to trigger general purpose interrupt of the slave.

TX FIFO
^^^^^^^

1. Call :cpp:func:`essl_get_tx_buffer_num` to know how many buffers the slave has prepared to
   receive data from the master. This is optional. The master will poll `tx_buffer_num` when it try
   to send packets to the slave, until the slave has enough buffer or timeout.

2. Call :cpp:func:`essl_send_paket` to send data to the slave.

RX FIFO
^^^^^^^

1. Call :cpp:func:`essl_get_rx_data_size` to know how many data the slave has prepared to send to
   the master. This is optional. When the master tries to receive data from the slave, it will update
   the `rx_data_size` for once, if the current `rx_data_size` is shorter than the buffer size the
   master prepared to receive. And it may poll the `rx_data_size` if the `rx_dat_size` keeps 0,
   until timeout.

2. Call :cpp:func:`essl_get_packet` to receive data from the slave.

Reset counters (Optional)
^^^^^^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`essl_reset_cnt` to reset the internal counter if you find the slave has reset its
counter.


Application Example
-------------------

The example below shows how ESP32 SDIO host and slave communicate with each other. The host use the ESSL SDIO.

:example:`peripherals/sdio`.

Please refer to the specific example README.md for details.

API Reference
-------------

.. include-build-file:: inc/essl.inc
.. include-build-file:: inc/essl_sdio.inc
