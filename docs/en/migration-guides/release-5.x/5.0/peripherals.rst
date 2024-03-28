Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

Peripheral Clock Gating
-----------------------

As usual, peripheral clock gating is still handled by driver itself, users do not need to take care of the peripheral module clock gating.

However, for advanced users who implement their own drivers based on ``hal`` and ``soc`` components, the previous clock gating include path has been changed from ``driver/periph_ctrl.h`` to ``esp_private/periph_ctrl.h``.

RTC Subsystem Control
---------------------

RTC control APIs have been moved from ``driver/rtc_cntl.h`` to ``esp_private/rtc_ctrl.h``.

ADC
---

ADC Oneshot & Continuous Mode Drivers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ADC oneshot mode driver has been redesigned.

- The new driver is in ``esp_adc`` component and the include path is ``esp_adc/adc_oneshot.h``.
- The legacy driver is still available in the previous include path ``driver/adc.h``.

The ADC continuous mode driver has been moved from ``driver`` component to ``esp_adc`` component.

- The include path has been changed from ``driver/adc.h`` to ``esp_adc/adc_continuous.h``.

Attempting to use the legacy include path ``driver/adc.h`` of either driver triggers the build warning below by default. However, the warning can be suppressed by enabling the :ref:`CONFIG_ADC_SUPPRESS_DEPRECATE_WARN` Kconfig option.

.. code-block:: text

    legacy adc driver is deprecated, please migrate to use esp_adc/adc_oneshot.h and esp_adc/adc_continuous.h for oneshot mode and continuous mode drivers respectively

ADC Calibration Driver
^^^^^^^^^^^^^^^^^^^^^^

The ADC calibration driver has been redesigned.

- The new driver is in ``esp_adc`` component and the include path is ``esp_adc/adc_cali.h`` and ``esp_adc/adc_cali_scheme.h``.

Legacy driver is still available by including ``esp_adc_cal.h``. However, if users still would like to use the include path of the legacy driver, users should add ``esp_adc`` component to the list of component requirements in CMakeLists.txt.

Attempting to use the legacy include path ``esp_adc_cal.h`` triggers the build warning below by default. However, the warning can be suppressed by enabling the :ref:`CONFIG_ADC_CALI_SUPPRESS_DEPRECATE_WARN` Kconfig option.

.. code-block:: text

    legacy adc calibration driver is deprecated, please migrate to use esp_adc/adc_cali.h and esp_adc/adc_cali_scheme.h

API Changes
^^^^^^^^^^^

- The ADC power management APIs ``adc_power_acquire`` and ``adc_power_release`` have made private and moved to ``esp_private/adc_share_hw_ctrl.h``.

    - The two APIs were previously made public due to a HW errata workaround.
    - Now, ADC power management is completely handled internally by drivers.
    - Users who still require this API can include ``esp_private/adc_share_hw_ctrl.h`` to continue using these functions.

- ``driver/adc2_wifi_private.h`` has been moved to ``esp_private/adc_share_hw_ctrl.h``.
- Enums ``ADC_UNIT_BOTH``, ``ADC_UNIT_ALTER``, and ``ADC_UNIT_MAX`` in ``adc_unit_t`` have been removed.
- The following enumerations have been removed as some of their enumeration values are not supported on all chips. This would lead to the driver triggering a runtime error if an unsupported value is used.

    - Enum ``ADC_CHANNEL_MAX``
    - Enum ``ADC_ATTEN_MAX``
    - Enum ``ADC_CONV_UNIT_MAX``

- API ``hall_sensor_read`` on ESP32 has been removed. Hall sensor is no longer supported on ESP32.
- API ``adc_set_i2s_data_source`` and ``adc_i2s_mode_init`` have been deprecated. Related enum ``adc_i2s_source_t`` has been deprecated. Please migrate to use ``esp_adc/adc_continuous.h``.
- API ``adc_digi_filter_reset``, ``adc_digi_filter_set_config``, ``adc_digi_filter_get_config`` and ``adc_digi_filter_enable`` have been removed. These APIs behaviours are not guaranteed. Enum ``adc_digi_filter_idx_t``, ``adc_digi_filter_mode_t`` and structure ``adc_digi_iir_filter_t`` have been removed as well.
- API ``esp_adc_cal_characterize`` has been deprecated, please migrate to ``adc_cali_create_scheme_curve_fitting`` or ``adc_cali_create_scheme_line_fitting`` instead.
- API ``esp_adc_cal_raw_to_voltage`` has been deprecated, please migrate to ``adc_cali_raw_to_voltage`` instead.
- API ``esp_adc_cal_get_voltage`` has been deprecated, please migrate to ``adc_oneshot_get_calibrated_result`` instead.

GPIO
----

- The previous Kconfig option `RTCIO_SUPPORT_RTC_GPIO_DESC` has been removed, thus the ``rtc_gpio_desc`` array is unavailable. Please use ``rtc_io_desc`` array instead.

- The user callback of a GPIO interrupt should no longer read the GPIO interrupt status register to get the GPIO's pin number of triggering the interrupt. You should use the callback argument to determine the GPIO's pin number instead.

    - Previously, when a GPIO interrupt occurs, the GPIO's interrupt status register is cleared after calling the user callbacks. Thus, it was possible for users to read the GPIO's interrupt status register inside the callback to determine which GPIO was used to trigger the interrupt.
    - However, clearing the interrupt status register after calling the user callbacks can potentially cause edge-triggered interrupts to be lost. For example, if an edge-triggered interrupt is triggered/retriggered while the user callbacks are being called, that interrupt will be cleared without its registered user callback being handled.
    - Now, the GPIO's interrupt status register is cleared **before** invoking the user callbacks. Thus, users can no longer read the GPIO interrupt status register to determine which pin has triggered the interrupt. Instead, users should use the callback argument to pass the pin number.

