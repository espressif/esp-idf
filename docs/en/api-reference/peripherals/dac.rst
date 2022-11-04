Digital To Analog Converter (DAC)
=================================

{IDF_TARGET_DAC_CH_1: default = "GPIO25", esp32 = "GPIO25", esp32s2 = "GPIO17"}
{IDF_TARGET_DAC_CH_2: default = "GPIO26", esp32 = "GPIO26", esp32s2 = "GPIO18"}

Overview
--------

{IDF_TARGET_NAME} has two 8-bit DAC (digital to analog converter) channels, connected to {IDF_TARGET_DAC_CH_1} (Channel 1) and {IDF_TARGET_DAC_CH_2} (Channel 2). Which means each channel of DAC can convert digital value 0~255 to the analog voltage 0~Vref, the output voltage can be calculate by::

    out_voltage = Vref * digi_val / 255

The DAC peripheral supports outputting analog signal in following ways:

1. Outputting a voltage directly. The DAC channel will keep outputting a specified voltage.
2. Outputting continuous analog signal by DMA. The DAC will convert the data in a buffer at the specified frequency.
3. Outputting a cosine wave by the cosine wave generator. The DAC channel can output a cosine wave with specified frequency and amplitude.

For other analog output options, see the :doc:`Sigma-delta Modulation module <sdm>` and the :doc:`LED Control module <ledc>`. Both modules produce high-frequency PWM/PDM output, which can be hardware low-pass filtered in order to generate a lower frequency analog output.

DAC File Structure
------------------

.. figure:: ../../../_static/diagrams/dac/dac_file_structure.png
    :align: center
    :alt: DAC file structure


**Public headers that need to be included in the DAC application**

- ``dac.h``: The top header file of legacy DAC driver, only included in the apps which use legacy driver API
- ``dac_oneshot.h``: The top header file of new DAC driver, should be included in the apps which use the new driver API with oneshot mode.
- ``dac_cosine.h``: The top header file of new DAC driver, should be included in the apps which use the new driver API with cosine mode.
- ``dac_continuous.h``: The top header file of new DAC driver, should be included in the apps which use the new driver API with continuous mode.

.. note::

    The legacy driver can't coexist with the new driver. Including ``dac.h`` to use the legacy driver or ``dac_oneshot.h``, ``dac_cosine.h`` and ``dac_continuous.h`` to use the new driver. The legacy driver might be removed in future.

Functional Overview
-------------------

Resources Management
^^^^^^^^^^^^^^^^^^^^

The DAC on {IDF_TARGET_NAME} has two channels, due to the software resources are separate, they could be managed by the :cpp:type:`dac_oneshot_handle_t`, :cpp:type:`dac_cosine_handle_t`:cpp:type:`dac_continuous_handle_t` according to the usage. Of cause, registering different modes on a same DAC channel is not allowed.

Direct Voltage Output (One-shot/Direct Mode)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The DAC channels in the group can convert a 8-bit digital value into the analog every time calling :cpp:func:`dac_oneshot_output_voltage` (it can be called in ISR), and then the analog voltage will be kept on the DAC channel until next conversion start. To start to convert the voltage, the DAC channels need to be registered by :cpp:func:`dac_oneshot_new_channel` first, and the channel will be enabled after it is registered.

Continuous Wave Output (Continuous/DMA Mode)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

DAC channels can convert digital data continuously via the DMA. There are three ways to writing DAC data:

    1. Normal writing (synchronous): It can transmit the data one time and keep blocking until all data has been loaded into the DMA buffer, and the voltage will be kept according to the last conversion value while no more data inputted, usually it is used to transport a long signal like an audio. To convert data continuously, the continuous channel handle need to be allocated by calling :cpp:func:`dac_continuous_new_channels` and the DMA conversion should be enabled by :cpp:func:`dac_continuous_enable`, then data can be written by :cpp:func:`dac_continuous_write` synchronously. Referring to :example:`peripherals/dac/dac_continuous/dac_audio` for example.
    2. Cyclical writing: It can convert a piece of data cyclically without blocking, no more operation needed after the data are loaded into the DMA buffer,but note that the inputted buffer size is limited by the descriptor number and the DMA buffer size, usually it is used to transport some short signal that need to be repeated, for example, a sine wave.  To achieve cyclical writing, :cpp:func:`dac_continuous_write_cyclically` can be called after the DAC continuous mode is enabled. For the cyclical writing example, please refer to :example:`peripherals/dac/dac_continuous/signal_generator`
    3. Asynchronous writing: It can transmit the data asynchronously based on the event callback. Thus :cpp:member:`dac_event_callbacks_t::on_convert_done` must be registered to use asynchronous mode, and then users can get the :cpp:type:`dac_event_data_t` in the callback which contains the DMA buffer address and length, allowing user to load the data into it directly. As mentioned, to use the asynchronous writing, :cpp:func:`dac_continuous_register_event_callback` need to be called to register the :cpp:member:`dac_event_callbacks_t::on_convert_done` before enabling, and then calling :cpp:func:`dac_continuous_start_async_writing` to start the asynchronous writing, note that once the asynchronous writing started, the callback function will be triggered continuously, :cpp:func:`dac_continuous_write_asynchronously` can help to load the data either in a separate task or the callback directly. For the asynchronous example, please refer to :example:`peripherals/dac/dac_continuous/dac_audio` as well.

