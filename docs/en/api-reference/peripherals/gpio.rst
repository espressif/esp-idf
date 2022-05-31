GPIO & RTC GPIO
===============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

.. only:: esp32

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

        - Strapping pin: GPIO0, GPIO2, GPIO5, GPIO12 (MTDI), and GPIO15 (MTDO) are strapping pins. For more infomation, please refer to `ESP32 datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_.
        - SPI0/1: GPIO6-11 and GPIO16-17 are usually connected to the SPI flash and PSRAM integrated on the module and therefore should not be used for other purposes.
        - JTAG: GPIO12-15 are usually used for inline debug.
        - GPI: GPIO34-39 can only be set as input mode and do not have software-enabled pullup or pulldown functions.
        - TXD & RXD are usually used for flashing and debugging.
        - ADC2: ADC2 pins cannot be used when Wi-Fi is used. So, if you are having trouble getting the value from an ADC2 GPIO while using Wi-Fi, you may consider using an ADC1 GPIO instead, which should solve your problem. For more details, please refer to `ADC limitations <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html#adc-limitations>`_.


.. only:: esp32s2

    The {IDF_TARGET_NAME} chip features 43 physical GPIO pins (GPIO0 ~ GPIO21 and GPIO26 ~ GPIO46). Each pin can be used as a general-purpose I/O, or be connected to an internal peripheral signal. Through IO MUX, RTC IO MUX and the GPIO matrix, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

    The table below provides more information on pin usage, and please note the comments in the table for GPIOs with restrictions.

    .. list-table::
       :header-rows: 1
       :widths: 8 12 12 20

       * - GPIO
         - Analog Function
         - RTC GPIO
         - Comment

       * - GPIO0
         -
         - RTC_GPIO0
         - Strapping pin

       * - GPIO1
         - ADC1_CH0
         - RTC_GPIO1
         -

       * - GPIO2
         - ADC1_CH1
         - RTC_GPIO2
         -

       * - GPIO3
         - ADC1_CH2
         - RTC_GPIO3
         -

       * - GPIO4
         - ADC1_CH3
         - RTC_GPIO4
         -

       * - GPIO5
         - ADC1_CH4
         - RTC_GPIO5
         -

       * - GPIO6
         - ADC1_CH5
         - RTC_GPIO6
         -

       * - GPIO7
         - ADC1_CH6
         - RTC_GPIO7
         -

       * - GPIO8
         - ADC1_CH7
         - RTC_GPIO8
         -

       * - GPIO9
         - ADC1_CH8
         - RTC_GPIO9
         -

       * - GPIO10
         - ADC1_CH9
         - RTC_GPIO10
         -

       * - GPIO11
         - ADC2_CH0
         - RTC_GPIO11
         -

       * - GPIO12
         - ADC2_CH1
         - RTC_GPIO12
         -

       * - GPIO13
         - ADC2_CH2
         - RTC_GPIO13
         -

       * - GPIO14
         - ADC2_CH3
         - RTC_GPIO14
         -

       * - GPIO15
         - ADC2_CH4
         - RTC_GPIO15
         -

       * - GPIO16
         - ADC2_CH5
         - RTC_GPIO16
         -

       * - GPIO17
         - ADC2_CH6
         - RTC_GPIO17
         -

       * - GPIO18
         - ADC2_CH7
         - RTC_GPIO18
         -

       * - GPIO19
         - ADC2_CH8
         - RTC_GPIO19
         -

       * - GPIO20
         - ADC2_CH9
         - RTC_GPIO20
         -

       * - GPIO21
         -
         - RTC_GPIO21
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

       * - GPIO29
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
         -

       * - GPIO37
         -
         -
         -

       * - GPIO38
         -
         -
         -

       * - GPIO39
         -
         -
         - JTAG

       * - GPIO40
         -
         -
         - JTAG

       * - GPIO41
         -
         -
         - JTAG

       * - GPIO42
         -
         -
         - JTAG

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
         - Strapping pin

       * - GPIO46
         -
         -
         - GPI；Strapping pin

    .. note::

         - Strapping pin: GPIO0, GPIO45 and GPIO46 are strapping pins. For more infomation, please refer to `ESP32-S2 datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf>`_
         - SPI0/1: GPIO26-32 are usually used for SPI flash and PSRAM and not recommended for other uses.
         - JTAG: GPIO39-42 are usually used for inline debug.
         - GPI: GPIO46 is fixed to pull-down and is input only.


.. only:: esp32c3

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

        - Strapping pin: GPIO2, GPIO8 and GPIO9 are strapping pins. For more infomation, please refer to `ESP32-C3 datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf>`_.
        - SPI0/1: GPIO12-17 are usually used for SPI flash and PSRAM and are not recommended for other uses.
        - USB-JTAG: GPIO 18 and 19 are used by USB-JTAG by default. In order to use them as GPIOs, USB-JTAG will be disabled by the drivers.
        - RTC: GPIO0-5 can be used when in Deep-sleep mode.


