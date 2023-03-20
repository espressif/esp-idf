Sleep Modes
===========

Overview
--------

{IDF_TARGET_NAME} is capable of light sleep and deep sleep power saving modes.

In light sleep mode, digital peripherals, most of the RAM, and CPUs are clock-gated, and supply voltage is reduced. Upon exit from light sleep, peripherals and CPUs resume operation, their internal state is preserved.

In deep sleep mode, CPUs, most of the RAM, and all the digital peripherals which are clocked from APB_CLK are powered off. The only parts of the chip which can still be powered on are: RTC controller, RTC peripherals (including ULP coprocessor), and RTC memories (slow and fast).

Wakeup from deep and light sleep modes can be done using several sources. These sources can be combined, in this case the chip will wake up when any one of the sources is triggered. Wakeup sources can be enabled using ``esp_sleep_enable_X_wakeup`` APIs and can be disabled using :cpp:func:`esp_sleep_disable_wakeup_source` API. Next section describes these APIs in detail. Wakeup sources can be configured at any moment before entering light or deep sleep mode.

Additionally, the application can force specific powerdown modes for the RTC peripherals and RTC memories using :cpp:func:`esp_sleep_pd_config` API.

Once wakeup sources are configured, application can enter sleep mode using :cpp:func:`esp_light_sleep_start` or :cpp:func:`esp_deep_sleep_start` APIs. At this point the hardware will be configured according to the requested wakeup sources, and RTC controller will either power down or power off the CPUs and digital peripherals.

WiFi/BT and sleep modes
-----------------------

In deep sleep and light sleep modes, wireless peripherals are powered down. Before entering light sleep modes, applications must disable WiFi and BT using appropriate calls (:cpp:func:`esp_bluedroid_disable`, :cpp:func:`esp_bt_controller_disable`, :cpp:func:`esp_wifi_stop`). WiFi and BT connections will not be maintained in deep sleep or light sleep, even if these functions are not called.

If WiFi connection needs to be maintained, enable WiFi modem sleep, and enable automatic light sleep feature (see :doc:`Power Management APIs <power_management>`). This will allow the system to wake up from sleep automatically when required by WiFi driver, thereby maintaining connection to the AP.

Wakeup sources
--------------

Timer
^^^^^

RTC controller has a built in timer which can be used to wake up the chip after a predefined amount of time. Time is specified at microsecond precision, but the actual resolution depends on the clock source selected for RTC SLOW_CLK. See chapter "Reset and Clock" of the {IDF_TARGET_NAME} Technical Reference Manual for details about RTC clock options.

This wakeup mode doesn't require RTC peripherals or RTC memories to be powered on during sleep.

:cpp:func:`esp_sleep_enable_timer_wakeup` function can be used to enable deep sleep wakeup using a timer.

Touch pad
^^^^^^^^^

RTC IO module contains logic to trigger wakeup when a touch sensor interrupt occurs. You need to configure the touch pad interrupt before the chip starts deep sleep.

.. only:: esp32

    Revisions 0 and 1 of the ESP32 only support this wakeup mode when RTC peripherals are not forced to be powered on (i.e. ESP_PD_DOMAIN_RTC_PERIPH should be set to ESP_PD_OPTION_AUTO).

:cpp:func:`esp_sleep_enable_touchpad_wakeup` function can be used to enable this wakeup source.


External wakeup (ext0)
^^^^^^^^^^^^^^^^^^^^^^

RTC IO module contains logic to trigger wakeup when one of RTC GPIOs is set to a predefined logic level. RTC IO is part of RTC peripherals power domain, so RTC peripherals will be kept powered on during deep sleep if this wakeup source is requested.

Because RTC IO module is enabled in this mode, internal pullup or pulldown resistors can also be used. They need to be configured by the application using :cpp:func:`rtc_gpio_pullup_en` and :cpp:func:`rtc_gpio_pulldown_en` functions, before calling :cpp:func:`esp_sleep_start`.

.. only:: esp32

    In revisions 0 and 1 of the ESP32, this wakeup source is incompatible with ULP and touch wakeup sources.

:cpp:func:`esp_sleep_enable_ext0_wakeup` function can be used to enable this wakeup source.

.. warning:: After wake up from sleep, IO pad used for wakeup will be configured as RTC IO. Before using this pad as digital GPIO, reconfigure it using ``rtc_gpio_deinit(gpio_num)`` function.

External wakeup (ext1)
^^^^^^^^^^^^^^^^^^^^^^

RTC controller contains logic to trigger wakeup using multiple RTC GPIOs. One of the two logic functions can be used to trigger wakeup:

    - wake up if any of the selected pins is high (``ESP_EXT1_WAKEUP_ANY_HIGH``)
    - wake up if all the selected pins are low (``ESP_EXT1_WAKEUP_ALL_LOW``)

This wakeup source is implemented by the RTC controller. As such, RTC peripherals and RTC memories can be powered down in this mode. However, if RTC peripherals are powered down, internal pullup and pulldown resistors will be disabled. To use internal pullup or pulldown resistors, request RTC peripherals power domain to be kept on during sleep, and configure pullup/pulldown resistors using ``rtc_gpio_`` functions, before entering sleep::

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    gpio_pullup_dis(gpio_num);
    gpio_pulldown_en(gpio_num);

.. warning:: After wake up from sleep, IO pad(s) used for wakeup will be configured as RTC IO. Before using these pads as digital GPIOs, reconfigure them using ``rtc_gpio_deinit(gpio_num)`` function.

:cpp:func:`esp_sleep_enable_ext1_wakeup` function can be used to enable this wakeup source.

