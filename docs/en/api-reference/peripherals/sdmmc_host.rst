SDMMC Host Driver
=================

Overview
--------

ESP32's SDMMC host peripheral has two slots:

- Slot 0 (:c:macro:`SDMMC_HOST_SLOT_0`) is an 8-bit slot. It uses ``HS1_*`` signals in the PIN MUX.
- Slot 1 (:c:macro:`SDMMC_HOST_SLOT_1`) is a 4-bit slot. It uses ``HS2_*`` signals in the PIN MUX.

Pin mappings of these slots are given in the table below.

    +--------+-------------+-------------+
    | Signal | Slot 0      | Slot 1      |
    +========+=============+=============+
    | CMD    | GPIO11      | GPIO15      |
    +--------+-------------+-------------+
    | CLK    | GPIO6       | GPIO14      |
    +--------+-------------+-------------+
    | D0     | GPIO7       | GPIO2       |
    +--------+-------------+-------------+
    | D1     | GPIO8       | GPIO4       |
    +--------+-------------+-------------+
    | D2     | GPIO9       | GPIO12      |
    +--------+-------------+-------------+
    | D3     | GPIO10      | GPIO13      |
    +--------+-------------+-------------+
    | D4     | GPIO16      |             |
    +--------+-------------+-------------+
    | D5     | GPIO17      |             |
    +--------+-------------+-------------+
    | D6     | GPIO5       |             |
    +--------+-------------+-------------+
    | D7     | GPIO18      |             |
    +--------+-------------+-------------+
    | CD     | any input via GPIO matrix |
    +--------+---------------------------+
    | WP     | any input via GPIO matrix |
    +--------+---------------------------+

The Card Detect and Write Protect signals can be routed to arbitrary pins using the GPIO matrix. To reserve the pins, set the ``gpio_cd`` and ``gpio_wp`` members of the :cpp:class:`sdmmc_slot_config_t` structure before calling :cpp:func:`sdmmc_host_init_slot`. Please note that it is not advised to specify a Card Detect pin when working with SDIO cards, because the card detect signal in ESP32 can also trigger SDIO slave interrupt.

.. warning::
    
    Pins used by Slot 0 (``HS1_*``) are also used to connect the SPI flash chip in ESP32-WROOM and ESP32-WROVER modules. These pins cannot be shared between an SD card and SPI flash. If you need to use Slot 0, connect SPI flash to different pins and set eFuses accordingly.


Supported Speed Modes
---------------------

SDMMC Host driver supports the following speed modes:

- Default Speed (20 MHz), 4-line/1-line (with SD cards), and 8-line (with 3.3 V eMMC)
- High Speed (40 MHz), 4-line/1-line (with SD cards), and 8-line (with 3.3 V eMMC)
- High Speed DDR (40 MHz), 4-line (with 3.3 V eMMC)

Speed modes not supported at present:

- High Speed DDR mode, 8-line eMMC
- UHS-I 1.8 V modes, 4-line SD cards


Using the SDMMC Host Driver
---------------------------

Of all the functions listed below, only the following ones will be used directly by most applications:

- :cpp:func:`sdmmc_host_init`
- :cpp:func:`sdmmc_host_init_slot`
- :cpp:func:`sdmmc_host_deinit`

Other functions, such as the ones given below, will be called by the SD/MMC protocol layer via function pointers in the :cpp:class:`sdmmc_host_t` structure:

- :cpp:func:`sdmmc_host_set_bus_width`
- :cpp:func:`sdmmc_host_set_card_clk`
- :cpp:func:`sdmmc_host_do_transaction` 


Configuring Bus Width and Frequency
-----------------------------------

With the default initializers for :cpp:class:`sdmmc_host_t` and :cpp:class:`sdmmc_slot_config_t` (:c:macro:`SDMMC_HOST_DEFAULT` and :c:macro:`SDMMC_SLOT_CONFIG_DEFAULT`), SDMMC Host driver will attempt to use the widest bus supported by the card (4 lines for SD, 8 lines for eMMC) and the frequency of 20 MHz.

In the designs where communication at 40 MHz frequency can be achieved, it is possible to increase the bus frequency by changing the ``max_freq_khz`` field of :cpp:class:`sdmmc_host_t`::

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

To configure the bus width, set the ``width`` field of :cpp:class:`sdmmc_slot_config_t`. For example, to set 1-line mode::

    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    slot.width = 1;


See also
--------

See :doc:`SD/SDIO/MMC Driver <../storage/sdmmc>` for the higher level driver which implements the protocol layer.

See :doc:`SD SPI Host Driver <sdspi_host>` for a similar driver which uses the SPI controller and is limited to SD protocol's SPI mode.

See :doc:`sd_pullup_requirements` for pullup support and compatibilities of modules and development kits.


API Reference
-------------

.. include:: /_build/inc/sdmmc_host.inc