.. only:: SOC_SDM_SUPPORTED

    Sigma-Delta Modulator
    ---------------------

    The Sigma-Delta Modulator driver has been redesigned into :doc:`SDM <../../../api-reference/peripherals/sdm>`.

    - The new driver implements a factory pattern, where the SDM channels are managed in a pool internally, thus users do not have to fix a SDM channel to a GPIO manually.
    - All SDM channels can be allocated dynamically.

    Although it is recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/sigmadelta.h``. However, by default, including ``driver/sigmadelta.h`` triggers the build warning below. The warning can be suppressed by Kconfig option :ref:`CONFIG_SDM_SUPPRESS_DEPRECATE_WARN`.

    .. code-block:: text

        The legacy sigma-delta driver is deprecated, please use driver/sdm.h

    The major breaking changes in concept and usage are listed as follows:

    Breaking Changes in Concepts
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - SDM channel representation has changed from ``sigmadelta_channel_t`` to :cpp:type:`sdm_channel_handle_t`, which is an opaque pointer.
    - SDM channel configurations are stored in :cpp:type:`sdm_config_t` now, instead the previous ``sigmadelta_config_t``.
    - In the legacy driver, users do not have to set the clock source for SDM channel. But in the new driver, users need to set a proper one in the :cpp:member:`sdm_config_t::clk_src`. The available clock sources are listed in the :cpp:type:`soc_periph_sdm_clk_src_t`.
    - In the legacy driver, users need to set a ``prescale`` for the channel, which reflects the frequency in which the modulator outputs a pulse. In the new driver, users should use :cpp:member:`sdm_config_t::sample_rate_hz` to set the over sample rate.
    - In the legacy driver, users set ``duty`` to decide the output analog value, it is now renamed to a more appropriate name ``density``.

    Breaking Changes in Usage
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    - Channel configuration was done by channel allocation, in :cpp:func:`sdm_new_channel`. In the new driver, only the ``density`` can be changed at runtime, by :cpp:func:`sdm_channel_set_pulse_density`. Other parameters like ``gpio number`` and ``prescale`` are only allowed to set during channel allocation.
    - Before further channel operations, users should **enable** the channel in advance, by calling :cpp:func:`sdm_channel_enable`. This function helps to manage some system level services, like **Power Management**.

Timer Group Driver
------------------

Timer Group driver has been redesigned into :doc:`GPTimer <../../../api-reference/peripherals/gptimer>`, which aims to unify and simplify the usage of general purpose timer.

Although it is recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/timer.h``. However, by default, including ``driver/timer.h`` triggers the build warning below. The warning can be suppressed by the Kconfig option :ref:`CONFIG_GPTIMER_SUPPRESS_DEPRECATE_WARN`.

.. code-block:: text

    legacy timer group driver is deprecated, please migrate to driver/gptimer.h

The major breaking changes in concept and usage are listed as follows:

Breaking Changes in Concepts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  ``timer_group_t`` and ``timer_idx_t`` which used to identify the hardware timer are removed from user's code. In the new driver, a timer is represented by :cpp:type:`gptimer_handle_t`.
-  Definition of timer clock source is moved to :cpp:type:`gptimer_clock_source_t`, the previous ``timer_src_clk_t`` is not used.
-  Definition of timer count direction is moved to :cpp:type:`gptimer_count_direction_t`, the previous ``timer_count_dir_t`` is not used.
-  Only level interrupt is supported, ``timer_intr_t`` and ``timer_intr_mode_t`` are not used.
-  Auto-reload is enabled by set the :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` flag. ``timer_autoreload_t`` is not used.

Breaking Changes in Usage
^^^^^^^^^^^^^^^^^^^^^^^^^

-  Timer initialization is done by creating a timer instance from :cpp:func:`gptimer_new_timer`. Basic configurations like clock source, resolution and direction should be set in :cpp:type:`gptimer_config_t`. Note that, specific configurations of alarm events are not needed during the installation stage of the driver.
-  Alarm event is configured by :cpp:func:`gptimer_set_alarm_action`, with parameters set in the :cpp:type:`gptimer_alarm_config_t`.
-  Setting and getting count value are done by :cpp:func:`gptimer_get_raw_count` and :cpp:func:`gptimer_set_raw_count`. The driver does not help convert the raw value into UTC time-stamp. Instead, the conversion should be done from user's side as the timer resolution is also known to the user.
-  The driver will install the interrupt service as well if :cpp:member:`gptimer_event_callbacks_t::on_alarm` is set to a valid callback function. In the callback, users do not have to deal with the low level registers (like "clear interrupt status", "re-enable alarm event" and so on). So functions like ``timer_group_get_intr_status_in_isr`` and ``timer_group_get_auto_reload_in_isr`` are not used anymore.
-  To update the alarm configurations when alarm event happens, one can call :cpp:func:`gptimer_set_alarm_action` in the interrupt callback, then the alarm will be re-enabled again.
-  Alarm will always be re-enabled by the driver if :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` is set to true.

UART
----

.. list-table::
    :width: 700 px
    :header-rows: 1

    * - Removed/Deprecated items
      - Replacement
      - Remarks
    * - ``uart_isr_register()``
      - None
      - UART interrupt handling is implemented by driver itself.
    * - ``uart_isr_free()``
      - None
      - UART interrupt handling is implemented by driver itself.
    * - ``use_ref_tick`` in :cpp:type:`uart_config_t`
      - :cpp:member:`uart_config_t::source_clk`
      - Select the clock source.
    * - ``uart_enable_pattern_det_intr()``
      - :cpp:func:`uart_enable_pattern_det_baud_intr`
      - Enable pattern detection interrupt.

I2C
---

