Migrate Peripherals to ESP-IDF 5.0
==================================

Peripheral Clock Gating
-----------------------

As usual, peripheral clock gating is still handled by driver itself, users don't need to take care of the peripheral module clock gating.

However, for advanced users who implement their own drivers based on ``hal`` and ``soc`` components, the previous clock gating include path has been changed from ``driver/periph_ctrl.h`` to ``esp_private/periph_ctrl.h``.

RTC Subsystem Control
---------------------

RTC control APIs have been moved from ``driver/rtc_cntl.h`` to ``esp_private/rtc_ctrl.h``.

SPI Flash Interface
-------------------

Version before v5.0, spi flash functions in rom can be included by ``esp32**/rom/spi_flash.h``. However, your code written for different chips may be filled with ROM headers of different versions. At the meantime not all the APIs can be used on all chips.

Therefore, the common APIs are extracted to ``esp_rom_spiflash.h``. Although it's not a breaking change, it is strongly recommended to only use the functions with prefix ``esp_rom_spiflash`` included by ``esp_rom_spiflash.h`` for better cross-compatibility.

To make the API clearer, we renamed the function ``esp_rom_spiflash_lock`` to ``esp_rom_spiflash_set_bp``. We renamed ``esp_rom_spiflash_unlock`` to ``esp_rom_spiflash_clear_bp``.

ENUM type ``esp_flash_speed_t`` has been deprecated. From now on, you can directly parse the real clock frequency value to the flash initialization structure. For example, if you want the flash frequency is 80M, you can write the code like:

.. code:: c

    esp_flash_spi_device_config_t dev_cfg = {
        // Other members
        .freq_mhz = 80,
        // Other members
    };

ADC
---

- Previous ``driver/adc2_wifi_private.h`` has been moved to ``esp_private/adc2_wifi.h``.
- Enums ``ADC_UNIT_BOTH``, ``ADC_UNIT_ALTER`` and ``ADC_UNIT_MAX`` in ``adc_unit_t`` are removed.
- Enum ``ADC_CHANNEL_MAX`` in ``adc_channel_t`` are removed. Some channels are not supported on some chips, driver will give a dynamic error if an unsupported channels are used.
- Enum ``ADC_ATTEN_MAX`` is removed. Some attenuations are not supported on some chips, driver will give a dynamic error if an unsupported attenuation is used.
- Enum ``ADC_CONV_UNIT_MAX`` is removed. Some convert mode are not supported on some chips, driver will give a dynamic error if an unsupported convert mode is used.

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

+-----------------------------------------------+-----------------------------------------------+----------------------------------------------------------+
| Removed/Deprecated items                      | Replacement                                   | Remarks                                                  |
+===============================================+===============================================+==========================================================+
| ``uart_isr_register()``                       | None                                          | UART interrupt handling is implemented by driver itself. |
+-----------------------------------------------+-----------------------------------------------+----------------------------------------------------------+
| ``uart_isr_free()``                           | None                                          | UART interrupt handling is implemented by driver itself. |
+-----------------------------------------------+-----------------------------------------------+----------------------------------------------------------+
| ``use_ref_tick`` in :cpp:type:`uart_config_t` | :cpp:member:`uart_config_t::source_clk`       | Select the clock source.                                 |
+-----------------------------------------------+-----------------------------------------------+----------------------------------------------------------+
| ``uart_enable_pattern_det_intr()``            | :cpp:func:`uart_enable_pattern_det_baud_intr` | Enable pattern detection interrupt.                      |
+-----------------------------------------------+-----------------------------------------------+----------------------------------------------------------+

I2C
---

+--------------------------+-------------+---------------------------------------------------------+
| Removed/Deprecated items | Replacement | Remarks                                                 |
+==========================+=============+=========================================================+
| ``i2c_isr_register()``   | None        | I2C interrupt handling is implemented by driver itself. |
+--------------------------+-------------+---------------------------------------------------------+
| ``i2c_isr_register()``   | None        | I2C interrupt handling is implemented by driver itself. |
+--------------------------+-------------+---------------------------------------------------------+
| ``i2c_opmode_t``         | None        | It's not used anywhere in esp-idf.                      |
+--------------------------+-------------+---------------------------------------------------------+

SPI
---

+--------------------------+----------------------------------+---------------------------------+
| Removed/Deprecated items | Replacement                      | Remarks                         |
+==========================+==================================+=================================+
| ``spi_cal_clock()``      | :cpp:func:`spi_get_actual_clock` | Get SPI real working frequency. |
+--------------------------+----------------------------------+---------------------------------+

