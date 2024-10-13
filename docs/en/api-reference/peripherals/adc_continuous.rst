Analog to Digital Converter (ADC) Continuous Mode Driver
========================================================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_ADC_NUM:default="two", esp32c2="one", esp32c6="one", esp32h2="one", esp32c5="one"}

Introduction
------------

The Analog to Digital Converter is integrated on the chip and is capable of measuring analog signals from specific analog IO pads. Additionally, the Direct Memory Access (DMA) functionality is utilized to efficiently retrieve ADC conversion results.

{IDF_TARGET_NAME} has {IDF_TARGET_ADC_NUM} ADC unit(s), which can be used in scenarios like:

- Generate one-shot ADC conversion result
- Generate continuous ADC conversion results

This guide introduces ADC continuous mode conversion.

Driver Concepts
^^^^^^^^^^^^^^^

ADC continuous mode conversion is made up of multiple conversion frames.

- Conversion Frame: One conversion frame contains multiple conversion results. Conversion frame size is configured in :cpp:func:`adc_continuous_new_handle` in bytes.
- Conversion Result: One conversion result contains multiple bytes, see :c:macro:`SOC_ADC_DIGI_RESULT_BYTES`. Its structure is :cpp:type:`adc_digi_output_data_t`, including ADC unit, ADC channel, and raw data.

.. image:: /../_static/diagrams/adc/adc_conversion_frame.png
    :scale: 100 %
    :align: center

Functional Overview
-------------------

The following sections of this document cover the typical steps to install the ADC continuous mode driver, and read ADC conversion results from a group of ADC channels continuously:

- :ref:`adc-continuous-resource-allocation`: covers which parameters should be set up to initialize the ADC continuous mode driver and how to deinitialize it.
- :ref:`adc-continuous-adc-configurations`: describes how to configure the ADC(s) to make it work under continuous mode.
- :ref:`adc-continuous-adc-control`: describes ADC control functions.
- :ref:`adc-continuous-register-event-callbacks`: describes how to hook user-specific code to an ADC continuous mode event callback function.
- :ref:`adc-continuous-read-conversion-result`: covers how to get ADC conversion result.
- :ref:`adc-continuous-hardware-limitations`: describes the ADC-related hardware limitations.
- :ref:`adc-continuous-power-management`: covers power management-related information.
- :ref:`adc-continuous-iram-safe`: covers the IRAM safe functions.
- :ref:`adc-continuous-thread-safety`: lists which APIs are guaranteed to be thread-safe by the driver.


.. _adc-continuous-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

The ADC continuous mode driver is implemented based on {IDF_TARGET_NAME} SAR ADC module. Different ESP targets might have different numbers of independent ADCs.

To create an ADC continuous mode driver handle, set up the required configuration structure :cpp:type:`adc_continuous_handle_cfg_t`:

- :cpp:member:`adc_continuous_handle_cfg_t::max_store_buf_size`: set the maximum size of the pool in bytes, and the driver saves ADC conversion result into the pool. If this pool is full, new conversion results will be lost.
- :cpp:member:`adc_continuous_handle_cfg_t::conv_frame_size`: set the size of the ADC conversion frame, in bytes.
- :cpp:member:`adc_continuous_handle_cfg_t::flags`: set the flags that can change the driver's behavior.

  - ``flush_pool``: auto flush the pool when it's full.


After setting up the above configurations for the ADC, call :cpp:func:`adc_continuous_new_handle` with the prepared :cpp:type:`adc_continuous_handle_cfg_t`. This function may fail due to various errors such as invalid arguments, insufficient memory, etc.

.. only:: esp32

    Especially, when this function returns :c:macro:`ESP_ERR_NOT_FOUND`, this means the I2S0 peripheral is in use. See :ref:`adc-continuous-hardware-limitations` for more information.

.. only:: esp32s2

    Especially, when this function returns :c:macro:`ESP_ERR_NOT_FOUND`, this means the SPI3 peripheral is in use. See :ref:`adc-continuous-hardware-limitations` for more information.

.. only:: SOC_GDMA_SUPPORTED

    Especially, when this function returns :c:macro:`ESP_ERR_NOT_FOUND`, this means there is no free GDMA channel.

If the ADC continuous mode driver is no longer used, you should deinitialize the driver by calling :cpp:func:`adc_continuous_deinit`.


