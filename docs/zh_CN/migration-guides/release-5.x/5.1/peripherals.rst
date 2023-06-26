外设
============

:link_to_translation:`en:[English]`

.. only:: SOC_DAC_SUPPORTED

    DAC
    ---

    DAC 驱动程序已经过重新设计（参考 :doc:`DAC API 参考 <../../../api-reference/peripherals/dac>`），以统一接口并扩展 DAC 外设的使用。建议使用更新后的驱动 API，但用户仍然可以通过包含路径 ``driver/dac.h`` 使用原有驱动。然而默认情况下，在文件中包含 ``driver/dac.h`` 会引发构建警告，例如 ``The legacy DAC driver is deprecated, please use 'driver/dac_oneshot.h', 'driver/dac_cosine.h' or 'driver/dac_continuous.h' instead``。可通过 Kconfig 选项 :ref:`CONFIG_DAC_SUPPRESS_DEPRECATE_WARN` 关闭该警告。

    主要概念和使用方法上的更新如下所示：

    主要概念更新
    ^^^^^^^^^^^^^^^^^^

    - 用于识别硬件通道的 ``dac_channel_t`` 已从用户空间被删除，更新后通道索引起始为 ``0``，请使用 `DAC_CHAN_0` 和 `DAC_CHAN_1` 代替原来的索引。在新驱动中可通过 :cpp:type:`dac_channel_mask_t` 删除 DAC 通道。这些通道可以被分配到同一个通道组中，由 :cpp:type:`dac_channels_handle_t` 表示。
    - ``dac_cw_scale_t`` 更新为 :cpp:type:`dac_cosine_atten_t`，以区分原有驱动程序和新驱动程序。
    - ``dac_cw_phase_t`` 更新为 :cpp:type:`dac_cosine_phase_t`，更新后，枚举值即为相位角。
    - ``dac_cw_config_t`` 更新为 :cpp:type:`dac_cosine_config_t`，但删除了 ``en_ch`` 字段，因为该字段应在分配通道组时被指定。

    .. only:: esp32s2

        - ``dac_digi_convert_mode_t`` 已被删除。驱动程序现在可以通过调用 :cpp:func:`dac_channels_write_continuously` 或 :cpp:func:`dac_channels_write_cyclically` 以不同方式传输 DMA 数据。
        - ``dac_digi_config_t`` 更新为 :cpp:type:`dac_continuous_config_t`。

    主要使用方法更新
    ^^^^^^^^^^^^^^^^^^^^

    - ``dac_pad_get_io_num`` 已被删除。
    - ``dac_output_voltage`` 更新为 :cpp:func:`dac_oneshot_output_voltage`。
    - ``dac_output_enable`` 已被删除，单次采样模式下在分配通道后启用。
    - ``dac_output_disable`` 已被删除，单次采样模式下在删除通道前被禁用。
    - ``dac_cw_generator_enable`` 更新为 :cpp:func:`dac_cosine_start`。
    - ``dac_cw_generator_disable`` 更新为 :cpp:func:`dac_cosine_stop`。
    - ``dac_cw_generator_config`` 更新为  :cpp:func:`dac_cosine_new_channel`。

    .. only:: esp32

        - ``dac_i2s_enable`` 更新为 :cpp:func:`dac_continuous_enable`，但首先需要通过 :cpp:func:`dac_continuous_new_channels` 分配连续 DAC 通道。
        - ``dac_i2s_disable`` 更新为 :cpp:func:`dac_continuous_disable`。

    .. only:: esp32s2

        - ``dac_digi_init`` 和 ``dac_digi_controller_config`` 合并为 :cpp:func:`dac_continuous_new_channels`。
        - ``dac_digi_deinit`` 更新为 :cpp:func:`dac_continuous_del_channels`。
        - ``dac_digi_start``、 ``dac_digi_fifo_reset`` 和 ``dac_digi_reset`` 合并为 :cpp:func:`dac_continuous_enable`。
        - ``dac_digi_stop`` 更新为 :cpp:func:`dac_continuous_disable`。


.. only:: SOC_GPSPI_SUPPORTED

    GPSPI
    -----

    不再支持以下函数。从 ESP-IDF v5.1 版本起，GPSPI 时钟源可配置。

    - ``spi_get_actual_clock`` 已废弃，更新为 :cpp:func:`spi_device_get_actual_freq`。

.. only:: SOC_LEDC_SUPPORTED

    LEDC
    ----

    - :cpp:enumerator:`soc_periph_ledc_clk_src_legacy_t::LEDC_USE_RTC8M_CLK` 已废弃，更新为 ``LEDC_USE_RC_FAST_CLK``。