.. only:: SOC_SDMMC_HOST_SUPPORTED

    SDMMC
    -----

    +----------------------------+----------------------------------------------+--------------------------+
    | Removed/Deprecated items   | Replacement                                  | Remarks                  |
    +============================+==============================================+==========================+
    | ``sdmmc_host_pullup_en()`` | set ``SDMMC_SLOT_FLAG_INTERNAL_PULLUP`` flag | Enable internal pull up. |
    |                            | in :cpp:member:`sdmmc_slot_config_t::flags`  |                          |
    +----------------------------+----------------------------------------------+--------------------------+

LEDC
-----

+------------------------------------------------+----------------------------------------------------+-----------------------------------+
| Removed/Deprecated items                       | Replacement                                        | Remarks                           |
+================================================+====================================================+===================================+
| ``bit_num`` in :cpp:type:`ledc_timer_config_t` | :cpp:member:`ledc_timer_config_t::duty_resolution` | Set resolution of the duty cycle. |
+------------------------------------------------+----------------------------------------------------+-----------------------------------+

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

    RMT driver has been redesigned (see :doc:`RMT transceiver <../api-reference/peripherals/rmt>`), which aims to unify and extend the usage of RMT peripheral. Although it's recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/rmt.h``. However, by default, including ``driver/rmt.h`` will bring a build warning like `The legacy RMT driver is deprecated, please use driver/rmt_tx.h and/or driver/rmt_rx.h`. The warning can be suppressed by the Kconfig option :ref:`CONFIG_RMT_SUPPRESS_DEPRECATE_WARN`.

    The major breaking changes in concept and usage are listed as follows:

    Breaking Changes in Concepts
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    - ``rmt_channel_t`` which used to identify the hardware channel are removed from user space. In the new driver, RMT channel is represented by :cpp:type:`rmt_channel_handle_t`. The channel is dynamic allocated by the driver, instead of designated by user.
    - ``rmt_item32_t`` is replaced by :cpp:type:`rmt_symbol_word_t`, which avoids a nested union inside a struct.
    - ``rmt_mem_t`` is removed, as we don't allow users to access RMT memory block (a.k.an RMTMEM) directly. Direct access to RMTMEM doesn't make sense but make mistakes, especially when the RMT channel also connected with a DMA channel.
    - ``rmt_mem_owner_t`` is removed, as the ownership is controller by driver, not by user anymore.
    - ``rmt_source_clk_t`` is replaced by :cpp:type:`rmt_clock_source_t`, note they're not binary compatible.
    - ``rmt_data_mode_t`` is removed, the RMT memory access mode is configured to always use Non-FIFO and DMA mode.
    - ``rmt_mode_t`` is removed, as the driver has stand alone install functions for TX and RX channels.
    - ``rmt_idle_level_t`` is removed, setting IDLE level for TX channel is available in :cpp:member:`rmt_transmit_config_t::eot_level`.
    - ``rmt_carrier_level_t`` is removed, setting carrier polarity is available in :cpp:member:`rmt_carrier_config_t::polarity_active_low`.
    - ``rmt_channel_status_t`` and ``rmt_channel_status_result_t`` are removed, they're not used anywhere.
    - transmitting by RMT channel doesn't expect user to prepare the RMT symbols, instead, user needs to provide an RMT Encoder to tell the driver how to convert user data into RMT symbols.


    Breaking Changes in Usage
    ~~~~~~~~~~~~~~~~~~~~~~~~~
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
    - ``rmt_set_rx_intr_en``, ``rmt_set_err_intr_en``, ``rmt_set_tx_intr_en``, ``rmt_set_tx_thr_intr_en`` and ``rmt_set_rx_thr_intr_en`` are removed. The new driver doesn't allow user to turn on/off interrupt from user space. Instead, it provides callback functions.
    - ``rmt_set_gpio`` and ``rmt_set_pin`` are removed. The new driver doesn't support to switch GPIO dynamically at runtime.
    - ``rmt_config`` is removed. In the new driver, basic configuration is done during the channel installation stage.
    - ``rmt_isr_register`` and ``rmt_isr_deregister`` are removed, the interrupt is allocated by the driver itself.
    - ``rmt_driver_install`` is replaced by :cpp:func:`rmt_new_tx_channel` and :cpp:func:`rmt_new_rx_channel`.
    - ``rmt_driver_uninstall`` is replaced by :cpp:func:`rmt_del_channel`.
    - ``rmt_fill_tx_items``, ``rmt_write_items`` and ``rmt_write_sample`` are removed. In the new driver, user needs to provide an encoder to "translate" the user data into RMT symbols.
    - ``rmt_get_counter_clock`` is removed, as the channel clock resolution is configured by user from :cpp:member:`rmt_tx_channel_config_t::resolution_hz`.
    - ``rmt_wait_tx_done`` is replaced by :cpp:func:`rmt_tx_wait_all_done`.
    - ``rmt_translator_init``, ``rmt_translator_set_context`` and ``rmt_translator_get_context`` are removed. In the new driver, the translator has been replaced by the RMT encoder.
    - ``rmt_get_ringbuf_handle`` is removed. The new driver doesn't use Ringbuffer to save RMT symbols. Instead, the incoming data are saved to the user provided buffer directly. The user buffer can even be mounted to DMA link internally.
    - ``rmt_register_tx_end_callback`` is replaced by :cpp:func:`rmt_tx_register_event_callbacks`, where user can register :cpp:member:`rmt_tx_event_callbacks_t::on_trans_done` event callback.
    - ``rmt_set_intr_enable_mask`` and ``rmt_clr_intr_enable_mask`` are removed, as the interrupt is handled by the driver, user doesn't need to take care of it.
    - ``rmt_add_channel_to_group`` and ``rmt_remove_channel_from_group`` are replaced by RMT sync manager. Please refer to :cpp:func:`rmt_new_sync_manager`.
    - ``rmt_set_tx_loop_count`` is removed. The loop count in the new driver is configured in :cpp:member:`rmt_transmit_config_t::loop_count`.
    - ``rmt_enable_tx_loop_autostop`` is removed. In the new driver, TX loop auto stop is always enabled if available, it's not configurable anymore.