.. only:: SOC_ADC_DIG_IIR_FILTER_SUPPORTED

    IIR filter
    ~~~~~~~~~~

    Two IIR filters are available when ADC is working in continuous mode. To create an ADC IIR filter, you should set up :cpp:type:`adc_continuous_iir_filter_config_t` and call :cpp:func:`adc_new_continuous_iir_filter`.

    - :cpp:member:`adc_digi_filter_config_t::unit`: ADC unit.
    - :cpp:member:`adc_digi_filter_config_t::channel`: ADC channel to be filtered.
    - :cpp:member:`adc_digi_filter_config_t::coeff`: Filter coefficient.

    .. only:: SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED

            On {IDF_TARGET_NAME}, the filter is per ADC unit. Once a filter is enabled, all the enabled ADC channels in this ADC unit will be filtered. However, we suggest only enabling one ADC channel per unit, when using the filter feature. Because the filtered results depend on the previous filtered result. So you should not enable multiple ADC channels, to avoid mixing the filtered results.

    To recycle a filter, you should call :cpp:func:`adc_del_continuous_iir_filter`.

    .. only:: not SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED

        .. note::

            If you use both filters on the same ADC channel, then only the first one will take effect.

.. only:: SOC_ADC_MONITOR_SUPPORTED

    Monitor
    ~~~~~~~

    {IDF_TARGET_SOC_ADC_DIGI_MONITOR_NUM} monitors are available when ADC is working under continuous mode, you can set one or two threshold(s) of a monitor on a working ADC channel, then the monitor will invoke interrupts every sample loop if conversion result outranges of the threshold. To create an ADC monitor, you need to set up the :cpp:type:`adc_monitor_config_t` and call :cpp:func:`adc_new_continuous_monitor`.

    - :cpp:member:`adc_monitor_config_t::adc_unit`: Configures which ADC unit the channel you want to monitor belongs to.
    - :cpp:member:`adc_monitor_config_t::channel`: The channel you want to monitor.
    - :cpp:member:`adc_monitor_config_t::h_threshold`: The high threshold, conversion result larger than this value invokes interrupt, set to -1 if do not use.
    - :cpp:member:`adc_monitor_config_t::l_threshold`: The low threshold, conversion result less than this value invokes interrupt, set to -1 if do not use.

    Once a monitor is created, you can operate it by following APIs to construct your apps.

    - :cpp:func:`adc_continuous_monitor_enable`: Enable a monitor.
    - :cpp:func:`adc_continuous_monitor_disable`: Disable a monitor.
    - :cpp:func:`adc_monitor_register_callbacks`: register user callbacks to take action when the ADC value exceeds of the threshold.
    - :cpp:func:`adc_del_continuous_monitor`: Delete a created monitor and free resources.

    .. only:: esp32s2

        .. NOTE::

            There are some hardware limitations on {IDF_TARGET_NAME}:
            1. Only one threshold is supported for one monitor.
            2. Only one monitor is supported for one ADC unit.
            3. All enabled channel(s) of a certain ADC unit in ADC continuous mode driver will be monitored. The :cpp:member:`adc_monitor_config_t::channel` parameter will not be used.

Initialize the ADC Continuous Mode Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 256,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));


Recycle the ADC Unit
~~~~~~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_continuous_deinit(handle));


.. _adc-continuous-adc-configurations:

ADC Configurations
^^^^^^^^^^^^^^^^^^

After the ADC continuous mode driver is initialized, set up the :cpp:type:`adc_continuous_config_t` to configure ADC IOs to measure analog signal:

- :cpp:member:`adc_continuous_config_t::pattern_num`: number of ADC channels that will be used.
- :cpp:member:`adc_continuous_config_t::adc_pattern`: list of configs for each ADC channel that will be used, see the description below.
- :cpp:member:`adc_continuous_config_t::sample_freq_hz`: expected ADC sampling frequency in Hz.
- :cpp:member:`adc_continuous_config_t::conv_mode`: continuous conversion mode.
- :cpp:member:`adc_continuous_config_t::format`: conversion output format.

Set :cpp:type:`adc_digi_pattern_config_t` with the following process:

- :cpp:member:`adc_digi_pattern_config_t::atten`: ADC attenuation. Refer to the On-Chip Sensor and Analog Signal Processing chapter in `TRM <{IDF_TARGET_TRM_EN_URL}>`__.
- :cpp:member:`adc_digi_pattern_config_t::channel`: the IO corresponding ADC channel number. See the note below.
- :cpp:member:`adc_digi_pattern_config_t::unit`: the ADC that the IO is subordinate to.
- :cpp:member:`adc_digi_pattern_config_t::bit_width`: the bitwidth of the raw conversion result.

.. note::

    For the IO corresponding ADC channel number, check `TRM <{IDF_TARGET_TRM_EN_URL}#sensor>`__ to acquire the ADC IOs. Besides, :cpp:func:`adc_continuous_io_to_channel` and :cpp:func:`adc_continuous_channel_to_io` can be used to acquire the ADC channels and ADC IOs.

To make these settings take effect, call :cpp:func:`adc_continuous_config` with the configuration structure above. This API may fail due to reasons like :c:macro:`ESP_ERR_INVALID_ARG`. When it returns :c:macro:`ESP_ERR_INVALID_STATE`, this means the ADC continuous mode driver is started, you should not call this API at this moment.

