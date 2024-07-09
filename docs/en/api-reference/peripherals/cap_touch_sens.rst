Capacitive Touch Sensor
=========================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_TOUCH_SENSOR_VERSION:default="NOT_UPDATED", esp32p4="v3"}

Introduction
---------------

A touch sensor system is built on a substrate which carries electrodes and relevant connections under a protective flat surface. When the surface is touched, the capacitance variation is used to evaluate if the touch was valid.

The sensing pads can be arranged in different combinations (e.g., matrix, slider), so that a larger area or more points can be detected. The touch pad sensing process is under the control of a hardware-implemented finite-state machine (FSM) which is initiated by software or a dedicated hardware timer.

For design, operation, and control registers of a touch sensor, see **{IDF_TARGET_NAME} Technical Reference Manual** > **On-Chip Sensors and Analog Signal Processing** [`PDF <{IDF_TARGET_TRM_EN_URL}#sensor>`__].

In-depth design details of touch sensors and firmware development guidelines for the {IDF_TARGET_NAME} are available in `Touch Sensor Application Note <https://github.com/espressif/esp-iot-solution/blob/release/v1.0/documents/touch_pad_solution/touch_sensor_design_en.md>`_.

Overview of Capacitive Touch Sensor Versions
-----------------------------------------------

+------------------+--------------+------------------------------------------------------------------------+
| Hardware Version |     Chip     |                        Main Features                                   |
+==================+==============+========================================================================+
|        V1        |  ESP32       | Version 1, the channel value decreases when it is touched              |
+------------------+--------------+------------------------------------------------------------------------+
|        V2        |  ESP32-S2    | Version 2, the channel value increases when it is touched              |
|                  |              | Supports waterproof, proximity sensing and sleep wake-up               |
|                  +--------------+------------------------------------------------------------------------+
|                  |  ESP32-S3    | Version 2, support proximity measurement done interrupt                |
+------------------+--------------+------------------------------------------------------------------------+
|        V3        |  ESP32-P4    | Version 3, support frequency hopping                                   |
+------------------+--------------+------------------------------------------------------------------------+

Overview of Touch Sensor Channels
------------------------------------

.. only:: esp32p4

    .. list-table::
      :header-rows: 1
      :widths: 20  20

      * - Channel
        - GPIO

      * - CH0
        - IO2

      * - CH1
        - IO3

      * - CH2
        - IO4

      * - CH3
        - IO5

      * - CH4
        - IO6

      * - CH5
        - IO7

      * - CH6
        - IO8

      * - CH7
        - IO9

      * - CH8
        - IO10

      * - CH9
        - IO11

      * - CH10
        - IO12

      * - CH11
        - IO13

      * - CH12
        - IO14

      * - CH13
        - IO15

      * - CH14
        - Internal

Terminology in the Driver
----------------------------

- **Touch Sensor Controller**: The controller of the touch sensor, responsible for configuring and managing the touch sensor.
- **Touch Sensor Channel**: A specific touch sensor sampling channel. A touch sensor module has multiple touch channels, which are usually connected to the touch pad for measuring the capacitance change. In the driver, sampling of **one** channel is called one ``measurement`` and the scanning of **all** registered channels is called one ``scan``.

.. only:: SOC_TOUCH_SUPPORT_FREQ_HOP

  - **Touch Sensor Sampling Configuration**: Touch sensor sampling configuration refers to all the hardware configurations that related to the sampling. It can  determine how the touch channels sample by setting the number of charging times, charging frequency, measurement interval, etc. The {IDF_TARGET_NAME} supports multiple sets of sample configuration, which means it can support frequency hopping.

.. only:: not SOC_TOUCH_SUPPORT_FREQ_HOP

  - **Touch Sensor Sampling Configuration**: Touch sensor sampling configuration refers to all the hardware configurations that related to the sampling. It can  determine how the touch channels sample by setting the number of charging times, charging frequency, measurement interval, etc. The {IDF_TARGET_NAME} only support one set of sample configuration, so it doesn't support frequency hopping.

File Structure
-----------------

.. figure:: ../../../_static/diagrams/cap_touch_sens/touch_file_structure.svg
    :align: center
    :alt: File Structure of Touch Sensor Driver

    File Structure of Touch Sensor Driver

Finite-state Machine
---------------------

The following diagram shows the state machine of the touch sensor driver, which describes the driver state after calling a function, and the constraint of the state transition.

.. figure:: ../../../_static/diagrams/cap_touch_sens/touch_state_machine.svg
    :align: center
    :alt: Finite-state Machine of Touch Sensor Driver

    Finite-state Machine of Touch Sensor Driver

