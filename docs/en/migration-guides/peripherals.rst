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

Previous `driver/adc2_wifi_private.h` has been moved to `esp_private/adc2_wifi.h`.

GPIO
----

The previous Kconfig option `RTCIO_SUPPORT_RTC_GPIO_DESC` has been removed, thus the ``rtc_gpio_desc`` array is unavailable. Please use ``rtc_io_desc`` array instead.

Timer Group Driver
------------------

Timer Group driver has been redesigned into :doc:`GPTimer <../api-reference/peripherals/gptimer>`, which aims to unify and simplify the usage of general purpose timer.
Although it's recommended to use the the new driver APIs, the legacy driver is till available in the previous include path ``driver/timer.h``. However, by default, including ``driver/timer.h`` will bring a build warning like "legacy timer group driver is deprecated, please migrate to driver/gptimer.h". The warning can be suppressed by the Kconfig option :ref:`CONFIG_GPTIMER_SUPPRESS_DEPRECATE_WARN`.

The major breaking changes in concept and usage are listed as follows:

Breaking Changes in Concepts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  ``timer_group_t`` and ``timer_idx_t`` which used to identify the hardware timer are removed from user's code. In the new driver, a timer is represented by :cpp:type:`gptimer_handle_t`.
-  Definition of timer source clock is moved to :cpp:type:`gptimer_clock_source_t`, the previous ``timer_src_clk_t`` is not used.
-  Definition of timer count direction is moved to :cpp:type:`gptimer_count_direction_t`, the previous ``timer_count_dir_t`` is not used.
-  Only level interrupt is supported, ``timer_intr_t`` and ``timer_intr_mode_t`` are not used.
-  Auto-reload is enabled by set the :cpp:member:`auto_reload_on_alarm` flag. ``timer_autoreload_t`` is not used.

Breaking Changes in Usage
~~~~~~~~~~~~~~~~~~~~~~~~~

-  Timer initialization is done by creating a timer instance from :cpp:func:`gptimer_new_timer`. Basic configurations like clock source, resolution and direction should be set in :cpp:type:`gptimer_config_t`. Note that, alarm event specific configurations are not needed during the driver install stage.
-  Alarm event is configured by :cpp:func:`gptimer_set_alarm_action`, with parameters set in the :cpp:type:`gptimer_alarm_config_t`.
-  Setting and getting count value are done by :cpp:func:`gptimer_get_raw_count` and :cpp:func:`gptimer_set_raw_count`. The driver doesn't help convert the raw value into UTC time-stamp. Instead, the conversion should be done form user's side as the timer resolution is also known to the user.
-  The driver will install the interrupt service as well if :cpp:member:`on_alarm` is set to a valid callback function. In the callback, user doesn't have to deal with the low level registers (like "clear interrupt status", "re-enable alarm event" and so on). So functions like ``timer_group_get_intr_status_in_isr`` and ``timer_group_get_auto_reload_in_isr`` are not used anymore.
-  To update the alarm configurations when alarm event happens, one can call :cpp:func:`gptimer_set_alarm_action` in the interrupt callback, then the alarm will be re-enabled again.
-  Alarm will always be re-enabled by the driver if :cpp:member:`auto_reload_on_alarm` is set to true.