See ADC continuous mode example :example:`peripherals/adc/continuous_read` to see configuration codes.


.. only:: SOC_ADC_DIG_IIR_FILTER_SUPPORTED

    To enable/disable the ADC IIR filter, you should call :cpp:func:`adc_continuous_iir_filter_enable` / :cpp:func:`adc_continuous_iir_filter_disable`.

.. only:: SOC_ADC_MONITOR_SUPPORTED

    To enable/disable the ADC monitor, you should call :cpp:func:`adc_continuous_monitor_enable` / :cpp:func:`adc_continuous_monitor_disable`.

.. _adc-continuous-adc-control:

ADC Control
^^^^^^^^^^^

Start and Stop
~~~~~~~~~~~~~~

Calling :cpp:func:`adc_continuous_start` makes the ADC start to measure analog signals from the configured ADC channels, and generate the conversion results.

On the contrary, calling :cpp:func:`adc_continuous_stop` stops the ADC conversion.

.. code::c

    ESP_ERROR_CHECK(adc_continuous_start(handle));

.. code:: c

    ESP_ERROR_CHECK(adc_continuous_stop(handle));


.. _adc-continuous-register-event-callbacks:

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

By calling :cpp:func:`adc_continuous_register_event_callbacks`, you can hook your own function to the driver ISR. Supported event callbacks are listed in :cpp:type:`adc_continuous_evt_cbs_t`.

- :cpp:member:`adc_continuous_evt_cbs_t::on_conv_done`: this is invoked when one conversion frame finishes.
- :cpp:member:`adc_continuous_evt_cbs_t::on_pool_ovf`: this is invoked when the internal pool is full. Newer conversion results will be discarded.

As the above callbacks are called in an ISR context, you should always ensure the callback function is suitable for an ISR context. Blocking logic should not appear in these callbacks. The callback function prototype is declared in :cpp:type:`adc_continuous_callback_t`.

You can also register your own context when calling :cpp:func:`adc_continuous_register_event_callbacks` by the parameter ``user_data``. This user data will be passed to the callback functions directly.

This function may fail due to reasons like :c:macro:`ESP_ERR_INVALID_ARG`. Especially, when :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE` is enabled, this error may indicate that the callback functions are not in the internal RAM. Check the error log for more details. Besides, when it fails due to :c:macro:`ESP_ERR_INVALID_STATE`, it indicates that the ADC continuous mode driver is started, and you should not add a callback at this moment.


Conversion Done Event
~~~~~~~~~~~~~~~~~~~~~

When the driver completes a conversion, it triggers the :cpp:member:`adc_continuous_evt_cbs_t::on_conv_done` event and fills the event data. Event data contains a buffer pointer to a conversion frame buffer, together with the size. Refer to :cpp:type:`adc_continuous_evt_data_t` to know the event data structure.

.. note::

    It is worth noting that, the data buffer :cpp:member:`adc_continuous_evt_data_t::conv_frame_buffer` is maintained by the driver itself. Therefore, never free this piece of memory.

.. note::

    When the Kconfig option :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE` is enabled, the registered callbacks and the functions called by the callbacks should be placed in IRAM. The involved variables should be placed in internal RAM as well.

Pool Overflow Event
~~~~~~~~~~~~~~~~~~~

The ADC continuous mode driver has an internal pool to save the conversion results. When the pool is full, a pool overflow event will emerge. Under this condition, the driver will not fill in the event data. This usually happens because the speed to read data from the pool by calling :cpp:func:`adc_continuous_read` is much slower than the ADC conversion speed.


.. _adc-continuous-read-conversion-result:

Read Conversion Result
^^^^^^^^^^^^^^^^^^^^^^

After calling :cpp:func:`adc_continuous_start`, the ADC continuous conversion starts. Call :cpp:func:`adc_continuous_read` to get the conversion results of the ADC channels. You need to provide a buffer to get the raw results.

Function :cpp:func:`adc_continuous_read` tries to read the expected length of conversion results each time.

- When calling :cpp:func:`adc_continuous_read`, you can request to read a conversion result of the specified length. Sometimes, however, the actual available conversion results may be less than the requested length, in which case the function still moves the data from the internal pool into the buffer you provided. Therefore, to learn the number of conversion results actually moved into the buffer, please check the value of ``out_length``.
- If there is no conversion result generated in the internal pool, the function will block for ``timeout_ms`` until the conversion results are generated. If there are still no generated results, the function will return :c:macro:`ESP_ERR_TIMEOUT`.
- If the generated results fill up the internal pool, newly generated results will be lost. Next time when :cpp:func:`adc_continuous_read` is called, this function will return :c:macro:`ESP_ERR_INVALID_STATE` to indicate this situation.

This API aims to give you a chance to read all the ADC continuous conversion results.