.. only:: esp32s3

    The {IDF_TARGET_NAME} chip features 45 physical GPIO pins (GPIO0 ~ GPIO21 and GPIO26 ~ GPIO48). Each pin can be used as a general-purpose I/O, or be connected to an internal peripheral signal. Through GPIO matrix, IO MUX, and RTC IO MUX, peripheral input signals can be from any GPIO pin, and peripheral output signals can be routed to any GPIO pin. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

    The table below provides more information on pin usage, and please note the comments in the table for GPIOs with restrictions.

    .. list-table::
       :header-rows: 1
       :widths: 8 12 12 20

       * - GPIO
         - Analog Function
         - RTC GPIO
         - Comment

       * - GPIO0
         -
         - RTC_GPIO0
         - Strapping pin

       * - GPIO1
         - ADC1_CH0
         - RTC_GPIO1
         -

       * - GPIO2
         - ADC1_CH1
         - RTC_GPIO2
         -

       * - GPIO3
         - ADC1_CH2
         - RTC_GPIO3
         - Strapping pin

       * - GPIO4
         - ADC1_CH3
         - RTC_GPIO4
         -

       * - GPIO5
         - ADC1_CH4
         - RTC_GPIO5
         -

       * - GPIO6
         - ADC1_CH5
         - RTC_GPIO6
         -

       * - GPIO7
         - ADC1_CH6
         - RTC_GPIO7
         -

       * - GPIO8
         - ADC1_CH7
         - RTC_GPIO8
         -

       * - GPIO9
         - ADC1_CH8
         - RTC_GPIO9
         -

       * - GPIO10
         - ADC1_CH9
         - RTC_GPIO10
         -

       * - GPIO11
         - ADC2_CH0
         - RTC_GPIO11
         -

       * - GPIO12
         - ADC2_CH1
         - RTC_GPIO12
         -

       * - GPIO13
         - ADC2_CH2
         - RTC_GPIO13
         -

       * - GPIO14
         - ADC2_CH3
         - RTC_GPIO14
         -

       * - GPIO15
         - ADC2_CH4
         - RTC_GPIO15
         -

       * - GPIO16
         - ADC2_CH5
         - RTC_GPIO16
         -

       * - GPIO17
         - ADC2_CH6
         - RTC_GPIO17
         -

       * - GPIO18
         - ADC2_CH7
         - RTC_GPIO18
         -

       * - GPIO19
         - ADC2_CH8
         - RTC_GPIO19
         - USB-JTAG

       * - GPIO20
         - ADC2_CH9
         - RTC_GPIO20
         - USB-JTAG

       * - GPIO21
         -
         - RTC_GPIO21
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

       * - GPIO29
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
         - SPI0/1

       * - GPIO34
         -
         -
         - SPI0/1

       * - GPIO35
         -
         -
         - SPI0/1

       * - GPIO36
         -
         -
         - SPI0/1

       * - GPIO37
         -
         -
         - SPI0/1

       * - GPIO38
         -
         -
         -

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
         - Strapping pin

       * - GPIO46
         -
         -
         - Strapping pin

       * - GPIO47
         -
         -
         -

       * - GPIO48
         -
         -
         -

    .. Note::

        - Strapping pin: GPIO0, GPIO3, GPIO45 and GPIO46 are strapping pins. For more infomation, please refer to `ESP32-S3 datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf>`_.
        - SPI0/1: GPIO26-32 are usually used for SPI flash and PSRAM and not recommended for other uses. When using Octal Flash or Octal PSRAM or both, GPIO33~37 are connected to SPIIO4 ~ SPIIO7 and SPIDQS. Therefore, on boards embedded with ESP32-S3R8 / ESP32-S3R8V chip, GPIO33~37 are also not recommended for other uses.
        - USB-JTAG: GPIO 19 and 20 are used by USB-JTAG by default. In order to use them as GPIOs, USB-JTAG will be disabled by the drivers.

.. only:: esp32c2

    The {IDF_TARGET_NAME} chip features 21 physical GPIO pins (GPIO0 ~ GPIO20). For chip variants with an SiP flash built in, GPIO11 ~ GPIO17 are dedicated to connecting the SiP flash; therefore, only 14 GPIO pins are available.

    Each pin can be used as a general-purpose I/O, or to be connected to an internal peripheral signal. Through GPIO matrix and IO MUX, peripheral input signals can be from any IO pins, and peripheral output signals can be routed to any IO pins. Together these modules provide highly configurable I/O. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

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

        - Strapping pin: GPIO8 and GPIO9 are strapping pins. For more infomation, please refer to `ESP8684 datasheet <https://www.espressif.com/sites/default/files/documentation/esp8684_datasheet_en.pdf>`_.
        - SPI0/1: GPIO12-17 are usually used for SPI flash and not recommended for other uses.
        - RTC: GPIO0-5 can be used when in Deep-sleep mode.


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    There is also separate "RTC GPIO" support, which functions when GPIOs are routed to the "RTC" low-power and analog subsystem. These pin functions can be used when:

    .. list::

        - In Deep-sleep mode
        :SOC_ULP_SUPPORTED: - The :doc:`Ultra Low Power co-processor <../../api-reference/system/ulp>` is running
        - Analog functions such as ADC/DAC/etc are in use.

Application Example
-------------------

GPIO output and input interrupt example: :example:`peripherals/gpio/generic_gpio`.

API Reference - Normal GPIO
---------------------------

.. include-build-file:: inc/gpio.inc
.. include-build-file:: inc/gpio_types.inc


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    API Reference - RTC GPIO
    ------------------------

    .. include-build-file:: inc/rtc_io.inc
    .. include-build-file:: inc/rtc_io_types.inc
