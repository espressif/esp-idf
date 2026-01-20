外设驱动
========

:link_to_translation:`en:[English]`

公共变化
--------

旧版驱动依赖移除
~~~~~~~~~~~~~~~~

旧版驱动组件 :component:`driver` 已被弃用，不再包含对以下驱动组件的公共依赖：

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

如果您的项目使用了老旧的 :component:`driver` 驱动组件，强烈建议移除对 :component:`driver` 驱动组件的依赖，并为项目添加新驱动组件（通常是 ``esp_driver_xxx``）的依赖。

如仍需保留老旧的 :component:`driver` 驱动组件（例如您的项目依赖旧版 ``i2c`` 驱动），请将 :component:`driver` 组件保留在项目的组件依赖列表文件中（通常是 ``<project_root>/main/CMakeLists.txt`` ），并根据项目需要手动添加 :component:`driver` 中不再包含的驱动组件依赖。

所有驱动的 ``io_loop_back`` 配置已被移除
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

不同的驱动对象可以共享同一个 GPIO 编号，联合起来可以实现更加复杂的功能。比如将 RMT 外设的 TX 通道和 RX 通道绑定在同一个 GPIO 上，进而模拟单总线的读写时序。在以前的版本中，你需要在驱动的配置中额外设置 ``io_loop_back`` 来实现这种“回环”功能，现在，这个配置已经被移除。不同的驱动只需要在配置中设置相同的 GPIO 编号就能实现这个功能。

外设时钟门控
~~~~~~~~~~~~

外设的时钟门控现在由驱动层统一管理，用户无需手动控制外设模块的时钟开关。相关的 API 位于私有头文件 ``esp_private/periph_ctrl.h`` 中。原先用于同样目的的头文件 ``driver/periph_ctrl.h`` 已被移除。

RTC 子系统控制
~~~~~~~~~~~~~~

低功耗模块通常会共享一些资源，比如中断号。为避免资源冲突，私有头文件 ``esp_private/rtc_ctrl.h`` 提供了相关 API 方便管理这些共享资源。原先用于同样目的的头文件 ``driver/rtc_cntl.h`` 已被移除。

驱动头文件移除 FreeRTOS 依赖
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

自 v6.0 起，为了增强 IDF 驱动的通用性，所有公开的驱动头文件不再包含具体操作系统(FreeRTOS)相关的头文件。因此，以前依赖这种隐式包含逻辑的应用代码，在 v6.0 之后需要显示添加对应的 FreeRTOS 头文件。

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

    可变参数生成器 API 已被移除
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    本版本中已移除旧版的可变参数（"varg"）生成器 API。原先使用可变参数方式配置生成器动作的代码，需迁移到显式、类型化的 API。这些新 API 使用配置结构体和类型明确的设置函数（例如 :cpp:type:`mcpwm_generator_config_t`、:cpp:type:`mcpwm_gen_timer_event_action_t` 和 :cpp:type:`mcpwm_generator_set_action_on_timer_event`）。

    迁移步骤（摘要）：

    - 用辅助结构体/宏和专用设置函数替换 varg 风格的动作配置::

        .. code-block:: c

            /* 旧版（varg）*/
            mcpwm_generator_set_actions_on_compare_event(my_generator,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, my_comparator, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, my_comparator, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END());

            /* 新版 */
            mcpwm_generator_set_action_on_compare_event(my_generator,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, my_comparator, MCPWM_GEN_ACTION_LOW));
            mcpwm_generator_set_action_on_compare_event(my_generator,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, my_comparator, MCPWM_GEN_ACTION_HIGH));

GPIO
----

- :func:`gpio_iomux_in` 和 :func:`gpio_iomux_out` 已被 :func:`gpio_iomux_input` 和 :func:`gpio_iomux_output` 函数取代， 并移至 ``esp_private/gpio.h`` 头文件中作为仅供内部使用的私有 API。

- ``rom_`` 前缀已添加进 ``components/esp_rom/esp32xx/include/esp32xx/rom/gpio.h`` 中的所有 GPIO ROM API。例如，:func:`gpio_iomux_in` 现在为 :func:`rom_gpio_iomux_in`。

