SPI Flash and External SPI RAM Configuration
============================================

:link_to_translation:`zh_CN:[中文]`

This page is a guide for configuring SPI flash and external SPI RAM. Supported frequency and mode combination, error handling are also elaborated.

Terminology
-----------

.. list-table::
    :header-rows: 1
    :widths: 20 80
    :align: center

    * - Term
      - Definition
    * - **SPI**
      - Serial Peripheral Interface
    * - **MSPI**
      - Memory SPI Peripheral, SPI Peripheral dedicated for memory
    * - **SDR**
      - Single Data Transfer Rate (SDR), also known as STR (Single Transfer Rate)
    * - **DDR**
      - Double Data Transfer Rate (DDR), also known as DTR (Double Transfer Rate)
    * - **line mode**
      - Number of signals used to transfer data in the data phase of SPI transactions. e.g., for 4-bit-mode, the speed of the data phase would be 4 bit per clock cycle.
    * - **FxRx**
      - F stands for flash, R stands for PSRAM, x stands for line mode. e.g., F4R4 stands for an {IDF_TARGET_NAME} with Quad flash and Quad PSRAM.

.. note::

    On {IDF_TARGET_NAME}, MSPI stands for the SPI0/1. SPI0 and SPI1 share a common SPI bus. The main flash and PSRAM are connected to the MSPI peripheral. CPU accesses them via Cache.


.. _flash-psram-configuration:

How to Configure Flash and PSRAM
--------------------------------

``idf.py menuconfig`` is used to open the configuration menu.

Configure the Flash
^^^^^^^^^^^^^^^^^^^

The flash related configurations are under ``Serial flasher config`` menu.

1. Flash type used on the board. For Octal flash, select :ref:`CONFIG_ESPTOOLPY_OCT_FLASH`. For Quad flash, uncheck this configuration.
2. Flash line mode. Select a line mode in :ref:`CONFIG_ESPTOOLPY_FLASHMODE`. The higher the line mode is, the faster the SPI speed is. See terminology above about the line mode.
3. Flash sample mode. Select a sample mode in :ref:`CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE`. DDR mode is faster than SDR mode. See terminology above about SDR and DDR mode.
4. Flash speed. Select a Flash frequency in :ref:`CONFIG_ESPTOOLPY_FLASHFREQ`.
5. Flash size. Flash size, in megabytes. Select a flash size in :ref:`CONFIG_ESPTOOLPY_FLASHSIZE`.

Configure the PSRAM
^^^^^^^^^^^^^^^^^^^

To enable PSRAM, please enable the :ref:`CONFIG_SPIRAM` under ``Component config / Hardware Settings`` menu. Then all the PSRAM related configurations will be visible under ``SPI RAM config`` menu.

1. PSRAM type used on the board. Select a type in :ref:`CONFIG_SPIRAM_MODE` for Quad or Octal PSRAM.
2. PSRAM speed. Select a PSRAM frequency in :ref:`CONFIG_SPIRAM_SPEED`.

.. note::

    Configuration 1 of flash and PSRAM should be selected according to your actual hardware.

    For the reset of the above configurations:

    - Flash and PSRAM share the same internal clock.
    - Quad flash only supports STR mode. Octal flash may support either/both STR/DTR modes under OPI mode, depending on the flash model and the vendor.
    - Quad PSRAM only supports STR mode, while Octal PSRAM only supports DTR mode.

    Therefore, some limitations should be noticed when configuring configuration 2, 3 and 4 of flash, and configuration 2 of PSRAM. Please refer to :ref:`All Supported Modes and Speeds <flash-psram-combination>`.

.. note::

    If a board with Octal flash resets before the second-stage bootloader, please refer to :ref:`Error Handling Chapter <flash-psram-error>`.


.. _flash-psram-combination:

All Supported Modes and Speeds
------------------------------

.. note::

    For MSPI DDR mode, the data are sampled on both the positive edge and the negative edge. e.g., if a flash is set to 80 MHz and DDR mode, then the final speed of the flash is 160 MHz. This is faster than the flash setting to 120 Mhz and STR mode.

.. important::

    120 MHz DDR mode is an experimental feature. You will only see it when:

    - :ref:`CONFIG_IDF_EXPERIMENTAL_FEATURES` is enabled

    With above step, you will find 120 MHz option is visible.

    Risks:

    If your chip powers on at a certain temperature, then after the temperature increases or decreases over 20 celsius degree, the accesses to/from PSRAM/flash will crash randomly. Flash access crash will lead to program crash.

    Note 20 celsius degree is not a totally correct number. This value may changes among chips.

