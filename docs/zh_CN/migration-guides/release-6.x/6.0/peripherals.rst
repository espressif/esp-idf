外设驱动
========

:link_to_translation:`en:[English]`

公共变化
--------

所有驱动的 ``io_loop_back`` 配置已被移除
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

不同的驱动对象可以共享同一个 GPIO 编号，联合起来可以实现更加复杂的功能。比如将 RMT 外设的 TX 通道和 RX 通道绑定在同一个 GPIO 上，进而模拟单总线的读写时序。在以前的版本中，你需要在驱动的配置中额外设置 ``io_loop_back`` 来实现这种“回环”功能，现在，这个配置已经被移除。不同的驱动只需要在配置中设置相同的 GPIO 编号就能实现这个功能。

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
- ``i2c_slave_transmit`` 已被 ``i2c_slave_write`` 取代.
- ``i2c_slave_write_ram`` 被移除。
- ``i2c_slave_read_ram`` 被移除。

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
