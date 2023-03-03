Temperature Sensor
==================

Introduction
------------

The {IDF_TARGET_NAME} has a built-in sensor used to measure the chip's internal temperature. The temperature sensor module contains an 8-bit Sigma-Delta ADC and a DAC to compensate for the temperature offset.

Due to restrictions of hardware, the sensor has predefined measurement ranges with specific measurement errors. See the table below for details.

+------------------------+------------------------+
|  predefined range (°C) |       error (°C)       |
+========================+========================+
|        50 ~ 125        |           < 3          |
+------------------------+------------------------+
|        20 ~ 100        |           < 2          |
+------------------------+------------------------+
|       -10 ~ 80         |           < 1          |
+------------------------+------------------------+
|       -30 ~ 50         |           < 2          |
+------------------------+------------------------+
|       -40 ~ 20         |           < 3          |
+------------------------+------------------------+

.. note::

    The temperature sensor is designed primarily to measure the temperature changes inside the chip. The temperature value depends on factors like microcontroller clock frequency or I/O load. Generally, the chip's internal temperature might be higher than the ambient temperature.

Functional Overview
-------------------

.. list::

    -  `Resource Allocation <#resource-allocation>`__ - covers which parameters should be set up to get a temperature sensor handle and how to recycle the resources when temperature sensor finishes working.
    -  `Enable and Disable Temperature Sensor <#enable-and-disable-temperature-sensor>`__ - covers how to enable and disable the temperature sensor.
    -  `Get Temperature Value <#get-temperature-value>`__ - covers how to get the real-time temperature value.
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: - `Temperature Threshold Interrupt <#install-temperature-threshold-callback>`__ - describes how to register a temperature threshold callback.
    -  `Power Management <#power-management>`__ - covers how temperature sensor is affected when changing power mode (i.e. light sleep).
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: - `IRAM Safe <#iram-safe>`__ - describes tips on how to make the temperature sensor interrupt work better along with a disabled cache.
    -  `Thread Safety <#thread-safety>`__ - covers how to make the driver to be thread safe.

Resource Allocation
^^^^^^^^^^^^^^^^^^^

The {IDF_TARGET_NAME} has just one built-in temperature sensor hardware. The temperature sensor instance is represented by :cpp:type:`temperature_sensor_handle_t`, which is also the bond of the context. It would always be the parameter of the temperature APIs with the information of hardware and configurations, so user can just create a pointer of type :cpp:type:`temperature_sensor_handle_t` and passing to APIs as needed.

In order to install a built-in temperature sensor instance, the first thing is to evaluate the temperature range in your detection environment (For example: if the testing environment is in a room, the range you evaluate might be 10 °C ~ 30 °C; if the testing in a lamp bulb, the range you evaluate might be 60 °C ~ 110 °C). Based on that, the following configuration structure should be defined in advance:
:cpp:type:`temperature_sensor_config_t`:

-  :cpp:member:`range_min`. The minimum value of testing range you have evaluated.
-  :cpp:member:`range_max`. The maximum value of testing range you have evaluated.

After the ranges are set, the structure could be passed to :cpp:func:`temperature_sensor_install`, which will instantiate the temperature sensor instance and return a handle.

As mentioned above, different measure ranges have different measurement errors. The user doesn't need to care about the measurement error because we have an internal mechanism to choose the minimum error according to the given range.

If the temperature sensor is no longer needed, you need to call :cpp:func:`temperature_sensor_uninstall` to free the temperature sensor resource.

Creating a Temperature Sensor Handle
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Step1: Evaluate the testing range. In this example, the range is 20 °C ~ 50 °C.
* Step2: Configure the range and obtain a handle

.. code:: c

    temperature_sensor_handle_t temp_handle = NULL;
    temperature_sensor_config_t temp_sensor = {
        .range_min = 20,
        .range_max = 50,
    };
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor, &temp_handle));

Enable and Disable Temperature Sensor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Enable the temperature sensor by calling :cpp:func:`temperature_sensor_enable`. The internal temperature sensor circuit will start to work. The driver state will transit from init to enable.
2. To Disable the temperature sensor, please call :cpp:func:`temperature_sensor_disable`.

