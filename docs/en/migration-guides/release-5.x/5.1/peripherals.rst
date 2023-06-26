Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

.. only:: SOC_DAC_SUPPORTED

    DAC
    ---

    DAC driver has been redesigned (see :doc:`DAC API Reference <../../../api-reference/peripherals/dac>`), which aims to unify the interface and extend the usage of DAC peripheral. Although it is recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/dac.h``. However, by default, including ``driver/dac.h`` will bring a build warning like ``The legacy DAC driver is deprecated, please use 'driver/dac_oneshot.h', 'driver/dac_cosine.h' or 'driver/dac_continuous.h' instead``. The warning can be suppressed by the Kconfig option :ref:`CONFIG_DAC_SUPPRESS_DEPRECATE_WARN`.

    The major breaking changes in concept and usage are listed as follows:

    Breaking Changes in Concepts
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    - ``dac_channel_t`` which was used to identify the hardware channel are removed from user space. The channel index now starts from ``0``, so please use  `DAC_CHAN_0` and `DAC_CHAN_1` instead. And in the new driver, DAC channels can be selected by using :cpp:type:`dac_channel_mask_t`. And these channels can be allocated in a same channel group which is represented by :cpp:type:`dac_channels_handle_t`.
    - ``dac_cw_scale_t`` is replaced by :cpp:type:`dac_cosine_atten_t` to decouple the legacy driver and the new driver.
    - ``dac_cw_phase_t`` is replaced by :cpp:type:`dac_cosine_phase_t`. The enumerate value is now the phase angle directly.
    - ``dac_cw_config_t`` is replaced by :cpp:type:`dac_cosine_config_t`, but the ``en_ch`` field is removed because it should be specified while allocating the channel group.

    .. only:: esp32s2

        - ``dac_digi_convert_mode_t`` is removed. The driver now can transmit DMA data in different ways by calling :cpp:func:`dac_channels_write_continuously` or :cpp:func:`dac_channels_write_cyclically`.
        - ``dac_digi_config_t`` is replaced by :cpp:type:`dac_continuous_config_t`.

    Breaking Changes in Usage
    ~~~~~~~~~~~~~~~~~~~~~~~~~

    - ``dac_pad_get_io_num`` is removed.
    - ``dac_output_voltage`` is replaced by :cpp:func:`dac_oneshot_output_voltage`.
    - ``dac_output_enable`` is removed. For oneshot mode, it will be enabled after the channel is allocated.
    - ``dac_output_disable`` is removed. For oneshot mode, it will be disabled before the channel is deleted.
    - ``dac_cw_generator_enable`` is replaced by :cpp:func:`dac_cosine_start`.
    - ``dac_cw_generator_disable`` is replaced by :cpp:func:`dac_cosine_stop`.
    - ``dac_cw_generator_config`` is replaced by :cpp:func:`dac_cosine_new_channel`.

    .. only:: esp32

        - ``dac_i2s_enable`` is replaced by :cpp:func:`dac_continuous_enable`, but it needs to allocate the continuous DAC channel first by :cpp:func:`dac_continuous_new_channels`.
        - ``dac_i2s_disable`` is replaced by :cpp:func:`dac_continuous_disable`.

    .. only:: esp32s2

        - ``dac_digi_init`` and ``dac_digi_controller_config`` is merged into :cpp:func:`dac_continuous_new_channels`.
        - ``dac_digi_deinit`` is replaced by :cpp:func:`dac_continuous_del_channels`.
        - ``dac_digi_start``, ``dac_digi_fifo_reset`` and ``dac_digi_reset`` are merged into :cpp:func:`dac_continuous_enable`.
        - ``dac_digi_stop`` is replaced by :cpp:func:`dac_continuous_disable`.


.. only:: SOC_GPSPI_SUPPORTED

    GPSPI
    -----

    Following items are deprecated. Since ESP-IDF v5.1, GPSPI clock source is configurable.

    - ``spi_get_actual_clock`` is deprecated, you should use :cpp:func:`spi_device_get_actual_freq` instead.

.. only:: SOC_LEDC_SUPPORTED

    LEDC
    ----

    - :cpp:enumerator:`soc_periph_ledc_clk_src_legacy_t::LEDC_USE_RTC8M_CLK` is deprecated. Please use ``LEDC_USE_RC_FAST_CLK`` instead.