The diagram above is the finite-state machine of the touch sensor driver, which describes how the state transferred by invoking different APIs. ``<other_configurations>`` in the diagram stands for the other optional configurations, like reconfigurations to the touch sensor controller or channels, callback registration, filter, and so on.

.. note::

    :cpp:func:`touch_channel_read_data` can be called at any time after the channel is registered (i.e., since ``INIT`` state), but please take care of the validation of the data.

Functionality Introduction
------------------------------

Categorized by functionality, the APIs of Capacitive Touch Sensor mainly include:

.. list::

  - `Touch Sensor Controller Management <#touch-ctrl>`__
  - `Touch Sensor Channel Management <#touch-chan>`__
  - `Filter Configuration <#touch-filter>`__
  - `Callback <#touch-callback>`__
  - `Enable and Disable <#touch-enable>`__
  - `Continuous Scan <#touch-conti-scan>`__
  - `Oneshot Scan <#touch-oneshot-scan>`__
  - `Benchmark Configuration <#touch-benchmark>`__
  - `Read Measurement Data <#touch-read>`__
  :SOC_TOUCH_SUPPORT_WATERPROOF: - `Waterproof Configuration <#touch-waterproof>`__
  :SOC_TOUCH_SUPPORT_PROX_SENSING: - `Proximity Sensing Configuration <#touch-prox-sensing>`__
  :SOC_TOUCH_SUPPORT_SLEEP_WAKEUP: - `Sleep Wake-up Configuration <#touch-sleep-wakeup>`__

.. _touch-ctrl:

Touch Sensor Controller Management
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Touch Sensor is controlled by controller handle  :cpp:type:`touch_sensor_handle_t`, it can be initialized and allocated by :cpp:func:`touch_sensor_new_controller`.

.. code-block:: c

    // Some target has multiple sets of sample configuration can be set, here take one for example
    #define SAMPLE_NUM 1
    touch_sensor_handle_t sens_handle = NULL;
    // sample configuration
    touch_sensor_sample_config_t sample_cfg[SAMPLE_NUM] = {
        // Specify sample configuration or apply the default sample configuration via `TOUCH_SENSOR_Vn_DEFAULT_SAMPLE_CONFIG`
        // ...
    };
    // Use the default touch controller configuration
    touch_sensor_config_t touch_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(SAMPLE_NUM, sample_cfg);
    // Allocate a new touch sensor controller handle
    ESP_ERROR_CHECK(touch_sensor_new_controller(&touch_cfg, &sens_handle));

To delete the controller handle and free the software and hardware resources, please call :cpp:func:`touch_sensor_del_controller`. But note that you need to delete the other resources that based on the controller first, like the registered touch channels, otherwise it can't be deleted directly.

.. code-block:: c

    ESP_ERROR_CHECK(touch_sensor_del_controller(sens_handle));

You can also update the configurations via :cpp:func:`touch_sensor_reconfig_controller` before the controller is enabled.

.. code-block:: c

    touch_sensor_config_t touch_cfg = {
        // New controller configurations
        // ...
    };
    ESP_ERROR_CHECK(touch_sensor_reconfig_controller(sens_handle, &touch_cfg));

.. _touch-chan:

Touch Sensor Channel Management
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are multiple touch channels in the touch sensor module, the touch sensor channel is controlled by the channel handle :cpp:type:`touch_channel_handle_t`. It can be initialized and allocated by :cpp:func:`touch_sensor_new_channel`.

.. code-block:: c

    // ...
    touch_channel_config_t chan_cfg = {
        // Touch channel configurations
        // ...
    };
    touch_channel_handle_t chan_handle = NULL;
    int chan_id = 0;
    // Allocate a new touch sensor controller handle
    ESP_ERROR_CHECK(touch_sensor_new_channel(sens_handle, chan_id, &chan_cfg, &chan_handle));

To delete the touch channel handle and free the software and hardware resources, please call :cpp:func:`touch_sensor_del_channel`.

.. code-block:: c

    ESP_ERROR_CHECK(touch_sensor_del_channel(chan_handle));

You can also update the configurations via :cpp:func:`touch_sensor_reconfig_channel` before the controller is enabled.

.. code-block:: c

    touch_channel_config_t chan_cfg = {
        // New touch channel configurations
        // ...
    };
    ESP_ERROR_CHECK(touch_sensor_reconfig_channel(chan_handle, &chan_cfg));

.. _touch-filter:

Filter Configuration
^^^^^^^^^^^^^^^^^^^^^^

