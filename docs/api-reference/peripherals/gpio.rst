GPIO & RTC GPIO
===============

Overview
--------

The ESP32 chip features 40 physical GPIO pads. Some GPIO pads cannot be used or do not have the corresponding pin on the chip package(refer to technical reference manual). Each pad can be used as a general purpose I/O or can be connected to an internal peripheral signal.

- Note that GPIO6-11 are usually used for SPI flash.
- GPIO34-39 can only be set as input mode and do not have software pullup or pulldown functions.

There is also separate "RTC GPIO" support, which functions when GPIOs are routed to the "RTC" low-power and analog subsystem. These pin functions can be used when in deep sleep, when the :doc:`Ultra Low Power co-processor <../../api-guides/ulp>` is running, or when analog functions such as ADC/DAC/etc are in use.

Application Example
-------------------

GPIO output and input interrupt example: :example:`peripherals/gpio`.

API Reference - Normal GPIO
---------------------------

.. include:: /_build/inc/gpio.inc

API Reference - RTC GPIO
------------------------

.. include:: /_build/inc/rtc_io.inc

