SD Pull-up Requirements
=======================

Espressif hardware products are designed for multiple use cases which may require different pull states on pins. For this reason, the pull state of particular pins on certain products needs to be adjusted to provide the pull-ups required in the SD bus.

SD pull-up requirements apply to cases where {IDF_TARGET_NAME} uses the SPI or SDMMC controller to communicate with SD cards. When an SD card is operating in SPI mode or 1-bit SD mode, the CMD and DATA (DAT0 - DAT3) lines of the SD bus must be pulled up by 10 kOhm resistors. SD cards and SDIO devices should also have pull-ups on all above-mentioned lines (regardless of whether these lines are connected to the host) in order to prevent them from entering a wrong state.

.. only:: esp32

    By default, the MTDI bootstrapping pin is incompatible with the DAT2 line pull-up if the flash voltage is 3.3 V. For more information, see :ref:`mtdi_strapping_pin` below.

.. todo::

   Add a diagram of the Bus lines and pullups

This document has the following structure:

- `Overview of compatibility`_ between the default pull states on pins of Espressif's products and the states required by the SD bus
- `Solutions`_ - ideas on how to resolve compatibility issues
- `Related information`_ - other relevant information


.. _compatibility_overview_espressif_hw_sdio:

Overview of Compatibility
-------------------------

This section provides an overview of compatibility issues that might occur when using SDIO (secure digital input output). Since the SD bus needs to be connected to pull-ups, these issues should be resolved regardless of whether they are related to master (host) or slave (device). Each issue has links to its respective solution. A solution for a host and device may differ.


