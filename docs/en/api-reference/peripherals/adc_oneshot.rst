Analog to Digital Converter (ADC) Oneshot Mode Driver
=====================================================


Introduction
------------

The Analog to Digital Converter is an on-chip sensor which is able to measure analog signals from dedicated analog IO pads.

The ADC on {IDF_TARGET_NAME} can be used in scenario(s) like:

- Generate one-shot ADC conversion result

.. only:: SOC_ADC_DMA_SUPPORTED

    - Generate continuous ADC conversion results

This guide will introduce ADC oneshot mode conversion.


Functional Overview
-------------------

The following sections of this document cover the typical steps to install and operate an ADC:

-  `Resource Allocation <#resource-allocation>`__ - covers which parameters should be set up to get an ADC handle and how to recycle the resources when ADC finishes working.
-  `Unit Configuration <#unit-configuration>`__ - covers the parameters that should be set up to configure the ADC unit, so as to get ADC conversion raw result.
-  `Read Conversion Result <#read-conversion-result>`__ - covers how to get ADC conversion raw result.
-  `Hardware Limitations <#hardware-limitations>`__ - describes the ADC related hardware limitations.
-  `Power Management <#power-management>`__ - covers power management related.
-  `IRAM Safe <#iram-safe>`__ - describes tips on how to read ADC conversion raw result when cache is disabled.
-  `Thread Safety <#thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.
-  `Kconfig Options <#kconfig-options>`__ - lists the supported Kconfig options that can be used to make a different effect on driver behavior.


Resource Allocation
^^^^^^^^^^^^^^^^^^^

The ADC oneshot mode driver is implemented based on {IDF_TARGET_NAME} SAR ADC module. Different ESP chips might have different number of independent ADCs. From oneshot mode driver's point of view, an ADC instance is represented by :cpp:type:`adc_oneshot_unit_handle_t`.

To install an ADC instance, set up the required initial configuration structure :cpp:type:`adc_oneshot_unit_init_cfg_t`:

-  :cpp:member:`adc_oneshot_unit_init_cfg_t::unit_id` selects the ADC. Please refer to the `datasheet <{IDF_TARGET_TRM_EN_URL}>`__ to know dedicated analog IOs for this ADC.
-  :cpp:member:`adc_oneshot_unit_init_cfg_t::ulp_mode` sets if the ADC will be working under super low power mode.

.. todo::

   Add ULP ADC related docs here.

After setting up the initial configurations for the ADC, call :cpp:func:`adc_oneshot_new_unit` with the prepared :cpp:type:`adc_oneshot_unit_init_cfg_t`. This function will return an ADC unit handle, if the allocation is successful.

This function may fail due to various errors such as invalid argumemts, insufficient memory, etc. Specifically, when the to-be-allocated ADC instance is registered already, this function will return :c:macro:`ESP_ERR_NOT_FOUND` error. Number of available ADC(s) is recorded by :c:macro:`SOC_ADC_PERIPH_NUM`.

If a previously created ADC instance is no loger required, you should recycle the ADC instance by calling :cpp:func:`adc_oneshot_del_unit`, related hardware and software resources will be recycled as well.


Create an ADC Unit Handle under Normal Oneshot Mode
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


Unit Configuration
^^^^^^^^^^^^^^^^^^

After an ADC instance is created, set up the :cpp:type:`adc_oneshot_chan_cfg_t` to configure ADC IO to measure analog signal:

-  :cpp:member:`adc_oneshot_chan_cfg_t::atten`, ADC attenuation. Refer to the On-Chip Sensor chapter in `TRM <{IDF_TARGET_TRM_EN_URL}>`__.
-  :cpp:member:`adc_oneshot_chan_cfg_t::channel`, the IO corresponding ADC channel number. See below note.
-  :cpp:member:`adc_oneshot_chan_cfg_t::bitwidth`, the bitwidth of the raw conversion result.

.. note::

    For the IO corresponding ADC channel number. Check `datasheet <{IDF_TARGET_TRM_EN_URL}>`__ to know the ADC IOs.
    On the other hand, :cpp:func:`adc_continuous_io_to_channel` and :cpp:func:`adc_continuous_channel_to_io` can be used to know the ADC channels and ADC IOs.

To make these settings take effect, call :cpp:func:`adc_oneshot_config_channel` with above configuration structure. Especially, this :cpp:func:`adc_oneshot_config_channel` can be called multiple times to configure different ADC channels. Drvier will save these per channel configurations internally.


