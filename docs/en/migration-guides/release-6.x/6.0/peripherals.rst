Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

Common Changes
--------------

All drivers' ``io_loop_back`` configuration have been removed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Different driver objects can share the same GPIO number, enabling more complex functionalities. For example, you can bind the TX and RX channels of the RMT peripheral to the same GPIO to simulate 1-Wire bus read and write timing. In previous versions, you needed to configure the ``io_loop_back`` setting in the driver to achieve this "loopback" functionality. Now, this configuration has been removed. Simply configuring the same GPIO number in different drivers will achieve the same functionality.

ADC
---

The legacy ADC driver ``driver/adc.h`` is deprecated since version 5.0 (see :ref:`deprecate_adc_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_adc`, and the header file path is ``esp_adc/adc_oneshot.h``, ``esp_adc/adc_continuous.h``, ``esp_adc/adc_cali.h`` and ``esp_adc/adc_cali_scheme.h``.

RMT
---

The ``io_od_mode`` member in the :cpp:type:`rmt_tx_channel_config_t` configuration structure has been removed. If you want to use open-drain mode, you need to manually call the :func:`gpio_od_enable` function.

.. only:: SOC_MCPWM_SUPPORTED

    MCPWM
    -----

    The ``io_od_mode`` member in the :cpp:type:`mcpwm_generator_config_t` configuration structure has been removed. If you want to use open-drain mode, you need to manually call the :func:`gpio_od_enable` function.

    The ``pull_up`` and ``pull_down`` members have been removed from the following configuration structures. You need to manually call the :func:`gpio_set_pull_mode` function to configure the pull-up and pull-down resistors for the IO:

    .. list::

        - :cpp:type:`mcpwm_generator_config_t`
        - :cpp:type:`mcpwm_gpio_fault_config_t`
        - :cpp:type:`mcpwm_gpio_sync_src_config_t`
        - :cpp:type:`mcpwm_capture_channel_config_t`

    The default MCPWM group clock divider has been changed to 1. This allows you to obtain a higher default resolution.

    Legacy MCPWM Driver is Removed
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    The legacy MCPWM driver ``driver/mcpwm.h`` is deprecated since version 5.0 (see :ref:`deprecate_mcpwm_legacy_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_driver_mcpwm`, and the header file path is ``driver/mcpwm_prelude``.

GPIO
----

:func:`gpio_iomux_in` and :func:`gpio_iomux_out` have been replaced by :func:`gpio_iomux_input` and :func:`gpio_iomux_output`, and have been moved to ``esp_private/gpio.h`` header file as private APIs for internal use only.

LEDC
----

- :func:`ledc_timer_set` has been removed. Use :func:`ledc_timer_config` or :func:`ledc_set_freq` instead.

- ``LEDC_APB_CLK_HZ`` and ``LEDC_REF_CLK_HZ`` have been removed.

- Removed esp_driver_gpio as a public required component from esp_driver_ledc.

- :func:`ledc_isr_register` has been deprecated. LEDC interrupt handling is implemented by driver itself, please only register event callbacks if necessary.

- :cpp:member:`ledc_channel_config_t::intr_type` has been deprecated. `LEDC_INTR_FADE_END` interrupt enable / disable control is handled by the driver internally. Users can still register a callback for this interrupt by :cpp:func:`ledc_cb_register`.

- :cpp:enumerator:`soc_periph_ledc_clk_src_legacy_t::LEDC_USE_RTC8M_CLK` has been removed. Please use ``LEDC_USE_RC_FAST_CLK`` instead.

UART
----

``UART_FIFO_LEN`` macro has been removed. Please use ``UART_HW_FIFO_LEN`` instead.

I2C
---

I2C slave has been redesigned in v5.4. In the current version, the old I2C slave driver has been removed. For details, please refer to the I2C slave section in the programming guide.

The major breaking changes in concept and usage are listed as follows:

Major Changes in Concepts
~~~~~~~~~~~~~~~~~~~~~~~~~

- Previously, the I2C slave driver performed active read and write operations. In the new version, these operations are handled passively via callbacks triggered by master events, aligning with standard I2C slave behavior.

Major Changes in Usage
~~~~~~~~~~~~~~~~~~~~~~

- ``i2c_slave_receive`` has been removed. In the new driver, data reception is handled via callbacks.
- ``i2c_slave_transmit`` has been replaced by ``i2c_slave_write``.
- ``i2c_slave_write_ram`` has been removed.
- ``i2c_slave_read_ram`` has been removed.

Meanwhile, I2C master also has some change in its APIs' definitions.

Major Changes in Usage
~~~~~~~~~~~~~~~~~~~~~~

Following functions now will return ``ESP_ERR_INVALID_RESPONSE`` instead of ``ESP_ERR_INVALID_STATE`` when NACK from the bus is detected:
- ``i2c_master_transmit``
- ``i2c_master_multi_buffer_transmit``
- ``i2c_master_transmit_receive``
- ``i2c_master_execute_defined_operations``

Legacy Timer Group Driver is Removed
------------------------------------

The legacy timer group driver ``driver/timer.h`` is deprecated since version 5.0 (see :ref:`deprecate_gptimer_legacy_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_driver_gptimer`, and the header file path is ``driver/gptimer.h``.

.. only:: SOC_I2S_SUPPORTED

    Legacy I2S Driver is Removed
    ------------------------------------

    - The legacy i2s driver ``driver/i2s.h`` is deprecated since version 5.0 (see :ref:`deprecate_i2s_legacy_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_driver_i2s`, and the header file path is ``driver/i2s_std.h``, ``driver/i2s_pdm.h`` and ``driver/i2s_tdm.h``.
    - API ``i2s_set_adc_mode``,  ``i2s_adc_enable`` and ``i2s_adc_disable`` are deprecated since version 5.0. Starting from version 6.0, these APIs are completely removed.

.. only:: SOC_PCNT_SUPPORTED

    Legacy PCNT Driver is Removed
    ------------------------------------

    The legacy PCNT driver ``driver/pcnt.h`` is deprecated since version 5.0 (see :ref:`deprecate_pcnt_legacy_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_driver_pcnt`, and the header file path is ``driver/pulse_cnt.h``.

GDMA
----

- The ``GDMA_ISR_IRAM_SAFE`` Kconfig option has been removed due to potential risks. Now, the interrupt behavior of different DMA channels during Cache disabled periods are independent of each other.
- ``gdma_new_channel`` is removed. When requesting a GDMA channel, use either ``gdma_new_ahb_channel`` or ``gdma_new_axi_channel`` according to the bus type.
- The ``sram_trans_align`` and ``psram_trans_align`` members have been removed from :cpp:type:`async_memcpy_config_t`. Use :cpp:member:`async_memcpy_config_t::dma_burst_size` to set the DMA burst transfer size.
- The ``esp_dma_capable_malloc`` and ``esp_dma_capable_calloc`` functions have been removed. Use :cpp:func:`heap_caps_malloc` and :cpp:func:`heap_caps_calloc` from :component_file:`heap/include/esp_heap_caps.h` with ``MALLOC_CAP_DMA|MALLOC_CAP_CACHE_ALIGNED`` to allocate memory suitable for DMA and cache alignment.

SDMMC
-----

- The ``get_dma_info`` member in the :cpp:type:`sdmmc_host_t` structure, as well as the ``sdspi_host_get_dma_info`` and ``sdmmc_host_get_dma_info`` functions, have been removed. DMA configuration is now handled internally by the driver.

.. only:: SOC_DAC_SUPPORTED

    Legacy DAC Driver is Removed
    ------------------------------------

    The legacy DAC driver ``driver/dac.h`` is deprecated since version 5.1 (see :ref:`deprecate_dac_legacy_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_driver_dac`, and the header file path is ``driver/dac_oneshot.h``, ``driver/dac_continuous.h`` and ``driver/dac_cosine.h``.

.. only:: SOC_TEMP_SENSOR_SUPPORTED

    Legacy Temperature Sensor Driver is Removed
    -------------------------------------------

    The legacy temperature sensor driver ``driver/temp_sensor.h`` is deprecated since version 5.0 (see :ref:`deprecate_tsens_legacy_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_driver_tsens`, and the header file path is ``driver/temperature_sensor.h``.

.. only:: SOC_SDM_SUPPORTED

    Legacy Sigma-Delta Modulator Driver is Removed
    ----------------------------------------------

    The legacy Sigma-Delta Modulator driver ``driver/sigmadelta.h`` is deprecated since version 5.0 (see :ref:`deprecate_sdm_legacy_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_driver_sdm`, and the header file path is ``driver/sdm.h``.

LCD
---

- The GPIO number type in the LCD driver has been changed from ``int`` to the more type-safe ``gpio_num_t``. For example, instead of using ``5`` as the GPIO number, you now need to use ``GPIO_NUM_5``.
- The ``psram_trans_align`` and ``sram_trans_align`` members in the :cpp:type:`esp_lcd_i80_bus_config_t` structure have been replaced by the :cpp:member:`esp_lcd_i80_bus_config_t::dma_burst_size` member, which sets the DMA burst transfer size.
- The ``psram_trans_align`` and ``sram_trans_align`` members in the :cpp:type:`esp_lcd_rgb_panel_config_t` structure have also been replaced by the :cpp:member:`esp_lcd_rgb_panel_config_t::dma_burst_size` member for configuring the DMA burst transfer size.
- The ``octal_mode`` and ``quad_mode`` flags in the :cpp:type:`esp_lcd_panel_io_spi_config_t` structure have been removed. The driver now automatically detects the data line mode of the current SPI bus.
- The ``color_space`` and ``rgb_endian`` configuration options in the :cpp:type:`esp_lcd_panel_dev_config_t` structure have been replaced by the :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` member, which sets the RGB element order. The corresponding types ``lcd_color_rgb_endian_t`` and ``esp_lcd_color_space_t`` have also been removed; use :cpp:type:`lcd_rgb_element_order_t` instead.
- The ``esp_lcd_panel_disp_off`` function has been removed. Please use the :func:`esp_lcd_panel_disp_on_off` function to control display on/off.
- The ``on_bounce_frame_finish`` member in :cpp:type:`esp_lcd_rgb_panel_event_callbacks_t` has been replaced by :cpp:member:`esp_lcd_rgb_panel_event_callbacks_t::on_frame_buf_complete`, which indicates that a complete frame buffer has been sent to the LCD controller.

SPI
---

The :ref:`CONFIG_SPI_MASTER_IN_IRAM` option is now invisible by default in menuconfig and depends on :ref:`CONFIG_FREERTOS_IN_IRAM`. This change was made to prevent potential crashes when SPI functions in IRAM call FreeRTOS functions that are placed in flash.

To enable SPI master IRAM optimization:

1. Navigate to ``Component config`` → ``FreeRTOS`` → ``Port`` in menuconfig
2. Enable ``Place FreeRTOS functions in IRAM`` (:ref:`CONFIG_FREERTOS_IN_IRAM`)
3. Navigate to ``Component config`` → ``ESP-Driver:SPI Configurations``
4. Enable ``Place transmitting functions of SPI master into IRAM`` (:ref:`CONFIG_SPI_MASTER_IN_IRAM`)

Note that enabling :ref:`CONFIG_FREERTOS_IN_IRAM` will increase IRAM usage. Consider this trade-off when optimizing for SPI performance.