.. list-table::
    :width: 700 px
    :header-rows: 1

    * - Removed/Deprecated items
      - Replacement
      - Remarks
    * - ``i2c_isr_register()``
      - None
      - I2C interrupt handling is implemented by driver itself.
    * - ``i2c_isr_register()``
      - None
      - I2C interrupt handling is implemented by driver itself.
    * - ``i2c_opmode_t``
      - None
      - It is not used anywhere in ESP-IDF.

SPI
---

.. list-table::
    :width: 700 px
    :header-rows: 1

    * - Removed/Deprecated items
      - Replacement
      - Remarks
    * - ``spi_cal_clock()``
      - :cpp:func:`spi_get_actual_clock`
      - Get SPI real working frequency.

- The internal header file ``spi_common_internal.h`` has been moved to ``esp_private/spi_common_internal.h``.

.. only:: SOC_SDMMC_HOST_SUPPORTED

    SDMMC
    -----

    .. list-table::
        :width: 700 px
        :header-rows: 1

        * - Removed/Deprecated items
          - Replacement
          - Remarks
        * - ``sdmmc_host_pullup_en()``
          - set ``SDMMC_SLOT_FLAG_INTERNAL_PULLUP`` flag in :cpp:member:`sdmmc_slot_config_t::flags`
          - Enable internal pull up.

LEDC
-----

.. list-table::
    :width: 700 px
    :header-rows: 1

    * - Removed/Deprecated items
      - Replacement
      - Remarks
    * - ``bit_num`` in :cpp:type:`ledc_timer_config_t`
      - :cpp:member:`ledc_timer_config_t::duty_resolution`
      - Set resolution of the duty cycle.

.. only:: SOC_PCNT_SUPPORTED

    Pulse Counter Driver
    --------------------

    Pulse counter driver has been redesigned (see :doc:`PCNT <../../../api-reference/peripherals/pcnt>`), which aims to unify and simplify the usage of PCNT peripheral.

    Although it is recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/pcnt.h``. However, including ``driver/pcnt.h`` triggers the build warning below by default. The warning can be suppressed by the Kconfig option :ref:`CONFIG_PCNT_SUPPRESS_DEPRECATE_WARN`.

    .. code-block:: text

        legacy pcnt driver is deprecated, please migrate to use driver/pulse_cnt.h

    The major breaking changes in concept and usage are listed as follows:

    Breaking Changes in Concepts
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - ``pcnt_port_t``, ``pcnt_unit_t`` and ``pcnt_channel_t`` which used to identify the hardware unit and channel are removed from user's code. In the new driver, PCNT unit is represented by :cpp:type:`pcnt_unit_handle_t`, likewise, PCNT channel is represented by :cpp:type:`pcnt_channel_handle_t`. Both of them are opaque pointers.
    - ``pcnt_evt_type_t`` is not used any more, they have been replaced by a universal **Watch Point Event**. In the event callback :cpp:type:`pcnt_watch_cb_t`, it is still possible to distinguish different watch points from :cpp:type:`pcnt_watch_event_data_t`.
    - ``pcnt_count_mode_t`` is replaced by :cpp:type:`pcnt_channel_edge_action_t`, and ``pcnt_ctrl_mode_t`` is replaced by :cpp:type:`pcnt_channel_level_action_t`.

    Breaking Changes in Usage
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    - Previously, the PCNT unit configuration and channel configuration were combined into a single function: ``pcnt_unit_config``. They are now split into the two factory APIs: :cpp:func:`pcnt_new_unit` and :cpp:func:`pcnt_new_channel` respectively.

        - Only the count range is necessary for initializing a PCNT unit. GPIO number assignment has been moved to :cpp:func:`pcnt_new_channel`.
        - High/Low control mode and positive/negative edge count mode are set by stand-alone functions: :cpp:func:`pcnt_channel_set_edge_action` and :cpp:func:`pcnt_channel_set_level_action`.

    - ``pcnt_get_counter_value`` is replaced by :cpp:func:`pcnt_unit_get_count`.
    - ``pcnt_counter_pause`` is replaced by :cpp:func:`pcnt_unit_stop`.
    - ``pcnt_counter_resume`` is replaced by :cpp:func:`pcnt_unit_start`.
    - ``pcnt_counter_clear`` is replaced by :cpp:func:`pcnt_unit_clear_count`.
    - ``pcnt_intr_enable`` and ``pcnt_intr_disable`` are removed. In the new driver, the interrupt is enabled by registering event callbacks :cpp:func:`pcnt_unit_register_event_callbacks`.
    - ``pcnt_event_enable`` and ``pcnt_event_disable`` are removed. In the new driver, the PCNT events are enabled/disabled by adding/removing watch points :cpp:func:`pcnt_unit_add_watch_point`, :cpp:func:`pcnt_unit_remove_watch_point`.
    - ``pcnt_set_event_value`` is removed. In the new driver, event value is also set when adding watch point by :cpp:func:`pcnt_unit_add_watch_point`.
    - ``pcnt_get_event_value`` and ``pcnt_get_event_status`` are removed. In the new driver, these information are provided by event callback :cpp:type:`pcnt_watch_cb_t` in the :cpp:type:`pcnt_watch_event_data_t`.
    - ``pcnt_isr_register`` and ``pcnt_isr_unregister`` are removed. Register of the ISR handler from user's code is no longer permitted. Users should register event callbacks instead by calling :cpp:func:`pcnt_unit_register_event_callbacks`.
    - ``pcnt_set_pin`` is removed and the new driver no longer allows the switching of the GPIO at runtime. If users want to change to other GPIOs, please delete the existing PCNT channel by :cpp:func:`pcnt_del_channel` and reinstall with the new GPIO number by :cpp:func:`pcnt_new_channel`.
    - ``pcnt_filter_enable``, ``pcnt_filter_disable`` and ``pcnt_set_filter_value`` are replaced by :cpp:func:`pcnt_unit_set_glitch_filter`. Meanwhile, ``pcnt_get_filter_value`` has been removed.
    - ``pcnt_set_mode`` is replaced by :cpp:func:`pcnt_channel_set_edge_action` and :cpp:func:`pcnt_channel_set_level_action`.
    - ``pcnt_isr_service_install``, ``pcnt_isr_service_uninstall``, ``pcnt_isr_handler_add`` and ``pcnt_isr_handler_remove`` are replaced by :cpp:func:`pcnt_unit_register_event_callbacks`. The default ISR handler is lazy installed in the new driver.