The ADC conversion results read from the above function are raw data. To calculate the voltage based on the ADC raw results, this formula can be used:

.. parsed-literal::

    Vout = Dout * Vmax / Dmax       (1)

where:

.. list-table::
    :header-rows: 1
    :widths: 20 80
    :align: center

    * - Vout
      - Digital output result, standing for the voltage.
    * - Dout
      - ADC raw digital reading result.
    * - Vmax
      - Maximum measurable input analog voltage, this is related to the ADC attenuation, please refer to the On-Chip Sensor and Analog Signal Processing chapter in `Datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`__.
    * - Dmax
      - Maximum of the output ADC raw digital reading result, which is 2^bitwidth, where the bitwidth is the :cpp:member:`adc_digi_pattern_config_t::bit_width` configured before.

To do further calibration to convert the ADC raw result to voltage in mV, please refer to :doc:`adc_calibration`.

.. _adc-continuous-hardware-limitations:

.. _hardware_limitations_adc_continuous:

Hardware Limitations
^^^^^^^^^^^^^^^^^^^^

- A specific ADC unit can only work under one operating mode at any one time, either continuous mode or one-shot mode. :cpp:func:`adc_continuous_start` has provided the protection.

- Random Number Generator (RNG) uses ADC as an input source. When ADC continuous mode driver works, the random number generated from RNG will be less random.

.. only:: esp32 or esp32s2

    - ADC2 is also used by Wi-Fi. :cpp:func:`adc_continuous_start` has provided the protection between Wi-Fi driver and ADC continuous mode driver.

.. only:: esp32

    - ADC continuous mode driver uses I2S0 peripheral as hardware DMA FIFO. Therefore, if I2S0 is in use already, the :cpp:func:`adc_continuous_new_handle` will return :c:macro:`ESP_ERR_NOT_FOUND`.

    - ESP32 DevKitC: GPIO 0 cannot be used due to external auto program circuits.

    - ESP-WROVER-KIT: GPIO 0, 2, 4, and 15 cannot be used due to external connections for different purposes.

.. only:: esp32s2

    - ADC continuous mode driver uses SPI3 peripheral as hardware DMA FIFO. Therefore, if SPI3 is in use already, the :cpp:func:`adc_continuous_new_handle` will return :c:macro:`ESP_ERR_NOT_FOUND`.

.. only:: esp32c3

    - ADC2 DMA functionality is no longer supported to retrieve ADC conversion results due to hardware limitations, as unstable results have been observed. This issue can be found in `ESP32C3 Errata <https://www.espressif.com/sites/default/files/documentation/esp32-c3_errata_en.pdf>`_. For compatibility, you can enable :ref:`CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3` to force use ADC2.

.. only:: esp32s3

    - ADC2 DMA functionality is no longer supported to retrieve ADC conversion results due to hardware limitations, as unstable results have been observed. This issue can be found in `ESP32S3 Errata <https://www.espressif.com/sites/default/files/documentation/esp32-s3_errata_en.pdf>`_. For compatibility, you can enable :ref:`CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3` to force use ADC2.

    .. _adc-continuous-power-management:

.. only:: not esp32s3

    .. _adc-continuous-power-management:

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled, i.e., :ref:`CONFIG_PM_ENABLE` is on, the APB clock frequency may be adjusted when the system is in an idle state, thus potentially changing the behavior of ADC continuous conversion.

However, the continuous mode driver can prevent this change by acquiring a power management lock of type :cpp:enumerator:`ESP_PM_APB_FREQ_MAX`. The lock is acquired after the continuous conversion is started by :cpp:func:`adc_continuous_start`. Similarly, the lock will be released after :cpp:func:`adc_continuous_stop`. Therefore, :cpp:func:`adc_continuous_start` and :cpp:func:`adc_continuous_stop` should appear in pairs, otherwise, the power management will be out of action.


.. _adc-continuous-iram-safe:

IRAM Safe
^^^^^^^^^

All the ADC continuous mode driver APIs are not IRAM-safe. They are not supposed to be run when the Cache is disabled. By enabling the Kconfig option :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE`, the driver's internal ISR handler is IRAM-safe, which means even when the Cache is disabled, the driver will still save the conversion results into its internal pool.


.. _adc-continuous-thread-safety:

Thread Safety
^^^^^^^^^^^^^

ADC continuous mode driver APIs are not guaranteed to be thread-safe. However, the share hardware mutual exclusion is provided by the driver. See :ref:`adc-continuous-hardware-limitations` for more details.


Application Examples
--------------------

* :example:`peripherals/adc/continuous_read` demonstrates how to use the ADC Continuous Read Mode (DMA Mode) on {IDF_TARGET_NAME} development boards to read from GPIO pins via on-chip ADC modules.


API Reference
-------------

.. include-build-file:: inc/adc_continuous.inc