The filter can help to increase the stability in different use cases. The filter can be registered by calling :cpp:func:`touch_sensor_config_filter` and specify the configurations :cpp:type:`touch_sensor_filter_config_t`. These configurations mainly determine how to filter and update the benchmark and read data. Please note that all touch channels will share this filter.

To deregister the filter, you can call :cpp:func:`touch_sensor_config_filter` again, and set the second parameter (i.e. :cpp:type:`touch_sensor_filter_config_t` pointer) to ``NULL``.

.. code-block:: c

    // ...
    touch_sensor_filter_config_t filter_config = {
        // Filter configurations
        // ...
    };
    // Register the filter
    ESP_ERROR_CHECK(touch_sensor_config_filter(sens_handle, &filter_config));
    // ...
    // Deregister the filter
    ESP_ERROR_CHECK(touch_sensor_config_filter(sens_handle, NULL));

.. _touch-callback:

Callback
^^^^^^^^^^^^^

Calling :cpp:func:`touch_sensor_register_callbacks` to register the touch sensor event callbacks. Once the touch sensor events (like ``on_active``, ``on_inactive``) trigger, the corresponding callbacks will be invoked, so that to deal with the event in the upper application.

For the general example, when the measured data of the current touch channel exceed the ``benchmark`` + ``active_threshold``, this channel is activated, and the driver will call ``on_active`` callback to inform the application layer. Similar, when the active channel measured a lower data than ``benchmark`` + ``active_threshold``, then this channel will be inactivated, and ``on_inactive`` will be called to inform this channel is released.

.. note::

    To ensure the stability of the triggering and releasing, ``active_hysteresis`` and ``debounce_cnt`` can be configured to avoid the frequent triggering that caused by jitter and noise.

Please refer to :cpp:type:`touch_event_callbacks_t` for the details about the supported callbacks.

.. code-block:: c

    touch_event_callbacks_t callbacks = {
        .on_active = example_touch_on_active_cb,
        // Other callbacks
        // ...
    };
    // Register callbacks
    ESP_ERROR_CHECK(touch_sensor_register_callbacks(sens_handle, &callbacks, NULL));

    // To deregister callbacks, set the corresponding callback to NULL
    callbacks.on_active = NULL;
    // Other callbacks to deregister
    // ...
    ESP_ERROR_CHECK(touch_sensor_register_callbacks(sens_handle, &callbacks, NULL));

.. _touch-enable:

Enable and Disable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After finished the configuration of the touch controller and touch channels, :cpp:func:`touch_sensor_enable` can be called to enable the touch sensor controller. It will enter ``READY`` status and power on the registered channels, then you can start scanning and sampling the touch data. Note that you can only do scanning and reading operation once the controller is enabled. If you want to update the controller or channel configurations, you need to call :cpp:func:`touch_sensor_disable` first.

.. code-block:: c

    // Enable touch sensor
    ESP_ERROR_CHECK(touch_sensor_enable(sens_handle));
    // ...
    // Disable touch sensor
    ESP_ERROR_CHECK(touch_sensor_disable(sens_handle));

.. _touch-conti-scan:

Continuous Scan
^^^^^^^^^^^^^^^^^^

With the touch controller enabled, :cpp:func:`touch_sensor_start_continuous_scanning` can be called to start the continuous scanning to all the registered touch channels. The read data of these touch channels will be updated automatically in each scan. Calling :cpp:func:`touch_sensor_stop_continuous_scanning` can stop the continuous scan.

.. code-block:: c

    // Start continuous scan
    ESP_ERROR_CHECK(touch_sensor_start_continuous_scanning(sens_handle));
    // ...
    // Stop continuous scan
    ESP_ERROR_CHECK(touch_sensor_stop_continuous_scanning(sens_handle));

.. _touch-oneshot-scan:

Oneshot Scan
^^^^^^^^^^^^^^^

With the touch controller enabled, :cpp:func:`touch_sensor_trigger_oneshot_scanning` can be called to trigger an one-time scan to all the registered touch channels. Note that oneshot scan is a blocking function, it will keep blocking and only return when the scan is finished. Moreover, you can't trigger an oneshot scan after the continuous scan has started.

.. code-block:: c

    // Trigger an oneshot scan with timeout 1000 ms
    ESP_ERROR_CHECK(touch_sensor_trigger_oneshot_scanning(sens_handle, 1000));

.. _touch-benchmark:

Benchmark Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Normally, you don't have to set the benchmark manually, but you can force reset the benchmark to the current smooth value by calling  :cpp:func:`touch_channel_config_benchmark` when necessary.

