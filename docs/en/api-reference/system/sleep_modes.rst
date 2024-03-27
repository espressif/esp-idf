Sleep Modes
===========

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_SPI_POWER_DOMAIN:default="VDD_SPI", esp32="VDD_SDIO"}

Overview
--------

{IDF_TARGET_NAME} supports two major power saving modes: Light-sleep and Deep-sleep. According to the features used by an application, there are some sub sleep modes. See :ref:`sleep_modes` for these sleep modes and sub sleep modes. Additionally, there are some power-down options that can be configured to further reduce the power consumption. See :ref:`power_down_options` for more details.

There are several wakeup sources in the sleep modes. These sources can also be combined so that the chip will wake up when any of the sources are triggered. :ref:`api-reference-wakeup-source` describes these wakeup sources and configuration APIs in detail.

The configuration of power-down options and wakeup sources are optional. They can be configured at any moment before entering the sleep modes.

Then the application can call sleep start APIs to enter one of the sleep modes. See :ref:`enter_sleep` for more details. When the wakeup condition is met, the application is awoken from sleep. See :ref:`wakeup_cause` on how to get the wakeup cause, and :ref:`disable_sleep_wakeup_source` on how to handle the wakeup sources after wakeup.

.. _sleep_modes:

Sleep Modes
-----------

In Light-sleep mode, the digital peripherals, most of the RAM, and CPUs are clock-gated and their supply voltage is reduced. Upon exit from Light-sleep, the digital peripherals, RAM, and CPUs resume operation and their internal states are preserved.

In Deep-sleep mode, the CPUs, most of the RAM, and all digital peripherals that are clocked from APB_CLK are powered off. The only parts of the chip that remain powered on are:

    .. list::

        - RTC controller
        :SOC_ULP_SUPPORTED: - ULP coprocessor
        :SOC_RTC_FAST_MEM_SUPPORTED: - RTC FAST memory
        :SOC_RTC_SLOW_MEM_SUPPORTED: - RTC SLOW memory

.. only:: SOC_BT_SUPPORTED

    Wi-Fi/Bluetooth and Sleep Modes
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    In Deep-sleep and Light-sleep modes, the wireless peripherals are powered down. Before entering Deep-sleep or Light-sleep modes, the application must disable Wi-Fi and Bluetooth using the appropriate calls (i.e., :cpp:func:`esp_bluedroid_disable`, :cpp:func:`esp_bt_controller_disable`, :cpp:func:`esp_wifi_stop`). Wi-Fi and Bluetooth connections are not maintained in Deep-sleep or Light-sleep mode, even if these functions are not called.

    If Wi-Fi/Bluetooth connections need to be maintained, enable Wi-Fi/Bluetooth Modem-sleep mode and automatic Light-sleep feature (see :doc:`Power Management APIs <power_management>`). This allows the system to wake up from sleep automatically when required by the Wi-Fi/Bluetooth driver, thereby maintaining the connection.

.. only:: not SOC_BT_SUPPORTED

    Wi-Fi and Sleep Modes
    ^^^^^^^^^^^^^^^^^^^^^^^

    In Deep-sleep and Light-sleep modes, the wireless peripherals are powered down. Before entering Deep-sleep or Light-sleep modes, applications must disable Wi-Fi using the appropriate calls (:cpp:func:`esp_wifi_stop`). Wi-Fi connections are not maintained in Deep-sleep or Light-sleep mode, even if these functions are not called.

    If Wi-Fi connections need to be maintained, enable Wi-Fi Modem-sleep mode and automatic Light-sleep feature (see :doc:`Power Management APIs <power_management>`). This will allow the system to wake up from sleep automatically when required by the Wi-Fi driver, thereby maintaining a connection to the AP.

