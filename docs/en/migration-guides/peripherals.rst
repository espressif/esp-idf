Migrate Peripherals to ESP-IDF 5.0
==================================

Peripheral Clock Gating
-----------------------

As usual, peripheral clock gating is still handled by driver itself, users don't need to take care of the peripheral module clock gating.

However, for advanced users who implement their own drivers based on ``hal`` and ``soc`` components, the previous clock gating include path has been changed from ``driver/periph_ctrl.h`` to ``esp_private/periph_ctrl.h``.

SPI Flash Interface
-------------------

Version before v5.0, spi flash functions in rom can be included by ``esp32**/rom/spi_flash.h``. However, your code written for different chips may be filled with ROM headers of different versions. At the meantime not all the APIs can be used on all chips.

Therefore, the common APIs are extracted to ``esp_rom_spiflash.h``. Although it's not a breaking change, it is strongly recommended to only use the functions with prefix ``esp_rom_spiflash`` included by ``esp_rom_spiflash.h`` for better cross-compatibility.

To make the API clearer, we renamed the function ``esp_rom_spiflash_lock`` to ``esp_rom_spiflash_set_bp``. We renamed ``esp_rom_spiflash_unlock`` to ``esp_rom_spiflash_clear_bp``.

ADC
---

- Previous `driver/adc2_wifi_private.h` has been moved to `esp_private/adc2_wifi.h`.
- Enums `ADC_UNIT_BOTH`, `ADC_UNIT_ALTER` and `ADC_UNIT_MAX` in ``adc_unit_t`` are removed.

GPIO
----

The previous Kconfig option `RTCIO_SUPPORT_RTC_GPIO_DESC` has been removed, thus the ``rtc_gpio_desc`` array is unavailable. Please use ``rtc_io_desc`` array instead.

Timer Group Driver
------------------

Timer Group driver has been redesigned into :doc:`GPTimer <../api-reference/peripherals/gptimer>`, which aims to unify and simplify the usage of general purpose timer. Although it's recommended to use the the new driver APIs, the legacy driver is till available in the previous include path ``driver/timer.h``. However, by default, including ``driver/timer.h`` will bring a build warning like `legacy timer group driver is deprecated, please migrate to driver/gptimer.h`. The warning can be suppressed by the Kconfig option :ref:`CONFIG_GPTIMER_SUPPRESS_DEPRECATE_WARN`.

The major breaking changes in concept and usage are listed as follows:

Breaking Changes in Concepts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  ``timer_group_t`` and ``timer_idx_t`` which used to identify the hardware timer are removed from user's code. In the new driver, a timer is represented by :cpp:type:`gptimer_handle_t`.
-  Definition of timer source clock is moved to :cpp:type:`gptimer_clock_source_t`, the previous ``timer_src_clk_t`` is not used.
-  Definition of timer count direction is moved to :cpp:type:`gptimer_count_direction_t`, the previous ``timer_count_dir_t`` is not used.
-  Only level interrupt is supported, ``timer_intr_t`` and ``timer_intr_mode_t`` are not used.
-  Auto-reload is enabled by set the :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` flag. ``timer_autoreload_t`` is not used.

Breaking Changes in Usage
~~~~~~~~~~~~~~~~~~~~~~~~~

-  Timer initialization is done by creating a timer instance from :cpp:func:`gptimer_new_timer`. Basic configurations like clock source, resolution and direction should be set in :cpp:type:`gptimer_config_t`. Note that, alarm event specific configurations are not needed during the driver install stage.
-  Alarm event is configured by :cpp:func:`gptimer_set_alarm_action`, with parameters set in the :cpp:type:`gptimer_alarm_config_t`.
-  Setting and getting count value are done by :cpp:func:`gptimer_get_raw_count` and :cpp:func:`gptimer_set_raw_count`. The driver doesn't help convert the raw value into UTC time-stamp. Instead, the conversion should be done form user's side as the timer resolution is also known to the user.
-  The driver will install the interrupt service as well if :cpp:member:`gptimer_event_callbacks_t::on_alarm` is set to a valid callback function. In the callback, user doesn't have to deal with the low level registers (like "clear interrupt status", "re-enable alarm event" and so on). So functions like ``timer_group_get_intr_status_in_isr`` and ``timer_group_get_auto_reload_in_isr`` are not used anymore.
-  To update the alarm configurations when alarm event happens, one can call :cpp:func:`gptimer_set_alarm_action` in the interrupt callback, then the alarm will be re-enabled again.
-  Alarm will always be re-enabled by the driver if :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` is set to true.

UART
----

- ``uart_isr_register`` and ``uart_isr_free`` have been removed as the UART interrupt handling is closely related to the driver implementation.

I2C
---

- ``i2c_isr_register`` and ``i2c_isr_free`` have been removed as the I2C interrupt handling is closely related to the driver implementation.

