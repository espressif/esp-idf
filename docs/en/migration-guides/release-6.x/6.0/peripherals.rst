Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

Common Changes
--------------

Legacy Driver Dependencies Removal
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The legacy driver component :component:`driver` has been deprecated and no longer contains public dependencies on the following driver components:

.. list::
    :class: no-bullet

    - :component:`esp_driver_ana_cmpr`
    - :component:`esp_driver_dac`
    - :component:`esp_driver_gptimer`
    - :component:`esp_driver_i2s`
    - :component:`esp_driver_ledc`
    - :component:`esp_driver_mcpwm`
    - :component:`esp_driver_parlio`
    - :component:`esp_driver_pcnt`
    - :component:`esp_driver_rmt`
    - :component:`esp_driver_sdio`
    - :component:`esp_driver_sdm`
    - :component:`esp_driver_sdmmc`
    - :component:`esp_driver_sdspi`
    - :component:`esp_driver_spi`
    - :component:`esp_driver_tsens`
    - :component:`esp_driver_twai`
    - :component:`esp_driver_uart`
    - :component:`esp_driver_usb_serial_jtag`

If your project uses legacy :component:`driver` component, it is strongly recommended to remove :component:`driver` component dependencies, and add new driver component (usually ``esp_driver_xxx``) dependencies to your project.

If you still need to keep legacy :component:`driver` component (e.g. your project depends on the legacy ``i2c`` driver), please keep the :component:`driver` component remaining in the dependency list of your project (usually ``<project_root>/main/CMakeLists.txt``), and manually add component dependencies that are no longer included in :component:`driver` as needed.

All drivers' ``io_loop_back`` configuration have been removed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Different driver objects can share the same GPIO number, enabling more complex functionalities. For example, you can bind the TX and RX channels of the RMT peripheral to the same GPIO to simulate 1-Wire bus read and write timing. In previous versions, you needed to configure the ``io_loop_back`` setting in the driver to achieve this "loopback" functionality. Now, this configuration has been removed. Simply configuring the same GPIO number in different drivers will achieve the same functionality.

Peripheral Clock Gating
~~~~~~~~~~~~~~~~~~~~~~~~

Peripheral clock gating is managed within the driver layer. Users do not need to manually handle peripheral module clock gating. Its corresponding APIs are included via a private header file ``esp_private/periph_ctrl.h``. There used to be another header file ``driver/periph_ctrl.h`` for the same purpose, which is now removed.

RTC Subsystem Control
~~~~~~~~~~~~~~~~~~~~~~

Low power modules usually share some common resources like interrupt number. To avoid conflicts, some private APIs are created in the ``esp_private/rtc_ctrl.h`` header file to manage these shared resources with ease. There used to be another header file ``driver/rtc_cntl.h`` for the same purpose, which is now removed.

Removal of FreeRTOS Dependencies from Driver Header Files
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Starting from v6.0, to improve the portability of IDF drivers, all public driver header files no longer include operating-system–specific (FreeRTOS) headers. Therefore, application code that previously relied on this implicit inclusion must explicitly include the corresponding FreeRTOS headers when using v6.0 or later.

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

    Variadic Generator APIs are Removed
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    The legacy variadic ("varg") generator APIs have been removed in this release. Code that used vararg-style calls for generator action setup must be migrated to the explicit, typed APIs. These APIs use configuration structs and clearly-typed setter functions (for example, :cpp:type:`mcpwm_generator_config_t`, :cpp:type:`mcpwm_gen_timer_event_action_t` and :cpp:type:`mcpwm_generator_set_action_on_timer_event`).

    Migration steps (summary):

    - Replace varg-style action configuration with helper structs/macros and dedicated setter functions::

        .. code-block:: c

            /* Old (varg) */
            mcpwm_generator_set_actions_on_compare_event(my_generator,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, my_comparator, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, my_comparator, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END());

            /* New */
            mcpwm_generator_set_action_on_compare_event(my_generator,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, my_comparator, MCPWM_GEN_ACTION_LOW));
            mcpwm_generator_set_action_on_compare_event(my_generator,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, my_comparator, MCPWM_GEN_ACTION_HIGH));

GPIO
----