- ``MAX_PAD_GPIO_NUM``、 ``MAX_GPIO_NUM`` 和 ``DIG_IO_HOLD_BIT_SHIFT`` 宏已被移除。

- 为 :func:`gpio_uninstall_isr_service` 添加了 :cpp:type:`esp_err_t` 返回类型。

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

- ``UART_FIFO_LEN`` 已被移除。请使用 ``UART_HW_FIFO_LEN`` 代替。

- ``soc/uart_channel.h`` 头文件已被移除。所有 UART GPIO 查找宏都可以在 ``soc/uart_pins.h`` 中找到。例如，``UART_NUM_0_TXD_DIRECT_GPIO_NUM`` 等同于 ``U0TXD_GPIO_NUM``。

I2C
---

旧版 I2C 驱动生命周期终止
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. warning::

    旧版 I2C 驱动（``driver/i2c.h``）已在 ESP-IDF v6.0 中被标记为 **生命周期终止（End-of-Life, EOL）**，并计划在 **v7.0 中彻底移除**。

    - ESP-IDF 将不再为旧版驱动提供及时的更新、错误修复或安全补丁。
    - 强烈建议用户尽快迁移到新版 I2C 驱动：``driver/i2c_master.h`` 和 ``driver/i2c_slave.h``。
    - 如需暂时抑制编译警告，可在 menuconfig 中启用 ``Component config``  > ``Legacy Driver Configurations`` > ``Legacy I2C Driver Configurations`` > ``Suppress legacy driver deprecated warning``。

新版 I2C 驱动主要改进了从机和主机的使用方式，详细内容请参考 :ref:`I2C 迁移指南 <migration_guide_i2c_driver_5_2>` 和 :doc:`I2C 驱动编程指南 <../../../api-reference/peripherals/i2c>`。

I2C 从机驱动更新
~~~~~~~~~~~~~~~~~~

I2C 从机驱动在 v5.4 上已经被重新设计。在当前版本上，旧的 I2C 从机驱动已经被移除。

主要概念更新
^^^^^^^^^^^^

- 旧版本的 I2C 从机驱动是主动读写，这不符合 I2C 从机的一般用法。在新版的 I2C 从机中，I2C 的读写通过主机驱动产生的事件以触发回调被动完成。

主要用法更新
^^^^^^^^^^^^

- ``i2c_slave_receive`` 被移除，在新驱动中使用回调接收数据。
- ``i2c_slave_transmit`` 已被 ``i2c_slave_write`` 取代。
- ``i2c_slave_write_ram`` 被移除。
- ``i2c_slave_read_ram`` 被移除。

I2C 主机驱动更新
~~~~~~~~~~~~~~~~~~

I2C 主机驱动的 API 也有一些用法上的改动。

主要用法更新
^^^^^^^^^^^^

当主机在 I2C 总线上检测到 NACK，以下的函数目前会返回 ``ESP_ERR_INVALID_RESPONSE``，而不是像之前一样返回 ``ESP_ERR_INVALID_STATE``：

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

DMA 驱动
--------

- DMA 核心驱动程序现已从原来的 ``esp_hw_support`` 组件中移出，现作为单独的 ``esp_driver_dma`` 组件提供。如果你使用了 ``esp_async_memcpy.h`` 和 ``esp_dma_utils.h`` 驱动，请确保在项目中添加对 ``esp_driver_dma`` 组件的依赖。
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

    - :func:`sdm_channel_set_duty` 已被移除。请使用 :func:`sdm_channel_set_pulse_density` 替代。

LCD
---

