SD/SDIO/MMC Driver
==================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The SD/SDIO/MMC driver supports SD memory, SDIO cards, and eMMC chips. This is a protocol layer driver (:component_file:`sdmmc/include/sdmmc_cmd.h`) which can work together with:

.. list::
    :SOC_SDMMC_HOST_SUPPORTED: - SDMMC host driver (:component_file:`esp_driver_sdmmc/legacy/include/driver/sdmmc_host.h`), see :doc:`SDMMC Host API <../peripherals/sdmmc_host>` for more details.
    :SOC_GPSPI_SUPPORTED: - SDSPI host driver (:component_file:`esp_driver_sdspi/include/driver/sdspi_host.h`), see :doc:`SD SPI Host API <../peripherals/sdspi_host>` for more details.

Protocol Layer vs Host Layer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The SDMMC protocol layer described in this document handles the specifics of the SD protocol, such as the card initialization flow and various data transfer command flows. The protocol layer works with the host via the :cpp:class:`sdmmc_host_t` structure. This structure contains pointers to various functions of the host.

Host layer driver(s) implement the protocol layer driver by supporting these functions:

- Sending commands to slave devices
- Sending and receiving data
- Handling error conditions within the bus

.. blockdiag:: /../_static/diagrams/sd/sd_arch.diag
    :scale: 100%
    :caption: SD Host Side Component Architecture
    :align: center


Application Examples
--------------------

.. list::

    :SOC_SDMMC_HOST_SUPPORTED: - :example:`storage/sd_card/sdmmc` demonstrates how to operate an SD card formatted with the FatFS file system via the SDMMC interface.

    :SOC_SDMMC_HOST_SUPPORTED: - :example:`storage/emmc` demonstrates how to operate an eMMC chip formatted with the FatFS file system via the SDMMC interface.

    :SOC_GPSPI_SUPPORTED: - :example:`storage/sd_card/sdspi` demonstrates how to operate an SD card formatted with the FatFS file system via the SPI interface.

Protocol Layer API
------------------

The protocol layer is given the :cpp:class:`sdmmc_host_t` structure. This structure describes the SD/MMC host driver, lists its capabilities, and provides pointers to functions for the implementation driver. The protocol layer stores card-specific information in the :cpp:class:`sdmmc_card_t` structure. When sending commands to the SD/MMC host driver, the protocol layer uses the :cpp:class:`sdmmc_command_t` structure to describe the command, arguments, expected return values, and data to transfer if there is any.

Using API with SD Memory Cards
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list::
    :SOC_SDMMC_HOST_SUPPORTED: - To initialize the SDMMC host, call the host driver functions, e.g., :cpp:func:`sdmmc_host_init`, :cpp:func:`sdmmc_host_init_slot`.¸
    :SOC_GPSPI_SUPPORTED: - To initialize the SDSPI host, call the host driver functions, e.g., :cpp:func:`sdspi_host_init`, :cpp:func:`sdspi_host_init_slot`.
    - To initialize the card, call :cpp:func:`sdmmc_card_init` and pass to it the parameters ``host`` - the host driver information, and ``card`` - a pointer to the structure :cpp:class:`sdmmc_card_t` which will be filled with information about the card when the function completes.
    - To read and write sectors of the card, use :cpp:func:`sdmmc_read_sectors` and :cpp:func:`sdmmc_write_sectors` respectively and pass to it the parameter ``card`` - a pointer to the card information structure.

    - If the card is not used anymore, call the host driver function to disable the host peripheral and free the resources allocated by the driver (``sdmmc_host_deinit`` for SDMMC or ``sdspi_host_deinit`` for SDSPI).

.. only:: not SOC_SDMMC_HOST_SUPPORTED

    eMMC Support
    ^^^^^^^^^^^^

    {IDF_TARGET_NAME} does not have an SDMMC Host controller, and can only use SPI protocol for communication with cards. However, eMMC chips cannot be used over SPI. Therefore it is not possible to use eMMC chips with {IDF_TARGET_NAME}.

.. only:: SOC_SDMMC_HOST_SUPPORTED

    Using API with eMMC Chips
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    From the protocol layer's perspective, eMMC memory chips behave exactly like SD memory cards. Even though eMMCs are chips and do not have a card form factor, the terminology for SD cards can still be applied to eMMC due to the similarity of the protocol (`sdmmc_card_t`, `sdmmc_card_init`). Note that eMMC chips cannot be used over SPI, which makes them incompatible with the SD SPI host driver.

    To initialize eMMC memory and perform read/write operations, follow the steps listed for SD cards in the previous section.


    Using API with SDIO Cards
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    Initialization and the probing process are the same as with SD memory cards. The only difference is in data transfer commands in SDIO mode.

    During the card initialization and probing, performed with :cpp:func:`sdmmc_card_init`, the driver only configures the following registers of the IO card:

    1. The IO portion of the card is reset by setting RES bit in the I/O Abort (0x06) register.
    2. If 4-line mode is enabled in host and slot configuration, the driver attempts to set the Bus width field in the Bus Interface Control (0x07) register. If setting the filed is successful, which means that the slave supports 4-line mode, the host is also switched to 4-line mode.
    3. If high-speed mode is enabled in the host configuration, the SHS bit is set in the High Speed (0x13) register.

    In particular, the driver does not set any bits in (1) I/O Enable and Int Enable registers, (2) I/O block sizes, etc. Applications can set them by calling :cpp:func:`sdmmc_io_write_byte`.

    For card configuration and data transfer, choose the pair of functions relevant to your case from the table below.

    .. list-table::
       :widths: 55 25 20
       :header-rows: 1

       * - Action
         - Read Function
         - Write Function
       * - Read and write a single byte using IO_RW_DIRECT (CMD52)
         - :cpp:func:`sdmmc_io_read_byte`
         - :cpp:func:`sdmmc_io_write_byte`
       * - Read and write multiple bytes using IO_RW_EXTENDED (CMD53) in byte mode
         - :cpp:func:`sdmmc_io_read_bytes`
         - :cpp:func:`sdmmc_io_write_bytes`
       * - Read and write blocks of data using IO_RW_EXTENDED (CMD53) in block mode
         - :cpp:func:`sdmmc_io_read_blocks`
         - :cpp:func:`sdmmc_io_write_blocks`

    SDIO interrupts can be enabled by the application using the function :cpp:func:`sdmmc_io_enable_int`. When using SDIO in 1-line mode, the D1 line also needs to be connected to use SDIO interrupts.

    If you want the application to wait until the SDIO interrupt occurs, use :cpp:func:`sdmmc_io_wait_int`.

    .. only:: esp32

        There is a component ESSL (ESP Serial Slave Link) to use if you are communicating with an ESP32 SDIO slave. See :doc:`/api-reference/protocols/esp_serial_slave_link` and example :example:`peripherals/sdio/host`.

    Combo (Memory + IO) Cards
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    The driver does not support SD combo cards. Combo cards are treated as IO cards.


Thread Safety
^^^^^^^^^^^^^

Most applications need to use the protocol layer only in one task. For this reason, the protocol layer does not implement any kind of locking on the :cpp:class:`sdmmc_card_t` structure, or when accessing SDMMC or SD SPI host drivers. Such locking is usually implemented on a higher layer, e.g., in the filesystem driver.


API Reference
-------------

.. include-build-file:: inc/sdmmc_cmd.inc

.. include-build-file:: inc/sdmmc_types.inc