.. only:: esp32s2 or esp32s3 or esp32c2 or esp32c3

    Sub Sleep Modes
    ^^^^^^^^^^^^^^^

    Tables below list the sub sleep modes in the first row and the features they support in the first column. Modes that support more features may consume more power during sleep mode. The sleep system automatically selects the mode that satisfies all the features required by the user while consuming least power.

    Deep-sleep:

    .. list-table::
       :widths: auto
       :header-rows: 2

       * -
         - DSLP_ULTRA_LOW
         - DSLP_DEFAULT
         - DSLP_8MD256/
       * -
         -
         -
         - DSLP_ADC_TSENS
       * - ULP/Touch sensor (ESP32-S2 and ESP32-S3 only)
         - Y
         - Y
         - Y
       * - RTC IO input/RTC memory at high temperature
         -
         - Y
         - Y
       * - ADC_TSEN_MONITOR
         -
         -
         - Y
       * - 8MD256 as the clock source for RTC_SLOW_CLK
         -
         -
         - Y

    Features:

    1. RTC IO input/RTC memory at high temperature (experimental): Use RTC IO as input pins, or use RTC memory at high temperature. The chip can go into ultra low power mode when these features are disabled. Controlled by API :cpp:func:`rtc_sleep_enable_ultra_low`.

    2. ADC_TSEN_MONITOR: Use ADC/Temperature Sensor in monitor mode (controlled by ULP). Enabled by API :cpp:func:`ulp_adc_init` or its higher level APIs. Only available for ESP32-S2 and ESP32-S3 chips with monitor mode.

    3. 8MD256 as the clock source for RTC_SLOW_CLK: When 8MD256 is selected as the clock source for RTC_SLOW_CLK using the Kconfig option ``CONFIG_RTC_CLK_SRC_INT_8MD256``, the chip will automatically enter this sub sleep mode during Deep-sleep mode.

    Light-sleep:

    .. list-table::
       :widths: auto
       :header-rows: 2

       * -
         - LSLP_DEFAULT
         - LSLP_ADC_TSENS
         - LSLP_8MD256
         - LSLP_LEDC8M/
       * -
         -
         -
         -
         - LSLP_XTAL_FPU
       * - ULP/Touch sensor (ESP32-S2 and ESP32-S3 only)
         - Y
         - Y
         - Y
         - Y
       * - RTC IO input/RTC memory at high temperature
         - Y
         - Y
         - Y
         - Y
       * - ADC_TSEN_MONITOR
         -
         - Y
         - Y
         - Y
       * - 8MD256 as the clock source for RTC_SLOW_CLK
         -
         -
         - Y
         - Y
       * - 8 MHz RC clock source used by digital peripherals
         -
         -
         -
         - Y
       * - Keep the XTAL clock on
         -
         -
         -
         - Y

    Features: (Also see 8MD256 and ADC_TSEN_MONITOR features for Deep-sleep mode above)

    1. 8 MHz RC clock source used by digital peripherals: Currently, only LEDC uses this clock source during Light-sleep mode. When LEDC selects this clock source, this feature is automatically enabled.

    2. Keep the XTAL clock on: Keep the XTAL clock on during Light-sleep mode. Controlled by ``ESP_PD_DOMAIN_XTAL`` power domain.

    .. only:: esp32s2

        {IDF_TARGET_NAME} uses the same power mode for LSLP_8MD256, LSLP_LEDC8M, and LSLP_XTAL_FPU features.

    .. only:: esp32s3

        Default mode of {IDF_TARGET_NAME} already supports ADC_TSEN_MONITOR feature.

    .. only:: esp32c2 or esp32c3

        {IDF_TARGET_NAME} does not have ADC_TSEN_MONITOR or LSLP_ADC_TSENS feature.

.. _api-reference-wakeup-source:

Wakeup Sources
--------------

Wakeup sources can be enabled using ``esp_sleep_enable_X_wakeup`` APIs. Wakeup sources are not disabled after wakeup, you can disable them using :cpp:func:`esp_sleep_disable_wakeup_source` API if you do not need them any more. See :ref:`disable_sleep_wakeup_source`.

Following are the wakeup sources supported on {IDF_TARGET_NAME}.

Timer
^^^^^

The RTC controller has a built-in timer which can be used to wake up the chip after a predefined amount of time. Time is specified at microsecond precision, but the actual resolution depends on the clock source selected for RTC_SLOW_CLK.

.. only:: SOC_ULP_SUPPORTED

    For details on RTC clock options, see **{IDF_TARGET_NAME} Technical Reference Manual** > **ULP Coprocessor** [`PDF <{IDF_TARGET_TRM_EN_URL}#ulp>`__].

RTC peripherals or RTC memories do not need to be powered on during sleep in this wakeup mode.

:cpp:func:`esp_sleep_enable_timer_wakeup` function can be used to enable sleep wakeup using a timer.

