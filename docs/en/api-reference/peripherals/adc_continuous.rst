Analog to Digital Converter (ADC) Continuous Mode Driver
========================================================

{IDF_TARGET_ADC_NUM:default="two", esp32c2="one", esp32c6="one", esp32h2="one"}

Introduction
------------

The Analog to Digital Converter is an on-chip sensor which is able to measure analog signals from specific analog IO pads.

{IDF_TARGET_NAME} has {IDF_TARGET_ADC_NUM} ADC unit(s), which can be used in scenario(s) like:

- Generate one-shot ADC conversion result
- Generate continuous ADC conversion results

This guide will introduce ADC continuous mode conversion.

Driver Concepts
^^^^^^^^^^^^^^^

ADC continuous mode conversion is made up with multiple Conversion Frames.

- Conversion Frame: One Conversion Frame contains multiple Conversion Results. Conversion Frame size is configured in :cpp:func:`adc_continuous_new_handle`, in bytes.
- Conversion Result: One Conversion Result contains multiple bytes (see :c:macro:`SOC_ADC_DIGI_RESULT_BYTES`). Its structure is :cpp:type:`adc_digi_output_data_t`, including ADC unit, ADC channel and raw data.

.. image:: /../_static/diagrams/adc/adc_conversion_frame.png
    :scale: 100 %
    :align: center

Functional Overview
-------------------

The following sections of this document cover the typical steps to install the ADC continuous mode driver, and read ADC conversion results from group of ADC channels continuously:

-  `Resource Allocation <#resource-allocation>`__ - covers which parameters should be set up to initialize the ADC continuous mode driver and how to deinitialize it.
-  `ADC Configurations <#adc-configurations>`__ - describes how to configure the ADC(s) to make it work under continuous mode.
-  `ADC Control <#adc-control>`__ - describes ADC control functions.
-  `Register Event Callbacks <#register-event-callbacks>`__ - describes how to hook user specific code to an ADC continuous mode event callback function.
-  `Read Conversion Result <#read-conversion-result>`__ - covers how to get ADC conversion result.
-  `Hardware Limitations <#hardware-limitations>`__ - describes the ADC related hardware limitations.
-  `Power Management <#power-management>`__ - covers power management related.
-  `IRAM Safe <#iram-safe>`__ - covers the IRAM safe functions.
-  `Thread Safety <#thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.


Resource Allocation
^^^^^^^^^^^^^^^^^^^

The ADC continuous mode driver is implemented based on {IDF_TARGET_NAME} SAR ADC module. Different ESP targets might have different number of independent ADCs.

To create an ADC continuous mode driver handle, set up the required configuration structure :cpp:type:`adc_continuous_handle_cfg_t`:

-  :cpp:member:`adc_continuous_handle_cfg_t::max_store_buf_size` set the maximum size (in bytes) of the pool that the driver saves ADC conversion result into. If this pool is full, new conversion results will be lost.
-  :cpp:member:`adc_continuous_handle_cfg_t::conv_frame_size` set the size of the ADC conversion frame, in bytes.


After setting up above configurations for the ADC, call :cpp:func:`adc_continuous_new_handle` with the prepared :cpp:type:`adc_continuous_handle_cfg_t`. This function may fail due to various errors such as invalid argumemts, insufficient memory, etc.

.. only:: esp32

    Especially, when this function returns :c:macro:`ESP_ERR_NOT_FOUND`, this means the I2S0 peripheral is in use. See `Hardware Limitations <#hardware-limitations>`__ for more information.

.. only:: esp32s2

    Especially, when this function returns :c:macro:`ESP_ERR_NOT_FOUND`, this means the SPI3 peripheral is in use. See `Hardware Limitations <#hardware-limitations>`__ for more information.

.. only:: SOC_GDMA_SUPPORTED

    Especially, when this function returns :c:macro:`ESP_ERR_NOT_FOUND`, this means there is no free GDMA channel.