- LCD 驱动中的 GPIO 编号已经从 ``int`` 类型修改为更加类型安全的 ``gpio_num_t`` 类型。比如原来使用 ``5`` 作为 GPIO 编号，现在需要使用 ``GPIO_NUM_5``。
- :cpp:type:`esp_lcd_i80_bus_config_t` 结构体中的 ``psram_trans_align`` 和 ``sram_trans_align`` 均已被 :cpp:member:`esp_lcd_i80_bus_config_t::dma_burst_size` 成员取代，用来设置 DMA 的突发传输大小。
- :cpp:type:`esp_lcd_rgb_panel_config_t` 结构体中的 ``psram_trans_align`` 和 ``sram_trans_align`` 均已被 :cpp:member:`esp_lcd_rgb_panel_config_t::dma_burst_size` 成员取代，用来设置 DMA 的突发传输大小。
- :cpp:type:`esp_lcd_panel_dev_config_t` 结构体中的 ``color_space`` 和 ``rgb_endian`` 配置均已被 :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` 成员取代，用来设置 RGB 元素的排列顺序。对应的类型 ``lcd_color_rgb_endian_t`` 和 ``esp_lcd_color_space_t`` 也已被移除，请使用 :cpp:type:`lcd_rgb_element_order_t` 替代。
- ``esp_lcd_panel_disp_off`` 函数已被移除。请使用 :func:`esp_lcd_panel_disp_on_off` 函数来控制显示内容的开关。
- :cpp:type:`esp_lcd_rgb_panel_event_callbacks_t` 中的 ``on_bounce_frame_finish`` 成员已被 :cpp:member:`esp_lcd_rgb_panel_event_callbacks_t::on_frame_buf_complete` 成员取代，用于指示一个完整的帧缓冲区已被发送给 LCD 控制器。
- I2C 接口的 LCD IO 层驱动有两套实现，分别基于新、旧 I2C Master 总线驱动。由于旧版的 I2C Master 驱动逐渐被弃用，遂 LCD 的 IO 层也移除对旧版的支持，只使用 ``driver/i2c_master.h`` 中提供的 API。
- :cpp:type:`esp_lcd_dpi_panel_config_t` 结构体中的 ``pixel_format`` 成员已经被删除。建议仅使用 :cpp:member:`esp_lcd_dpi_panel_config_t::in_color_format` 来设定 MIPI DSI 驱动输入的像素数据格式。
- :cpp:type:`esp_lcd_rgb_panel_config_t` 结构体中的 ``bits_per_pixel`` 成员已经被删除。内部帧缓冲区的色彩深度现在由 :cpp:member:`esp_lcd_rgb_panel_config_t::in_color_format` 成员决定。
- ``esp_lcd_dpi_panel_set_color_conversion`` 函数已被 :cpp:func:`esp_lcd_dpi_panel_set_yuv_conversion` 取代,用于设置 YUV 到 RGB 的色彩转换配置。
- :cpp:func:`esp_lcd_rgb_panel_set_yuv_conversion` 函数的签名已改变。原先使用的 ``esp_lcd_yuv_conv_config_t`` 配置类型现已被 :cpp:type:`esp_lcd_color_conv_yuv_config_t` 取代。
- NT35510 LCD 设备驱动已经从 ESP-IDF 中移动到外部仓库，并且托管在了 `ESP Component Registry <https://components.espressif.com/components/espressif/esp_lcd_nt35510/versions/1.0.0/readme>`__ 上。如果你的项目使用到了 NT35510 驱动，你可以通过运行 ``idf.py add-dependency "espressif/esp_lcd_nt35510"`` 将它添加到你的项目中。
- :cpp:type:`esp_lcd_dpi_panel_config_t` 结构体中的 ``use_dma2d`` 成员已被移除。请使用 :func:`esp_lcd_dpi_panel_enable_dma2d` 函数来启用 DMA2D 功能。当不使用 DMA2D 时，可以减小 10KB 左右的二进制文件大小。

SPI
---

- :ref:`CONFIG_SPI_MASTER_IN_IRAM` 选项在 menuconfig 中默认不可见，并且依赖于 :ref:`CONFIG_FREERTOS_IN_IRAM`。这样修改是为了防止位于 IRAM 中的 SPI 函数调用位于 flash 中的 FreeRTOS 函数时可能发生的崩溃。
- 按照下列步骤，启用 SPI 主机 IRAM 优化：

    1. 在 menuconfig 中进入 ``Component config`` → ``FreeRTOS`` → ``Port``。
    2. 启用 ``Place FreeRTOS functions in IRAM`` (:ref:`CONFIG_FREERTOS_IN_IRAM`)。
    3. 在 menuconfig 中进入 ``Component config`` → ``ESP-Driver:SPI Configurations``。
    4. 启用 ``Place transmitting functions of SPI master into IRAM`` (:ref:`CONFIG_SPI_MASTER_IN_IRAM`)。

    .. note::

        启用 :ref:`CONFIG_FREERTOS_IN_IRAM` 会显著增加 IRAM 使用量。在优化 SPI 性能时，需进行权衡。