LCD
---

- The LCD panel initialization flow is slightly changed. Now the :cpp:func:`esp_lcd_panel_init` won't turn on the display automatically. User needs to call :cpp:func:`esp_lcd_panel_disp_on_off` to manually turn on the display. Note, this is different from turning on backlight. With this breaking change, user can flush a predefined pattern to the screen before turning on the screen. This can help avoid random noise on the screen after a power on reset.
- :cpp:func:`esp_lcd_panel_disp_off` is deprecated, please use :cpp:func:`esp_lcd_panel_disp_on_off` instead.

.. only:: SOC_MCPWM_SUPPORTED

    MCPWM
    -----

    - ``mcpwm_capture_enable`` is removed. To enable capture channel, please use :cpp:func:`mcpwm_capture_enable_channel`.
    - ``mcpwm_capture_disable`` is remove. To disable capture channel, please use :cpp:func:`mcpwm_capture_capture_disable_channel`.
    - ``mcpwm_sync_enable`` is removed. To configure synchronization, please use :cpp:func:`mcpwm_sync_configure`.
    - ``mcpwm_isr_register`` is removed. You can register event callbacks, for capture channels. e.g. :cpp:member:`mcpwm_capture_config_t::capture_cb`.
    - ``mcpwm_carrier_oneshot_mode_disable`` is removed. Disable the first pulse (a.k.a the one-shot pulse) in the carrier is not supported by hardware.

.. only:: SOC_DEDICATED_GPIO_SUPPORTED

    Dedicated GPIO Driver
    ---------------------

    - All of the dedicated GPIO related LL functionsn in ``cpu_ll.h`` have been moved to ``dedic_gpio_cpu_ll.h`` and renamed.

I2S driver
----------

Since the old driver is unable to support all the new features on ESP32-C3 & ESP32-S3, I2S driver is re-designed to make it more compatibile and flexibile to all the communication modes. New APIs are available by including :component_file:`driver/include/driver/i2s_controller.h`. Meanwhile, the old APIs in :component_file:`driver/deprecated/driver/i2s.h` are still supported for backward compatibility. But there will be warnings if you keep using the old APIs in your project, these warnings can be suppressed by the Kconfig option :ref:`CONFIG_I2S_SUPPRESS_DEPRECATE_WARN`. Here is the general overview of the current I2S files:

.. figure:: ../../_static/diagrams/i2s/i2s_file_structure.png
    :align: center
    :alt: I2S File Structure

Breaking changes in Concepts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- The minimum control unit in new I2S driver will be tx/rx channel instead of a whole I2S controller.

    1. The tx/rx channel in a same I2S controller can be controlled separately, that means they will be initialized, started or stopped separately. Especially for ESP32-C3 and ESP32-S3, tx and rx channels in one controller can be configured to different clocks or modes now, they are able to work in a totally separate way which can help to save the resources of I2S controller. But for ESP32 and ESP32-S2, though their tx/rx can be controlled separately, some hardware resources are still shared by tx and rx, they might affect each other if they are configured to different configurations;
    2. The channels can be registered to an available I2S controller automatically by choosing :cpp:member:`i2s_port_t::I2S_NUM_AUTO` as I2S port id. The driver will help you to search for the available tx/rx channel. Of cause, you can still choose a specific port for it;
    3. :c:type:`i2s_chan_handle_t` is the handle that used for identifying the I2S channels. All the APIs will require the channel handle, users need to maintain the channel handles by themselves;
    4. In order to distinguish tx/rx channel and sound channel, now the word 'channel' is only stand for the tx/rx channel in new driver, meanwhile the sound channel will be called 'slot'.

