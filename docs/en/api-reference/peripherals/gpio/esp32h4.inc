.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 40 physical GPIO pins (GPIO0 ~ GPIO39). Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

The table below provides more information on pin usage, and please note the comments in the table for GPIOs with restrictions.

.. list-table::
    :header-rows: 1
    :widths: 8 12 12 20

    * - GPIO
      - Analog Function
      - LP GPIO
      - Comments

    * - GPIO0
      -
      - LP_GPIO0
      -

    * - GPIO1
      -
      - LP_GPIO1
      -

    * - GPIO2
      -
      - LP_GPIO2
      -

    * - GPIO3
      -
      - LP_GPIO3
      -

    * - GPIO4
      -
      - LP_GPIO4
      -

    * - GPIO5
      -
      - LP_GPIO5
      -

    * - GPIO6
      -
      -
      - SPI0/1

    * - GPIO7
      -
      -
      - SPI0/1

    * - GPIO8
      -
      -
      - SPI0/1

    * - GPIO9
      -
      -
      - SPI0/1

    * - GPIO10
      -
      -
      - SPI0/1

    * - GPIO11
      -
      -
      - SPI0/1

    * - GPIO12
      -
      -
      - SPI0/1

    * - GPIO13
      -
      -
      - USB-JTAG

    * - GPIO14
      -
      -
      - USB-JTAG

    * - GPIO15
      -
      -
      -

    * - GPIO16
      -
      -
      -

    * - GPIO17
      -
      -
      - Strapping pin

    * - GPIO18
      -
      -
      -

    * - GPIO19
      -
      -
      -

    * - GPIO20
      -
      -
      -

    * - GPIO21
      -
      -
      -

    * - GPIO22
      -
      -
      -

    * - GPIO23
      -
      -
      -

    * - GPIO24
      -
      -
      -

    * - GPIO25
      -
      -
      -

    * - GPIO26
      -
      -
      -

    * - GPIO27
      -
      -
      -

    * - GPIO28
      - ADC1_CH0
      -
      -

    * - GPIO29
      - ADC1_CH1
      -
      -

    * - GPIO30
      - ADC1_CH2
      -
      -

    * - GPIO31
      - ADC1_CH3
      -
      -

    * - GPIO32
      - ADC1_CH4
      -
      -

    * - GPIO33
      -
      -
      -

    * - GPIO34
      -
      -
      -

    * - GPIO35
      -
      -
      -

    * - GPIO36
      -
      -
      - Strapping pin

    * - GPIO37
      -
      -
      - Strapping pin

    * - GPIO38
      -
      -
      -

    * - GPIO39
      -
      -
      -

.. note::

    - Strapping pin: GPIO17, GPIO36, and GPIO37 are strapping pins. For more information, please refer to `datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`__.
	  - SPI0/1: GPIO6 ~ GPIO12 are usually used for SPI flash and PSRAM. These pins are not recommended for other uses.
	  - USB-JTAG: GPIO13 and GPIO14 are used by USB-JTAG by default. If they are reconfigured to operate as normal GPIOs, USB-JTAG functionality will be disabled.

---
