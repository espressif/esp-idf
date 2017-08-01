SDMMC Host Peripheral
=====================

Overview
--------

SDMMC peripheral supports SD and MMC memory cards and SDIO cards. SDMMC software builds on top of SDMMC driver and consists of the following parts:

1. SDMMC host driver (``driver/sdmmc_host.h``) — this driver provides APIs to send commands to the slave device(s), send and receive data, and handling error conditions on the bus.

2. SDMMC protocol layer (``sdmmc_cmd.h``) — this component handles specifics of SD protocol such as card initialization and data transfer commands. Despite the name, only SD (SDSC/SDHC/SDXC) cards are supported at the moment. Support for MCC/eMMC cards can be added in the future.

Protocol layer works with the host via ``sdmmc_host_t`` structure. This structure contains pointers to various functions of the host.

In addition to SDMMC Host peripheral, ESP32 has SPI peripherals which can also be used to work with SD cards. This is supported using a variant of the host driver, ``driver/sdspi_host.h``. This driver has the same interface as SDMMC host driver, and the protocol layer can use either of two.

Application Example
-------------------

An example which combines SDMMC driver with FATFS library is provided in ``examples/storage/sd_card`` directory. This example initializes the card, writes and reads data from it using POSIX and C library APIs. See README.md file in the example directory for more information.


Protocol layer APIs
-------------------

Protocol layer is given ``sdmmc_host_t`` structure which describes the SD/MMC host driver, lists its capabilites, and provides pointers to functions of the driver. Protocol layer stores card-specific information in ``sdmmc_card_t`` structure. When sending commands to the SD/MMC host driver, protocol layer uses ``sdmmc_command_t`` structure to describe the command, argument, expected return value, and data to transfer, if any.

Normal usage of the protocol layer is as follows:

1. Call the host driver functions to initialize the host (e.g. ``sdmmc_host_init``, ``sdmmc_host_init_slot``). 
2. Call ``sdmmc_card_init`` to initialize the card, passing it host driver information (``host``) and a pointer to ``sdmmc_card_t`` structure which will be filled in (``card``).
3. To read and write sectors of the card, use ``sdmmc_read_sectors`` and ``sdmmc_write_sectors``, passing the pointer to card information structure (``card``).
4. When card is not used anymore, call the host driver function to disable SDMMC host peripheral and free resources allocated by the driver (e.g. ``sdmmc_host_deinit``).

Most applications need to use the protocol layer only in one task; therefore the protocol layer doesn't implement any kind of locking on the ``sdmmc_card_t`` structure, or when accessing SDMMC host driver. Such locking has to be implemented in the higher layer, if necessary (e.g. in the filesystem driver).

.. doxygenstruct:: sdmmc_host_t
    :members:

.. doxygendefine:: SDMMC_HOST_FLAG_1BIT
.. doxygendefine:: SDMMC_HOST_FLAG_4BIT
.. doxygendefine:: SDMMC_HOST_FLAG_8BIT
.. doxygendefine:: SDMMC_HOST_FLAG_SPI
.. doxygendefine:: SDMMC_FREQ_DEFAULT
.. doxygendefine:: SDMMC_FREQ_HIGHSPEED
.. doxygendefine:: SDMMC_FREQ_PROBING

.. doxygenstruct:: sdmmc_command_t
    :members:

.. doxygenstruct:: sdmmc_card_t
    :members:

.. doxygenstruct:: sdmmc_csd_t
    :members:

.. doxygenstruct:: sdmmc_cid_t
    :members:

.. doxygenstruct:: sdmmc_scr_t
    :members:

.. doxygenfunction:: sdmmc_card_init
.. doxygenfunction:: sdmmc_write_sectors
.. doxygenfunction:: sdmmc_read_sectors

SDMMC host driver APIs
----------------------

On the ESP32, SDMMC host peripheral has two slots:

- Slot 0 (``SDMMC_HOST_SLOT_0``) is an 8-bit slot. It uses ``HS1_*`` signals in the PIN MUX.
- Slot 1 (``SDMMC_HOST_SLOT_1``) is a 4-bit slot. It uses ``HS2_*`` signals in the PIN MUX.

Card Detect and Write Protect signals can be routed to arbitrary pins using GPIO matrix. To use these pins, set ``gpio_cd`` and ``gpio_wp`` members of ``sdmmc_slot_config_t`` structure when calling ``sdmmc_host_init_slot``.

Of all the funtions listed below, only ``sdmmc_host_init``, ``sdmmc_host_init_slot``, and ``sdmmc_host_deinit`` will be used directly by most applications. Other functions, such as ``sdmmc_host_set_bus_width``, ``sdmmc_host_set_card_clk``, and ``sdmmc_host_do_transaction`` will be called by the SD/MMC protocol layer via function pointers in ``sdmmc_host_t`` structure.

.. doxygenfunction:: sdmmc_host_init

.. doxygendefine:: SDMMC_HOST_SLOT_0
.. doxygendefine:: SDMMC_HOST_SLOT_1
.. doxygendefine:: SDMMC_HOST_DEFAULT
.. doxygendefine:: SDMMC_SLOT_WIDTH_DEFAULT

.. doxygenfunction:: sdmmc_host_init_slot

.. doxygenstruct:: sdmmc_slot_config_t
    :members:

.. doxygendefine:: SDMMC_SLOT_NO_CD
.. doxygendefine:: SDMMC_SLOT_NO_WP
.. doxygendefine:: SDMMC_SLOT_CONFIG_DEFAULT

.. doxygenfunction:: sdmmc_host_set_bus_width
.. doxygenfunction:: sdmmc_host_set_card_clk
.. doxygenfunction:: sdmmc_host_do_transaction
.. doxygenfunction:: sdmmc_host_deinit

SD SPI driver APIs
------------------

SPI controllers accessible via spi_master driver (HSPI, VSPI) can be used to work with SD cards. In SPI mode, SD driver has lower throughput than in 1-line SD mode. However SPI mode makes pin selection more flexible, as SPI peripheral can be connected to any ESP32 pins using GPIO Matrix. SD SPI driver uses software controlled CS signal. Currently SD SPI driver assumes that it can use the SPI controller exclusively, so applications which need to share SPI bus between SD cards and other peripherals need to make sure that SD card and other devices are not used at the same time from different tasks. 

SD SPI driver is represented using an ``sdmmc_host_t`` structure initialized using ``SDSPI_HOST_DEFAULT`` macro. For slot initialization, ``SDSPI_SLOT_CONFIG_DEFAULT`` can be used to fill in default pin mapping, which is the same as the pin mapping in SD mode.

SD SPI driver APIs are very similar to SDMMC host APIs. As with the SDMMC host driver, only ``sdspi_host_init``, ``sdspi_host_init_slot``, and ``sdspi_host_deinit`` functions are normally used by the applications. Other functions are called by the protocol level driver via function pointers in ``sdmmc_host_t`` structure.

.. doxygenfunction:: sdspi_host_init

.. doxygendefine:: SDSPI_HOST_DEFAULT

.. doxygenfunction:: sdspi_host_init_slot

.. doxygenstruct:: sdspi_slot_config_t
    :members:

.. doxygendefine:: SDSPI_SLOT_NO_CD
.. doxygendefine:: SDSPI_SLOT_NO_WP
.. doxygendefine:: SDSPI_SLOT_CONFIG_DEFAULT

.. doxygenfunction:: sdspi_host_set_card_clk
.. doxygenfunction:: sdspi_host_do_transaction
.. doxygenfunction:: sdspi_host_deinit