Configure Two ADC Channels
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    adc_oneshot_chan_cfg_t config = {
        .channel = EXAMPLE_ADC1_CHAN0,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, &config));

    config.channel = EXAMPLE_ADC1_CHAN1;
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, &config));


Read Conversion Result
^^^^^^^^^^^^^^^^^^^^^^

After above configurations, the ADC is ready to measure the analog siganl(s) from the configured ADC channel(s). Call :cpp:func:`adc_oneshot_read` to get the conversion raw result of an ADC channel.

-  :cpp:func:`adc_oneshot_read` is safer. ADC(s) are shared by some other drivers / peripherals, see `Hardware Limitations <#hardware-limitations>`__. This function takes some mutexes, to avoid concurrent hardware usage. Therefore, this function should not be used in an ISR context. This function may fail when the ADC is in use by other drivers / peripherals, and return :c:macro:`ESP_ERR_TIMEOUT`. Under this condition, the ADC raw result is invalid.

These two functions will both fail due to invalid arguments.

The ADC conversion results read from these two functions are raw data. To calculate the voltage based on the ADC raw results, this formula can be used:

.. parsed-literal::

    Vout = Dout * Vmax / Dmax       (1)

where:

======  =============================================================
Vout    Digital output result, standing for the voltage.
Dout    ADC raw digital reading result.
Vmax    Maximum measurable input analog voltage, this is related to the ADC attenuation, please refer to the On-Chip Sensor chapter in `TRM <{IDF_TARGET_TRM_EN_URL}>`__.
Dmax    Maximum of the output ADC raw digital reading result, which is 2^bitwidth, where bitwidth is the :cpp:member::`adc_oneshot_chan_cfg_t:bitwidth` configured before.
======  =============================================================

To do further calbration to convert the ADC raw result to voltage in mV, please refer to calibration doc :doc:`adc_calibration`.


Read Raw Result
~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0][0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0][0]);

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN1, &adc_raw[0][1]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, adc_raw[0][1]);


Hardware Limitations
^^^^^^^^^^^^^^^^^^^^

- Random Number Generator uses ADC as a input source. When ADC :cpp:func:`adc_oneshot_read` works, the random number generated from RNG will be less random.

.. only:: SOC_ADC_DMA_SUPPORTED

    - A specific ADC unit can only work under one operating mode at any one time, either continuous mode or oneshot mode. :cpp:func:`adc_oneshot_read` has provided the protection.

.. only:: esp32 or esp32s2 or esp32s3

    - ADC2 is also used by the Wi-Fi. :cpp:func:`adc_oneshot_read` has provided the protection between Wi-Fi driver and ADC oneshot mode driver.

.. only:: esp32c3

    - ADC2 oneshot mode is no longer supported, due to hardware limitation. The results are not stable. This issue can be found in `ESP32C3 Errata <https://www.espressif.com/sites/default/files/documentation/esp32-c3_errata_en.pdf>`_. For compatibility, you can enable :ref:`CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3` to force use ADC2.

.. only:: esp32

    - ESP32 DevKitC: GPIO 0 cannot be used due to external auto program circuits.

    - ESP-WROVER-KIT: GPIO 0, 2, 4 and 15 cannot be used due to external connections for different purposes.


Power Management
^^^^^^^^^^^^^^^^

When power management is enabled (i.e. :ref:`CONFIG_PM_ENABLE` is on), the system clock frequency may be adjusted when the system is in an idle state. However, the ADC oneshot mode driver works in a polling routine, the :cpp:func:`adc_oneshot_read` will poll the CPU until the function returns. During this period of time, the task in which ADC oneshot mode driver resides won't be blocked. Therefore the clock frequency is stable when reading.


IRAM Safe
^^^^^^^^^

By default, all the ADC oneshot mode driver APIs are not supposed to be run when the Cache is disabled (Cache may be disabled due to many reasons, such as Flash writing/erasing, OTA, etc.). If these APIs executes when the Cache is disabled, you will probably see errors like Illegal Instruction or Load/Store Prohibited.


Thread Safety
^^^^^^^^^^^^^

-  :cpp:func:`adc_oneshot_new_unit`
-  :cpp:func:`adc_oneshot_config_channel`
-  :cpp:func:`adc_oneshot_read`

Above functions are guaranteed to be thread safe. Therefore, you can call them from different RTOS tasks without protection by extra locks.

-  :cpp:func:`adc_oneshot_del_unit` is not thread safe. Besides, concurrently calling this function may result in thread-safe APIs fail.


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
