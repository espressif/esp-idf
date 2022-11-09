GPIO & RTC GPIO
===============

:link_to_translation:`zh_CN:[中文]`

GPIO Summary
------------

.. include:: gpio/{IDF_TARGET_PATH_NAME}.inc
    :start-after: gpio-summary
    :end-before: ---


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    There is also separate "RTC GPIO" support, which functions when GPIOs are routed to the "RTC" low-power and analog subsystem. These pin functions can be used when:

    .. list::

        - In Deep-sleep mode
        :SOC_ULP_SUPPORTED: - The :doc:`Ultra Low Power co-processor <../../api-reference/system/ulp>` is running
        - Analog functions such as ADC/DAC/etc are in use.

Application Example
-------------------

* GPIO output and input interrupt example: :example:`peripherals/gpio/generic_gpio`.

API Reference - Normal GPIO
---------------------------

.. include-build-file:: inc/gpio.inc
.. include-build-file:: inc/gpio_types.inc


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    API Reference - RTC GPIO
    ------------------------

    .. include-build-file:: inc/rtc_io.inc
    .. include-build-file:: inc/rtc_io_types.inc
