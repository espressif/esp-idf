.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 22 physical GPIO pins (GPIO0 ~ GPIO21). Each pin can be used as a general-purpose I/O, or be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

The table below provides more information on pin usage, and please note the comments in the table for GPIOs with restrictions.

.. list-table::
    :header-rows: 1
    :widths: 12 12 22

    * - GPIO
      - Analog Function
      - Comment

    * - GPIO0
      - ADC1_CH0
      - RTC

    * - GPIO1
      - ADC1_CH1
      - RTC

    * - GPIO2
      - ADC1_CH2
      - Strapping pin；RTC

    * - GPIO3
      - ADC1_CH3
      - RTC

    * - GPIO4
      - ADC1_CH4
      - RTC

    * - GPIO5
      - ADC2_CH0
      - RTC

    * - GPIO6
      -
      -

    * - GPIO7
      -
      -

    * - GPIO8
      -
      - Strapping pin

    * - GPIO9
      -
      - Strapping pin

    * - GPIO10
      -
      -

    * - GPIO11
      -
      -

    * - GPIO12
      -
      - SPI0/1

    * - GPIO13
      -
      - SPI0/1

    * - GPIO14
      -
      - SPI0/1

    * - GPIO15
      -
      - SPI0/1

    * - GPIO16
      -
      - SPI0/1

    * - GPIO17
      -
      - SPI0/1

    * - GPIO18
      -
      - USB-JTAG

    * - GPIO19
      -
      - USB-JTAG

    * - GPIO20
      -
      -

    * - GPIO21
      -
      -

.. note::

    - Strapping pin: GPIO2, GPIO8 and GPIO9 are strapping pins. For more information, please refer to `ESP32-C3 Datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`_.
    - SPI0/1: GPIO12 ~ GPIO17 are usually used for SPI flash and PSRAM and are not recommended for other uses.
    - USB-JTAG: GPIO18 and GPIO19 are used by USB-JTAG by default. If they are reconfigured to operate as normal GPIOs, USB-JTAG functionality will be disabled.
    - RTC: GPIO0 ~ GPIO5 can be used to wake up the chip from Deep-sleep mode. Other GPIOs can only wake up the chip from Light-sleep mode. For more information, please refer to Section :ref:`Wakeup Sources<api-reference-wakeup-source>`.

---