ULP coprocessor wakeup
^^^^^^^^^^^^^^^^^^^^^^

ULP coprocessor can run while the chip is in sleep mode, and may be used to poll sensors, monitor ADC or touch sensor values, and wake up the chip when a specific event is detected. ULP coprocessor is part of RTC peripherals power domain, and it runs the program stored in RTC slow memory. RTC slow memory will be powered on during sleep if this wakeup mode is requested. RTC peripherals will be automatically powered on before ULP coprocessor starts running the program; once the program stops running, RTC peripherals are automatically powered down again.

.. only:: esp32

    Revisions 0 and 1 of the ESP32 only support this wakeup mode when RTC peripherals are not forced to be powered on (i.e. ESP_PD_DOMAIN_RTC_PERIPH should be set to ESP_PD_OPTION_AUTO).

:cpp:func:`esp_sleep_enable_ulp_wakeup` function can be used to enable this wakeup source.

GPIO wakeup (light sleep only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In addition to EXT0 and EXT1 wakeup sources described above, one more method of wakeup from external inputs is available in light sleep mode. With this wakeup source, each pin can be individually configured to trigger wakeup on high or low level using :cpp:func:`gpio_wakeup_enable` function. Unlike EXT0 and EXT1 wakeup sources, which can only be used with RTC IOs, this wakeup source can be used with any IO (RTC or digital).

:cpp:func:`esp_sleep_enable_gpio_wakeup` function can be used to enable this wakeup source.

UART wakeup (light sleep only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When {IDF_TARGET_NAME} receives UART input from external devices, it is often required to wake up the chip when input data is available. UART peripheral contains a feature which allows waking up the chip from light sleep when a certain number of positive edges on RX pin are seen. This number of positive edges can be set using :cpp:func:`uart_set_wakeup_threshold` function. Note that the character which triggers wakeup (and any characters before it) will not be received by the UART after wakeup. This means that the external device typically needs to send an extra character to the {IDF_TARGET_NAME} to trigger wakeup, before sending the data.

:cpp:func:`esp_sleep_enable_uart_wakeup` function can be used to enable this wakeup source.


Power-down of RTC peripherals and memories
------------------------------------------

By default, :cpp:func:`esp_deep_sleep_start` and :cpp:func:`esp_light_sleep_start` functions will power down all RTC power domains which are not needed by the enabled wakeup sources. To override this behaviour, :cpp:func:`esp_sleep_pd_config` function is provided.

.. only:: esp32

    Note: in revision 0 of the ESP32, RTC fast memory will always be kept enabled in deep sleep, so that the deep sleep stub can run after reset. This can be overridden, if the application doesn't need clean reset behaviour after deep sleep.

If some variables in the program are placed into RTC slow memory (for example, using ``RTC_DATA_ATTR`` attribute), RTC slow memory will be kept powered on by default. This can be overridden using :cpp:func:`esp_sleep_pd_config` function, if desired.


Entering light sleep
--------------------

:cpp:func:`esp_light_sleep_start` function can be used to enter light sleep once wakeup sources are configured. It is also possible to go into light sleep with no wakeup sources configured, in this case the chip will be in light sleep mode indefinitely, until external reset is applied.

Entering deep sleep
-------------------

:cpp:func:`esp_deep_sleep_start` function can be used to enter deep sleep once wakeup sources are configured. It is also possible to go into deep sleep with no wakeup sources configured, in this case the chip will be in deep sleep mode indefinitely, until external reset is applied.

Configuring IOs
---------------

Some {IDF_TARGET_NAME} IOs have internal pullups or pulldowns, which are enabled by default. If an external circuit drives this pin in deep sleep mode, current consumption may increase due to current flowing through these pullups and pulldowns.

To isolate a pin, preventing extra current draw, call :cpp:func:`rtc_gpio_isolate` function.

For example, on ESP32-WROVER module, GPIO12 is pulled up externally. GPIO12 also has an internal pulldown in the ESP32 chip. This means that in deep sleep, some current will flow through these external and internal resistors, increasing deep sleep current above the minimal possible value.
Add the following code before :cpp:func:`esp_deep_sleep_start` to remove this extra current::

	rtc_gpio_isolate(GPIO_NUM_12);


UART output handling
--------------------

Before entering sleep mode, :cpp:func:`esp_deep_sleep_start` will flush the contents of UART FIFOs.

When entering light sleep mode using :cpp:func:`esp_light_sleep_start`, UART FIFOs will not be flushed. Instead, UART output will be suspended, and remaining characters in the FIFO will be sent out after wakeup from light sleep.

Checking sleep wakeup cause
---------------------------

:cpp:func:`esp_sleep_get_wakeup_cause` function can be used to check which wakeup source has triggered wakeup from sleep mode.

For touch pad and ext1 wakeup sources, it is possible to identify pin or touch pad which has caused wakeup using :cpp:func:`esp_sleep_get_touchpad_wakeup_status` and :cpp:func:`esp_sleep_get_ext1_wakeup_status` functions.


Disable sleep wakeup source
---------------------------

Previously configured wakeup source can be disabled later using :cpp:func:`esp_sleep_disable_wakeup_source` API. This function deactivates trigger for the given wakeup source. Additionally it can disable all triggers if the argument is ``ESP_SLEEP_WAKEUP_ALL``.

Application Example
-------------------

Implementation of basic functionality of deep sleep is shown in :example:`protocols/sntp` example, where ESP module is periodically waken up to retrieve time from NTP server.

More extensive example in :example:`system/deep_sleep` illustrates usage of various deep sleep wakeup triggers and ULP coprocessor programming.

API Reference
-------------

.. include-build-file:: inc/esp_sleep.inc


