外设驱动
========

:link_to_translation:`en:[English]`

公共变化
--------

所有驱动的 ``io_loop_back`` 配置已被移除
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

不同的驱动对象可以共享同一个 GPIO 编号，联合起来可以实现更加复杂的功能。比如将 RMT 外设的 TX 通道和 RX 通道绑定在同一个 GPIO 上，进而模拟单总线的读写时序。在以前的版本中，你需要在驱动的配置中额外设置 ``io_loop_back`` 来实现这种“回环”功能，现在，这个配置已经被移除。不同的驱动只需要在配置中设置相同的 GPIO 编号就能实现这个功能。

外设时钟门控
~~~~~~~~~~~~

外设的时钟门控现在由驱动层统一管理，用户无需手动控制外设模块的时钟开关。相关的 API 位于私有头文件 ``esp_private/periph_ctrl.h`` 中。原先用于同样目的的头文件 ``driver/periph_ctrl.h`` 已被移除。

RTC 子系统控制
~~~~~~~~~~~~~~

低功耗模块通常会共享一些资源，比如中断号。为避免资源冲突，私有头文件 ``esp_private/rtc_ctrl.h`` 提供了相关 API 方便管理这些共享资源。原先用于同样目的的头文件 ``driver/rtc_cntl.h`` 已被移除。

ADC
---

旧版的 ADC 驱动 ``driver/adc.h`` 在 5.0 的版本中就已经被弃用 （参考 :ref:`deprecate_adc_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_adc` 组件中，头文件引用路径为 ``esp_adc/adc_oneshot.h``, ``esp_adc/adc_continuous.h``, ``esp_adc/adc_cali.h`` 和 ``esp_adc/adc_cali_scheme.h``。

RMT
---

:cpp:type:`rmt_tx_channel_config_t` 配置结构体中的 ``io_od_mode`` 已经被移除。如果想要使用开漏模式，你需要手动调用 GPIO 驱动中的 :func:`gpio_od_enable` 函数。

.. only:: SOC_MCPWM_SUPPORTED

    MCPWM
    -----

    :cpp:type:`mcpwm_generator_config_t` 配置结构体中的 ``io_od_mode`` 已经被移除。如果想要使用开漏模式，你需要手动调用 GPIO 驱动中的 :func:`gpio_od_enable` 函数。

    以下配置结构体中的 ``pull_up`` 和 ``pull_down`` 成员已经被移除，你需要手动调用 GPIO 驱动中的 :func:`gpio_set_pull_mode` 函数来配置 IO 上拉和下拉电阻：

    .. list::

        - :cpp:type:`mcpwm_generator_config_t`
        - :cpp:type:`mcpwm_gpio_fault_config_t`
        - :cpp:type:`mcpwm_gpio_sync_src_config_t`
        - :cpp:type:`mcpwm_capture_channel_config_t`

    默认的 MCPWM 群组时钟分频器已改为 1。这样，你就可以获得比以前更高的默认分辨率。

    旧版 MCPWM 驱动被移除
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        旧版的 MCPWM 驱动 ``driver/mcpwm.h`` 在 5.0 的版本中就已经被弃用（请参考 :ref:`deprecate_mcpwm_legacy_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_driver_mcpwm` 组件中，头文件引用路径为 ``driver/mcpwm_prelude``。

GPIO
----

:func:`gpio_iomux_in` 和 :func:`gpio_iomux_out` 已被 :func:`gpio_iomux_input` 和 :func:`gpio_iomux_output` 函数取代， 并移至 ``esp_private/gpio.h`` 头文件中作为仅供内部使用的私有 API。

LEDC
----

- :func:`ledc_timer_set` 已被移除。请使用 :func:`ledc_timer_config` 或 :func:`ledc_set_freq` 代替。

- ``LEDC_APB_CLK_HZ`` 和 ``LEDC_REF_CLK_HZ`` 已被移除。

- ``LEDC_SLOW_CLK_RTC8M`` 宏已被移除。请使用 ``LEDC_SLOW_CLK_RC_FAST`` 代替。

- esp_driver_gpio 不再作为 esp_driver_ledc 的公共依赖组件。

- :func:`ledc_isr_register` 已被弃用。LEDC 中断处理由驱动内部实现，如果需要注册中断回调，仅需要注册事件回调即可。

- :cpp:member:`ledc_channel_config_t::intr_type` 已被弃用。`LEDC_INTR_FADE_END` 中断使能/禁用控制由驱动内部处理。用户仍可以通过 :cpp:func:`ledc_cb_register` 注册该中断的回调。

- :cpp:enumerator:`soc_periph_ledc_clk_src_legacy_t::LEDC_USE_RTC8M_CLK` 已被移除。请使用 ``LEDC_USE_RC_FAST_CLK`` 代替。

UART
----

``UART_FIFO_LEN`` 已被移除。请使用 ``UART_HW_FIFO_LEN`` 代替。

I2C
---

I2C 从机在 v5.4 上已经被重新设计。在当前版本上，老的 I2C 从机驱动已经被移除，详细内容请参考编程指南中关于 I2C 从机的部分。