- ESP32 和 ESP32S2 上已弃用的 HSPI 和 VSPI 相关 IOMUX 引脚宏已被移除。

PSRAM
-----

已弃用的头文件 ``esp_spiram.h`` 已被移除，请改用 ``esp_psram.h``。

SPI flash 驱动
--------------

- 已弃用的 ``enum`` 类型 ``esp_flash_speed_t`` 已被移除。主 flash 速度由 :ref:`CONFIG_ESPTOOLPY_FLASHFREQ` 选项控制。
- 已弃用的头文件 ``esp_spi_flash.h`` 已被移除。请改用 ``spi_flash_mmap.h``。
- 已弃用的 API ``spi_flash_dump_counters`` 已被移除。请改用 :cpp:func:`esp_flash_dump_counters`。
- 已弃用的 API ``spi_flash_get_counters`` 已被移除。请改用 :cpp:func:`esp_flash_get_counters`。
- 已弃用的 API ``spi_flash_reset_counters`` 已被移除。请改用 :cpp:func:`esp_flash_reset_counters`。
- ``esp_flash_os_functions_t::start`` 新增了一个参数 ``flags``。调用者和实现者应正确处理此参数。
- Kconfig 选项 ``CONFIG_SPI_FLASH_ROM_DRIVER_PATCH`` 已被移除，考虑到这个选项不会被广泛被用户使用，且有因误用而导致出现严重的问题，遂决定移除。

.. note::

    启用 :ref:`CONFIG_FREERTOS_IN_IRAM` 会显著增加 IRAM 使用量。在优化 SPI 性能时，需进行权衡。

Touch Element
-------------

``touch_element`` 组件已移至 `ESP Component Registry <https://components.espressif.com/components/espressif/touch_element/versions/1.0.0/readme>`__。

您可以通过运行 ``idf.py add-dependency "espressif/touch_element"`` 将这个依赖添加到您的项目中。

Touch Sensor
------------

第三版触摸传感器的驱动配置项 ``touch_sensor_sample_config_t::bypass_shield_output`` 已被移除，因为第三版触摸传感器硬件已不支持该功能。

旧版触摸传感器驱动依赖已被移除，现在你需要使用 ``int`` 类型代替 ``touch_pad_t`` 类型来指示触摸通道 ID。

I2S
---

- ``i2s_port_t`` 类型已被移除。请使用 ``int`` 类型代替。该类型原有的 enum 项 ``I2S_NUM_0``，``I2S_NUM_1``，``I2S_NUM_2`` 和 ``I2S_NUM_AUTO`` 已用宏定义代替，以保证兼容性。

USB
---

``usb`` 组件已迁移至 `乐鑫组件注册表 <https://components.espressif.com/components/espressif/usb>`__。

你可以通过运行 ``idf.py add-dependency "espressif/usb"``，将此依赖添加到项目中。

.. only:: SOC_TWAI_SUPPORTED

    双线汽车接口 (TWAI)
    --------------------

    TWAI 在 5.5 版本已经提供的新的驱动接口，支持更灵活的配置和更丰富的功能。旧版驱动不推荐再继续使用，请参考 5.5 迁移指南 :doc:`TWAI迁移指南 <../../release-5.x/5.5/peripherals>` 和新版驱动编程指南 :doc:`TWAI驱动编程指南 <../../../api-reference/peripherals/twai>` 进行移植。

    若需要继续使用旧版驱动，可以打开配置项 :ref:`CONFIG_TWAI_SUPPRESS_DEPRECATE_WARN` 以关闭编译警告。
