Deep Sleep
==========

Overview
--------

ESP32 is capable of deep sleep power saving mode. In this mode CPUs, most of the RAM, and all the digital peripherals which are clocked from APB_CLK are powered off. The only parts of the chip which can still be powered on are: RTC controller, RTC peripherals (including ULP coprocessor), and RTC memories (slow and fast).

Wakeup from deep sleep mode can be done using several sources. These sources can be combined, in this case the chip will wake up when any one of the sources is triggered. Wakeup sources can be enabled using ``esp_deep_sleep_enable_X_wakeup`` APIs. Next section describes these APIs in detail. Wakeup sources can be configured at any moment before entering deep sleep mode. 

Additionally, the application can force specific powerdown modes for the RTC peripherals and RTC memories using ``esp_deep_sleep_pd_config`` API.

Once wakeup sources are configured, application can start deep sleep using ``esp_deep_sleep_start`` API. At this point the hardware will be configured according to the requested wakeup sources, and RTC controller will power down the CPUs and digital peripherals.

Wakeup sources
--------------

Timer
^^^^^

RTC controller has a built in timer which can be used to wake up the chip after a predefined amount of time. Time is specified at microsecond precision, but the actual resolution depends on the clock source selected for RTC SLOW_CLK. See chapter "Reset and Clock" of the ESP32 Technical Reference Manual for details about RTC clock options. 

This wakeup mode doesn't require RTC peripherals or RTC memories to be powered on during deep sleep.

The following function can be used to enable deep sleep wakeup using a timer.

.. doxygenfunction:: esp_deep_sleep_enable_timer_wakeup


External wakeup (ext0)
^^^^^^^^^^^^^^^^^^^^^^

RTC IO module contains logic to trigger wakeup when one of RTC GPIOs is set to a predefined logic level. RTC IO is part of RTC peripherals power domain, so RTC peripherals will be kept powered on during deep sleep if this wakeup source is requested. 

Because RTC IO module is enabled in this mode, internal pullup or pulldown resistors can also be used. They need to be configured by the application using ``rtc_gpio_pullup_en`` and ``rtc_gpio_pulldown_en`` functions, before calling ``esp_deep_sleep_start``.

.. doxygenfunction:: esp_deep_sleep_enable_ext0_wakeup

External wakeup (ext1)
^^^^^^^^^^^^^^^^^^^^^^

RTC controller contains logic to trigger wakeup using multiple RTC GPIOs. One of the two logic functions can be used to trigger wakeup:

    - wake up if any of the selected pins is low
    - wake up if all the selected pins are high

This wakeup source is implemented by the RTC controller. As such, RTC peripherals and RTC memories can be powered off in this mode. However, if RTC peripherals are powered down, internal pullup and pulldown resistors will be disabled. To use internal pullup or pulldown resistors, request RTC peripherals power domain to be kept on during deep sleep, and configure pullup/pulldown resistors using ``rtc_gpio_`` functions, before entering deep sleep::

    esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    gpio_pullup_dis(gpio_num);
    gpio_pulldown_en(gpio_num);
    
The following function can be used to enable this wakeup mode:

.. doxygenfunction:: esp_deep_sleep_enable_ext1_wakeup

.. doxygenenum:: esp_ext1_wakeup_mode_t


ULP coprocessor wakeup
^^^^^^^^^^^^^^^^^^^^^^

ULP coprocessor can run while the chip is in deep sleep, and may be used to poll sensors, monitor ADC or touch sensor values, and wake up the chip when a specific event is detected. ULP coprocessor is part of RTC peripherals power domain, and it runs the program stored in RTC slow memeory. Therefore RTC peripherals and RTC slow memory will be powered on during deep sleep if this wakeup mode is requested.

The following function can be used to enable this wakeup mode:

.. doxygenfunction:: esp_deep_sleep_enable_ulp_wakeup

Power-down of RTC peripherals and memories
------------------------------------------

By default, ``esp_deep_sleep_start`` function will power down all RTC power domains which are not needed by the enabled wakeup sources. To override this behaviour, the following function is provided:

Note: on the first revision of the ESP32, RTC fast memory will always be kept enabled in deep sleep, so that the deep sleep stub can run after reset. This can be overriden, if the application doesn't need clean reset behaviour after deep sleep.

.. doxygenfunction:: esp_deep_sleep_pd_config
.. doxygenenum:: esp_deep_sleep_pd_domain_t
.. doxygenenum:: esp_deep_sleep_pd_option_t


Entering deep sleep
-------------------

The following function can be used to enter deep sleep once wakeup sources are configured. It is also possible to go into deep sleep with no wakeup sources configured, in this case the chip will be in deep sleep mode indefinetly, until external reset is applied.

.. doxygenfunction:: esp_deep_sleep_start