主要的概念上和用法上的改变如下所示:

主要概念更新
~~~~~~~~~~~~~~~~~~

- 老版本的 I2C 从机驱动是主动读写，这不符合 I2C 从机的一般用法。在新版的 I2C 从机中，I2C 的读写通过主机驱动产生的事件以触发回调被动完成。

主要用法更新
~~~~~~~~~~~~~~~~~~

- ``i2c_slave_receive`` 被移除， 在新驱动中使用回调接收数据。
- ``i2c_slave_transmit`` 已被 ``i2c_slave_write`` 取代。
- ``i2c_slave_write_ram`` 被移除。
- ``i2c_slave_read_ram`` 被移除。

同时，I2C的主机驱动也有一些API用法上的改动

主要用法更新
~~~~~~~~~~~~~~~~~~

当主机在I2C总线上检测到NACK，以下的函数目前会返回 ``ESP_ERR_INVALID_RESPONSE``，而不是像之前一样返回 ``ESP_ERR_INVALID_STATE``：
- ``i2c_master_transmit``
- ``i2c_master_multi_buffer_transmit``
- ``i2c_master_transmit_receive``
- ``i2c_master_execute_defined_operations``

旧版定时器组驱动被移除
----------------------

旧版的定时器组驱动 ``driver/timer.h`` 在 5.0 的版本中就已经被弃用 （参考 :ref:`deprecate_gptimer_legacy_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_driver_gptimer` 组件中，头文件引用路径为 ``driver/gptimer.h``。

.. only:: SOC_I2S_SUPPORTED

    旧版 I2S 驱动被移除
    ----------------------

    - 旧版的 I2S 驱动 ``driver/i2s.h`` 在 5.0 的版本中就已经被弃用（请参考 :ref:`deprecate_i2s_legacy_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_driver_i2s` 组件中，头文件引用路径为 ``driver/i2s_std.h``, ``driver/i2s_pdm.h`` and ``driver/i2s_tdm.h``。
    - ``i2s_set_adc_mode``,  ``i2s_adc_enable`` 和 ``i2s_adc_disable`` 在 5.0 版本中就已经被弃用。从 6.0 版本开始，这三个接口被完全移除。

.. only:: SOC_PCNT_SUPPORTED

    旧版 PCNT 驱动被移除
    ----------------------

    旧版的 PCNT 驱动 ``driver/pcnt.h`` 在 5.0 的版本中就已经被弃用 （参考 :ref:`deprecate_pcnt_legacy_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_driver_pcnt` 组件中，头文件引用路径为 ``driver/pulse_cnt.h``。

.. only:: SOC_RMT_SUPPORTED

    旧版 RMT 驱动被移除
    ----------------------

    旧版的 RMT 驱动 ``driver/rmt.h`` 在 5.0 的版本中就已经被弃用（请参考 :ref:`deprecate_rmt_legacy_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_driver_rmt` 组件中，头文件引用路径为 ``driver/rmt_tx.h``, ``driver/rmt_rx.h`` 和 ``driver/rmt_encoder.h``。

GDMA
----

- ``GDMA_ISR_IRAM_SAFE`` Kconfig 选项会带来不必要的风险，因此被移除。现在，不同的 GDMA 通道它们的中断在 Cache 关闭期间的行为可以互不影响。
- ``gdma_new_channel`` 已经被移除。现在当申请一个 GDMA 通道时，必须要根据实际使用的总线调用 ``gdma_new_ahb_channel`` 或 ``gdma_new_axi_channel`` 函数。
- :cpp:type:`async_memcpy_config_t` 中的 ``sram_trans_align`` 和 ``psram_trans_align`` 成员均已经被移除。请使用 :cpp:member:`async_memcpy_config_t::dma_burst_size` 来设置 DMA 的突发传输大小。
- ``esp_dma_capable_malloc`` 和 ``esp_dma_capable_calloc`` 函数已经被移除。请使用 :component_file:`heap/include/esp_heap_caps.h` 中的 :cpp:func:`heap_caps_malloc` 和 :cpp:func:`heap_caps_calloc` 函数搭配 ``MALLOC_CAP_DMA|MALLOC_CAP_CACHE_ALIGNED`` 分配满足 DMA 和 Cache 对齐要求的内存。

SDMMC
-----

- :cpp:type:`sdmmc_host_t` 结构体中的 ``get_dma_info`` 成员接口已经被移除。对应的 ``sdspi_host_get_dma_info`` 和 ``sdmmc_host_get_dma_info`` 函数也已经被移除。DMA 相关的设置会由驱动内部处理好。

.. only:: SOC_DAC_SUPPORTED

    旧版 DAC 驱动被移除
    ----------------------

    旧版的 DAC 驱动 ``driver/dac.h`` 在 5.1 的版本中就已经被弃用（请参考 :ref:`deprecate_dac_legacy_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_driver_dac` 组件中，头文件引用路径为 ``driver/dac_oneshot.h``， ``driver/dac_continuous.h`` 和 ``driver/dac_cosine.h``。