.. code-block:: c

    touch_chan_benchmark_config_t benchmark_cfg = {
        // Benchmark operations
        // ...
    };
    ESP_ERROR_CHECK(touch_channel_config_benchmark(chan_handle, &benchmark_cfg));

.. _touch-read:

Read Measurement Data
^^^^^^^^^^^^^^^^^^^^^^^^

Call :cpp:func:`touch_channel_read_data` to read the data with different types. Like, benchmark, smooth data, etc. You can refer to :cpp:type:`touch_chan_data_type_t` for the supported data types.

.. only:: SOC_TOUCH_SUPPORT_FREQ_HOP

    The {IDF_TARGET_NAME} supports frequency hopping by configuring multiple set of sample configurations, :cpp:func:`touch_channel_read_data` can read out the data of each sample configuration in a single call, you can determine the sample configuration number by  :cpp:member:`touch_sensor_config_t::sample_cfg_num`, and pass an array (which length is not smaller than the configuration number) to the third parameter ``*data``, so that all the measured data of this channel will be stored in the array.

.. code-block:: c

    #define SAMPLE_NUM  1  // Take one sample configuration set for example
    uint32_t smooth_data[SAMPLE_NUM] = {};
    // Read the smooth data
    ESP_ERROR_CHECK(touch_channel_read_data(chan_handle, TOUCH_CHAN_DATA_TYPE_SMOOTH, smooth_data));

.. _touch-waterproof:

.. only:: SOC_TOUCH_SUPPORT_WATERPROOF

    Waterproof Configuration
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The {IDF_TARGET_NAME} supports waterproof. Waterproof can be registered by calling :cpp:func:`touch_sensor_config_waterproof` and specify the configurations :cpp:type:`touch_waterproof_config_t`. There are two parts of the waterproof function:

    - Immersion (in-water) proof: :cpp:member:`touch_waterproof_config_t::guard_chan` can be specified for detecting immersion. It is usually designed as a ring on the PCB, which surrounds all the other touch pads. When this guard ring channel is triggered, that means the touch panel is immersed by water, all the touch channels will stop measuring to avoid falsely triggering.
    - Moisture (water-drop) proof: :cpp:member:`touch_waterproof_config_t::shield_chan` can be specified for detecting moisture. It usually uses the grid layout on the PCB, which covers the whole touch panel. The shield channel will charge and discharge synchronously with the current touch channel, when there is a water droplet covers both shield channel and normal touch channel, :cpp:member:`touch_waterproof_config_t::shield_drv` can strengthen the electrical coupling caused by the water droplets, and then reconfigure the active threshold based on the disturbance to eliminate the influence that introduced by the water droplet.

    To deregister the waterproof function, you can call :cpp:func:`touch_sensor_config_waterproof` again, and set the second parameter (i.e. :cpp:type:`touch_waterproof_config_t` pointer) to ``NULL``.

    .. code-block:: c

        touch_waterproof_config_t waterproof_cfg = {
            // Waterproof configurations
            // ...
        };
        // Register waterproof function
        ESP_ERROR_CHECK(touch_sensor_config_waterproof(sens_handle, &waterproof_cfg));
        // ...
        // Deregister waterproof function
        ESP_ERROR_CHECK(touch_sensor_config_waterproof(sens_handle, NULL));

.. _touch-prox-sensing:

.. only:: SOC_TOUCH_SUPPORT_PROX_SENSING

    Proximity Sensing Configuration
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The {IDF_TARGET_NAME} supports proximity sensing. Proximity sensing can be registered by calling :cpp:func:`touch_sensor_config_proximity_sensing` and specify the configurations :cpp:type:`touch_proximity_config_t`.

    Since the capacitance change caused by proximity sensing is far less than that caused by physical touch, large area of copper foil is often used on PCB to increase the sensing area. In addition, multiple rounds of scans are needed and the result of each scan will be accumulated in the driver to improve the measurement sensitivity. The scan times (rounds) can be determined by :cpp:member:`touch_proximity_config_t::scan_times` and the charging times of the proximity channel in one scan can be determined by :cpp:member:`touch_proximity_config_t::charge_times`. Generally, the larger the scan times and charging times is, the higher the sensitivity will be, however, the read data will be unstable if the sensitivity is too high. Proper parameters should be determined regarding the application.

    The accumulated proximity data can be read by :cpp:func:`touch_channel_read_data` with the data type :cpp:enumerator:`TOUCH_CHAN_DATA_TYPE_PROXIMITY`

    To deregister the proximity sensing, you can call :cpp:func:`touch_sensor_config_proximity_sensing` again, and set the second parameter (i.e. :cpp:type:`touch_proximity_config_t` pointer) to ``NULL``.

    .. code-block:: c

        touch_proximity_config_t prox_cfg = {
            // Proximity sensing configuration
            // ...
        };
        // Register the proximity sensing
        ESP_ERROR_CHECK(touch_sensor_config_proximity_sensing(sens_handle, &prox_cfg));
        // ...
        // Deregister the proximity sensing
        ESP_ERROR_CHECK(touch_sensor_config_proximity_sensing(sens_handle, NULL));