- :func:`gpio_iomux_in` and :func:`gpio_iomux_out` have been replaced by :func:`gpio_iomux_input` and :func:`gpio_iomux_output`, and have been moved to ``esp_private/gpio.h`` header file as private APIs for internal use only.

- ``rom_`` prefix has been added to all GPIO ROM APIs in ``components/esp_rom/esp32xx/include/esp32xx/rom/gpio.h``. For example, :func:`gpio_iomux_in` is now :func:`rom_gpio_iomux_in`.

- ``MAX_PAD_GPIO_NUM``, ``MAX_GPIO_NUM``, and ``DIG_IO_HOLD_BIT_SHIFT`` macros have been removed.

- Added the :cpp:type:`esp_err_t` return type to :func:`gpio_uninstall_isr_service`.

LEDC
----

- :func:`ledc_timer_set` has been removed. Use :func:`ledc_timer_config` or :func:`ledc_set_freq` instead.

- ``LEDC_APB_CLK_HZ`` and ``LEDC_REF_CLK_HZ`` have been removed.

- ``LEDC_SLOW_CLK_RTC8M`` macro has been removed. Please use ``LEDC_SLOW_CLK_RC_FAST`` instead.

- Removed esp_driver_gpio as a public required component from esp_driver_ledc.

- :func:`ledc_isr_register` has been deprecated. LEDC interrupt handling is implemented by driver itself, please only register event callbacks if necessary.

- :cpp:member:`ledc_channel_config_t::intr_type` has been deprecated. `LEDC_INTR_FADE_END` interrupt enable / disable control is handled by the driver internally. Users can still register a callback for this interrupt by :cpp:func:`ledc_cb_register`.

- :cpp:enumerator:`soc_periph_ledc_clk_src_legacy_t::LEDC_USE_RTC8M_CLK` has been removed. Please use ``LEDC_USE_RC_FAST_CLK`` instead.

UART
----

- ``UART_FIFO_LEN`` macro has been removed. Please use ``UART_HW_FIFO_LEN`` instead.

- ``soc/uart_channel.h`` header file has been removed. All UART GPIO lookup macros can be found in ``soc/uart_pins.h``. For example, ``UART_NUM_0_TXD_DIRECT_GPIO_NUM`` is equivalent to ``U0TXD_GPIO_NUM``.

I2C
---

Legacy I2C Driver End-of-Life
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. warning::

    The legacy I2C driver (``driver/i2c.h``) has been marked as **End-of-Life (EOL)** in ESP-IDF v6.0 and is scheduled for **removal in v7.0**.

    - ESP-IDF will not provide updates, bug fixes, or security patches for the legacy driver timely.
    - Users are strongly recommended to migrate to the new I2C drivers: ``driver/i2c_master.h`` and ``driver/i2c_slave.h``.
    - To temporarily suppress the compile-time warning, enable ``Component config`` > ``Legacy Driver Configurations`` > ``Legacy I2C Driver Configurations`` > ``Suppress legacy driver deprecated warning`` in menuconfig.

The new I2C drivers provide improved slave and master functionality. For details, please refer to the :ref:`I2C Migration Guide <migration_guide_i2c_driver_5_2>` and the :doc:`I2C Driver Programming Guide <../../../api-reference/peripherals/i2c>`.

I2C Slave Driver Updates
~~~~~~~~~~~~~~~~~~~~~~~~~

The I2C slave driver has been redesigned in v5.4. In the current version, the old I2C slave driver has been removed.

Major Changes in Concepts
^^^^^^^^^^^^^^^^^^^^^^^^^

- Previously, the I2C slave driver performed active read and write operations. In the new version, these operations are handled passively via callbacks triggered by master events, aligning with standard I2C slave behavior.

Major Changes in Usage
^^^^^^^^^^^^^^^^^^^^^^

- ``i2c_slave_receive`` has been removed. In the new driver, data reception is handled via callbacks.
- ``i2c_slave_transmit`` has been replaced by ``i2c_slave_write``.
- ``i2c_slave_write_ram`` has been removed.
- ``i2c_slave_read_ram`` has been removed.

I2C Master Driver Updates
~~~~~~~~~~~~~~~~~~~~~~~~~~

The I2C master driver also has some changes in its API definitions.

Major Changes in Usage
^^^^^^^^^^^^^^^^^^^^^^

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