.. only:: SOC_TEMP_SENSOR_SUPPORTED

    Temperature Sensor Driver
    -------------------------

    The temperature sensor driver has been redesigned and it is recommended to use the new driver. However, the old driver is still available but cannot be used with the new driver simultaneously.

    The new driver can be included via ``driver/temperature_sensor.h``. The old driver is still available in the previous include path ``driver/temp_sensor.h``. However, including ``driver/temp_sensor.h`` triggers the build warning below by default. The warning can be suppressed by enabling the menuconfig option :ref:`CONFIG_TEMP_SENSOR_SUPPRESS_DEPRECATE_WARN`.

    .. code-block:: text

        legacy temperature sensor driver is deprecated, please migrate to driver/temperature_sensor.h

    Configuration contents has been changed. In the old version, users need to configure ``clk_div`` and ``dac_offset``. While in the new version, users only need to choose ``tsens_range``.

    The process of using temperature sensor has been changed. In the old version, users can use ``config->start->read_celsius`` to get value. In the new version, users should install the temperature sensor driver firstly, by ``temperature_sensor_install`` and uninstall it when finished. For more information, please refer to :doc:`Temperature Sensor <../../../api-reference/peripherals/temp_sensor>` .

.. only:: SOC_RMT_SUPPORTED

    RMT Driver
    ----------

    RMT driver has been redesigned (see :doc:`RMT transceiver <../../../api-reference/peripherals/rmt>`), which aims to unify and extend the usage of RMT peripheral.

    Although it is recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/rmt.h``. However, including ``driver/rmt.h`` triggers the build warning below by default. The warning can be suppressed by the Kconfig option :ref:`CONFIG_RMT_SUPPRESS_DEPRECATE_WARN`.

    .. code-block:: text

        The legacy RMT driver is deprecated, please use driver/rmt_tx.h and/or driver/rmt_rx.h

    The major breaking changes in concept and usage are listed as follows:

    Breaking Changes in Concepts
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - ``rmt_channel_t`` which used to identify the hardware channel are removed from user space. In the new driver, RMT channel is represented by :cpp:type:`rmt_channel_handle_t`. The channel is dynamically allocated by the driver, instead of designated by user.
    - ``rmt_item32_t`` is replaced by :cpp:type:`rmt_symbol_word_t`, which avoids a nested union inside a struct.
    - ``rmt_mem_t`` is removed, as we do not allow users to access RMT memory block (a.k.an RMTMEM) directly. Direct access to RMTMEM does not make sense but make mistakes, especially when the RMT channel also connected with a DMA channel.
    - ``rmt_mem_owner_t`` is removed, as the ownership is controlled by driver, not by user anymore.
    - ``rmt_source_clk_t`` is replaced by :cpp:type:`rmt_clock_source_t`, and note they are not binary compatible.
    - ``rmt_data_mode_t`` is removed, the RMT memory access mode is configured to always use Non-FIFO and DMA mode.
    - ``rmt_mode_t`` is removed, as the driver has stand alone install functions for TX and RX channels.
    - ``rmt_idle_level_t`` is removed, setting IDLE level for TX channel is available in :cpp:member:`rmt_transmit_config_t::eot_level`.
    - ``rmt_carrier_level_t`` is removed, setting carrier polarity is available in :cpp:member:`rmt_carrier_config_t::polarity_active_low`.
    - ``rmt_channel_status_t`` and ``rmt_channel_status_result_t`` are removed, they are not used anywhere.
    - Transmitting by RMT channel does not expect user to prepare the RMT symbols, instead, user needs to provide an RMT Encoder to tell the driver how to convert user data into RMT symbols.

    Breaking Changes in Usage
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    - Channel installation has been separated for TX and RX channels into :cpp:func:`rmt_new_tx_channel` and :cpp:func:`rmt_new_rx_channel`.
    - ``rmt_set_clk_div`` and ``rmt_get_clk_div`` are removed. Channel clock configuration can only be done during channel installation.
    - ``rmt_set_rx_idle_thresh`` and ``rmt_get_rx_idle_thresh`` are removed. In the new driver, the RX channel IDLE threshold is redesigned into a new concept :cpp:member:`rmt_receive_config_t::signal_range_max_ns`.
    - ``rmt_set_mem_block_num`` and ``rmt_get_mem_block_num`` are removed. In the new driver, the memory block number is determined by :cpp:member:`rmt_tx_channel_config_t::mem_block_symbols` and :cpp:member:`rmt_rx_channel_config_t::mem_block_symbols`.
    - ``rmt_set_tx_carrier`` is removed, the new driver uses :cpp:func:`rmt_apply_carrier` to set carrier behavior.
    - ``rmt_set_mem_pd`` and ``rmt_get_mem_pd`` are removed. The memory power is managed by the driver automatically.
    - ``rmt_memory_rw_rst``, ``rmt_tx_memory_reset`` and ``rmt_rx_memory_reset`` are removed. Memory reset is managed by the driver automatically.
    - ``rmt_tx_start`` and ``rmt_rx_start`` are merged into a single function :cpp:func:`rmt_enable`, for both TX and RX channels.
    - ``rmt_tx_stop`` and ``rmt_rx_stop`` are merged into a single function :cpp:func:`rmt_disable`, for both TX and RX channels.
    - ``rmt_set_memory_owner`` and ``rmt_get_memory_owner`` are removed. RMT memory owner guard is added automatically by the driver.
    - ``rmt_set_tx_loop_mode`` and ``rmt_get_tx_loop_mode`` are removed. In the new driver, the loop mode is configured in :cpp:member:`rmt_transmit_config_t::loop_count`.
    - ``rmt_set_source_clk`` and ``rmt_get_source_clk`` are removed. Configuring clock source is only possible during channel installation by :cpp:member:`rmt_tx_channel_config_t::clk_src` and :cpp:member:`rmt_rx_channel_config_t::clk_src`.
    - ``rmt_set_rx_filter`` is removed. In the new driver, the filter threshold is redesigned into a new concept :cpp:member:`rmt_receive_config_t::signal_range_min_ns`.
    - ``rmt_set_idle_level`` and ``rmt_get_idle_level`` are removed. Setting IDLE level for TX channel is available in :cpp:member:`rmt_transmit_config_t::eot_level`.
    - ``rmt_set_rx_intr_en``, ``rmt_set_err_intr_en``, ``rmt_set_tx_intr_en``, ``rmt_set_tx_thr_intr_en`` and ``rmt_set_rx_thr_intr_en`` are removed. The new driver does not allow user to turn on/off interrupt from user space. Instead, it provides callback functions.
    - ``rmt_set_gpio`` and ``rmt_set_pin`` are removed. The new driver does not support to switch GPIO dynamically at runtime.
    - ``rmt_config`` is removed. In the new driver, basic configuration is done during the channel installation stage.
    - ``rmt_isr_register`` and ``rmt_isr_deregister`` are removed, the interrupt is allocated by the driver itself.
    - ``rmt_driver_install`` is replaced by :cpp:func:`rmt_new_tx_channel` and :cpp:func:`rmt_new_rx_channel`.
    - ``rmt_driver_uninstall`` is replaced by :cpp:func:`rmt_del_channel`.
    - ``rmt_fill_tx_items``, ``rmt_write_items`` and ``rmt_write_sample`` are removed. In the new driver, user needs to provide an encoder to "translate" the user data into RMT symbols.
    - ``rmt_get_counter_clock`` is removed, as the channel clock resolution is configured by user from :cpp:member:`rmt_tx_channel_config_t::resolution_hz`.
    - ``rmt_wait_tx_done`` is replaced by :cpp:func:`rmt_tx_wait_all_done`.
    - ``rmt_translator_init``, ``rmt_translator_set_context`` and ``rmt_translator_get_context`` are removed. In the new driver, the translator has been replaced by the RMT encoder.
    - ``rmt_get_ringbuf_handle`` is removed. The new driver does not use Ringbuffer to save RMT symbols. Instead, the incoming data are saved to the user provided buffer directly. The user buffer can even be mounted to DMA link internally.
    - ``rmt_register_tx_end_callback`` is replaced by :cpp:func:`rmt_tx_register_event_callbacks`, where user can register :cpp:member:`rmt_tx_event_callbacks_t::on_trans_done` event callback.
    - ``rmt_set_intr_enable_mask`` and ``rmt_clr_intr_enable_mask`` are removed, as the interrupt is handled by the driver, user does not need to take care of it.
    - ``rmt_add_channel_to_group`` and ``rmt_remove_channel_from_group`` are replaced by RMT sync manager. Please refer to :cpp:func:`rmt_new_sync_manager`.
    - ``rmt_set_tx_loop_count`` is removed. The loop count in the new driver is configured in :cpp:member:`rmt_transmit_config_t::loop_count`.
    - ``rmt_enable_tx_loop_autostop`` is removed. In the new driver, TX loop auto stop is always enabled if available, it is not configurable anymore.

