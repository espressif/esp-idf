SPI Flash and External SPI RAM Configuration
============================================

This page is a guide for configuring SPI Flash and external SPI RAM. Supported frequency and mode combination, error handling are also elaborated.

Terminology
-----------

=============      ===========================
Term               Definition
=============      ===========================
**SPI**            Serial Peripheral Interface
**MSPI**           Memory SPI Peripheral, SPI Peripheral dedicated for memory
**SDR**            Single Data Rate
**DDR**            Double Data Rate
**line mode**      Number of signals used to transfer data in the data phase of SPI transactions. e.g., for 4-bit-mode, the speed of the data phase would be 4 bit per clock cycle.
**FxRx**           F stands for Flash, R stands for PSRAM, x stands for line mode. e.g. F4R4 stands for an {IDF_TARGET_NAME} with Quad Flash and Quad PSRAM
=============      ===========================

.. note::

    On {IDF_TARGET_NAME}, MSPI stands for the SPI0/1. SPI0 and SPI1 share a common SPI bus. The main Flash and PSRAM are connected to the MSPI peripheral. CPU accesses them via Cache.


.. _flash-psram-configuration:

How to configure Flash and PSRAM
--------------------------------

``idf.py menuconfig`` is used to open the configuration menu.

Configure the Flash
^^^^^^^^^^^^^^^^^^^

The Flash related configurations are under ``Serial flasher config`` menu.

1. Flash type used on the board. For Octal Flash, select :ref:`CONFIG_ESPTOOLPY_OCT_FLASH`. For Quad Flash, uncheck this configuration.
2. Flash line mode. Select a line mode in :ref:`CONFIG_ESPTOOLPY_FLASHMODE`. The higher the line mode is, the faster the SPI speed is. See terminology above about the line mode.
3. Flash sample mode. Select a sample mode in :ref:`CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE`. DDR mode is faster than SDR mode. See terminology above about SDR and DDR mode.
4. Flash speed. Select a Flash frequency in :ref:`CONFIG_ESPTOOLPY_FLASHFREQ`.
5. Flash size. Flash size, in megabytes. Select a Flash size in :ref:`CONFIG_ESPTOOLPY_FLASHSIZE`.

Configure the PSRAM
^^^^^^^^^^^^^^^^^^^

To enable PSRAM, please enable the :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_SPIRAM_SUPPORT` under ``Component config / {IDF_TARGET_NAME}-Specific`` menu. Then all the PSRAM related configurations will be visible under ``SPI RAM config`` menu.

1. PSRAM type used on the board. Select a type in :ref:`CONFIG_SPIRAM_MODE` for Quad or Octal PSRAM.
2. PSRAM speed. Select a PSRAM frequency in :ref:`CONFIG_SPIRAM_SPEED`.

.. note::

    Configuration 1 of Flash and PSRAM should be selected according to your actual hardware.

    For the reset of the above configurations:

    - Flash and PSRAM share the same internal clock.
    - Quad Flash only supports STR mode. Octal Flash may support either/both STR/DTR modes under OPI mode, depending on the flash model and the vendor.
    - Quad PSRAM only supports STR mode, while Octal PSRAM only supports DTR mode.

    Therefore, some limitations should be noticed when configuring configuration 2, 3 and 4 of Flash, and configuration 2 of PSRAM. Please refer to :ref:`All Supported Modes and Speeds <flash-psram-combination>`

.. note::

    If a board with Octal Flash resets before the second-stage bootloader, please refer to :ref:`Error Handling Chapter <flash-psram-error>`


.. _flash-psram-combination:

All Supported Modes and Speeds
------------------------------

.. note::

    For MSPI DDR mode, the data are sampled on both the positive edge and the negative edge. e.g.: if a Flash is set to 80 MHz and DDR mode, then the final speed of the Flash is 160 MHz. This is faster than the Flash setting to 120 Mhz and STR mode.


F8R8 Hardware
^^^^^^^^^^^^^

======= =============== ======= ============
 Group   Flash mode      Group   PSRAM mode
======= =============== ======= ============
 A       120 MHz SDR     A       N.A.
 B       80 MHz DDR      B       80 MHz DDR
 C       80 MHz SDR      C       40 MHz DDR
 C       40 MHz DDR      C
 C       < 40 MHz        C
 D                       D       disable
======= =============== ======= ============

1. Flash mode in group A works with PSRAM mode in group A/D
2. Flash mode in group B/C works with PSRAM mode in group B/C/D


F4R8 Hardware
^^^^^^^^^^^^^

======= =============== ======= ============
 Group   Flash mode      Group   PSRAM mode
======= =============== ======= ============
 A       120 MHz SDR     A       N.A.
 B       80 MHz  SDR     B       80MHz DDR
 C       40 MHz  SDR     C       40MHz DDR
 C       20 MHz  SDR     C
 D                       D       disable
======= =============== ======= ============

1. Flash mode in group A works with PSRAM mode in group A/D
2. Flash mode in group B/C works with PSRAM mode in group B/C/D


F4R4 Hardware
^^^^^^^^^^^^^

====== =============== ====== ============
 Type   Flash           Type   PSRAM
====== =============== ====== ============
 A      120 MHz         A      120MHz
 B      80 MHz          B      80MHz
 C      40 MHz          C      40MHz
 C      20 MHz          C
 D                      D      disable
====== =============== ====== ============

1. Flash in A works with PSRAM in A/C/D
2. Flash in B works with PSRAM in B/C/D
3. Flash in C works with PSRAM in A/B/C/D


.. _flash-psram-error:

Error handling
--------------

1. If a board with Octal Flash resets before the second-stage bootloader:

.. code-block:: c

    ESP-ROM:esp32s3-20210327
    Build:Mar 27 2021
    rst:0x7 (TG0WDT_SYS_RST),boot:0x18 (SPI_FAST_FLASH_BOOT)
    Saved PC:0x400454d5
    SPIWP:0xee
    mode:DOUT, clock div:1
    load:0x3fcd0108,len:0x171c
    ets_loader.c 78

it may mean that the necessary efuses are not correctly burnt. please check the eFuse bits of the chip using command ``espefuse.py summary``.

The 1st bootloader relies on an eFuse bit ``FLASH_TYPE`` to reset the Flash into the default mode (SPI mode). If this bit is not burnt and the flash is working in OPI mode, 1st bootloader may not be able to read from the flash and load the following images.

Run this command to burn the eFuse bit:

.. code-block:: python

    python3 ./espefuse.py -p /dev/<serial_device> --do-not-confirm burn_efuse FLASH_TYPE 1

.. note::

    This step is irreversible. Please do check if your hardware is actually using an Octal Flash.
