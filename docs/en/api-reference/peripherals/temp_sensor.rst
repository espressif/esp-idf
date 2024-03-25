Temperature Sensor
==================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The {IDF_TARGET_NAME} has a built-in sensor used to measure the chip's internal temperature. The temperature sensor module contains an 8-bit Sigma-Delta analog-to-digital converter (ADC) and a digital-to-analog converter (DAC) to compensate for the temperature measurement.

Due to restrictions of hardware, the sensor has predefined measurement ranges with specific measurement errors. See the table below for details.

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - Predefined Range (°C)
      - Error (°C)
    * - 50 ~ 125
      - < 3
    * - 20 ~ 100
      - < 2
    * - -10 ~ 80
      - < 1
    * - -30 ~ 50
      - < 2
    * - -40 ~ 20
      - < 3

.. note::

    The temperature sensor is designed primarily to measure the temperature changes inside the chip. The internal temperature of a chip is usually higher than the ambient temperature, and is affected by factors such as the microcontroller's clock frequency or I/O load, and the external thermal environment.

Functional Overview
-------------------

The description of the temperature sensor functionality is divided into the following sections:

.. list::

    - :ref:`temp-resource-allocation` - covers which parameters should be set up to get a temperature sensor handle and how to recycle the resources when the temperature sensor finishes working.
    - :ref:`temp-enable-and-disable-temperature-sensor` - covers how to enable and disable the temperature sensor.
    - :ref:`temp-get-temperature-value` - covers how to get the real-time temperature value.
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: - :ref:`temp-install-temperature-threshold-callback` - describes how to register a temperature threshold callback.
    - :ref:`temp-power-management` - covers how the temperature sensor is affected when changing power mode (e.g., Light-sleep mode).
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: - :ref:`temp-iram-safe` - describes tips on how to make the temperature sensor interrupt work better along with a disabled cache.
    - :ref:`temp-thread-safety` - covers how to make the driver to be thread-safe.
    :SOC_TEMPERATURE_SENSOR_SUPPORT_ETM: - :ref:`temperature-sensor-etm-event-and-task` - describes what the events and tasks can be connected to the ETM channel.

.. _temp-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

The {IDF_TARGET_NAME} has just one built-in temperature sensor hardware. The temperature sensor instance is represented by :cpp:type:`temperature_sensor_handle_t`, which is also the bond of the context. By using :cpp:type:`temperature_sensor_handle_t`, the temperature sensor properties can be accessed and modified in different function calls to control and manage the temperature sensor. The variable would always be the parameter of the temperature APIs with the information of hardware and configurations, so you can just create a pointer of type :cpp:type:`temperature_sensor_handle_t` and passing to APIs as needed.

In order to install a built-in temperature sensor instance, the first thing is to evaluate the temperature range in your detection environment. For example, if the testing environment is in a room, the range you evaluate might be 10 °C ~ 30 °C; if the testing in a lamp bulb, the range you evaluate might be 60 °C ~ 110 °C. Based on that, configuration structure :cpp:type:`temperature_sensor_config_t` should be defined in advance:

- :cpp:member:`range_min`: The minimum value of the testing range you have evaluated.
- :cpp:member:`range_max`: The maximum value of the testing range you have evaluated.

After the ranges are set, the structure could be passed to :cpp:func:`temperature_sensor_install`, which will instantiate the temperature sensor instance and return a handle.

As mentioned above, different measure ranges have different measurement errors. You do not need to care about the measurement error because we have an internal mechanism to choose the minimum error according to the given range.

If the temperature sensor is no longer needed, you need to call :cpp:func:`temperature_sensor_uninstall` to free the temperature sensor resource.

Creating a Temperature Sensor Handle
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Step 1: Evaluate the testing range. In this example, the range is 20 °C ~ 50 °C.
* Step 2: Configure the range and obtain a handle.

.. code:: c

    temperature_sensor_handle_t temp_handle = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(20, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_handle));

.. _temp-enable-and-disable-temperature-sensor:

Enable and Disable Temperature Sensor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Enable the temperature sensor by calling :cpp:func:`temperature_sensor_enable`. The internal temperature sensor circuit will start to work. The driver state will transit from init to enable.
2. To Disable the temperature sensor, please call :cpp:func:`temperature_sensor_disable`.

.. _temp-get-temperature-value:

Get Temperature Value
^^^^^^^^^^^^^^^^^^^^^

After the temperature sensor is enabled by :cpp:func:`temperature_sensor_enable`, you can get the current temperature by calling :cpp:func:`temperature_sensor_get_celsius`.

.. code:: c

    // Enable temperature sensor
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));
    // Get converted sensor data
    float tsens_out;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
    printf("Temperature in %f °C\n", tsens_out);
    // Disable the temperature sensor if it is not needed and save the power
    ESP_ERROR_CHECK(temperature_sensor_disable(temp_handle));