LCD
---

- The LCD panel initialization flow is slightly changed. Now the :cpp:func:`esp_lcd_panel_init` will not turn on the display automatically. User needs to call :cpp:func:`esp_lcd_panel_disp_on_off` to manually turn on the display. Note, this is different from turning on backlight. With this breaking change, user can flash a predefined pattern to the screen before turning on the screen. This can help avoid random noise on the screen after a power on reset.
- :cpp:func:`esp_lcd_panel_disp_off` is deprecated, please use :cpp:func:`esp_lcd_panel_disp_on_off` instead.
- ``dc_as_cmd_phase`` is removed. The SPI LCD driver currently does not support a 9-bit SPI LCD. Please always use a dedicated GPIO to control the LCD D/C line.
- The way to register RGB panel event callbacks has been moved from the :cpp:type:`esp_lcd_rgb_panel_config_t` into a separate API :cpp:func:`esp_lcd_rgb_panel_register_event_callbacks`. However, the event callback signature is not changed.
- Previous ``relax_on_idle`` flag in :cpp:type:`esp_lcd_rgb_panel_config_t` has been renamed into :cpp:member:`esp_lcd_rgb_panel_config_t::refresh_on_demand`, which expresses the same meaning but with a clear name.
- If the RGB LCD is created with the ``refresh_on_demand`` flag enabled, the driver will not start a refresh in the :cpp:func:`esp_lcd_panel_draw_bitmap`. Now users have to call :cpp:func:`esp_lcd_rgb_panel_refresh` to refresh the screen by themselves.
- :cpp:type:`esp_lcd_color_space_t` is deprecated, please use :cpp:type:`lcd_color_space_t` to describe the color space, and use :cpp:type:`lcd_rgb_element_order_t` to describe the data order of RGB color.

