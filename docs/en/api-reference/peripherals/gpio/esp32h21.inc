.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 26 physical GPIO pins (GPIO0 ~ GPIO25). Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

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
      -
      -

    * - GPIO1
      - ADC1_CH0
      -
      -

    * - GPIO2
      - ADC1_CH1
      -
      -

    * - GPIO3
      - ADC1_CH2
      -
      -

    * - GPIO4
      - ADC1_CH3
      -
      -

    * - GPIO5
      - ADC1_CH4
      - LP_GPIO0
      -

    * - GPIO6
      -
      - LP_GPIO1
      -

    * - GPIO7
      -
      - LP_GPIO2
      -

    * - GPIO8
      -
      - LP_GPIO3
      - Strapping pin

    * - GPIO9
      -
      - LP_GPIO4
      -

    * - GPIO10
      -
      - LP_GPIO5
      -

    * - GPIO11
      -
      - LP_GPIO6
      -

    * - GPIO12
      -
      -
      -

    * - GPIO13
      -
      -
      - Strapping pin

    * - GPIO14
      -
      -
      - Strapping pin

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
      - USB-JTAG

    * - GPIO18
      -
      -
      - USB-JTAG

    * - GPIO19
      -
      -
      - SPI0/1

    * - GPIO20
      -
      -
      - SPI0/1

    * - GPIO21
      -
      -
      - SPI0/1

    * - GPIO22
      -
      -
      - SPI0/1

    * - GPIO23
      -
      -
      - SPI0/1

    * - GPIO24
      -
      -
      - SPI0/1

    * - GPIO25
      -
      -
      - SPI0/1

.. note::

    - Strapping pin: GPIO8, GPIO13, and GPIO14 are strapping pins. For more information, please refer to `datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`__.
	  - SPI0/1: GPIO19 ~ GPIO25 are usually used for SPI flash and not recommended for other uses.
	  - USB-JTAG: GPIO17 and GPIO18 are used by USB-JTAG by default. If they are reconfigured to operate as normal GPIOs, USB-JTAG functionality will be disabled.

---