If the ADC continuous mode driver is no longer used, you should deinitialize the driver by calling :cpp:func:`adc_continuous_deinit`.


.. only:: SOC_ADC_DIG_IIR_FILTER_SUPPORTED

    Two IIR filters are available when ADC is working under continuous mode. To create an ADC IIR filter, you should set up the :cpp:type:`adc_continuous_iir_filter_config_t`, and call :cpp:func:`adc_new_continuous_iir_filter`.

    - :cpp:member:`adc_digi_filter_config_t::unit`, ADC  unit.
    - :cpp:member:`adc_digi_filter_config_t::channel`, ADC channel to be filtered.
    - :cpp:member:`adc_digi_filter_config_t::coeff`, filter coefficient.

    .. only:: SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED

            On ESP32S2, the filter is per ADC unit. Once a filter is enabled, all the enabled ADC channels in this ADC unit will be filtered. However, we suggest only enabling one ADC channel per unit, when using the filter feature. Because the filtered results depend on the previous filtered result. So you should not enable multiple ADC channels, to avoid mixing the filtered results.

    To recycle a filter, you should call :cpp:func:`adc_del_continuous_iir_filter`.

    .. only:: not SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED

        .. note::

            If you use both the filters on a same ADC channel, then only the first one will take effect.

Initialize the ADC Continuous Mode Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 100,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config));


Recycle the ADC Unit
~~~~~~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_continuous_deinit());


ADC Configurations
^^^^^^^^^^^^^^^^^^

After the ADC continuous mode driver is initialized, set up the :cpp:type:`adc_continuous_config_t` to configure ADC IOs to measure analog signal:

-  :cpp:member:`adc_continuous_config_t::pattern_num`, number of ADC channels that will be used.
-  :cpp:member:`adc_continuous_config_t::adc_pattern`, list of configs for each ADC channel that will be used, see below description.
-  :cpp:member:`adc_continuous_config_t::sample_freq_hz`, expected ADC sampling frequency in Hz.
-  :cpp:member:`adc_continuous_config_t::conv_mode`, continuous conversion mode.
-  :cpp:member:`adc_continuous_config_t::format`, conversion output format.

For :cpp:type:`adc_digi_pattern_config_t`:

- :cpp:member:`adc_digi_pattern_config_t::atten`, ADC attenuation. Refer to the On-Chip Sensor chapter in `TRM <{IDF_TARGET_TRM_EN_URL}>`__.
- :cpp:member:`adc_digi_pattern_config_t::channel`, the IO corresponding ADC channel number. See below note.
- :cpp:member:`adc_digi_pattern_config_t::unit`, the ADC that the IO is subordinate to.
- :cpp:member:`adc_digi_pattern_config_t::bit_width`, the bitwidth of the raw conversion result.

.. note::

    For the IO corresponding ADC channel number. Check `datasheet <{IDF_TARGET_TRM_EN_URL}>`__ to acquire the ADC IOs.
    On the other hand, :cpp:func:`adc_continuous_io_to_channel` and :cpp:func:`adc_continuous_channel_to_io` can be used to acquire the ADC channels and ADC IOs.

To make these settings take effect, call :cpp:func:`adc_continuous_config` with the configuration structure above.
This API may fail due to reasons like :c:macro:`ESP_ERR_INVALID_ARG`. When it returns :c:macro:`ESP_ERR_INVALID_STATE`, this means the ADC continuous mode driver is started, you shouldn't call this API at this moment.

See ADC continuous mode example :example:`peripherals/adc/continuous_read` to see configuration codes.


.. only:: SOC_ADC_DIG_IIR_FILTER_SUPPORTED

    To enable / disable the ADC IIR filter, you should call :cpp:func:`adc_continuous_iir_filter_enable` / :cpp:func:`adc_continuous_iir_filter_disable`.

ADC Control
^^^^^^^^^^^

Start and Stop
~~~~~~~~~~~~~~