.. only:: SOC_MCPWM_SUPPORTED

    MCPWM
    -----

    MCPWM driver was redesigned (see :doc:`MCPWM <../../../api-reference/peripherals/mcpwm>`), meanwhile, the legacy driver is deprecated.

    The new driver's aim is to make each MCPWM submodule independent to each other, and give the freedom of resource connection back to users.

    Although it is recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/mcpwm.h``. However, using legacy driver triggers the build warning below by default. This warning can be suppressed by the Kconfig option :ref:`CONFIG_MCPWM_SUPPRESS_DEPRECATE_WARN`.

    .. code-block:: text

        legacy MCPWM driver is deprecated, please migrate to the new driver (include driver/mcpwm_prelude.h)

    The major breaking changes in concept and usage are listed as follows:

    Breaking Changes in Concepts
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The new MCPWM driver is object-oriented, where most of the MCPWM submodule has a driver object associated with it. The driver object is created by factory function like :cpp:func:`mcpwm_new_timer`. IO control function always needs an object handle, in the first place.

    The legacy driver has an inappropriate assumption, that is the MCPWM operator should be connected to different MCPWM timer. In fact, the hardware does not have such limitation. In the new driver, a MCPWM timer can be connected to multiple operators, so that the operators can achieve the best synchronization performance.

    The legacy driver presets the way to generate a PWM waveform into a so called ``mcpwm_duty_type_t``. However, the duty cycle modes listed there are far from sufficient. Likewise, legacy driver has several preset ``mcpwm_deadtime_type_t``, which also does not cover all the use cases. What is more, user usually gets confused by the name of the duty cycle mode and dead-time mode. In the new driver, there are no such limitation, but user has to construct the generator behavior from scratch.

    In the legacy driver, the ways to synchronize the MCPWM timer by GPIO, software and other timer module are not unified. It increased learning costs for users. In the new driver, the synchronization APIs are unified.

    The legacy driver has mixed the concepts of "Fault detector" and "Fault handler". Which make the APIs very confusing to users. In the new driver, the fault object just represents a failure source, and we introduced a new concept -- **brake** to express the concept of "Fault handler". What is more, the new driver supports software fault.

    The legacy drive only provides callback functions for the capture submodule. The new driver provides more useful callbacks for various MCPWM submodules, like timer stop, compare match, fault enter, brake, etc.

    - ``mcpwm_io_signals_t`` and ``mcpwm_pin_config_t`` are not used. GPIO configuration has been moved into submodule's configuration structure.
    - ``mcpwm_timer_t``, ``mcpwm_generator_t`` are not used. Timer and generator are represented by :cpp:type:`mcpwm_timer_handle_t` and :cpp:type:`mcpwm_gen_handle_t`.
    - ``mcpwm_fault_signal_t`` and ``mcpwm_sync_signal_t`` are not used. Fault and sync source are represented by :cpp:type:`mcpwm_fault_handle_t` and :cpp:type:`mcpwm_sync_handle_t`.
    - ``mcpwm_capture_signal_t`` is not used. A capture channel is represented by :cpp:type:`mcpwm_cap_channel_handle_t`.

    Breaking Changes in Usage
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    - ``mcpwm_gpio_init`` and ``mcpwm_set_pin``: GPIO configurations are moved to submodule's own configuration. e.g., set the PWM GPIO in :cpp:member:`mcpwm_generator_config_t::gen_gpio_num`.
    - ``mcpwm_init``: To get an expected PWM waveform, users need to allocated at least one MCPWM timer and MCPWM operator, then connect them by calling :cpp:func:`mcpwm_operator_connect_timer`. After that, users should set the generator's actions on various events by calling e.g., :cpp:func:`mcpwm_generator_set_actions_on_timer_event`, :cpp:func:`mcpwm_generator_set_actions_on_compare_event`.
    - ``mcpwm_group_set_resolution``: in the new driver, the group resolution is fixed to the maximum, usually it is 80 MHz.
    - ``mcpwm_timer_set_resolution``: MCPWM Timer resolution is set in :cpp:member:`mcpwm_timer_config_t::resolution_hz`.
    - ``mcpwm_set_frequency``: PWM frequency is determined by :cpp:member:`mcpwm_timer_config_t::resolution_hz`, :cpp:member:`mcpwm_timer_config_t::count_mode` and :cpp:member:`mcpwm_timer_config_t::period_ticks`.
    - ``mcpwm_set_duty``: To set the PWM duty cycle, users should call :cpp:func:`mcpwm_comparator_set_compare_value` to change comparator's threshold.
    - ``mcpwm_set_duty_type``: There is no preset duty cycle types. The duty cycle type is configured by setting different generator actions. e.g., :cpp:func:`mcpwm_generator_set_actions_on_timer_event`.
    - ``mcpwm_set_signal_high`` and ``mcpwm_set_signal_low`` are replaced by :cpp:func:`mcpwm_generator_set_force_level`. In the new driver, it is implemented by setting force action for the generator, instead of changing the duty cycle to 0% or 100% at the background.
    - ``mcpwm_start`` and ``mcpwm_stop`` are replaced by :cpp:func:`mcpwm_timer_start_stop`. You have more modes to start and stop the MCPWM timer, see :cpp:type:`mcpwm_timer_start_stop_cmd_t`.
    - ``mcpwm_carrier_init`` is replaced by :cpp:func:`mcpwm_operator_apply_carrier`.
    - ``mcpwm_carrier_enable`` and ``mcpwm_carrier_disable``: Enabling and disabling carrier submodule is done automatically by checking whether the carrier configuration structure :cpp:type:`mcpwm_carrier_config_t` is NULL.
    - ``mcpwm_carrier_set_period`` is replaced by :cpp:member:`mcpwm_carrier_config_t::frequency_hz`.
    - ``mcpwm_carrier_set_duty_cycle`` is replaced by :cpp:member:`mcpwm_carrier_config_t::duty_cycle`.
    - ``mcpwm_carrier_oneshot_mode_enable`` is replaced by :cpp:member:`mcpwm_carrier_config_t::first_pulse_duration_us`.
    - ``mcpwm_carrier_oneshot_mode_disable`` is removed. Disabling the first pulse (a.k.a the one-shot pulse) in the carrier is never supported by the hardware.
    - ``mcpwm_carrier_output_invert`` is replaced by :cpp:member:`mcpwm_carrier_config_t::invert_before_modulate` and :cpp:member:`mcpwm_carrier_config_t::invert_after_modulate`.
    - ``mcpwm_deadtime_enable`` and ``mcpwm_deadtime_disable`` are replaced by :cpp:func:`mcpwm_generator_set_dead_time`.
    - ``mcpwm_fault_init`` is replaced by :cpp:func:`mcpwm_new_gpio_fault`.
    - ``mcpwm_fault_set_oneshot_mode``, ``mcpwm_fault_set_cyc_mode`` are replaced by :cpp:func:`mcpwm_operator_set_brake_on_fault` and :cpp:func:`mcpwm_generator_set_actions_on_brake_event`.
    - ``mcpwm_capture_enable`` is removed. It is duplicated to :cpp:func:`mcpwm_capture_enable_channel`.
    - ``mcpwm_capture_disable`` is removed. It is duplicated to :cpp:func:`mcpwm_capture_capture_disable_channel`.
    - ``mcpwm_capture_enable_channel`` and ``mcpwm_capture_disable_channel`` are replaced by :cpp:func:`mcpwm_capture_channel_enable` and :cpp:func:`mcpwm_capture_channel_disable`.
    - ``mcpwm_capture_signal_get_value`` and ``mcpwm_capture_signal_get_edge``: Capture timer count value and capture edge are provided in the capture event callback, via :cpp:type:`mcpwm_capture_event_data_t`. Capture data are only valuable when capture event happens. Providing single API to fetch capture data is meaningless.
    - ``mcpwm_sync_enable`` is removed. It is duplicated to :cpp:func:`mcpwm_sync_configure`.
    - ``mcpwm_sync_configure`` is replaced by :cpp:func:`mcpwm_timer_set_phase_on_sync`.
    - ``mcpwm_sync_disable`` is equivalent to setting :cpp:member:`mcpwm_timer_sync_phase_config_t::sync_src` to ``NULL``.
    - ``mcpwm_set_timer_sync_output`` is replaced by :cpp:func:`mcpwm_new_timer_sync_src`.
    - ``mcpwm_timer_trigger_soft_sync`` is replaced by :cpp:func:`mcpwm_soft_sync_activate`.
    - ``mcpwm_sync_invert_gpio_synchro`` is equivalent to setting :cpp:member:`mcpwm_gpio_sync_src_config_t::active_neg`.
    - ``mcpwm_isr_register`` is removed. You can register various event callbacks instead. For example, to register capture event callback, users can use :cpp:func:`mcpwm_capture_channel_register_event_callbacks`.

