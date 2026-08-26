.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 21 physical GPIO pins (GPIO0 ~ GPIO20). For chip variants with an SiP flash built in, GPIO11 ~ GPIO17 are dedicated to connecting the SiP flash; therefore, only 14 GPIO pins are available.

Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

The table below provides more information on pin usage, and please note the comments in the table for GPIOs with restrictions.

.. list-table::
    :header-rows: 1
    :widths: 12 12 22

    * - GPIO
      - Analog Function
      - Comments

    * - GPIO0
      - ADC1_CH0
      - RTC

    * - GPIO1
      - ADC1_CH1
      - RTC

    * - GPIO2
      - ADC1_CH2
      - RTC

    * - GPIO3
      - ADC1_CH3
      - RTC

    * - GPIO4
      - ADC1_CH4
      - RTC

    * - GPIO5
      -
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
      -

    * - GPIO19
      -
      -

    * - GPIO20
      -
      -

.. note::

    - Strapping pin: GPIO8 and GPIO9 are strapping pins. For more information, please refer to `ESP8684 datasheet <https://www.espressif.com/sites/default/files/documentation/esp8684_datasheet_en.pdf>`_.
    - SPI0/1: GPIO12-17 are usually used for SPI flash and not recommended for other uses.
    - RTC: GPIO0-5 can be used to wake up the chip from Deep-sleep mode. Other GPIOs can only wake up the chip from Light-sleep mode. For more information, please refer to Section :ref:`Wakeup Sources<api-reference-wakeup-source>`.

---