.. only:: SOC_PCNT_SUPPORTED

    Pulse Counter Driver
    --------------------

    Pulse counter driver has been redesigned (see :doc:`PCNT <../api-reference/peripherals/pcnt>`), which aims to unify and simplify the usage of PCNT peripheral. Although it's recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/pcnt.h``. However, by default, including ``driver/pcnt.h`` will bring a build warning like `legacy pcnt driver is deprecated, please migrate to use driver/pulse_cnt.h`. The warning can be suppressed by the Kconfig option :ref:`CONFIG_PCNT_SUPPRESS_DEPRECATE_WARN`.

    The major breaking changes in concept and usage are listed as follows:

    Breaking Changes in Concepts
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    - ``pcnt_port_t``, ``pcnt_unit_t`` and ``pcnt_channel_t`` which used to identify the hardware unit and channel are removed from user's code. In the new driver, PCNT unit is represented by :cpp:type:`pcnt_unit_handle_t`, likewise, PCNT channel is represented by :cpp:type:`pcnt_channel_handle_t`. Both of them are opaque pointers.
    - ``pcnt_evt_type_t`` is not used any more, they have been replaced by a universal **Watch Point Event**. In the event callback :cpp:type:`pcnt_watch_cb_t`, it's still possible to distinguish different watch points from :cpp:type:`pcnt_watch_event_data_t`.
    - ``pcnt_count_mode_t`` is replaced by :cpp:type:`pcnt_channel_edge_action_t`, and ``pcnt_ctrl_mode_t`` is replaced by :cpp:type:`pcnt_channel_level_action_t`.

    Breaking Changes in Usage
    ~~~~~~~~~~~~~~~~~~~~~~~~~

    - In the legacy driver, the PCNT unit configuration and channel configuration were combined into a single function: ``pcnt_unit_config``. Now this is split into two factory APIs: :cpp:func:`pcnt_new_unit` and :cpp:func:`pcnt_new_channel`. Only the count range is necessary for initializing a PCNT unit. GPIO number assignment has been moved to :cpp:func:`pcnt_new_channel`. High/Low control mode and positive/negative edge count mode are set by stand-alone functions: :cpp:func:`pcnt_channel_set_edge_action` and :cpp:func:`pcnt_channel_set_level_action`.
    - ``pcnt_get_counter_value`` is replaced by :cpp:func:`pcnt_unit_get_count`.
    - ``pcnt_counter_pause`` is replaced by :cpp:func:`pcnt_unit_stop`.
    - ``pcnt_counter_resume`` is replaced by :cpp:func:`pcnt_unit_start`.
    - ``pcnt_counter_clear`` is replaced by :cpp:func:`pcnt_unit_clear_count`.
    - ``pcnt_intr_enable`` and ``pcnt_intr_disable`` are removed. In the new driver, the interrupt is enabled by registering event callbacks :cpp:func:`pcnt_unit_register_event_callbacks`.
    - ``pcnt_event_enable`` and ``pcnt_event_disable`` are removed. In the new driver, the PCNT events are enabled/disabled by adding/removing watch points :cpp:func:`pcnt_unit_add_watch_point`, :cpp:func:`pcnt_unit_remove_watch_point`.
    - ``pcnt_set_event_value`` is removed. In the new driver, event value is also set when adding watch point by :cpp:func:`pcnt_unit_add_watch_point`.
    - ``pcnt_get_event_value`` and ``pcnt_get_event_status`` are removed. In the new driver, these information are provided by event callback :cpp:type:`pcnt_watch_cb_t` in the :cpp:type:`pcnt_watch_event_data_t`.
    - ``pcnt_isr_register`` and ``pcnt_isr_unregister`` are removed. Register of the ISR handler from user code is no longer permitted. Users should register event callbacks instead by calling :cpp:func:`pcnt_unit_register_event_callbacks`.
    - ``pcnt_set_pin`` is removed and the new driver no longer allows the switching of the GPIO at runtime. If you want to change to other GPIOs, please delete the existing PCNT channel by :cpp:func:`pcnt_del_channel` and reinstall with the new GPIO number by :cpp:func:`pcnt_new_channel`.
    - ``pcnt_filter_enable``, ``pcnt_filter_disable``, and ``pcnt_set_filter_value`` are replaced by :cpp:func:`pcnt_unit_set_glitch_filter`. Meanwhile, ``pcnt_get_filter_value`` has been removed.
    - ``pcnt_set_mode`` is replaced by :cpp:func:`pcnt_channel_set_edge_action` and :cpp:func:`pcnt_channel_set_level_action`.
    - ``pcnt_isr_service_install``, ``pcnt_isr_service_uninstall``, ``pcnt_isr_handler_add`` and ``pcnt_isr_handler_remove`` are replaced by :cpp:func:`pcnt_unit_register_event_callbacks`. The default ISR handler is lazy installed in the new driver.

.. only:: SOC_TEMP_SENSOR_SUPPORTED

    Temperature Sensor Driver
    -------------------------

    - Old API header ``temp_sensor.h`` has been redesigned as ``temperature_sensor.h``, it is recommended to use the new driver and the old driver is not allowed to be used at the same time.
    - Although it's recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/temp_sensor.h``. However, by default, including ``driver/temp_sensor.h`` will bring a build warning like "legacy temperature sensor driver is deprecated, please migrate to driver/temperature_sensor.h". The warning can be suppressed by enabling the menuconfig option :ref:`CONFIG_TEMP_SENSOR_SUPPRESS_DEPRECATE_WARN`.
    - Configuration contents has been changed. In old version, user need to configure the ``clk_div`` and ``dac_offset``. While in new version, user only need to choose ``tsens_range``
    - The process of using temperature sensor has been changed. In old version, user can use ``config->start->read_celsius`` to get value. In the new version, user must install the temperature sensor driver firstly, by ``temperature_sensor_install`` and uninstall it when finished. For more information, you can refer to :doc:`Temperature Sensor <../api-reference/peripherals/temp_sensor>` .

.. only:: SOC_RMT_SUPPORTED

    RMT Driver
    ----------

    - ``rmt_set_intr_enable_mask`` and ``rmt_clr_intr_enable_mask`` are removed, as the interrupt is handled by the driver, user doesn't need to take care of it.
    - ``rmt_set_pin`` is removed, as ``rmt_set_gpio`` can do the same thing.
    - ``rmt_memory_rw_rst`` is removed, user can use ``rmt_tx_memory_reset`` and ``rmt_rx_memory_reset`` for TX and RX channel respectively.
