.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 28 physical GPIO pins (GPIO0 ~ GPIO27). Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

The table below provides more information on pin usage, and please note the comments in the table for GPIOs with restrictions.

.. list-table::
    :header-rows: 1
    :widths: 8 12 20

    * - GPIO
      - Analog Function
      - Comments

    * - GPIO0
      -
      -

    * - GPIO1
      - ADC1_CH0
      -

    * - GPIO2
      - ADC1_CH1
      - Strapping pin

    * - GPIO3
      - ADC1_CH2
      - Strapping pin

    * - GPIO4
      - ADC1_CH3
      -

    * - GPIO5
      - ADC1_CH4
      -

    * - GPIO6
      -
      -

    * - GPIO7
      -
      - RTC

    * - GPIO8
      -
      - Strapping pin, RTC

    * - GPIO9
      -
      - Strapping pin, RTC

    * - GPIO10
      - ANA_CMPR_CH0 reference voltage
      - RTC

    * - GPIO11
      - ANA_CMPR_CH0 input (non-inverting)
      - RTC

    * - GPIO12
      -
      - RTC

    * - GPIO13
      -
      - RTC

    * - GPIO14
      -
      - RTC

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
      - SPI0/1

    * - GPIO19
      -
      - SPI0/1

    * - GPIO20
      -
      - SPI0/1

    * - GPIO21
      -
      - SPI0/1

    * - GPIO22
      -
      -

    * - GPIO23
      -
      -

    * - GPIO24
      -
      -

    * - GPIO25
      -
      - Strapping pin

    * - GPIO26
      -
      - USB-JTAG

    * - GPIO27
      -
      - USB-JTAG

.. note::

    - Strapping pin: GPIO2, GPIO3, GPIO8, GPIO9, and GPIO25 are strapping pins. For more information, please refer to `ESP32H2 datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`_.
	  - SPI0/1: GPIO15-21 are usually used for SPI flash and not recommended for other uses.
	  - USB-Serial-JTAG: GPIO 26 and 27 are used by USB-Serial-JTAG by default. In order to use them as GPIOs, USB-Serial-JTAG will be disabled by the drivers.
    - For chip variants with an SiP flash built in, GPIO15 ~ GPIO21 are dedicated to connecting the SiP flash and are not fan-out to the external pins. In addition, GPIO6 ~ GPIO7 are also not fan-out to the external pins. In conclusion, only GPIO0~ GPIO5, GPIO8~ GPIO14, GPIO22~ GPIO27 are available to users.
    - RTC: GPIO7-14 can be used to wake up the chip from Deep-sleep mode. Other GPIOs can only wake up the chip from Light-sleep mode. For more information, please refer to Section :ref:`Wakeup Sources<api-reference-wakeup-source>`.

---