.. only:: SOC_RMT_SUPPORTED

    Legacy RMT Driver is Removed
    -------------------------------

    The legacy RMT driver ``driver/rmt.h`` is deprecated since version 5.0 (see :ref:`deprecate_rmt_legacy_driver`). Starting from version 6.0, the legacy driver is completely removed. The new driver is placed in the :component:`esp_driver_rmt`, and the header file path is ``driver/rmt_tx.h``, ``driver/rmt_rx.h`` and ``driver/rmt_encoder.h``.

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

    - :func:`sdm_channel_set_duty` has been removed. Use :func:`sdm_channel_set_pulse_density` instead.

LCD
---

- The GPIO number type in the LCD driver has been changed from ``int`` to the more type-safe ``gpio_num_t``. For example, instead of using ``5`` as the GPIO number, you now need to use ``GPIO_NUM_5``.
- The ``psram_trans_align`` and ``sram_trans_align`` members in the :cpp:type:`esp_lcd_i80_bus_config_t` structure have been replaced by the :cpp:member:`esp_lcd_i80_bus_config_t::dma_burst_size` member, which sets the DMA burst transfer size.
- The ``psram_trans_align`` and ``sram_trans_align`` members in the :cpp:type:`esp_lcd_rgb_panel_config_t` structure have also been replaced by the :cpp:member:`esp_lcd_rgb_panel_config_t::dma_burst_size` member for configuring the DMA burst transfer size.
- The ``color_space`` and ``rgb_endian`` configuration options in the :cpp:type:`esp_lcd_panel_dev_config_t` structure have been replaced by the :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` member, which sets the RGB element order. The corresponding types ``lcd_color_rgb_endian_t`` and ``esp_lcd_color_space_t`` have also been removed; use :cpp:type:`lcd_rgb_element_order_t` instead.
- The ``esp_lcd_panel_disp_off`` function has been removed. Please use the :func:`esp_lcd_panel_disp_on_off` function to control display on/off.
- The ``on_bounce_frame_finish`` member in :cpp:type:`esp_lcd_rgb_panel_event_callbacks_t` has been replaced by :cpp:member:`esp_lcd_rgb_panel_event_callbacks_t::on_frame_buf_complete`, which indicates that a complete frame buffer has been sent to the LCD controller.
- The LCD IO layer driver for the I2C interface previously had two implementations, based on the new and legacy I2C master bus drivers. As the legacy I2C driver is being deprecated, support for it in the LCD IO layer has been removed. Only the APIs provided in ``driver/i2c_master.h`` are now used.
- ``pixel_format`` member in the :cpp:type:`esp_lcd_dpi_panel_config_t` structure has been removed. It is recommended to only use :cpp:member:`esp_lcd_dpi_panel_config_t::in_color_format` to set the MIPI DSI driver's input pixel data format.
- ``bits_per_pixel`` member in the :cpp:type:`esp_lcd_rgb_panel_config_t` structure has been removed. The color depth of the internal framebuffer is now determined by the :cpp:member:`esp_lcd_rgb_panel_config_t::in_color_format` member.
- ``esp_lcd_dpi_panel_set_color_conversion`` function is replaced by :cpp:func:`esp_lcd_dpi_panel_set_yuv_conversion` to set YUV to RGB color conversion profile.
- :cpp:func:`esp_lcd_rgb_panel_set_yuv_conversion` function has a different signature. The ``esp_lcd_yuv_conv_config_t`` configuration type is now replaced by :cpp:type:`esp_lcd_color_conv_yuv_config_t`.
- The NT35510 LCD device driver has been moved out of ESP-IDF and is now hosted in the `ESP Component Registry <https://components.espressif.com/components/espressif/esp_lcd_nt35510/versions/1.0.0/readme>`__. If your project uses the NT35510 driver, you can add it to your project by running ``idf.py add-dependency "espressif/esp_lcd_nt35510"``.
- The ``use_dma2d`` member in the :cpp:type:`esp_lcd_dpi_panel_config_t` has been removed. Please use the :func:`esp_lcd_dpi_panel_enable_dma2d` function to enable DMA2D for the DPI panel. When not using DMA2D, the binary file size can be reduced by around 10KB.

SPI
---

