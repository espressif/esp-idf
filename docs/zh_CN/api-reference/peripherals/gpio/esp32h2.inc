.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features X physical GPIO pins (GPIOX ~ GPIOX). Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

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

---