- I2S communication modes are extracted into three modes.

    1. **Standard mode**: Standard mode always has two slots, it can support Philip, MSB and PCM(short sync) format, please refer to :component_file:`hal/include/hal/i2s_std.h` for details;
    2. **PDM mode**: PDM mode only support two slots with 16 bits data width, but the configurations of PDM TX and PDM RX are little bit different. For PDM TX, the sample rate can be set by :cpp:member:`i2s_pdm_tx_clk_config_t::sample_rate`, and its clock frequency is depended on the up-sampling configuration. For PDM RX, the sample rate can be set by :cpp:member:`i2s_pdm_rx_clk_config_t::sample_rate`, and its clock frequency is depended on the down-sampling configuration. Please refer to :component_file:`hal/include/hal/i2s_pdm.h` for details;
    3. **TDM mode**: TDM mode can support upto 16 slots. It can work in Philip, MSB, PCM(short sync) and PCM(long sync) format, please refer to :component_file:`hal/include/hal/i2s_tdm.h` for details;
    4. When we allocate a new channel in a specific mode, we must initialize this channel by corresponding slot configurations and clock configurations. We strongly recommend to use the helper macros to generate the default configurations, in case the default values will be changed one day.
    5. Although there are three modes, they still share some general slot and clock configurations which are defined in :component_file:`hal/include/hal/i2s_hal.h`

- States and state-machine are adopted in the new I2S driver to avoid APIs are called in wrong state.

- The slot configurations and clock configurations can be configured separately.

    1. Calling :func:`i2s_init_channel` to initialize the slot/clock/gpio_pin configurations;
    2. Calling :c:func:`i2s_set_slot` can change the slot configurations after initialization;
    3. Calling :c:func:`i2s_set_clock` can change the clock configurations after initialization.

- ADC and DAC modes are removed. They will only be supported in their own driver and legacy I2S driver.

- :c:func:`i2s_write_channel` and :c:func:`i2s_read_channel` can be aborted by :c:func:`i2s_abort_reading_writing` now.

Breaking Changes in Usage
~~~~~~~~~~~~~~~~~~~~~~~~~

To use the new I2S driver, please follow these steps:

1. Calling :c:func:`i2s_new_channel` to aquire the channel handles. We should specify the GPIO pins, work mode, work role and I2S port in this step. Besides, we need to input the tx or rx handle to acuire the channel handles that generated by the driver. We don't have to input both two tx and rx handles but at least one handle is needed. While we input both two handles, the driver will work in duplex mode, both tx and rx channel will be avaliable on a same port, and they will share the MCLK, BCLK and WS signal, there will be MCLK(optional), BCLK, WS, DATA_IN and DATA_OUT signals in this case. But if we only input tx or rx handle, this channel will only work in simplex mode.

2. Calling :c:func:`i2s_init_channel` to initialize the channel that specified by the given channel handle. We are supposed to input corresponding slot and clock configurations according to the mode that we set in the first step.

3. (Optional) We can acquire the event queue handle by :c:func:`i2s_get_event_queue` if we want to monitor the I2S event.

4. Calling :c:func:`i2s_start_channel` to start the I2S channel. In the new driver, I2S won't start automatically after installed anymore, we are supposed to know clearly whether the channel has started or not.

5. Reading or writing data by :c:func:`i2s_read_channel` or :c:func:`i2s_write_channel`. Of cause we can only use rx channel handle in :c:func:`i2s_read_channel` and tx channel handle in :c:func:`i2s_write_channel`.

6. (Optional) We can clear the legacy data in DMA buffer by :c:func:`i2s_clear_dma_buffer`. If we need the driver clear the tx dma buffer automatically, we can set :c:member`auto_clear` in slot configurations.

7. (Optional) We can change the slot and clock configurations after initialized by :c:func:`i2s_set_slot`, :c:func:`i2s_set_clock`, but we have to call :c:func:`i2s_stop_channel` before we update the configurations.

8. (Optional) We can delete the i2s channel by calling :c:func:`i2s_del_channel` if we don't need it any more. The related resources will be released if delete function is called.