.. note::

    The PSRAM requires a phase point calibration algorithm when operating at 120M. The phase point setting is related to the temperature at startup. When the temperature increases / decreases significantly during the operation of the chip, the PSRAM may experience read/write errors. To solve this problem, you can enable dynamic adjustment of the PSRAM phase point based on the temperature value with :ref:`CONFIG_SPIRAM_TIMING_TUNING_POINT_VIA_TEMPERATURE_SENSOR`. This creates a task that measures the temperature every :ref:`CONFIG_SPIRAM_TIMING_MEASURE_TEMPERATURE_INTERVAL_SECOND` seconds and adjusts the PSRAM phase point accordingly.

F8R8 Hardware
^^^^^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 20 30 20 30
    :align: center

    * - Group
      - Flash mode
      - Group
      - PSRAM mode
    * - A
      - 120 MHz DDR
      - A
      - 120 MHz DDR
    * - A
      - 120 MHz SDR
      - A
      -
    * - B
      - 80 MHz DDR
      - B
      - 80 MHz DDR
    * - C
      - 80 MHz SDR
      - C
      - 40 MHz DDR
    * - C
      - 40 MHz DDR
      - C
      -
    * - C
      - < 40 MHz
      - C
      -
    * - D
      -
      - D
      - disable

1. Flash mode in group A works with PSRAM mode in group A/D.
2. Flash mode in group B/C works with PSRAM mode in group B/C/D.


F4R8 Hardware
^^^^^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 20 30 20 30
    :align: center

    * - Group
      - Flash mode
      - Group
      - PSRAM mode
    * - A
      - 120 MHz SDR
      - A
      - 120 MHz DDR
    * - B
      - 80 MHz SDR
      - B
      - 80 MHz DDR
    * - C
      - 40 MHz SDR
      - C
      - 40 MHz DDR
    * - C
      - 20 MHz SDR
      - C
      -
    * - D
      -
      - D
      - disable

1. Flash mode in group A works with PSRAM mode in group A/D.
2. Flash mode in group B/C works with PSRAM mode in group B/C/D.


F4R4 Hardware
^^^^^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 20 30 20 30
    :align: center

    * - Group
      - Flash mode
      - Group
      - PSRAM mode
    * - A
      - 120 MHz
      - A
      - 120 MHz
    * - B
      - 80 MHz
      - B
      - 80 MHz
    * - C
      - 40 MHz
      - C
      - 40 MHz
    * - C
      - 20 MHz
      - C
      -
    * - D
      -
      - D
      - disable

1. Flash mode in group A works with PSRAM in group A/C/D.
2. Flash mode in group B works with PSRAM in group B/C/D.
3. Flash mode in group C works with PSRAM in group A/B/C/D.


.. _flash-psram-error:

Error Handling
--------------

1. If a board with Octal flash resets before the second-stage bootloader:

    .. code-block:: c

        ESP-ROM:esp32s3-20210327
        Build:Mar 27 2021
        rst:0x7 (TG0WDT_SYS_RST),boot:0x18 (SPI_FAST_FLASH_BOOT)
        Saved PC:0x400454d5
        SPIWP:0xee
        mode:DOUT, clock div:1
        load:0x3fcd0108,len:0x171c
        ets_loader.c 78

   this may mean that the necessary eFuses are not correctly burnt. Please check the eFuse bits of the chip using ``idf.py efuse-summary``.

   The ROM bootloader relies on an eFuse bit ``FLASH_TYPE`` to reset the flash into the default mode (SPI mode). If this bit is not burnt and the flash is working in OPI mode, ROM bootloader may not be able to read from the flash and load the following images.

2. If you enabled :ref:`CONFIG_ESPTOOLPY_OCT_FLASH`, and there's an error log saying:

    .. code-block:: c

        Octal flash option selected, but EFUSE not configured!

   this means:

   - either you're using a board with a Quad flash,
   - or you're using a board with an Octal flash, but the eFuse bit ``FLASH_TYPE`` isn't burnt. Espressif guarantees this bit is burnt during module manufacturing, but if the module is manufactured by others, this may happen.


Here is a method to burn the eFuse bit:

.. code-block:: shell

    idf.py -p PORT efuse-burn --do-not-confirm FLASH_TYPE 1

.. note::

    This step is irreversible. Please do check if your hardware is actually using an Octal flash.
