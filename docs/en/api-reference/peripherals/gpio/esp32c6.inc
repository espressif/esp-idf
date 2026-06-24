.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 31 physical GPIO pins (GPIO0 ~ GPIO30). Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

The table below provides more information on pin usage, and please note the comments in the table for GPIOs with restrictions.

.. list-table::
    :header-rows: 1
    :widths: 8 12 12 20

    * - GPIO
      - Analog Function
      - LP GPIO
      - Comments

    * - GPIO0
      - ADC1_CH0
      - LP_GPIO0
      -

    * - GPIO1
      - ADC1_CH1
      - LP_GPIO1
      -

    * - GPIO2
      - ADC1_CH2
      - LP_GPIO2
      -

    * - GPIO3
      - ADC1_CH3
      - LP_GPIO3
      -

    * - GPIO4
      - ADC1_CH4
      - LP_GPIO4
      - Strapping pin

    * - GPIO5
      - ADC1_CH5
      - LP_GPIO5
      - Strapping pin

    * - GPIO6
      - ADC1_CH6
      - LP_GPIO6
      -

    * - GPIO7
      -
      - LP_GPIO7
      -

    * - GPIO8
      -
      -
      - Strapping pin

    * - GPIO9
      -
      -
      - Strapping pin

    * - GPIO10
      -
      -
      -

    * - GPIO11
      -
      -
      -

    * - GPIO12
      -
      -
      - USB-JTAG

    * - GPIO13
      -
      -
      - USB-JTAG

    * - GPIO14
      -
      -
      -

    * - GPIO15
      -
      -
      - Strapping pin

    * - GPIO16
      -
      -
      -

    * - GPIO17
      -
      -
      -

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
      - SPI0/1

    * - GPIO25
      -
      -
      - SPI0/1

    * - GPIO26
      -
      -
      - SPI0/1

    * - GPIO27
      -
      -
      - SPI0/1

    * - GPIO28
      -
      -
      - SPI0/1

    * - GPIO29
      -
      -
      - SPI0/1

    * - GPIO30
      -
      -
      - SPI0/1

.. note::

    - Strapping pin: GPIO4, GPIO5, GPIO8, GPIO9, and GPIO15 are strapping pins. For more information, please refer to `datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`__.
	  - SPI0/1: GPIO24 ~ GPIO30 are usually used for SPI flash and not recommended for other uses.
	  - USB-JTAG: GPIO12 and GPIO13 are used by USB-JTAG by default. If they are reconfigured to operate as normal GPIOs, USB-JTAG functionality will be disabled.
    - For chip variants with an SiP flash built in, GPIO24 ~ GPIO30 are dedicated to connecting the SiP flash; GPIO10 ~ GPIO11 are not led out to any chip pins; therefore, only the remaining 22 GPIO pins are available.
    - For chip variants without an in-package flash, GPIO14 is not led out to any chip pins.

---