.. only:: SOC_DEDICATED_GPIO_SUPPORTED

    Dedicated GPIO Driver
    ---------------------

    - All of the dedicated GPIO related Low Level (LL) functions in ``cpu_ll.h`` have been moved to ``dedic_gpio_cpu_ll.h`` and renamed.

.. only:: SOC_I2S_SUPPORTED

    I2S Driver
    ----------

    The I2S driver has been redesigned (see :doc:`I2S Driver <../../../api-reference/peripherals/i2s>`), which aims to rectify the shortcomings of the driver that were exposed when supporting all the new features of ESP32-C3 & ESP32-S3. The new driver's APIs are available by including corresponding I2S mode's header files :component_file:`esp_driver_i2s/include/driver/i2s_std.h`, :component_file:`esp_driver_i2s/include/driver/i2s_pdm.h`, or :component_file:`esp_driver_i2s/include/driver/i2s_tdm.h`.

    Meanwhile, the old driver's APIs in :component_file:`driver/deprecated/driver/i2s.h` are still supported for backward compatibility. But there will be warnings if users keep using the old APIs in their projects, these warnings can be suppressed by the Kconfig option :ref:`CONFIG_I2S_SUPPRESS_DEPRECATE_WARN`.

    Here is the general overview of the current I2S files:

    .. figure:: ../../../../_static/diagrams/i2s/i2s_file_structure.png
        :align: center
        :alt: I2S File Structure

    Breaking changes in Concepts
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Independent TX/RX channels
    """"""""""""""""""""""""""

    The minimum control unit in new I2S driver are now individual TX/RX channels instead of an entire I2S controller (that consists of multiple channels).

    - The TX and RX channels of the same I2S controller can be controlled separately, meaning that they are configured such that they can be started or stopped separately.
    - The :cpp:type:`i2s_chan_handle_t` handle type is used to uniquely identify I2S channels. All the APIs require the channel handle and users need to maintain the channel handles by themselves.
    - On the ESP32-C3 and ESP32-S3, TX and RX channels in the same controller can be configured to different clocks or modes.
    - However, on the ESP32 and ESP32-S2, the TX and RX channels of the same controller still share some hardware resources. Thus, configurations may cause one channel to affect another channel in the same controller.
    - The channels can be registered to an available I2S controller automatically by setting :cpp:enumerator:`i2s_port_t::I2S_NUM_AUTO` as I2S port ID which causes the driver to search for the available TX/RX channels. However, the driver also supports registering channels to a specific port.
    - In order to distinguish between TX/RX channels and sound channels, the term "channel" in the context of the I2S driver only refers to TX/RX channels. Meanwhile, sound channels are referred to as "slots".

    I2S Mode Categorization
    """"""""""""""""""""""""

    I2S communication modes are categorized into the following three modes. Note that:

    - **Standard mode**: Standard mode always has two slots, it can support Philips, MSB, and PCM (short frame sync) formats. Please refer to :component_file:`esp_driver_i2s/include/driver/i2s_std.h` for more details.
    - **PDM mode**: PDM mode only supports two slots with 16-bit data width, but the configurations of PDM TX and PDM RX are slightly different. For PDM TX, the sample rate can be set by :cpp:member:`i2s_pdm_tx_clk_config_t::sample_rate`, and its clock frequency depends on the up-sampling configuration. For PDM RX, the sample rate can be set by :cpp:member:`i2s_pdm_rx_clk_config_t::sample_rate`, and its clock frequency depends on the down-sampling configuration. Please refer to :component_file:`esp_driver_i2s/include/driver/i2s_pdm.h` for details.
    - **TDM mode**: TDM mode can support up to 16 slots. It can work in Philips, MSB, PCM (short frame sync), and PCM (long frame sync) formats. Please refer to :component_file:`esp_driver_i2s/include/driver/i2s_tdm.h` for details.

    When allocating a new channel in a specific mode, users should initialize that channel by its corresponding function. It is strongly recommended to use the helper macros to generate the default configurations in case the default values are changed in the future.

    Independent Slot and Clock Configuration
    """"""""""""""""""""""""""""""""""""""""

    The slot configurations and clock configurations can be configured separately.

    - Call :cpp:func:`i2s_channel_init_std_mode`, :cpp:func:`i2s_channel_init_pdm_rx_mode`, :cpp:func:`i2s_channel_init_pdm_tx_mode`, or :cpp:func:`i2s_channel_init_tdm_mode` to initialize the slot/clock/gpio_pin configurations.
    - Calling :cpp:func:`i2s_channel_reconfig_std_slot`, :cpp:func:`i2s_channel_reconfig_pdm_rx_slot`, :cpp:func:`i2s_channel_reconfig_pdm_tx_slot`, or :cpp:func:`i2s_channel_reconfig_tdm_slot` can change the slot configurations after initialization.
    - Calling :cpp:func:`i2s_channel_reconfig_std_clock`, :cpp:func:`i2s_channel_reconfig_pdm_rx_clock`, :cpp:func:`i2s_channel_reconfig_pdm_tx_clock`, or :cpp:func:`i2s_channel_reconfig_tdm_clock` can change the clock configurations after initialization.
    - Calling :cpp:func:`i2s_channel_reconfig_std_gpio`, :cpp:func:`i2s_channel_reconfig_pdm_rx_gpio`, :cpp:func:`i2s_channel_reconfig_pdm_tx_gpio`, or :cpp:func:`i2s_channel_reconfig_tdm_gpio` can change the GPIO configurations after initialization.

    Misc
    """"

    - States and state-machine are adopted in the new I2S driver to avoid APIs called in wrong state.
    - ADC and DAC modes are removed. They are only supported in their own drivers and the legacy I2S driver.

    Breaking Changes in Usage
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    To use the new I2S driver, please follow these steps:

    1. Call :cpp:func:`i2s_new_channel` to acquire channel handles. We should specify the work role and I2S port in this step. Besides, the TX or RX channel handle will be generated by the driver. Inputting both two TX and RX channel handles is not necessary but at least one handle is needed. In the case of inputting both two handles, the driver will work at the duplex mode. Both TX and RX channels will be available on a same port, and they will share the MCLK, BCLK and WS signal. But if only one of the TX or RX channel handle is inputted, this channel will only work in the simplex mode.
    2. Call :func:`i2s_channel_init_std_mode`, :func:`i2s_channel_init_pdm_rx_mode`, :func:`i2s_channel_init_pdm_tx_mode` or :func:`i2s_channel_init_tdm_mode` to initialize the channel to the specified mode. Corresponding slot, clock and GPIO configurations are needed in this step.
    3. (Optional) Call :cpp:func:`i2s_channel_register_event_callback` to register the ISR event callback functions. I2S events now can be received by the callback function synchronously, instead of from the event queue asynchronously.
    4. Call :cpp:func:`i2s_channel_enable` to start the hardware of I2S channel. In the new driver, I2S does not start automatically after installed, and users are supposed to know clearly whether the channel has started or not.
    5. Read or write data by :cpp:func:`i2s_channel_read` or :cpp:func:`i2s_channel_write`. Certainly, only the RX channel handle is suppoesd to be inputted in :cpp:func:`i2s_channel_read` and the TX channel handle in :cpp:func:`i2s_channel_write`.
    6. (Optional) The slot, clock and GPIO configurations can be changed by corresponding 'reconfig' functions, but :cpp:func:`i2s_channel_disable` must be called before updating the configurations.
    7. Call :cpp:func:`i2s_channel_disable` to stop the hardware of I2S channel.
    8. Call :cpp:func:`i2s_del_channel` to delete and release the resources of the channel if it is not needed any more, but the channel must be disabled before deleting it.

.. only:: SOC_TWAI_SUPPORTED

    TWAI Driver
    -----------

    The deprecated ``CAN`` peripheral driver is removed. Please use ``TWAI`` driver instead (i.e., include ``driver/twai.h`` in your application).

Register Access Macros
----------------------

Previously, all register access macros could be used as expressions, so the following was allowed::

    uint32_t val = REG_SET_BITS(reg, bits, mask);

In ESP-IDF v5.0, register access macros which write or read-modify-write the register can no longer be used as expressions, and can only be used as statements. This applies to the following macros: ``REG_WRITE``, ``REG_SET_BIT``, ``REG_CLR_BIT``, ``REG_SET_BITS``, ``REG_SET_FIELD``, ``WRITE_PERI_REG``, ``CLEAR_PERI_REG_MASK``, ``SET_PERI_REG_MASK``, ``SET_PERI_REG_BITS``.

To store the value which would have been written into the register, split the operation as follows::

    uint32_t new_val = REG_READ(reg) | mask;
    REG_WRITE(reg, new_val);

To get the value of the register after modification (which may be different from the value written), add an explicit read::

    REG_SET_BITS(reg, bits, mask);
    uint32_t new_val = REG_READ(reg);
