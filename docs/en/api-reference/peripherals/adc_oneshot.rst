Analog to Digital Converter (ADC) Oneshot Mode Driver
=====================================================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_ADC_NUM:default="two", esp32c2="one", esp32c6="one", esp32h2="one"}

Introduction
------------

The Analog to Digital Converter is integrated on the chip and is capable of measuring analog signals from specific analog IO pins.

{IDF_TARGET_NAME} has {IDF_TARGET_ADC_NUM} ADC unit(s), which can be used in scenario(s) like:

- Generate one-shot ADC conversion result

.. only:: SOC_ADC_DMA_SUPPORTED

    - Generate continuous ADC conversion results

This guide introduces ADC oneshot mode conversion.


Functional Overview
-------------------

The following sections of this document cover the typical steps to install and operate an ADC:

- :ref:`adc-oneshot-resource-allocation` - covers which parameters should be set up to get an ADC handle and how to recycle the resources when ADC finishes working.
- :ref:`adc-oneshot-unit-configuration` - covers the parameters that should be set up to configure the ADC unit, so as to get ADC conversion raw result.
- :ref:`adc-oneshot-read-conversion-result` - covers how to get ADC conversion raw result.
- :ref:`hardware_limitations_adc_oneshot` - describes the ADC-related hardware limitations.
- :ref:`adc-oneshot-power-management` - covers power management-related information.
- :ref:`adc-oneshot-iram-safe` - describes tips on how to read ADC conversion raw results when the cache is disabled.
- :ref:`adc-oneshot-thread-safety` - lists which APIs are guaranteed to be thread-safe by the driver.
- :ref:`adc-oneshot-kconfig-options` - lists the supported Kconfig options that can be used to make a different effect on driver behavior.

.. _adc-oneshot-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

The ADC oneshot mode driver is implemented based on {IDF_TARGET_NAME} SAR ADC module. Different ESP chips might have different numbers of independent ADCs. From the oneshot mode driver's point of view, an ADC instance is represented by :cpp:type:`adc_oneshot_unit_handle_t`.

To install an ADC instance, set up the required initial configuration structure :cpp:type:`adc_oneshot_unit_init_cfg_t`:

- :cpp:member:`adc_oneshot_unit_init_cfg_t::unit_id` selects the ADC. Please refer to the `datasheet <{IDF_TARGET_TRM_EN_URL}>`__ to know dedicated analog IOs for this ADC.
- :cpp:member:`adc_oneshot_unit_init_cfg_t::clk_src` selects the source clock of the ADC. If set to 0, the driver will fall back to using a default clock source, see :cpp:type:`adc_oneshot_clk_src_t` to know the details.
- :cpp:member:`adc_oneshot_unit_init_cfg_t::ulp_mode` sets if the ADC will be working under ULP mode.

.. todo::

   Add ULP ADC-related docs here.

After setting up the initial configurations for the ADC, call :cpp:func:`adc_oneshot_new_unit` with the prepared :cpp:type:`adc_oneshot_unit_init_cfg_t`. This function will return an ADC unit handle if the allocation is successful.

This function may fail due to various errors such as invalid arguments, insufficient memory, etc. Specifically, when the to-be-allocated ADC instance is registered already, this function will return :c:macro:`ESP_ERR_NOT_FOUND` error. Number of available ADC(s) is recorded by :c:macro:`SOC_ADC_PERIPH_NUM`.

If a previously created ADC instance is no longer required, you should recycle the ADC instance by calling :cpp:func:`adc_oneshot_del_unit`, related hardware and software resources will be recycled as well.

Create an ADC Unit Handle Under Normal Oneshot Mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));


Recycle the ADC Unit
~~~~~~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));


.. _adc-oneshot-unit-configuration:

Unit Configuration
^^^^^^^^^^^^^^^^^^

After an ADC instance is created, set up the :cpp:type:`adc_oneshot_chan_cfg_t` to configure ADC IOs to measure analog signal:

- :cpp:member:`adc_oneshot_chan_cfg_t::atten`, ADC attenuation. Refer to `TRM <{IDF_TARGET_TRM_EN_URL}>`__ > ``On-Chip Sensor and Analog Signal Processing``.
- :cpp:member:`adc_oneshot_chan_cfg_t::bitwidth`, the bitwidth of the raw conversion result.

.. note::

    For the IO corresponding ADC channel number, check `datasheet <{IDF_TARGET_TRM_EN_URL}>`__ to know the ADC IOs.

    Additionally, :cpp:func:`adc_continuous_io_to_channel` and :cpp:func:`adc_continuous_channel_to_io` can be used to know the ADC channels and ADC IOs.

To make these settings take effect, call :cpp:func:`adc_oneshot_config_channel` with the above configuration structure. You should specify an ADC channel to be configured as well. Function :cpp:func:`adc_oneshot_config_channel` can be called multiple times to configure different ADC channels. The Driver will save each of these channel configurations internally.