.. only:: esp32

    On ESP32, DAC digital controller can be connected internally to the I2S0 and use its DMA for continuous conversion. Although the DAC only needs 8-bit data for conversion, it has to be left shifted 8 bits (i.e. the high 8 bits in 16-bit slot) to satisfy the I2S communication format. But the driver can help to expand automatically, if you want to expand manually, please disable :ref:`CONFIG_DAC_DMA_AUTO_16BIT_ALIGN` in the menuconfig.

    The clock of DAC digital controller comes from I2S0 as well, so there are two kinds of clock source can be selected:

    - :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_PLL_D2` can support frequency between 19.6 KHz to several MHz. It is the default clock which can also be selected by :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_DEFAULT`.
    - :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_APLL` can support frequency between 648 Hz to several MHz, however, it might be occupied by other peripherals, then it may not provide the required frequency. But it doesn't mean APLL is not available in this case, it can still work as long as it can be divided to the target DAC DMA frequency correctly.

.. only:: esp32s2

    On ESP32-S2, DAC digital controller can be connected internally to the SPI3 and use its DMA for continuous conversion.

    The clock source of DAC digital controller are:

    - :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_APB` can support frequency between 77 Hz to several MHz. It is the default clock which can also be selected by :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_DEFAULT`.
    - :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_APLL` can support frequency between 6 Hz to several MHz, however, it might be occupied by other peripherals, then it may not provide the required frequency. But it doesn't mean APLL is not available in this case, it can still work as long as it can be divided to the target DAC DMA frequency correctly.


Cosine Wave Output (Cosine Mode)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The DAC peripheral has one cosine wave generator, it can generate cosine wave on the channels, users can specify the frequency, amplitude and phase of the cosine wave. To output the cosine wave, please acquire the DAC to cosine mode handle by :cpp:func:`dac_cosine_new_channel` first, and then start the cosine wave generator by :cpp:func:`dac_cosine_start`.

Currently, the source clock of the cosine wave generator only comes from ``RTC_FAST`` which can be chosen by :cpp:enumerator:`dac_cosine_clk_src_t::DAC_COSINE_CLK_SRC_RTC_FAST`, it is also the default clock source which is same as :cpp:enumerator:`dac_cosine_clk_src_t::DAC_COSINE_CLK_SRC_RTC_DEFAULT`.

Power Management
^^^^^^^^^^^^^^^^

When the power management is enabled (i.e. :ref:`CONFIG_PM_ENABLE` is on), the system will adjust or stop the source clock of DAC before going into light sleep, thus potentially influence to the DAC signals may lead the data conversion goes wrong.

When using DAC driver in continuous mode, it can prevent the system from changing or stopping the source clock in DMA or cosine wave mode by acquiring a power management lock. When the source clock is generated from APB, the lock type will be set to :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_APB_FREQ_MAX` and when the source clock is APLL (only in DMA mode), it will be set to :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_NO_LIGHT_SLEEP`. Whenever the DAC is converting (i.e. DMA or cosine wave generator is working), the driver will guarantee that the power management lock is acquired after calling :cpp:func:`dac_continuous_enable`. Likewise, the driver will release the lock when :cpp:func:`dac_continuous_disable` is called.

IRAM Safe
^^^^^^^^^

By default, the DAC DMA interrupt will be deferred when the Cache is disabled for reasons like writing/erasing Flash. Thus the DMA EOF interrupt will not get executed in time, which is not expected in a real-time application.

There's a Kconfig option :ref:`CONFIG_DAC_ISR_IRAM_SAFE` that will:

1. Enable the interrupt being serviced even when cache is disabled

2. Place driver object into DRAM (in case it's linked to PSRAM by accident)

This will allow the interrupt to run while the cache is disabled but will come at the cost of increased IRAM consumption.

Thread Safety
^^^^^^^^^^^^^

All the public DAC APIs are guaranteed to be thread safe by the driver, which means, users can call them from different RTOS tasks without protection by extra locks. Notice that DAC driver uses mutex lock to ensure the thread safety, thus the APIs except :cpp:func:`dac_oneshot_output_voltage` are not allowed to be used in ISR.

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_DAC_ISR_IRAM_SAFE` controls whether the default ISR handler can work when cache is disabled, see `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_DAC_SUPPRESS_DEPRECATE_WARN` controls whether to suppress the compiling warning message while using the legacy DAC driver.
- :ref:`CONFIG_DAC_ENABLE_DEBUG_LOG` is used to enabled the debug log output. Enable this option will increase the firmware binary size.

.. only:: esp32

    - :ref:`CONFIG_DAC_DMA_AUTO_16BIT_ALIGN` will auto expand the 8-bit data to 16-bit data in the driver to satisfy the I2S DMA format.

Application Example
-------------------

The basic examples for the ``One-shot Mode``, ``Continuous Mode`` and ``Cosine Mode`` can be found in:

- :example:`peripherals/dac/dac_oneshot`
- :example:`peripherals/dac/dac_continuous`
- :example:`peripherals/dac/dac_cosine_wave`

API Reference
-------------

.. include-build-file:: inc/dac_oneshot.inc
.. include-build-file:: inc/dac_cosine.inc
.. include-build-file:: inc/dac_continuous.inc
.. include-build-file:: inc/components/driver/include/driver/dac_types.inc
.. include-build-file:: inc/components/hal/include/hal/dac_types.inc
