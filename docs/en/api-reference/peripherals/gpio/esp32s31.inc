.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 60 physical GPIO pins (GPIO0 ~ GPIO61, except GPIO29, GPIO41). Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

The table below provides more information on pin usage, and please note the comments in the table for GPIOs with restrictions.

.. list-table::
    :header-rows: 1
    :widths: 8 12 12 20

    * - GPIO
      - Analog Function
      - LP GPIO
      - Comment

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
      - DAC0
      - LP_GPIO4
      -

    * - GPIO5
      - DAC1
      - LP_GPIO5
      -

    * - GPIO6
      - TOUCH0
      - LP_GPIO6
      -

    * - GPIO7
      - TOUCH1
      - LP_GPIO7
      -

    * - GPIO8
      - TOUCH2
      -
      -

    * - GPIO9
      - TOUCH3
      -
      -

    * - GPIO10
      - TOUCH4
      -
      -

    * - GPIO11
      - TOUCH5
      -
      -

    * - GPIO12
      - TOUCH6
      -
      -

    * - GPIO13
      - TOUCH7
      -
      -

    * - GPIO14
      - TOUCH8
      -
      -

    * - GPIO15
      - TOUCH9
      -
      -

    * - GPIO16
      - TOUCH10
      -
      -

    * - GPIO17
      - TOUCH11
      -
      -

    * - GPIO18
      - TOUCH12
      -
      -

    * - GPIO19
      - TOUCH13
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
      - SPI0/1

    * - GPIO27
      -
      -
      - SPI0/1

    * - GPIO28
      -
      -
      - SPI0/1

    * - GPIO30
      -
      -
      - SPI0/1

    * - GPIO31
      -
      -
      - SPI0/1

    * - GPIO32
      -
      -
      - SPI0/1

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
      - Strapping pin

    * - GPIO39
      -
      -
      - Strapping pin

    * - GPIO40
      -
      -
      - Strapping pin

    * - GPIO42
      - ADC1_CH0_N
      -
      -

    * - GPIO43
      - ADC1_CH0_P
      -
      -

    * - GPIO44
      - ADC1_CH1_N
      -
      -

    * - GPIO45
      - ADC1_CH1_P
      -
      -

    * - GPIO46
      - ADC1_CH2_N
      -
      -

    * - GPIO47
      - ADC1_CH2_P
      -
      -

    * - GPIO48
      - ADC1_CH3_N
      -
      -

    * - GPIO49
      - ADC1_CH3_P
      -
      -

    * - GPIO50
      - ADC2_CH0_N
      -
      -

    * - GPIO51
      - ADC2_CH0_P
      -
      -

    * - GPIO52
      - ADC2_CH1_N
      -
      -

    * - GPIO53
      - ADC2_CH1_P
      -
      -

    * - GPIO54
      - ADC2_CH2_N
      -
      -

    * - GPIO55
      - ADC2_CH2_P
      -
      -

    * - GPIO56
      - ADC2_CH3_N
      -
      -

    * - GPIO57
      - ADC2_CH3_P
      -
      -

    * - GPIO58
      -
      -
      - UART0

    * - GPIO59
      -
      -
      - UART0

    * - GPIO60
      -
      -
      - Strapping pin

    * - GPIO61
      -
      -
      - Strapping pin

.. note::

    - Strapping pin: GPIO36 ~ GPIO40, GPIO60 ~ GPIO61 are strapping pins. For more information, please refer to `{IDF_TARGET_NAME} datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`_.
    - SPI0/1: GPIO26 ~ GPIO32 are usually used for SPI flash, they're not recommended for other uses.

---