Configure Two ADC Channels
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN1, &config));


.. _adc-oneshot-read-conversion-result:

Read Conversion Result
^^^^^^^^^^^^^^^^^^^^^^

After above configurations, the ADC is ready to measure the analog signal(s) from the configured ADC channel(s). Call :cpp:func:`adc_oneshot_read` to get the conversion raw result of an ADC channel.

- :cpp:func:`adc_oneshot_read` is safe to use. ADC(s) are shared by some other drivers/peripherals, see :ref:`hardware_limitations_adc_oneshot`. This function uses mutexes to avoid concurrent hardware usage. Therefore, this function should not be used in an ISR context. This function may fail when the ADC is in use by other drivers/peripherals, and return :c:macro:`ESP_ERR_TIMEOUT`. Under this condition, the ADC raw result is invalid.

This function will fail due to invalid arguments.

The ADC conversion results read from this function are raw data. To calculate the voltage based on the ADC raw results, this formula can be used:

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
      - Maximum measurable input analog voltage, this is related to the ADC attenuation, please refer to `TRM <{IDF_TARGET_TRM_EN_URL}>`__ > ``On-Chip Sensor and Analog Signal Processing``.
    * - Dmax
      -  Maximum of the output ADC raw digital reading result, which is 2^bitwidth, where bitwidth is the :cpp:member::`adc_oneshot_chan_cfg_t:bitwidth` configured before.

To do further calibration to convert the ADC raw result to voltage in mV, please refer to calibration doc :doc:`adc_calibration`.


Read Raw Result
~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0][0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0][0]);

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN1, &adc_raw[0][1]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, adc_raw[0][1]);


.. _hardware_limitations_adc_oneshot:

Hardware Limitations
^^^^^^^^^^^^^^^^^^^^

- Random Number Generator (RNG) uses ADC as an input source. When ADC :cpp:func:`adc_oneshot_read` works, the random number generated from RNG will be less random.

.. only:: SOC_ADC_DMA_SUPPORTED

    - A specific ADC unit can only work under one operating mode at any one time, either continuous mode or oneshot mode. :cpp:func:`adc_oneshot_read` has provided the protection.

.. only:: esp32 or esp32s2 or esp32s3

    - ADC2 is also used by Wi-Fi. :cpp:func:`adc_oneshot_read` has provided protection between the Wi-Fi driver and ADC oneshot mode driver.

.. only:: esp32c3

    - ADC2 oneshot mode is no longer supported, due to hardware limitations. The results are not stable. This issue can be found in `ESP32-C3 Series SoC Errata <https://www.espressif.com/sites/default/files/documentation/esp32-c3_errata_en.pdf>`_. For compatibility, you can enable :ref:`CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3` to force use ADC2.

.. only:: esp32

    - ESP32-DevKitC: GPIO0 cannot be used in oneshot mode, because the DevKit has used it for auto-flash.

    - ESP-WROVER-KIT: GPIO 0, 2, 4, and 15 cannot be used due to external connections for different purposes.

    .. _adc-oneshot-power-management:

.. only:: not esp32

    .. _adc-oneshot-power-management:

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled, i.e., :ref:`CONFIG_PM_ENABLE` is on, the system clock frequency may be adjusted when the system is in an idle state. However, the ADC oneshot mode driver works in a polling routine, the :cpp:func:`adc_oneshot_read` will poll the CPU until the function returns. During this period of time, the task in which ADC oneshot mode driver resides will not be blocked. Therefore the clock frequency is stable when reading.


.. _adc-oneshot-iram-safe:

IRAM Safe
^^^^^^^^^

By default, all the ADC oneshot mode driver APIs are not supposed to be run when the Cache is disabled. Cache may be disabled due to many reasons, such as Flash writing/erasing, OTA, etc. If these APIs execute when the Cache is disabled, you will probably see errors like ``Illegal Instruction`` or ``Load/Store Prohibited``.


.. _adc-oneshot-thread-safety:

Thread Safety
^^^^^^^^^^^^^

- :cpp:func:`adc_oneshot_new_unit`
- :cpp:func:`adc_oneshot_config_channel`
- :cpp:func:`adc_oneshot_read`

Above functions are guaranteed to be thread-safe. Therefore, you can call them from different RTOS tasks without protection by extra locks.

- :cpp:func:`adc_oneshot_del_unit` is not thread-safe. Besides, concurrently calling this function may result in failures of the above thread-safe APIs.


.. _adc-oneshot-kconfig-options:

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_ADC_ONESHOT_CTRL_FUNC_IN_IRAM` controls where to place the ADC fast read function (IRAM or Flash), see `IRAM Safe <#iram-safe>`__ for more details.


Application Examples
--------------------

* ADC oneshot mode example: :example:`peripherals/adc/oneshot_read`.


API Reference
-------------

.. include-build-file:: inc/adc_types.inc
.. include-build-file:: inc/adc_oneshot.inc