.. only:: SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    .. _temp-install-temperature-threshold-callback:

    Install Temperature Threshold Callback
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    {IDF_TARGET_NAME} supports automatically triggering to monitor the temperature value continuously. When the temperature value reaches a given threshold, an interrupt will happen. Thus you can install your own interrupt callback functions to do what they want, e.g., alarm, restart, etc. The following information indicates how to prepare a threshold callback.

    - :cpp:member:`temperature_sensor_event_callbacks_t::on_threshold`: As this function is called within the ISR context, you must ensure that the function does not attempt to block, e.g., by making sure that only FreeRTOS APIs with the ``ISR`` suffix are called from within the function, etc. The function prototype is declared in :cpp:type:`temperature_thres_cb_t`.

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

    .. _temp-power-management:

.. only:: not SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    .. _temp-power-management:

Power Management
^^^^^^^^^^^^^^^^

As the temperature sensor does not use the APB clock, it will keep working no matter if the power management is enabled with ``CONFIG_PM_ENABLE``.

.. only:: SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    .. _temp-iram-safe:

    IRAM Safe
    ^^^^^^^^^

    By default, the temperature sensor interrupt will be deferred when the cache is disabled for reasons like writing/erasing flash. Thus the event callback functions will not get executed in time, which is not expected in a real-time application.

    There is a Kconfig option :ref:`CONFIG_TEMP_SENSOR_ISR_IRAM_SAFE` that will:

    1. Enable the interrupt that is being serviced even when the cache is disabled.
    2. Place all functions that are used by the ISR into IRAM.

    This allows the interrupt to run while the cache is disabled but comes at the cost of increased IRAM consumption.

    .. _temp-thread-safety:

.. only:: not SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    .. _temp-thread-safety:

Thread Safety
^^^^^^^^^^^^^

In the temperature sensor driver, we do not add any protection to ensure the thread safety, because typically this driver is only supposed to be used in one task. If you have to use this driver in different tasks, please add extra locks to protect it.

.. only:: SOC_TEMPERATURE_SENSOR_SUPPORT_ETM

    .. _temperature-sensor-etm-event-and-task:

    ETM Event and Task
    ^^^^^^^^^^^^^^^^^^

    Temperature Sensor is able to generate events that can interact with the :doc:`ETM </api-reference/peripherals/etm>` module. The supported events are listed in the :cpp:type:`temperature_sensor_etm_event_type_t`. You can call :cpp:func:`temperature_sensor_new_etm_event` to get the corresponding ETM event handle. The supported tasks are listed in the :cpp:type:`temperature_sensor_etm_task_type_t`. You can call :cpp:func:`temperature_sensor_new_etm_task` to get the corresponding ETM task handle.

    .. note::

        - :cpp:enumerator:`TEMPERATURE_SENSOR_EVENT_OVER_LIMIT` for :cpp:member:`temperature_sensor_etm_event_type_t::event_type` depends on what kind of threshold you set first. If you set the absolute threshold by :cpp:func:`temperature_sensor_set_absolute_threshold`, then the :cpp:enumerator:`TEMPERATURE_SENSOR_EVENT_OVER_LIMIT` refers to absolute threshold. Likewise, if you set the delta threshold by :cpp:func:`temperature_sensor_set_delta_threshold`, then the :cpp:enumerator:`TEMPERATURE_SENSOR_EVENT_OVER_LIMIT` refers to delta threshold.

    For how to connect the event and task to an ETM channel, please refer to the :doc:`ETM </api-reference/peripherals/etm>` documentation.

Unexpected Behaviors
--------------------

1. The value you get from the chip is usually different from the ambient temperature. It is because the temperature sensor is built inside the chip. To some extent, it measures the temperature of the chip.

2. When installing the temperature sensor, the driver may print ``the boundary you gave cannot meet the range of internal temperature sensor``. It is because the built-in temperature sensor has a testing limit. The error comes from the incorrect configuration of :cpp:type:`temperature_sensor_config_t` as follow:

    (1) Totally out of range, like 200 °C ~ 300 °C.
    (2) Cross the boundary of each predefined measurement. like 40 °C ~ 110 °C.

Application Example
-------------------

.. list::

    * Temperature sensor reading example: :example:`peripherals/temperature_sensor/temp_sensor`.
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: * Temperature sensor value monitor example: :example:`peripherals/temperature_sensor/temp_sensor_monitor`.

API Reference
----------------------------------

.. include-build-file:: inc/temperature_sensor.inc
.. include-build-file:: inc/temperature_sensor_types.inc

.. only:: SOC_TEMPERATURE_SENSOR_SUPPORT_ETM

    .. include-build-file:: inc/temperature_sensor_etm.inc
