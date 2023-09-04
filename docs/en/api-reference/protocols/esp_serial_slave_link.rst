ESP Serial Slave Link
=====================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

Espressif provides several chips that can work as slaves. These slave devices rely on some common buses, and have their own communication protocols over those buses. The ``esp_serial_slave_link`` component is designed for the master to communicate with ESP slave devices through those protocols over the bus drivers.

After an ``esp_serial_slave_link`` device is initialized properly, the application can use it to communicate with the ESP slave devices conveniently.

.. note::

  The ESP-IDF component ``esp_serial_slave_link`` has been moved from ESP-IDF since version v5.0 to a separate repository:

  * `ESSL component on GitHub <https://github.com/espressif/idf-extra-components/tree/master/esp_serial_slave_link>`__

  To add ESSL component in your project, please run ``idf.py add-dependency espressif/esp_serial_slave_link``.


Espressif Device Protocols
--------------------------

For more details about Espressif device protocols, see the following documents.

.. toctree::
    :maxdepth: 1

    :SOC_SDIO_SLAVE_SUPPORTED: esp_sdio_slave_protocol
    esp_spi_slave_protocol

Terminology
-----------

- ESSL: Abbreviation for ESP Serial Slave Link, the component described by this document.

- Master: The device running the ``esp_serial_slave_link`` component.

- ESSL device: a virtual device on the master associated with an ESP slave device. The device context has the knowledge of the slave protocol above the bus, relying on some bus drivers to communicate with the slave.

- ESSL device handle: a handle to ESSL device context containing the configuration, status and data required by the ESSL component. The context stores the driver configurations, communication state, data shared by master and slave, etc.

   - The context should be initialized before it is used, and get deinitialized if not used any more. The master application operates on the ESSL device through this handle.

- ESP slave: the slave device connected to the bus, which ESSL component is designed to communicate with.

- Bus: The bus over which the master and the slave communicate with each other.

- Slave protocol: The special communication protocol specified by Espressif HW/SW over the bus.

- TX buffer num: a counter, which is on the slave and can be read by the master, indicates the accumulated buffer numbers that the slave has loaded to the hardware to receive data from the master.

- RX data size: a counter, which is on the slave and can be read by the master, indicates the accumulated data size that the slave has loaded to the hardware to send to the master.

Services Provided by ESP Slave
------------------------------

There are some common services provided by the Espressif slaves:

1. Tohost Interrupts: The slave can inform the master about certain events by the interrupt line. (optional)

2. Frhost Interrupts: The master can inform the slave about certain events.

3. TX FIFO (master to slave): The slave can receive data from the master in units of receiving buffers.

   The slave updates the TX buffer num to inform the master how much data it can receive, and the master then read the TX buffer num, and take off the used buffer number to know how many buffers are remaining.

4. RX FIFO (slave to master): The slave can send data in stream to the master. The SDIO slave can also indicate it has new data to send to master by the interrupt line.

   The slave updates the RX data size to inform the master how much data it has prepared to send, and then the master read the data size, and take off the data length it has already received to know how many data is remaining.

5. Shared registers: The master can read some part of the registers on the slave, and also write these registers to let the slave read.

.. only:: SOC_SDIO_SLAVE_SUPPORTED

   The services provided by the slave depends on the slave's model. See :ref:`esp_sdio_slave_caps` and :ref:`esp_spi_slave_caps` for more details.

.. only:: not SOC_SDIO_SLAVE_SUPPORTED

   The services provided by the slave depends on the slave's model. See :ref:`esp_spi_slave_caps` for more details.


Initialization of ESP Serial Slave Link
---------------------------------------

.. _essl_sdio_slave_init:

ESP SDIO Slave
^^^^^^^^^^^^^^

The ESP SDIO slave link (ESSL SDIO) devices relies on the SDMMC component. It includes the usage of communicating with ESP SDIO Slave device via the SDMMC Host or SDSPI Host feature. The ESSL device should be initialized as below:

1. Initialize a SDMMC card (see :doc:` Document of SDMMC driver </api-reference/storage/sdmmc>`) structure.

2. Call :cpp:func:`sdmmc_card_init` to initialize the card.

3. Initialize the ESSL device with :cpp:type:`essl_sdio_config_t`. The ``card`` member should be the :cpp:type:`sdmmc_card_t` got in step 2, and the ``recv_buffer_size`` member should be filled correctly according to pre-negotiated value.

4. Call :cpp:func:`essl_init` to do initialization of the SDIO part.

5. Call :cpp:func:`essl_wait_for_ready` to wait for the slave to be ready.

ESP SPI Slave
^^^^^^^^^^^^^

.. note::

    If you are communicating with the ESP SDIO Slave device through SPI interface, you should use the :ref:`SDIO interface <essl_sdio_slave_init>` instead.

Has not been supported yet.

APIs
----

After the initialization process above is performed, you can call the APIs below to make use of the services provided by the slave:

Tohost Interrupts (Optional)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Call :cpp:func:`essl_get_intr_ena` to know which events trigger the interrupts to the master.

2. Call :cpp:func:`essl_set_intr_ena` to set the events that trigger interrupts to the master.

3. Call :cpp:func:`essl_wait_int` to wait until interrupt from the slave, or timeout.

4. When interrupt is triggered, call :cpp:func:`essl_get_intr` to know which events are active, and call :cpp:func:`essl_clear_intr` to clear them.

Frhost Interrupts
^^^^^^^^^^^^^^^^^

1. Call :cpp:func:`essl_send_slave_intr` to trigger general purpose interrupt of the slave.

TX FIFO
^^^^^^^

1. Call :cpp:func:`essl_get_tx_buffer_num` to know how many buffers the slave has prepared to receive data from the master. This is optional. The master will poll ``tx_buffer_num`` when it tries to send packets to the slave, until the slave has enough buffer or timeout.

2. Call :cpp:func:`essl_send_packet` to send data to the slave.

RX FIFO
^^^^^^^

1. Call :cpp:func:`essl_get_rx_data_size` to know how many data the slave has prepared to send to the master. This is optional. When the master tries to receive data from the slave, it updates the ``rx_data_size`` for once, if the current ``rx_data_size`` is shorter than the buffer size the master prepared to receive. And it may poll the ``rx_data_size`` if the ``rx_data_size`` keeps 0, until timeout.

2. Call :cpp:func:`essl_get_packet` to receive data from the slave.

Reset Counters (Optional)
^^^^^^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`essl_reset_cnt` to reset the internal counter if you find the slave has reset its counter.


Application Example
-------------------

The example below shows how {IDF_TARGET_NAME} SDIO host and slave communicate with each other. The host uses the ESSL SDIO:

:example:`peripherals/sdio`

Please refer to the specific example README.md for details.

API Reference
-------------

.. include-build-file:: inc/essl.inc
.. include-build-file:: inc/essl_sdio.inc
.. include-build-file:: inc/essl_spi.inc
