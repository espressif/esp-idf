.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

The {IDF_TARGET_NAME} chip features 34 physical GPIO pins (GPIO0 ~ GPIO19, GPIO21 ~ GPIO23, GPIO25 ~ GPIO27, and GPIO32 ~ GPIO39). Each pin can be used as a general-purpose I/O, or be connected to an internal peripheral signal. Through IO MUX, RTC IO MUX and the GPIO matrix, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

The table below provides more information on pin usage, and please note the comments in the table  for GPIOs with restrictions.

.. list-table::
    :header-rows: 1
    :widths: 8 12 12 20

    * - GPIO
      - Analog Function
      - RTC GPIO
      - Comments

    * - GPIO0
      - ADC2_CH1
      - RTC_GPIO11
      - Strapping pin

    * - GPIO1
      -
      -
      - TXD

    * - GPIO2
      - ADC2_CH2
      - RTC_GPIO12
      - Strapping pin

    * - GPIO3
      -
      -
      - RXD

    * - GPIO4
      - ADC2_CH0
      - RTC_GPIO10
      -

    * - GPIO5
      -
      -
      - Strapping pin

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
      - ADC2_CH5
      - RTC_GPIO15
      - Strapping pin; JTAG

    * - GPIO13
      - ADC2_CH4
      - RTC_GPIO14
      - JTAG

    * - GPIO14
      - ADC2_CH6
      - RTC_GPIO16
      - JTAG

    * - GPIO15
      - ADC2_CH3
      - RTC_GPIO13
      - Strapping pin; JTAG

    * - GPIO16
      -
      -
      - SPI0/1

    * - GPIO17
      -
      -
      - SPI0/1

    * - GPIO18
      -
      -
      -

    * - GPIO19
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

    * - GPIO25
      - ADC2_CH8
      - RTC_GPIO6
      -

    * - GPIO26
      - ADC2_CH9
      - RTC_GPIO7
      -

    * - GPIO27
      - ADC2_CH7
      - RTC_GPIO17
      -

    * - GPIO32
      - ADC1_CH4
      - RTC_GPIO9
      -

    * - GPIO33
      - ADC1_CH5
      - RTC_GPIO8
      -

    * - GPIO34
      - ADC1_CH6
      - RTC_GPIO4
      - GPI

    * - GPIO35
      - ADC1_CH7
      - RTC_GPIO5
      - GPI

    * - GPIO36
      - ADC1_CH0
      - RTC_GPIO0
      - GPI

    * - GPIO37
      - ADC1_CH1
      - RTC_GPIO1
      - GPI

    * - GPIO38
      - ADC1_CH2
      - RTC_GPIO2
      - GPI

    * - GPIO39
      - ADC1_CH3
      - RTC_GPIO3
      - GPI

.. note::

    - Strapping pin: GPIO0, GPIO2, GPIO5, GPIO12 (MTDI), and GPIO15 (MTDO) are strapping pins. For more information, please refer to `ESP32 datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`_.
    - SPI0/1: GPIO6-11 and GPIO16-17 are usually connected to the SPI flash and PSRAM integrated on the module and therefore should not be used for other purposes.
    - JTAG: GPIO12-15 are usually used for inline debug.
    - GPI: GPIO34-39 can only be set as input mode and do not have software-enabled pullup or pulldown functions.
    - TXD & RXD are usually used for flashing and debugging.
    - ADC2: ADC2 pins cannot be used when Wi-Fi is used. So, if you are having trouble getting the value from an ADC2 GPIO while using Wi-Fi, you may consider using an ADC1 GPIO instead, which should solve your problem. For more details, please refer to :ref:`Hardware Limitations of ADC Continuous Mode <hardware_limitations_adc_continuous>` and :ref:`Hardware Limitations of ADC Oneshot Mode <hardware_limitations_adc_oneshot>`.
    - Please do not use the interrupt of GPIO36 and GPIO39 when using ADC or Wi-Fi and Bluetooth with sleep mode enabled. Please refer to `ESP32 ECO and Workarounds for Bugs <https://espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf>`_ > Section 3.11 for the detailed description of the issue.

---