Systems on a Chip (SoCs)
^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    - ESP32 (except for D2WD versions, see `ESP32 datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_):

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2` for models with 3.3 V flash chip

    - ESP32-D2WD:

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`no_pull-up_on_gpio12`

.. only:: SOC_SDMMC_USE_GPIO_MATRIX

    {IDF_TARGET_NAME} SDMMC host controller allows using any of GPIOs for any of SD interface signals. However, it is recommended to avoid using strapping GPIOs, GPIOs with internal weak pull-downs and GPIOs commonly used for other purposes to prevent conflicts:

.. only:: esp32s3

    - GPIO0 (strapping pin)
    - GPIO45, GPIO46 (strapping pins, internal weak pulldown)
    - GPIO26 - GPIO32 (commonly used for SPI Flash and PSRAM)
    - GPIO33 - GPIO37 (when using chips and modules with Octal SPI Flash or Octal PSRAM)
    - GPIO43, GPIO44 (GPIOs used for UART0 by default)
    - GPIO19, GPIO20 (GPIOs used for USB by default)


Systems in Packages (SIP)
^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    - ESP32-PICO-D4:

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`


Modules
^^^^^^^

.. only:: esp32

    - ESP32-WROOM-32 Series, including ESP32-WROOM-32, ESP32-WROOM-32D, ESP32-WROOM-32U, and ESP32-SOLO-1

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`

    - ESP32-WROVER Series, including ESP32-WROVER and ESP32-WROVER-I

        - :ref:`sd_pull-up_no_pull-ups`

    - ESP32-WROVER-B Series, including ESP32-WROVER-B and ESP32-WROVER-IB

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`


.. _sdio_dev_kits:

Development Boards
^^^^^^^^^^^^^^^^^^

.. only:: esp32

    - ESP32-PICO-KIT, including PICO-KIT v4.1, v4.0, and v3

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`
        - :ref:`gpio2_strapping_pin`

    - ESP32-DevKitC, including ESP32-DevKitC v4 and v2

        - :ref:`sd_pull-up_no_pull-ups`
        - :ref:`strapping_conflicts_dat2`
        - :ref:`gpio2_strapping_pin`

    - ESP-WROVER-KIT

        - Required pull-ups are provided
        - :ref:`pull-up_conflicts_on_gpio13` (v4.1, v3, v2, and v1)
        - :ref:`strapping_conflicts_dat2` (v4.1, v2, and v1)
        - :ref:`gpio2_strapping_pin` (v2, v1)

        You can determine the version of your ESP23-WROVER-KIT by checking which module is mounted on it:

        - ESP32-WROVER-B on v4.1
        - ESP32-WROVER on v3
        - ESP32-WROOM-32 on v1 and v2

    - ESP32-LyraTD-MSC

        - Required pull-ups are provided
        - :ref:`strapping_conflicts_dat2`

    - ESP32-LyraT

        - Required pull-ups are provided
        - :ref:`pull-up_conflicts_on_gpio13`

.. only:: esp32s3

    - ESP32-S3-DevKitC-1

        - :ref:`sd_pull-up_no_pull-ups`

    - ESP32-S3-USB-OTG

        - The board may be used in 1-line and 4-line SD mode or SPI mode.
        - Required pull-ups are provided on GPIOs 33-38.

    - ESP32-S3-EYE

        - The board is limited to 1-line SD mode.
        - Required pull-ups are provided on GPIOs 38-40.

.. only:: SOC_SDIO_SLAVE_SUPPORTED

    Non-Espressif Hosts
    ^^^^^^^^^^^^^^^^^^^

    Please make sure that your SDIO host provides necessary pull-ups for all SD bus signals.


Solutions
---------

.. _sd_pull-up_no_pull-ups:

No Pull-ups
^^^^^^^^^^^

If you use a development board without pull-ups, you can do the following:

- If your host and slave device are on separate boards, replace one of them with a board that has pull-ups. For the list of Espressif's development boards with pull-ups, go to :ref:`sdio_dev_kits`.
- Attach external pull-ups by connecting each pin which requires a pull-up to VDD via a 10 kOhm resistor.

.. only:: esp32

    .. _pull-up_conflicts_on_gpio13:

    Pull-up Conflicts on GPIO13
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^

    If DAT3 of your device is not properly pulled up, you have the following options:

    - Use 1-bit SD mode and tie the device's DAT3 to VDD
    - Use SPI mode
    - Perform one of the following actions on the GPIO13 pin:
        - Remove the pull-down resistors
        - Attach a pull-up resistor of less than 5 kOhm (2 kOhm suggested)
        - Pull it up or drive it high either by using the host or with 3.3 V on VDD in 1-bit SD mode


    .. _strapping_conflicts_dat2:

    Conflicts Between Bootstrap and SDIO on DAT2
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    There is a conflict between the boot strapping requirements of the ESP32 and the SDIO protocol. For details, see :ref:`mtdi_strapping_pin`.

    To resolve the conflict, you have the following options:

    1. (Recommended) Burn the flash voltage selection eFuses. This permanently configures the internal regulator's output voltage to 3.3 V, and GPIO12 will not be used as a bootstrapping pin. After that, connect a pull-up resistor to GPIO12.

    .. warning::

        Burning eFuses is irreversible! The issue list above might be out of date, so please make sure that the module you are burning has a 3.3 V flash chip by checking the information on https://www.espressif.com/. If you burn the 3.3 V eFuses on a module with a 1.8 V flash chip, the module will stop functioning.

    If you are sure that you need to irreversibly burn eFuses, go to your ESP-IDF directory and run the following command:

    .. code-block:: bash

        components/esptool_py/esptool/espefuse.py set_flash_voltage 3.3V

    This command burns the ``XPD_SDIO_TIEH``, ``XPD_SDIO_FORCE``, and ``XPD_SDIO_REG`` eFuses. After all the three eFuses are burned to value 1, the internal VDD_SDIO flash voltage regulator is permanently set to 3.3 V. You will see the following log if the burning succeeds:

    .. code-block:: bash

        espefuse.py v2.6
        Connecting....

        Enable internal flash voltage regulator (VDD_SDIO) to 3.3 V.
        The following eFuses are burned: XPD_SDIO_FORCE, XPD_SDIO_REG, XPD_SDIO_TIEH.
        This is an irreversible operation.
        Type 'BURN' (all capitals) to continue.
        BURN
        VDD_SDIO setting complete.

    To check the status of the eFuses, run::

        ``components/esptool_py/esptool/espefuse.py summary``

    If running from an automated flashing script, ``espefuse.py`` has an option ``--do-not-confirm``.

    For more details, see **{IDF_TARGET_NAME} Technical Reference Manual** [`PDF <{IDF_TARGET_TRM_EN_URL}#efuse>`__].

    2. **If using 1-bit SD mode or SPI mode**, disconnect the DAT2 pin and make sure it is pulled high. For this, do one the following:

        - Leave the host's DAT2 floating and directly connect the slave's DAT2 to VDD.
        - For a slave device, build a firmware with the option ``SDIO_SLAVE_FLAG_DAT2_DISABLED`` and re-flash your device. This option helps avoid slave detecting on the DAT2 line. Note that 4-bit SD mode is no longer supported by the standard Card Common Control Register (CCCR); however, the host is not aware of that. The use of 4-bit SD mode has to be disabled on the host's side.


    .. _no_pull-up_on_gpio12:

    No Pull-up on GPIO12
    ^^^^^^^^^^^^^^^^^^^^

    Your module is compatible with the SDIO protocol. Just connect GPIO12 to VDD via a 10 kOhm resistor.


    .. _gpio2_strapping_pin:

    Download Mode Not Working (minor issue)
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    When the GPIO2 pin is pulled high in accordance with the SD pull-up requirements, you cannot enter Download mode because GPIO2 is a bootstrapping pin which in this case must be pulled low.

    There are the following solutions:

    - For boards that require shorting the GPIO0 and GPIO2 pins with a jumper, put the jumper in place, and the auto-reset circuit pulls GPIO2 low along with GPIO0 before entering Download mode.
    - For boards with components attached to their GPIO2 pin (such as pull-down resistors and/or LEDs), check the schematic of your development board for anything connected to GPIO2.

        - **LEDs** would not affect operation in most cases.
        - **Pull-down resistors** can interfere with DAT0 signals and must be removed.

    If the above solutions do not work for you, please determine if it is the host or slave device that has pull-ups affecting their GPIO2, then locate these pull-ups and remove them.


.. _related_info_sdio:

Related Information
-------------------

.. only:: esp32

    .. _mtdi_strapping_pin:

    MTDI Strapping Pin
    ^^^^^^^^^^^^^^^^^^

    MTDI (GPIO12) is used as a bootstrapping pin to select the output voltage of an internal regulator (VDD_SDIO) which powers the flash chip. This pin has an internal pull-down, so, if left unconnected, it will read low level at startup, which leads to selecting the default 3.3 V operation.

    All ESP32-WROVER modules, excluding ESP32-WROVER-B, use 1.8 V flash and have internal pull-ups on GPIO12. Other modules that use 3.3 V flash have no pull-ups on the GPIO12 pin, and this pin is slightly pulled down internally.

    When adding a pull-up to this pin for SD card operation, consider the following:

    - For boards that do not use the internal regulator (VDD_SDIO) to power flash, GPIO12 can be pulled high.
    - For boards using 1.8 V flash chips, GPIO12 needs to be pulled high at reset. This is fully compatible with the SD card operation.
    - On boards using the internal regulator and a 3.3 V flash chip, GPIO12 must be pulled low at reset. This is incompatible with the SD card operation. For reference information on compatibility of Espressif's boards with the SD card operation, see :ref:`compatibility_overview_espressif_hw_sdio`.


    Internal Pull-ups and Strapping Requirements
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Using external resistors is always preferable. However, Espressif's products have internal weak pull-up and pull-down resistors which can be enabled and used instead of external ones. Please keep in mind that this solution cannot guarantee reliable SDIO communication.

    With that said, the information about these internal pull-ups and strapping requirements can still be useful. Espressif hardware products have different weak internal pull-ups/pull-downs connected to CMD and DATA pins. The table below shows the default pull-up and pull-down states of the CMD and DATA pins.

    The following abbreviations are used in the table:

    - **WPU**: Weak pull-up inside the SoC
    - **WPD**: Weak pull-down inside the SoC
    - **PU**: Pull-up inside Espressif modules but outside the SoC

    .. list-table:: Default pull-up and pull-down states of the CMD and DATA pins
       :widths: 25 25 25 25
       :header-rows: 1

       * - GPIO number
         - Pin Name
         - Startup State
         - Strapping Requirement
       * - **15**
         - CMD
         - WPU
         -
       * - **2**
         - DAT0
         - WPD
         - Low for Download mode
       * - **4**
         - DAT1
         - WPD
         -
       * - **12**
         - DAT2
         - PU for 1.8 V flash; WPD for 3.3 V flash
         - High for 1.8 V flash; Low for 3.3 V flash
       * - **13**
         - DAT3
         - WPU
         -
