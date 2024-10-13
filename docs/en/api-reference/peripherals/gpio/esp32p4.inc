.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 55 physical GPIO pins (GPIO0 ~ GPIO54). Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

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
      - TOUCH0
      - LP_GPIO2
      -

    * - GPIO3
      - TOUCH1
      - LP_GPIO3
      -

    * - GPIO4
      - TOUCH2
      - LP_GPIO4
      -

    * - GPIO5
      - TOUCH3
      - LP_GPIO5
      -

    * - GPIO6
      - TOUCH4
      - LP_GPIO6
      -

    * - GPIO7
      - TOUCH5
      - LP_GPIO7
      -

    * - GPIO8
      - TOUCH6
      - LP_GPIO8
      -

    * - GPIO9
      - TOUCH7
      - LP_GPIO9
      -

    * - GPIO10
      - TOUCH8
      - LP_GPIO10
      -

    * - GPIO11
      - TOUCH9
      - LP_GPIO11
      -

    * - GPIO12
      - TOUCH10
      - LP_GPIO12
      -

    * - GPIO13
      - TOUCH11
      - LP_GPIO13
      -

    * - GPIO14
      - TOUCH12
      - LP_GPIO14
      -

    * - GPIO15
      - TOUCH13
      - LP_GPIO15
      -

    * - GPIO16
      - ADC1_CH0
      -
      -

    * - GPIO17
      - ADC1_CH1
      -
      -

    * - GPIO18
      - ADC1_CH2
      -
      -

    * - GPIO19
      - ADC1_CH3
      -
      -

    * - GPIO20
      - ADC1_CH4
      -
      -

    * - GPIO21
      - ADC1_CH5
      -
      -

    * - GPIO22
      - ADC1_CH6
      -
      -

    * - GPIO23
      - ADC1_CH7
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
      -
      -
      -

    * - GPIO29
      -
      -
      -

    * - GPIO30
      -
      -
      -

    * - GPIO31
      -
      -
      -

    * - GPIO32
      -
      -
      -

    * - GPIO33
      -
      -
      -

    * - GPIO34
      -
      -
      - Strapping pin

    * - GPIO35
      -
      -
      - Strapping pin

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
      -

    * - GPIO40
      -
      -
      -

    * - GPIO41
      -
      -
      -

    * - GPIO42
      -
      -
      -

    * - GPIO43
      -
      -
      -

    * - GPIO44
      -
      -
      -

    * - GPIO45
      -
      -
      -

    * - GPIO46
      -
      -
      -

    * - GPIO47
      -
      -
      -

    * - GPIO48
      -
      -
      -

    * - GPIO49
      - ADC1_CH8
      -
      -

    * - GPIO50
      - ADC1_CH9
      -
      -

    * - GPIO51
      - ADC1_CH10, ANA_CMPR_CH0 reference voltage
      -
      -

    * - GPIO52
      - ADC1_CH11, ANA_CMPR_CH0 input (non-inverting)
      -
      -

    * - GPIO53
      - ADC1_CH12, ANA_CMPR_CH1 reference voltage
      -
      -

    * - GPIO54
      - ADC1_CH13, ANA_CMPR_CH1 input (non-inverting)
      -
      -

.. note::

    - Strapping pin: GPIO34, GPIO35, GPIO36, GPIO37, and GPIO38 are strapping pins. For more information, please refer to `datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`__.
	  - USB-JTAG: GPIO24 and GPIO25 are used by USB-JTAG by default. If they are reconfigured to operate as normal GPIOs, USB-JTAG functionality will be disabled.

---
