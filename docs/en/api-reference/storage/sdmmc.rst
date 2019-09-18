SD/SDIO/MMC Driver
==================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The SD/SDIO/MMC driver currently supports SD memory, SDIO cards, and eMMC chips. This is a protocol level driver built on top of SDMMC and SD SPI host drivers.

SDMMC and SD SPI host drivers (:component:`driver/include/driver/sdmmc_host.h`) provide API functions for:

- Sending commands to slave devices
- Sending and receiving data
- Handling error conditions within the bus

For functions used to initialize and configure:

- SDMMC host, see :doc:`SDMMC Host API <../peripherals/sdmmc_host>`
- SD SPI host, see :doc:`SD SPI Host API <../peripherals/sdspi_host>`


The SDMMC protocol layer described in this document handles the specifics of the SD protocol, such as the card initialization and data transfer commands.

The protocol layer works with the host via the :cpp:class:`sdmmc_host_t` structure. This structure contains pointers to various functions of the host.


Application Example
-------------------

An example which combines the SDMMC driver with the FATFS library is provided in the :example:`storage/sd_card` directory of ESP-IDF examples. This example initializes the card, then writes and reads data from it using POSIX and C library APIs. See README.md file in the example directory for more information.



Protocol layer API
------------------

The protocol layer is given the :cpp:class:`sdmmc_host_t` structure. This structure describes the SD/MMC host driver, lists its capabilities, and provides pointers to functions of the driver. The protocol layer stores card-specific information in the :cpp:class:`sdmmc_card_t` structure. When sending commands to the SD/MMC host driver, the protocol layer uses the :cpp:class:`sdmmc_command_t` structure to describe the command, arguments, expected return values, and data to transfer if there is any.


Using API with SD memory cards
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. To initialize the host, call the host driver functions, e.g., :cpp:func:`sdmmc_host_init`, :cpp:func:`sdmmc_host_init_slot`.
2. To initialize the card, call :cpp:func:`sdmmc_card_init` and pass to it the parameters ``host`` - the host driver information, and ``card`` - a pointer to the structure :cpp:class:`sdmmc_card_t` which will be filled with information about the card when the function completes.
3. To read and write sectors of the card, use :cpp:func:`sdmmc_read_sectors` and :cpp:func:`sdmmc_write_sectors` respectively and pass to it the parameter ``card`` - a pointer to the card information structure.
4. If the card is not used anymore, call the host driver function - e.g., :cpp:func:`sdmmc_host_deinit` - to disable the host peripheral and free the resources allocated by the driver.


Using API with eMMC chips
^^^^^^^^^^^^^^^^^^^^^^^^^

From the protocol layer's perspective, eMMC memory chips behave exactly like SD memory cards. Even though eMMCs are chips and do not have a card form factor, the terminology for SD cards can still be applied to eMMC due to the similarity of the protocol (`sdmmc_card_t`, `sdmmc_card_init`). Note that eMMC chips cannot be used over SPI, which makes them incompatible with the SD SPI host driver.

To initialize eMMC memory and perform read/write operations, follow the steps listed for SD cards in the previous section.


Using API with SDIO cards
^^^^^^^^^^^^^^^^^^^^^^^^^

Initialization and the probing process is the same as with SD memory cards. The only difference is in data transfer commands in SDIO mode.

During the card initialization and probing, performed with :cpp:func:`sdmmc_card_init`, the driver only configures the following registers of the IO card:

1. The IO portion of the card is reset by setting RES bit in the I/O Abort (0x06) register.
2. If 4-line mode is enabled in host and slot configuration, the driver attempts to set the Bus width field in the Bus Interface Control (0x07) register. If setting the filed is successful, which means that the slave supports 4-line mode, the host is also switched to 4-line mode.
3. If high-speed mode is enabled in the host configuration, the SHS bit is set in the High Speed (0x13) register.

In particular, the driver does not set any bits in (1) I/O Enable and Int Enable registers, (2) I/O block sizes, etc. Applications can set them by calling :cpp:func:`sdmmc_io_write_byte`.

For card configuration and data transfer, choose the pair of functions relevant to your case from the table below.

=========================================================================  =================================  =================================
Action                                                                     Read Function                      Write Function                   
=========================================================================  =================================  =================================
Read and write a single byte using IO_RW_DIRECT (CMD52)                    :cpp:func:`sdmmc_io_read_byte`     :cpp:func:`sdmmc_io_write_byte`
Read and write multiple bytes using IO_RW_EXTENDED (CMD53) in byte mode    :cpp:func:`sdmmc_io_read_bytes`    :cpp:func:`sdmmc_io_write_bytes`
Read and write blocks of data using IO_RW_EXTENDED (CMD53) in block mode   :cpp:func:`sdmmc_io_read_blocks`   :cpp:func:`sdmmc_io_write_blocks`
=========================================================================  =================================  =================================

SDIO interrupts can be enabled by the application using the function :cpp:func:`sdmmc_io_enable_int`. When using SDIO in 1-line mode, the D1 line also needs to be connected to use SDIO interrupts.

If you want the application to wait until the SDIO interrupt occurs, use :cpp:func:`sdmmc_io_wait_int`.


Combo (memory + IO) cards
^^^^^^^^^^^^^^^^^^^^^^^^^

The driver does not support SD combo cards. Combo cards are treated as IO cards.


Thread safety
^^^^^^^^^^^^^

Most applications need to use the protocol layer only in one task. For this reason, the protocol layer does not implement any kind of locking on the :cpp:class:`sdmmc_card_t` structure, or when accessing SDMMC or SD SPI host drivers. Such locking is usually implemented on a higher layer, e.g., in the filesystem driver.


API Reference
-------------

.. include:: /_build/inc/sdmmc_cmd.inc

.. include:: /_build/inc/sdmmc_types.inc