.. only:: SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP

    Touchpad
    ^^^^^^^^^

    The RTC IO module contains the logic to trigger wakeup when a touch sensor interrupt occurs. To wakeup from a touch sensor interrupt, users need to configure the touch pad interrupt before the chip enters Deep-sleep or Light-sleep modes.

    .. only:: esp32

        Revisions 0 and 1 of ESP32 only support this wakeup mode when RTC peripherals are not forced to be powered on (i.e., ESP_PD_DOMAIN_RTC_PERIPH should be set to ESP_PD_OPTION_AUTO).

    :cpp:func:`esp_sleep_enable_touchpad_wakeup` function can be used to enable this wakeup source.

.. only:: SOC_PM_SUPPORT_EXT0_WAKEUP

    External Wakeup (``ext0``)
    ^^^^^^^^^^^^^^^^^^^^^^^^^^

    The RTC IO module contains the logic to trigger wakeup when one of RTC GPIOs is set to a predefined logic level. RTC IO is part of the RTC peripherals power domain, so RTC peripherals will be kept powered on during Deep-sleep if this wakeup source is requested.

    The RTC IO module is enabled in this mode, so internal pullup or pulldown resistors can also be used. They need to be configured by the application using :cpp:func:`rtc_gpio_pullup_en` and :cpp:func:`rtc_gpio_pulldown_en` functions before calling :cpp:func:`esp_deep_sleep_start`.

    .. only:: esp32

        In revisions 0 and 1 of ESP32, this wakeup source is incompatible with ULP and touch wakeup sources.

    :cpp:func:`esp_sleep_enable_ext0_wakeup` function can be used to enable this wakeup source.

    .. warning::

        After waking up from sleep, the IO pad used for wakeup will be configured as RTC IO. Therefore, before using this pad as digital GPIO, users need to reconfigure it using :cpp:func:`rtc_gpio_deinit` function.

.. only:: SOC_PM_SUPPORT_EXT1_WAKEUP

    External Wakeup (``ext1``)
    ^^^^^^^^^^^^^^^^^^^^^^^^^^

    The RTC controller contains the logic to trigger wakeup using multiple RTC GPIOs. One of the following two logic functions can be used to trigger ext1 wakeup:

    .. only:: esp32

        - wake up if any of the selected pins is high (``ESP_EXT1_WAKEUP_ANY_HIGH``)
        - wake up if all the selected pins are low (``ESP_EXT1_WAKEUP_ALL_LOW``)

    .. only:: esp32s2 or esp32s3 or esp32c6 or esp32h2

        - wake up if any of the selected pins is high (``ESP_EXT1_WAKEUP_ANY_HIGH``)
        - wake up if any of the selected pins is low (``ESP_EXT1_WAKEUP_ANY_LOW``)

    This wakeup source is controlled by the RTC controller. Unlike ``ext0``, this wakeup source supports wakeup even when the RTC peripheral is powered down. Although the power domain of the RTC peripheral, where RTC IOs are located, is powered down during sleep modes, ESP-IDF will automatically lock the state of the wakeup pin before the system enters sleep modes and unlock upon exiting sleep modes. Therefore, the internal pull-up or pull-down resistors can still be configured for the wakeup pin::

        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
        rtc_gpio_pullup_dis(gpio_num);
        rtc_gpio_pulldown_en(gpio_num);

    If we turn off the ``RTC_PERIPH`` domain, we will use the HOLD feature to maintain the pull-up and pull-down on the pins during sleep. HOLD feature will be acted on the pin internally before the system enters sleep modes, and this can further reduce power consumption::

        rtc_gpio_pullup_dis(gpio_num);
        rtc_gpio_pulldown_en(gpio_num);

    If certain chips lack the ``RTC_PERIPH`` domain, we can only use the HOLD feature to maintain the pull-up and pull-down on the pins during sleep modes::

        gpio_pullup_dis(gpio_num);
        gpio_pulldown_en(gpio_num);

    :cpp:func:`esp_sleep_enable_ext1_wakeup_io` function can be used to append ext1 wakeup IO and set corresponding wakeup level.

    :cpp:func:`esp_sleep_disable_ext1_wakeup_io` function can be used to remove ext1 wakeup IO.

    .. only:: SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN

        The RTC controller also supports triggering wakeup, allowing configurable IO to use different wakeup levels simultaneously. This can be configured with :cpp:func:`esp_sleep_enable_ext1_wakeup_io`.

    .. only:: not SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN

        .. note::

           Due to hardware limitation, when we use more than one IO for EXT1 wakeup, it is not allowed to configure different wakeup levels for the IOs, and there is corresponding inspection mechanism in :cpp:func:`esp_sleep_enable_ext1_wakeup_io`.

    .. warning::

        - To use the EXT1 wakeup, the IO pad(s) are configured as RTC IO. Therefore, before using these pads as digital GPIOs, users need to reconfigure them by calling the :cpp:func:`rtc_gpio_deinit` function.

        - If the RTC peripherals are configured to be powered down (which is by default), the wakeup IOs will be set to the holding state before entering sleep. Therefore, after the chip wakes up from Light-sleep, please call ``rtc_gpio_hold_dis`` to disable the hold function to perform any pin re-configuration. For Deep-sleep wakeup, this is already being handled at the application startup stage.