Get Temperature Value
^^^^^^^^^^^^^^^^^^^^^

After the temperature sensor is enabled by :cpp:func:`temperature_sensor_enable`, user can get the current temperature by calling :cpp:func:`temperature_sensor_get_celsius`.

.. code:: c

    // Enable temperature sensor
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));
    // Get converted sensor data
    float tsens_out;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
    printf("Temperature in %f °C\n", tsens_out);
    // Disable the temperature sensor if it's not needed and save the power
    ESP_ERROR_CHECK(temperature_sensor_disable(temp_handle));

.. only:: SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    Install Temperature Threshold Callback
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    {IDF_TARGET_NAME} supports automatically triggering to monitor the temperature value continuously. When temperature value reaches a given threshold, an interrupt will happen. Thus users can install their own interrupt callback functions to do what they want. (e.g. alarm, restart, etc.). Following information indicates how to prepare a threshold callback.

    -  :cpp:member:`temperature_sensor_event_callbacks_t::on_threshold`. As this function is called within the ISR context, you must ensure that the function does not attempt to block (e.g., by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function, etc.). The function prototype is declared in :cpp:type:`temperature_thres_cb_t`.

    You can save your own context to :cpp:func:`temperature_sensor_register_callbacks` as well, via the parameter ``user_arg``. The user data will be directly passed to the callback function.

    .. code:: c

        IRAM_ATTR static bool temp_sensor_monitor_cbs(temperature_sensor_handle_t tsens, const temperature_sensor_threshold_event_data_t *edata, void *user_data)
        {
            ESP_DRAM_LOGI("tsens", "Temperature value is higher or lower than threshold, value is %d\n...\n\n", edata->celsius_value);
            return false;
        }

        // Callback configurations
        temperature_sensor_abs_threshold_config_t threshold_cfg = {
            .high_threshold = 50,
            .low_threshold = -10,
        };
        // Set absolute value monitor threshold.
        temperature_sensor_set_absolute_threshold(temp_sensor, &threshold_cfg);
        // Register interrupt callback
        temperature_sensor_event_callbacks_t cbs = {
            .on_threshold = temp_sensor_monitor_cbs,
        };
        // Install temperature callback.
        temperature_sensor_register_callbacks(temp_sensor, &cbs, NULL);

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled (i.e. ``CONFIG_PM_ENABLE`` is on), temperature sensor will still keep working because it uses XTAL clock (on ESP32-C3) or RTC clock (on ESP32-S2/S3).

.. only:: SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    IRAM Safe
    ^^^^^^^^^

    By default, the temperature sensor interrupt will be deferred when the Cache is disabled for reasons like writing/erasing Flash. Thus the event callback functions will not get executed in time, which is not expected in a real-time application.

    There's a Kconfig option :ref:`CONFIG_TEMP_SENSOR_ISR_IRAM_SAFE` that will:

    1. Enable the interrupt being serviced even when cache is disabled.
    2. Place all functions that used by the ISR into IRAM.

    This will allow the interrupt to run while the cache is disabled but will come at the cost of increased IRAM consumption.

Thread Safety
^^^^^^^^^^^^^

In temperature sensor we don't add any protection to keep the thread safe. Because from the common usage, temperature sensor should only be called in one task. If you must use this driver in different tasks, please add extra locks to protect it.

Unexpected Behaviors
--------------------

1. The value user gets from the chip is usually different from the ambient temperature. It is because the temperature sensor is built inside the chip. To some extent, it measures the temperature of the chip.

2. When installing the temperature sensor, the driver gives a 'the boundary you gave cannot meet the range of internal temperature sensor' error feedback. It is because the built-in temperature sensor has testing limit. The error due to setting :cpp:type:`temperature_sensor_config_t`:

    (1) Totally out of range, like 200 °C ~ 300 °C.
    (2) Cross the boundary of each predefined measurement. like 40 °C ~ 110 °C.

Application Example
-------------------

.. list::

    * Temperature sensor reading example: :example:`peripherals/temperature_sensor/temp_sensor`.
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: * Temperature sensor value monitor example: :example:`peripherals/temperature_sensor/temp_sensor`.

API Reference
----------------------------------

.. include-build-file:: inc/temperature_sensor.inc