- The :ref:`CONFIG_SPI_MASTER_IN_IRAM` option is now invisible by default in menuconfig and depends on :ref:`CONFIG_FREERTOS_IN_IRAM`. This change was made to prevent potential crashes when SPI functions in IRAM call FreeRTOS functions that are placed in flash.
- To enable SPI master IRAM optimization:

    1. Navigate to ``Component config`` → ``FreeRTOS`` → ``Port`` in menuconfig.
    2. Enable ``Place FreeRTOS functions in IRAM`` (:ref:`CONFIG_FREERTOS_IN_IRAM`).
    3. Navigate to ``Component config`` → ``ESP-Driver:SPI Configurations`` in menuconfig.
    4. Enable ``Place transmitting functions of SPI master into IRAM`` (:ref:`CONFIG_SPI_MASTER_IN_IRAM`).

    .. note::

        Note that enabling :ref:`CONFIG_FREERTOS_IN_IRAM` will increase IRAM usage. Consider this trade-off when optimizing for SPI performance.

- Deprecated HSPI and VSPI related IOMUX pin macros on ESP32 and ESP32S2 have been removed.

PSRAM
-----

Deprecated header file ``esp_spiram.h`` has been removed. Please use ``esp_psram.h`` instead.

SPI Flash Driver
----------------

- Deprecated ``enum`` type ``esp_flash_speed_t`` has been removed. The main flash speed is controlled by :ref:`CONFIG_ESPTOOLPY_FLASHFREQ` option.
- Deprecated header file ``esp_spi_flash.h`` has been removed. Please use ``spi_flash_mmap.h`` instead.
- Deprecated API ``spi_flash_dump_counters`` has been removed. Please use :cpp:func:`esp_flash_dump_counters` instead.
- Deprecated API ``spi_flash_get_counters`` has been removed. Please use :cpp:func:`esp_flash_get_counters` instead.
- Deprecated API ``spi_flash_reset_counters`` has been removed. Please use :cpp:func:`esp_flash_reset_counters` instead.
- New argument ``flags`` is added to ``esp_flash_os_functions_t::start``. Caller and implementer should handle this argument properly.
- Kconfig option ``CONFIG_SPI_FLASH_ROM_DRIVER_PATCH`` has been removed. Considering that this option is unlikely to be widely used by users and may cause serious issues if misused, it has been decided to remove it.

.. note::

    Note that enabling :ref:`CONFIG_FREERTOS_IN_IRAM` will increase IRAM usage. Consider this trade-off when optimizing for SPI performance.

Touch Element
-------------

The ``touch_element`` component is moved to `ESP Component Registry <https://components.espressif.com/components/espressif/touch_element/versions/1.0.0/readme>`__.

You can add this dependency to your project by running ``idf.py add-dependency "espressif/touch_element"``.

Touch Sensor
------------

The ``touch_sensor_sample_config_t::bypass_shield_output`` member for version 3 touch sensor has been removed because it is not supported in the version 3 hardware.

The dependencies of legacy touch sensor driver are removed from ULP touch driver, now you need to use ``int`` instead of ``touch_pad_t`` to indicate the touch channel ID.

I2S
---

- ``i2s_port_t`` type has been removed. Please use ``int`` type instead. Its enum items ``I2S_NUM_0``, ``I2S_NUM_1``, ``I2S_NUM_2`` and ``I2S_NUM_AUTO`` have been replaced by macro definitions to ensure compatibility.

USB
---

The ``usb`` component has been moved to `ESP Component Registry <https://components.espressif.com/components/espressif/usb>`__.

You can add this dependency to your project by running ``idf.py add-dependency "espressif/usb"``.

.. only:: SOC_TWAI_SUPPORTED

    TWAI
    ----

    TWAI has provided a new driver interface in version 5.5, which supports more flexible configurations and richer features. The legacy driver is not recommended to be used anymore. Please refer to the 5.5 migration guide :doc:`TWAI migration guide <../../release-5.x/5.5/peripherals>` and the new driver programming guide :doc:`TWAI driver programming guide <../../../api-reference/peripherals/twai>` for migration.

    If you still need to use the legacy driver, you can enable the configuration option :ref:`CONFIG_TWAI_SUPPRESS_DEPRECATE_WARN` to close the deprecation warnings.