.. only:: SOC_TEMP_SENSOR_SUPPORTED

    旧版温度传感器驱动被移除
    ------------------------------------

    旧版的温度传感器驱动 ``driver/temp_sensor.h`` 在 5.1 的版本中就已经被弃用（请参考 :ref:`deprecate_tsens_legacy_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_driver_tsens` 组件中，头文件引用路径为 ``driver/temperature_sensor.h``。

.. only:: SOC_SDM_SUPPORTED

    旧版 Sigma-Delta 调制器驱动被移除
    ---------------------------------

    旧版的 Sigma-Delta 调制器驱动 ``driver/sigmadelta.h`` 在 5.0 的版本中就已经被弃用（请参考 :ref:`deprecate_sdm_legacy_driver`）。从 6.0 版本开始，旧版驱动被完全移除。新驱动位于 :component:`esp_driver_sdm` 组件中，头文件引用路径为 ``driver/sdm.h``。

LCD
---

- LCD 驱动中的 GPIO 编号已经从 ``int`` 类型修改为更加类型安全的 ``gpio_num_t`` 类型。比如原来使用 ``5`` 作为 GPIO 编号，现在需要使用 ``GPIO_NUM_5``。
- :cpp:type:`esp_lcd_i80_bus_config_t` 结构体中的 ``psram_trans_align`` 和 ``sram_trans_align`` 均已被 :cpp:member:`esp_lcd_i80_bus_config_t::dma_burst_size` 成员取代，用来设置 DMA 的突发传输大小。
- :cpp:type:`esp_lcd_rgb_panel_config_t` 结构体中的 ``psram_trans_align`` 和 ``sram_trans_align`` 均已被 :cpp:member:`esp_lcd_rgb_panel_config_t::dma_burst_size` 成员取代，用来设置 DMA 的突发传输大小。
- :cpp:type:`esp_lcd_panel_dev_config_t` 结构体中的 ``color_space`` 和 ``rgb_endian`` 配置均已被 :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` 成员取代，用来设置 RGB 元素的排列顺序。对应的类型 ``lcd_color_rgb_endian_t`` 和 ``esp_lcd_color_space_t`` 也已被移除，请使用 :cpp:type:`lcd_rgb_element_order_t` 替代。
- ``esp_lcd_panel_disp_off`` 函数已被移除。请使用 :func:`esp_lcd_panel_disp_on_off` 函数来控制显示内容的开关。
- :cpp:type:`esp_lcd_rgb_panel_event_callbacks_t` 中的 ``on_bounce_frame_finish`` 成员已被 :cpp:member:`esp_lcd_rgb_panel_event_callbacks_t::on_frame_buf_complete` 成员取代，用于指示一个完整的帧缓冲区已被发送给 LCD 控制器。
- I2C 接口的 LCD IO 层驱动有两套实现，分别基于新、旧 I2C Master 总线驱动。由于旧版的 I2C Master 驱动逐渐被弃用，遂 LCD 的 IO 层也移除对旧版的支持，只使用 ``driver/i2c_master.h`` 中提供的 API。
- :cpp:member:`esp_lcd_dpi_panel_config_t::pixel_format` 成员已经被废弃。建议仅使用 :cpp:member:`esp_lcd_dpi_panel_config_t::in_color_format` 来设定 MIPI DSI 驱动输入的像素数据格式。

SPI
---

:ref:`CONFIG_SPI_MASTER_IN_IRAM` 选项在 menuconfig 中默认不可见，并且依赖于 :ref:`CONFIG_FREERTOS_IN_IRAM`。这样修改是为了防止位于 IRAM 中的 SPI 函数调用位于 flash 中的 FreeRTOS 函数时可能发生的崩溃。

要启用 SPI 主机 IRAM 优化：

1. 在 menuconfig 中进入 ``Component config`` → ``FreeRTOS`` → ``Port``
2. 启用 ``Place FreeRTOS functions in IRAM`` (:ref:`CONFIG_FREERTOS_IN_IRAM`)
3. 在 menuconfig 中进入 ``Component config`` → ``ESP-Driver:SPI Configurations``
4. 启用 ``Place transmitting functions of SPI master into IRAM`` (:ref:`CONFIG_SPI_MASTER_IN_IRAM`)

请注意，启用 :ref:`CONFIG_FREERTOS_IN_IRAM` 会显著增加 IRAM 使用量。在优化 SPI 性能时，需进行权衡。

Touch Element
-------------

``touch_element`` 组件已移至 [ESP Component Registry](https://components.espressif.com/components/espressif/touch_element/versions/1.0.0/readme)。

您可以通过运行 ``idf.py add-dependency "espressif/touch_element"`` 将这个依赖添加到您的项目中。

Touch Sensor
------------

第三版触摸传感器的驱动配置项 ``touch_sensor_sample_config_t::bypass_shield_output`` 已被移除，因为第三版触摸传感器硬件已不支持该功能。
