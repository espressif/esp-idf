SD SPI Host Driver
==================

Overview
--------

The SD SPI host driver allows using the SPI2 (HSPI) or SPI3 (VSPI) controller for communication with SD cards. This driver's naming pattern was adopted from the :doc:`SDMMC Host <sdmmc_host>` driver due to their similarity. Likewise, the APIs of both drivers are also very similar.

The SD SPI host driver has the following modes:

- **SPI mode**: offers lower throughput but makes pin selection more flexible. With the help of the GPIO matrix, an SPI peripheral's signals can be routed to any ESP32 pin.
- **1-bit SD mode**: offers higher throughput but requires routing the signals through their dedicated IO_MUX pins only.

The SD SPI driver uses software-controlled CS signal.

Currently, the SD SPI driver cannot handle multi-threaded environments as does not support time-division multiplexing on the same SPI bus. It means that if your application needs to communicate with an SD card and other devices on the same SPI bus, the application itself must ensure that its different tasks do not try to access the SPI slaves at the same time.


How to Use
----------

The state and configurations of the SD SPI host driver are stored in a :cpp:type:`sdmmc_host_t` structure. This structure can be initialized using the :c:macro:`SDSPI_HOST_DEFAULT` macro.

The state and configurations of the SD slot are stored in a :cpp:type:`sdmmc_slot_config_t` structure. Use the macro :c:macro:`SDSPI_SLOT_CONFIG_DEFAULT` to initialize the structure and to fill in the default pin mappings (SD mode pin mappings).

Only the following driver's API functions are normally used by most applications:

- :cpp:func:`sdspi_host_init`
- :cpp:func:`sdspi_host_init_slot`
- :cpp:func:`sdspi_host_deinit`

Other functions are mostly used by the protocol level SD/SDIO/MMC driver via function pointers in the :cpp:type:`sdmmc_host_t` structure. For more details, see :doc:`the SD/SDIO/MMC Driver <../storage/sdmmc>`.

.. note::
    
    SD over SPI does not support speeds above :c:macro:`SDMMC_FREQ_DEFAULT` due to the limitations of the SPI driver.


.. todo

..    The SD SPI API reference could use more detail such as:
..    - Configuration. What are some key points of concern regarding slot configuration.
..    - Which function/how is a transaction done?
..    - Are there code snippets or corresponding application examples?


API Reference
-------------

.. include:: /_build/inc/sdspi_host.inc