Calling :cpp:func:`adc_continuous_start` will make the ADC start to measure analog signals from the configured ADC channels, and generate the conversion results.
On the contrary, calling :cpp:func:`adc_continuous_stop` will stop the ADC conversion.

.. code::c

    ESP_ERROR_CHECK(adc_continuous_start());

.. code:: c

    ESP_ERROR_CHECK(adc_continuous_stop());


Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

By calling :cpp:func:`adc_continuous_register_event_callbacks`, you can hook your own function to the driver ISR. Supported event callbacks are listed in :cpp:type:`adc_continuous_evt_cbs_t`
- :cpp:member:`adc_continuous_evt_cbs_t::on_conv_done`, this is invoked when one conversion frame finishes.
- :cpp:member:`adc_continuous_evt_cbs_t::on_pool_ovf`, this is invoked when internal pool is full. Newer conversion results will be discarded.

As above callbacks are called in an ISR context, you should always ensure the callback function is suitable for an ISR context. Blocking logics should not appear in these callbacks. Callback function prototype is declared in :cpp:type:`adc_continuous_callback_t`.

You can also register your own context when calling :cpp:func:`adc_continuous_register_event_callbacks`, by the parameter ``user_data``. This user data will be passed to the callback functions directly.

This function may fail due to reasons like :c:macro:`ESP_ERR_INVALID_ARG`. Specially, when :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE` is enabled, this error may indicate that the callback functions aren't in internal RAM. Check error log to know this. Besides, when it fails due to :c:macro:`ESP_ERR_INVALID_STATE`, this means the ADC continuous mode driver is started, you shouldn't add callback at this moment.


Conversion Done Event
~~~~~~~~~~~~~~~~~~~~~

The driver will fill in the event data of a :cpp:member:`adc_continuous_evt_cbs_t::on_conv_done` event. Event data contains a buffer pointer to a conversion frame buffer, together with the size. Refer to :cpp:type:`adc_continuous_evt_data_t` to know the event data structure.

.. note::

    It is worth noting that, the data buffer :cpp:member:`adc_continuous_evt_data_t::conv_frame_buffer` is maintained by the driver itself. Therefore, never free this piece of memory.

.. note::

    When the Kconfig option :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE` is enabled, the registered callbacks and the functions called by the callbacks should be placed in IRAM. The involved variables should be placed in internal RAM as well.

Pool Overflow Event
~~~~~~~~~~~~~~~~~~~

The ADC continuous mode driver has an internal pool to save the conversion results. When the pool is full, a pool overflow event will emerge. Under this condition, the driver won't fill in the event data. This usually happens the speed to read data from the pool (by calling :cpp:func:`adc_continuous_read`) is much slower than the ADC conversion speed.


Read Conversion Result
^^^^^^^^^^^^^^^^^^^^^^

After calling :cpp:func:`adc_continuous_start`, the ADC continuous conversion starts. Call :cpp:func:`adc_continuous_read` to get the conversion results of the ADC channels. You need to provide a buffer to get the raw results.

This function will try to read the expected length of conversion results each time.

- If the requested length isn't reached, the function will still move the data from the internal pool to the buffer you prepared. Therefore, check the `out_length` to know the actual size of conversion results.
- If there is no conversion result generated in the internal pool, the function will block for `timeout_ms` until the conversion results are generated. If there is still no generated results, the function will return :c:macro:`ESP_ERR_TIMEOUT`.
- If the generated results fill up the internal pool, new generated results will be lost. Next time when the :cpp:func:`adc_continuous_read` is called, this function will return :c:macro:`ESP_ERR_INVALID_STATE` indicating this situation.

This API aims to give you a chance to read all the ADC continuous conversion results.

The ADC conversion results read from above function are raw data. To calculate the voltage based on the ADC raw results, this formula can be used:

.. parsed-literal::

    Vout = Dout * Vmax / Dmax       (1)

where:

======  =============================================================
Vout    Digital output result, standing for the voltage.
Dout    ADC raw digital reading result.
Vmax    Maximum measurable input analog voltage, this is related to the ADC attenuation, please refer to the On-Chip Sensor chapter in `TRM <{IDF_TARGET_TRM_EN_URL}>`__.
Dmax    Maximum of the output ADC raw digital reading result, which is 2^bitwidth, where bitwidth is the :cpp:member::`adc_digi_pattern_config_t:bit_width` configured before.
======  =============================================================

To do further calbration to convert the ADC raw result to voltage in mV, please refer to calibration doc :doc:`adc_calibration`.

.. _hardware_limitations_adc_continuous:

Hardware Limitations
^^^^^^^^^^^^^^^^^^^^

- A specific ADC unit can only work under one operating mode at any one time, either continuous mode or oneshot mode. :cpp:func:`adc_continuous_start` has provided the protection.

- Random Number Generator uses ADC as an input source. When ADC continuous mode driver works, the random number generated from RNG will be less random.

.. only:: esp32 or esp32s2

    - ADC2 is also used by the Wi-Fi. :cpp:func:`adc_continuous_start` has provided the protection between Wi-Fi driver and ADC continuous mode driver.

.. only:: esp32

    - ADC continuous mode driver uses I2S0 peripheral as hardware DMA fifo. Therefore, if I2S0 is in use already, the :cpp:func:`adc_continuous_new_handle` will return :c:macro:`ESP_ERR_NOT_FOUND`.

    - ESP32 DevKitC: GPIO 0 cannot be used due to external auto program circuits.

    - ESP-WROVER-KIT: GPIO 0, 2, 4 and 15 cannot be used due to external connections for different purposes.

.. only:: esp32s2

    - ADC continuous mode driver uses SPI3 peripheral as hardware DMA fifo. Therefore, if SPI3 is in use already, the :cpp:func:`adc_continuous_new_handle` will return :c:macro:`ESP_ERR_NOT_FOUND`.

.. only:: esp32c3

    - ADC2 continuous mode is no longer supported, due to hardware limitation. The results are not stable. This issue can be found in `ESP32C3 Errata <https://www.espressif.com/sites/default/files/documentation/esp32-c3_errata_en.pdf>`_. For compatibility, you can enable :ref:`CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3` to force use ADC2.

.. only:: esp32s3

    - ADC2 continuous mode is no longer supported, due to hardware limitation. The results are not stable. This issue can be found in `ESP32S3 Errata <https://www.espressif.com/sites/default/files/documentation/esp32-s3_errata_en.pdf>`_. For compatibility, you can enable :ref:`CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3` to force use ADC2.


Power Management
^^^^^^^^^^^^^^^^

When power management is enabled (i.e. :ref:`CONFIG_PM_ENABLE` is on), the APB clock frequency may be adjusted when the system is in an idle state, thus potentially changing the behavior of ADC continuous conversion.

However, the continuous mode driver can prevent this change by acquiring a power management lock of type :cpp:enumerator:`ESP_PM_APB_FREQ_MAX`. The lock is acquired after the continuous conversion is started by :cpp:func:`adc_continuous_start`. Similarly, the lock will be released after :cpp:func:`adc_continuous_stop`. Therefore, :cpp:func:`adc_continuous_start` and :cpp:func:`adc_continuous_stop` should appear in pairs, otherwise the power management will be out of action.


IRAM Safe
^^^^^^^^^

All the ADC continuous mode driver APIs are not IRAM-safe. They are not supposed to be run when the Cache is disabled. By enabling the Kconfig option :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE`, driver internal ISR handler is IRAM-safe, which means even when the Cache is disabled, the driver will still save the conversion results into its internal pool.


Thread Safety
^^^^^^^^^^^^^

ADC continuous mode driver APIs are not guaranteed to be thread safe. However, the share hardware mutual exclusion is provided by the driver. See `Hardware Limitations <#hardware-limitations>`__ for more details.


Application Examples
--------------------

* ADC continuous mode example: :example:`peripherals/adc/continuous_read`.


API Reference
-------------

.. include-build-file:: inc/adc_continuous.inc
