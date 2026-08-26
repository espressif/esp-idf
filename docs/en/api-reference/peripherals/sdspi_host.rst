SD SPI Host Driver
==================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The SD SPI host driver allows communication with one or more SD cards using the SPI Master driver, which utilizes the SPI host. Each card is accessed through an SD SPI device, represented by an SD SPI handle :cpp:type:`sdspi_dev_handle_t`, which returns when the device is attached to an SPI bus by calling :cpp:func:`sdspi_host_init_device`. It is important to note that the SPI bus should be initialized beforehand by :cpp:func:`spi_bus_initialize`.

.. only:: esp32

    This driver's naming pattern was adopted from the :doc:`sdmmc_host` due to their similarity. Likewise, the APIs of both drivers are also very similar.

    SD SPI driver that accesses the SD card in SPI mode offers lower throughput but makes pin selection more flexible. With the help of the GPIO matrix, an SPI peripheral's signals can be routed to any {IDF_TARGET_NAME} pin. Otherwise, if an SDMMC host driver is used (see :doc:`sdmmc_host`) to access the card in SD 1-bit/4-bit mode, higher throughput can be reached while requiring routing the signals through their dedicated IO_MUX pins only.

With the help of :doc:`spi_master` the SD SPI host driver based on, the SPI bus can be shared among SD cards and other SPI devices. The SPI Master driver will handle exclusive access from different tasks.

The SD SPI driver uses software-controlled CS signal.

How to Use
----------

Firstly, use the macro :c:macro:`SDSPI_DEVICE_CONFIG_DEFAULT` to initialize the structure :cpp:type:`sdspi_device_config_t`, which is used to initialize an SD SPI device. This macro will also fill in the default pin mappings, which are the same as the pin mappings of the SDMMC host driver. Modify the host and pins of the structure to desired value. Then call ``sdspi_host_init_device`` to initialize the SD SPI device and attach to its bus.

Then use the :c:macro:`SDSPI_HOST_DEFAULT` macro to initialize the :cpp:type:`sdmmc_host_t` structure, which is used to store the state and configurations of the upper layer (SD/SDIO/MMC driver). Modify the ``slot`` parameter of the structure to the SD SPI device SD SPI handle just returned from ``sdspi_host_init_device``. Call ``sdmmc_card_init`` with the :cpp:type:`sdmmc_host_t` to probe and initialize the SD card.

Now you can use SD/SDIO/MMC driver functions to access your card!

Other Details
-------------

Only the following driver's API functions are normally used by most applications:

- :cpp:func:`sdspi_host_init`
- :cpp:func:`sdspi_host_init_device`
- :cpp:func:`sdspi_host_remove_device`
- :cpp:func:`sdspi_host_deinit`

Other functions are mostly used by the protocol level SD/SDIO/MMC driver via function pointers in the :cpp:type:`sdmmc_host_t` structure. For more details, see :doc:`../storage/sdmmc`.

.. note::

    SD over SPI does not support speeds above :c:macro:`SDMMC_FREQ_DEFAULT` due to the limitations of the SPI driver.

.. warning::

    If you want to share the SPI bus among SD card and other SPI devices, there are some restrictions, see :doc:`sdspi_share`.

.. todo

..    The SD SPI API reference could use more detail such as:
..    - Configuration. What are some key points of concern regarding slot configuration.
..    - Which function/how is a transaction done?
..    - Are there code snippets or corresponding application examples?

Related Docs
--------------

.. toctree::
   :maxdepth: 1

   sdspi_share


API Reference
-------------

.. include-build-file:: inc/sdspi_host.inc
