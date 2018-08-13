SD/SDIO/MMC Driver
==================

Overview
--------

SD/SDIO/MMC driver currently supports SD memory, SDIO cards, and eMMC chips. This protocol level driver builds on top of SDMMC and SD SPI host drivers.

SDMMC and SD SPI host drivers (``driver/sdmmc_host.h``) provide APIs to send commands to the slave device(s), send and receive data, and handle error conditions on the bus.
    
- See :doc:`SDMMC Host API <../peripherals/sdmmc_host>` for functions used to initialize and configure SDMMC host.
- See :doc:`SD SPI Host API <../peripherals/sdspi_host>` for functions used to initialize and configure SD SPI host.

SDMMC protocol layer (``sdmmc_cmd.h``), described in this document, handles specifics of SD protocol such as card initialization and data transfer commands.

Protocol layer works with the host via :cpp:class:`sdmmc_host_t` structure. This structure contains pointers to various functions of the host.

Application Example
-------------------

An example which combines SDMMC driver with FATFS library is provided in ``examples/storage/sd_card`` directory. This example initializes the card, writes and reads data from it using POSIX and C library APIs. See README.md file in the example directory for more information.


Protocol layer APIs
-------------------

Protocol layer is given :cpp:class:`sdmmc_host_t` structure which describes the SD/MMC host driver, lists its capabilites, and provides pointers to functions of the driver. Protocol layer stores card-specific information in :cpp:class:`sdmmc_card_t` structure. When sending commands to the SD/MMC host driver, protocol layer uses :cpp:class:`sdmmc_command_t` structure to describe the command, argument, expected return value, and data to transfer, if any.

Usage with SD memory cards
^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Call the host driver functions to initialize the host (e.g. :cpp:func:`sdmmc_host_init`, :cpp:func:`sdmmc_host_init_slot`).
2. Call :cpp:func:`sdmmc_card_init` to initialize the card, passing it host driver information (``host``) and a pointer to :cpp:class:`sdmmc_card_t` structure which will be filled in (``card``).
3. To read and write sectors of the card, use :cpp:func:`sdmmc_read_sectors` and :cpp:func:`sdmmc_write_sectors`, passing the pointer to card information structure (``card``).
4. When card is not used anymore, call the host driver function to disable the host peripheral and free resources allocated by the driver (e.g. :cpp:func:`sdmmc_host_deinit`).

Usage with eMMC chips
^^^^^^^^^^^^^^^^^^^^^

From the perspective of the protocol layer, eMMC memory chips behave the same way as SD memory cards. Because of similarity of the protocol, even though eMMC are chips don't have the "card" form factor, same terminology is used as for SD cards (`sdmmc_card_t`, `sdmmc_card_init`). Note that eMMC chips can not be used over SPI, therefore are incompatible with SD SPI host driver.

To initialize eMMC memory and do read/write operations, follow the steps listed above for SD cards.


Usage with SDIO cards
^^^^^^^^^^^^^^^^^^^^^

Initialization an probing process is the same as with SD memory cards. Only data transfer commands differ in SDIO mode.

During probing and card initialization (done by :cpp:func:`sdmmc_card_init`), the driver only configures the following registers of the IO card:

1. The IO portion of the card is reset by setting RES bit in "I/O Abort" (0x06) register.
2. If 4-line mode is enalbed in host and slot configuration, driver attempts to set "Bus width" field in "Bus Interface Control" (0x07) register. If that succeeds (which means that slave supports 4-line mode), host is also switched to 4-line mode.
3. If high-speed mode is enabled in host configuration, SHS bit is set in "High Speed" (0x13) register.

In particular, the driver does not set any of the bits in I/O Enable, Int Enable registers, IO block sizes, etc. Applications can set these by calling :cpp:func:`sdmmc_io_write_byte`.

For card configuration and data transfer, use one of the following functions:

- :cpp:func:`sdmmc_io_read_byte`, :cpp:func:`sdmmc_io_write_byte` — read and write single byte using IO_RW_DIRECT (CMD52).
- :cpp:func:`sdmmc_io_read_bytes`,  :cpp:func:`sdmmc_io_write_bytes` — read and write multiple bytes using IO_RW_EXTENDED (CMD53), in byte mode.
- :cpp:func:`sdmmc_io_read_blocks`, :cpp:func:`sdmmc_io_write_blocks` — read and write blocks of data using IO_RW_EXTENDED (CMD53), in block mode.

SDIO interrupts can be enabled by the application using :cpp:func:`sdmmc_io_enable_int` function. When using SDIO in 1-line mode, D1 line also needs to be connected to use SDIO interrupts.

The application can wait for SDIO interrupt to occur using :cpp:func:`sdmmc_io_wait_int`.

Combo (memory + IO) cards
^^^^^^^^^^^^^^^^^^^^^^^^^

The driver does not support SD combo cards. Combo cards will be treated as IO cards.


Thread safety
^^^^^^^^^^^^^

Most applications need to use the protocol layer only in one task; therefore the protocol layer doesn't implement any kind of locking on the :cpp:class:`sdmmc_card_t` structure, or when accessing SDMMC or SD SPI host drivers. Such locking is usually implemented in the higher layer (e.g. in the filesystem driver).


API Reference
-------------

.. include:: /_build/inc/sdmmc_cmd.inc

.. include:: /_build/inc/sdmmc_types.inc