.. only:: SOC_ULP_SUPPORTED

    ULP Coprocessor Wakeup
    ^^^^^^^^^^^^^^^^^^^^^^

    ULP coprocessor can run while the chip is in sleep mode, and may be used to poll sensors, monitor ADC or touch sensor values, and wake up the chip when a specific event is detected. ULP coprocessor is part of the RTC peripherals power domain, and it runs the program stored in RTC SLOW memory. RTC SLOW memory will be powered on during sleep if this wakeup mode is requested. RTC peripherals will be automatically powered on before ULP coprocessor starts running the program; once the program stops running, RTC peripherals are automatically powered down again.

    .. only:: esp32

        Revisions 0 and 1 of ESP32 only support this wakeup mode when RTC peripherals are not forced to be powered on (i.e., ESP_PD_DOMAIN_RTC_PERIPH should be set to ESP_PD_OPTION_AUTO).

    :cpp:func:`esp_sleep_enable_ulp_wakeup` function can be used to enable this wakeup source.

.. only:: SOC_RTCIO_WAKE_SUPPORTED

    GPIO Wakeup (Light-sleep Only)
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. only:: (SOC_PM_SUPPORT_EXT0_WAKEUP or SOC_PM_SUPPORT_EXT1_WAKEUP)

        In addition to EXT0 and EXT1 wakeup sources described above, one more method of wakeup from external inputs is available in Light-sleep mode. With this wakeup source, each pin can be individually configured to trigger wakeup on high or low level using :cpp:func:`gpio_wakeup_enable` function. Unlike EXT0 and EXT1 wakeup sources, which can only be used with RTC IOs, this wakeup source can be used with any IO (RTC or digital).

    .. only:: not (SOC_PM_SUPPORT_EXT0_WAKEUP or SOC_PM_SUPPORT_EXT1_WAKEUP)

        One more method of wakeup from external inputs is available in Light-sleep mode. With this wakeup source, each pin can be individually configured to trigger wakeup on high or low level using :cpp:func:`gpio_wakeup_enable` function. This wakeup source can be used with any IO (RTC or digital).

    :cpp:func:`esp_sleep_enable_gpio_wakeup` function can be used to enable this wakeup source.

    .. warning::

        Before entering Light-sleep mode, check if any GPIO pin to be driven is part of the {IDF_TARGET_SPI_POWER_DOMAIN} power domain. If so, this power domain must be configured to remain ON during sleep.

        For example, on ESP32-WROOM-32 board, GPIO16 and GPIO17 are linked to {IDF_TARGET_SPI_POWER_DOMAIN} power domain. If they are configured to remain high during Light-sleep, the power domain should be configured to remain powered ON. This can be done with :cpp:func:`esp_sleep_pd_config()`::

            esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_ON);

.. only:: not SOC_RTCIO_WAKE_SUPPORTED

    GPIO Wakeup
    ^^^^^^^^^^^

    Any IO can be used as the external input to wake up the chip from Light-sleep. Each pin can be individually configured to trigger wakeup on high or low level using the :cpp:func:`gpio_wakeup_enable` function. Then the :cpp:func:`esp_sleep_enable_gpio_wakeup` function should be called to enable this wakeup source.

    Additionally, IOs that are powered by the VDD3P3_RTC power domain can be used to wake up the chip from Deep-sleep. The wakeup pin and wakeup trigger level can be configured by calling :cpp:func:`esp_deep_sleep_enable_gpio_wakeup`. The function will enable the Deep-sleep wakeup for the selected pin.

    .. only:: esp32c6 or esp32h2

       .. note::

           In Light-sleep mode, setting Kconfig option :ref:`CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP` will invalidate GPIO wakeup.

