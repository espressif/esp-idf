GPIO & RTC GPIO
===============

Overview
--------

.. only:: esp32

    The {IDF_TARGET_NAME} chip features 40 physical GPIO pads. Some GPIO pads cannot be used or do not have the corresponding pin on the chip package. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__]. Each pad can be used as a general purpose I/O or can be connected to an internal peripheral signal.

    - Note that GPIO6-11 are usually used for SPI flash.
    - GPIO34-39 can only be set as input mode and do not have software pullup or pulldown functions.

.. only:: esp32s2

    The {IDF_TARGET_NAME} chip features 43 physical GPIO pads. Some GPIO pads cannot be used or do not have the corresponding pin on the chip package. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__]. Each pad can be used as a general purpose I/O or can be connected to an internal peripheral signal.

    - Note that GPIO26-32 are usually used for SPI flash.
    - GPIO46 is fixed to pull-down and is input only

.. only:: esp32c3

    The {IDF_TARGET_NAME} chip features 22 physical GPIO pads. Some GPIO pads cannot be used or do not have the corresponding pin on the chip package. For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *IO MUX and GPIO Matrix (GPIO, IO_MUX)* [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__]. Each pad can be used as a general purpose I/O or can be connected to an internal peripheral signal.

    - Note that GPIO12-17 are usually used for SPI flash.
    - Note that GPIO 18 and 19 are used by USB-JTAG by default. In order to use them as GPIOs, USB-JTAG will be disabled by the drivers.
    - {IDF_TARGET_NAME} doesn't have separate "RTC GPIO" support. However, GPIO0-5 keep the rtc feature, which can be used for power-management and analog subsystem.

.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    There is also separate "RTC GPIO" support, which functions when GPIOs are routed to the "RTC" low-power and analog subsystem. These pin functions can be used when:

    .. list::

        - In deep sleep
        :SOC_ULP_SUPPORTED: - The :doc:`Ultra Low Power co-processor <../../api-guides/ulp>` is running
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