.. _touch-sleep-wakeup:

.. only:: SOC_TOUCH_SUPPORT_SLEEP_WAKEUP

    Sleep Wake-up Configuration
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The {IDF_TARGET_NAME} supports waking-up the chip from light sleep or deep sleep with the touch sensor as a wake-up source. The wake-up functionality can be registered by calling  :cpp:func:`touch_sensor_config_sleep_wakeup` and specifying the configurations :cpp:type:`touch_sleep_config_t`.

    After registering the touch sensor sleep wake-up, the chip will continue to sample the touch channels after sleep, which will increase the power consumption during the sleep. To reduce the sleep power consumption, you can reduce the number of charging and discharging times, increase the sampling interval, etc.

    Moreover, please note that the operations like sampling, wake-up are all done by hardware when the main core is sleeping. Since this driver runs on the main core, it cannot provide functions such as reading or configuring during the sleep.

    .. only:: SOC_RISCV_COPROC_SUPPORTED

        If you want to read or configure the touch sensor during the sleep, you can turn to the driver ``components/ulp/ulp_riscv/ulp_core/include/ulp_riscv_touch_ulp_core.h`` which based on the :doc:`Ultra Low Power (ULP) Coprocessor <../system/ulp>`.

    - Light sleep wake-up: you need to set :cpp:member:`slp_wakeup_lvl` to :cpp:enumerator:`TOUCH_LIGHT_SLEEP_WAKEUP` to enable the light sleep wake-up by touch sensor. Note that any registered touch channel can wake-up the chip from light sleep.
    - Deep sleep wake-up: beside setting :cpp:member:`slp_wakeup_lvl` to :cpp:enumerator:`TOUCH_DEEP_SLEEP_WAKEUP`, you need to specify :cpp:member:`deep_slp_chan` additionally. Only the specified channel can wake-up the chip from the deep sleep, in order to reduce the power consumption. And also, the driver supports to store another set of configurations for the deep sleep via :cpp:member:`deep_slp_sens_cfg`, this set of configurations only takes effect during the deep sleep, you can customize the configurations to save more power. The configurations will be reset to the previous set after waking-up from the deep sleep. Please be aware that, not only deep sleep wake-up, but also light sleep wake-up will be enabled when the :cpp:member:`slp_wakeup_lvl` is :cpp:enumerator:`TOUCH_DEEP_SLEEP_WAKEUP`.

    To deregister the sleep wake-up function, you can call :cpp:func:`touch_sensor_config_sleep_wakeup` again, and set the second parameter (i.e. :cpp:type:`touch_sleep_config_t` pointer) to ``NULL``.

    .. code-block:: c

        touch_sleep_config_t light_slp_cfg = {
            .slp_wakeup_lvl = TOUCH_LIGHT_SLEEP_WAKEUP,
        };
        // Register the light sleep wake-up
        ESP_ERROR_CHECK(touch_sensor_config_sleep_wakeup(sens_handle, &light_slp_cfg));
        // ...
        // Deregister the light sleep wake-up
        ESP_ERROR_CHECK(touch_sensor_config_sleep_wakeup(sens_handle, NULL));
        touch_sleep_config_t deep_slp_cfg = {
            .slp_wakeup_lvl = TOUCH_DEEP_SLEEP_WAKEUP,
            .deep_slp_chan = dslp_chan_handle,
            // Other deep sleep configurations
            // ...
        };
        // Register the deep sleep wake-up
        ESP_ERROR_CHECK(touch_sensor_config_sleep_wakeup(sens_handle, &deep_slp_cfg));

Application Examples
------------------------

    - :example:`peripherals/touch_sensor/touch_sensor_v3` demonstrates how to register touch channels and read the data, including hardware requirements and project configuration instructions.

API Reference
-------------

.. only:: esp32p4

    .. include-build-file:: inc/touch_sens.inc
    .. include-build-file:: inc/touch_sens_types.inc
    .. include-build-file:: inc/touch_version_types.inc