UART Wakeup (Light-sleep Only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When {IDF_TARGET_NAME} receives UART input from external devices, it is often necessary to wake up the chip when input data is available. The UART peripheral contains a feature which allows waking up the chip from Light-sleep when a certain number of positive edges on RX pin are seen. This number of positive edges can be set using :cpp:func:`uart_set_wakeup_threshold` function. Note that the character which triggers wakeup (and any characters before it) will not be received by the UART after wakeup. This means that the external device typically needs to send an extra character to the {IDF_TARGET_NAME} to trigger wakeup before sending the data.

:cpp:func:`esp_sleep_enable_uart_wakeup` function can be used to enable this wakeup source.

After waking-up from UART, you should send some extra data through the UART port in Active mode, so that the internal wakeup indication signal can be cleared. Otherwises, the next UART wake-up would trigger with two less rising edges than the configured threshold value.

    .. only:: esp32c6 or esp32h2

       .. note::

           In Light-sleep mode, setting Kconfig option :ref:`CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP` will invalidate UART wakeup.

.. _disable_sleep_wakeup_source:

Disable Sleep Wakeup Source
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Previously configured wakeup sources can be disabled later using :cpp:func:`esp_sleep_disable_wakeup_source` API. This function deactivates trigger for the given wakeup source. Additionally, it can disable all triggers if the argument is ``ESP_SLEEP_WAKEUP_ALL``.

.. _power_down_options:

Power-down Options
------------------

The application can force specific powerdown modes for RTC peripherals and RTC memories. In Deep-sleep mode, we can also isolate some IOs to further reduce current consumption.

Power-down of RTC Peripherals and Memories
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, :cpp:func:`esp_deep_sleep_start` and :cpp:func:`esp_light_sleep_start` functions power down all RTC power domains which are not needed by the enabled wakeup sources. To override this behaviour, :cpp:func:`esp_sleep_pd_config` function is provided.

.. only:: esp32

    Note: in revision 0 of ESP32, RTC FAST memory is always kept enabled in Deep-sleep, so that the Deep-sleep stub can run after reset. This can be overridden, if the application does not need clean reset behaviour after Deep-sleep.

.. only:: SOC_RTC_SLOW_MEM_SUPPORTED

    If some variables in the program are placed into RTC SLOW memory (for example, using ``RTC_DATA_ATTR`` attribute), RTC SLOW memory will be kept powered on by default. This can be overridden using :cpp:func:`esp_sleep_pd_config` function, if desired.

.. only:: not SOC_RTC_SLOW_MEM_SUPPORTED and SOC_RTC_FAST_MEM_SUPPORTED

    In {IDF_TARGET_NAME}, there is only RTC FAST memory, so if some variables in the program are marked by ``RTC_DATA_ATTR``, ``RTC_SLOW_ATTR`` or ``RTC_FAST_ATTR`` attributes, all of them go to RTC FAST memory. It will be kept powered on by default. This can be overridden using :cpp:func:`esp_sleep_pd_config` function, if desired.

Power-down of Flash
^^^^^^^^^^^^^^^^^^^

By default, to avoid potential issues, :cpp:func:`esp_light_sleep_start` function does **not** power down flash. To be more specific, it takes time to power down the flash and during this period the system may be woken up, which then actually powers up the flash before this flash could be powered down completely. As a result, there is a chance that the flash may not work properly.

So, in theory, it is ok if you only wake up the system after the flash is completely powered down. However, in reality, the flash power-down period can be hard to predict (for example, this period can be much longer when you add filter capacitors to the flash's power supply circuit) and uncontrollable (for example, the asynchronous wake-up signals make the actual sleep time uncontrollable).

.. warning::

    If a filter capacitor is added to your flash power supply circuit, please do everything possible to avoid powering down flash.

Therefore, it is recommended not to power down flash when using ESP-IDF. For power-sensitive applications, it is recommended to use Kconfig option :ref:`CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND` to reduce the power consumption of the flash during Light-sleep, instead of powering down the flash.

.. only:: SOC_SPIRAM_SUPPORTED

    It is worth mentioning that PSRAM has a similar Kconfig option :ref:`CONFIG_ESP_SLEEP_PSRAM_LEAKAGE_WORKAROUND`.

However, for those who have fully understood the risk and are still willing to power down the flash to further reduce the power consumption, please check the following mechanisms:

    .. list::

        - Setting Kconfig option :ref:`CONFIG_ESP_SLEEP_POWER_DOWN_FLASH` only powers down the flash when the RTC timer is the only wake-up source **and** the sleep time is longer than the flash power-down period.
        - Calling ``esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_OFF)`` powers down flash when the RTC timer is not enabled as a wakeup source **or** the sleep time is longer than the flash power-down period.

.. note::

    .. list::

        - ESP-IDF does not provide any mechanism that can power down the flash in all conditions when Light-sleep.
        - :cpp:func:`esp_deep_sleep_start` function forces power down flash regardless of user configuration.

Configuring IOs (Deep-sleep Only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some {IDF_TARGET_NAME} IOs have internal pullups or pulldowns, which are enabled by default. If an external circuit drives this pin in Deep-sleep mode, current consumption may increase due to current flowing through these pullups and pulldowns.

.. only:: SOC_RTCIO_HOLD_SUPPORTED and SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    To isolate a pin to prevent extra current draw, call :cpp:func:`rtc_gpio_isolate` function.

    For example, on ESP32-WROVER module, GPIO12 is pulled up externally, and it also has an internal pulldown in the ESP32 chip. This means that in Deep-sleep, some current flows through these external and internal resistors, increasing Deep-sleep current above the minimal possible value.

    Add the following code before :cpp:func:`esp_deep_sleep_start` to remove such extra current::

        rtc_gpio_isolate(GPIO_NUM_12);

.. only:: esp32c2 or esp32c3

    In Deep-sleep mode:
        - digital GPIOs (GPIO6 ~ 21) are in a high impedance state.
        - RTC GPIOs (GPIO0 ~ 5) can be in the following states, depending on their hold function enabled or not:
            - if the hold function is not enabled, RTC GPIOs will be in a high impedance state.
            - if the hold function is enabled, RTC GPIOs will retain the pin state latched at that hold moment.

.. _enter_sleep:

Entering Sleep
--------------

:cpp:func:`esp_light_sleep_start` or :cpp:func:`esp_deep_sleep_start` functions can be used to enter Light-sleep or Deep-sleep modes correspondingly. After that, the system configures the parameters of RTC controller according to the requested wakeup sources and power-down options.

It is also possible to enter sleep modes with no wakeup sources configured. In this case, the chip will be in sleep modes indefinitely until external reset is applied.

UART Output Handling
^^^^^^^^^^^^^^^^^^^^

Before entering sleep mode, :cpp:func:`esp_deep_sleep_start` will flush the contents of UART FIFOs.

When entering Light-sleep mode using :cpp:func:`esp_light_sleep_start`, UART FIFOs will not be flushed. Instead, UART output will be suspended, and remaining characters in the FIFO will be sent out after wakeup from Light-sleep.

.. _wakeup_cause:

Checking Sleep Wakeup Cause
---------------------------

:cpp:func:`esp_sleep_get_wakeup_cause` function can be used to check which wakeup source has triggered wakeup from sleep mode.

.. only:: SOC_TOUCH_SENSOR_SUPPORTED

    For touchpad, it is possible to identify which touch pin has caused wakeup using :cpp:func:`esp_sleep_get_touchpad_wakeup_status` functions.

.. only:: SOC_PM_SUPPORT_EXT1_WAKEUP

    For ext1 wakeup sources, it is possible to identify which touch pin has caused wakeup using :cpp:func:`esp_sleep_get_ext1_wakeup_status` functions.

Application Example
-------------------

.. list::

    - :example:`protocols/sntp`: the implementation of basic functionality of Deep-sleep, where ESP module is periodically waken up to retrieve time from NTP server.
    - :example:`wifi/power_save`: the usage of Wi-Fi Modem-sleep mode and automatic Light-sleep feature to maintain Wi-Fi connections.
    :SOC_BT_SUPPORTED: - :example:`bluetooth/nimble/power_save`: the usage of Bluetooth Modem-sleep mode and automatic Light-sleep feature to maintain Bluetooth connections.
    :SOC_ULP_SUPPORTED: - :example:`system/deep_sleep`: the usage of various Deep-sleep wakeup triggers and ULP coprocessor programming.
    :not SOC_ULP_SUPPORTED: - :example:`system/deep_sleep`: the usage of Deep-sleep wakeup triggered by various sources supported by the chip (RTC Timer, GPIO, EXT0, EXT1, Touch Sensor, etc.).
    - :example:`system/light_sleep`: the usage of Light-sleep wakeup triggered by various sources supported by the chip (Timer, GPIO, Touch Sensor, etc.).


API Reference
-------------

.. include-build-file:: inc/esp_sleep.inc
