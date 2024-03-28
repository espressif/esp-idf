Touch Sensor
============

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_TOUCH_SENSOR_VERSION:default="v2", esp32="v1"}

Introduction
------------

A touch sensor system is built on a substrate which carries electrodes and relevant connections under a protective flat surface. When the surface is touched, the capacitance variation is used to evaluate if the touch was valid.

The sensing pads can be arranged in different combinations (e.g., matrix, slider), so that a larger area or more points can be detected. The touch pad sensing process is under the control of a hardware-implemented finite-state machine (FSM) which is initiated by software or a dedicated hardware timer.

For design, operation, and control registers of a touch sensor, see **{IDF_TARGET_NAME} Technical Reference Manual** > **On-Chip Sensors and Analog Signal Processing** [`PDF <{IDF_TARGET_TRM_EN_URL}#sensor>`__].

In-depth design details of touch sensors and firmware development guidelines for {IDF_TARGET_NAME} are available in `Touch Sensor Application Note <https://github.com/espressif/esp-iot-solution/blob/release/v1.0/documents/touch_pad_solution/touch_sensor_design_en.md>`_.

.. only:: esp32

    For more information about testing touch sensors in various configurations, please check the `Guide for ESP32-Sense-Kit <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32/esp32-sense-kit/user_guide.html>`_.

Functionality Overview
----------------------

Description of API is broken down into groups of functions to provide a quick overview of the following features:

- Initialization of touch pad driver
- Configuration of touch pad GPIO pins
- Taking measurements
- Adjusting parameters of measurements
- Filtering measurements
- Touch detection methods
- Setting up interrupts to report touch detection
- Waking up from Sleep mode on interrupt

For detailed description of a particular function, please go to Section :ref:`touch_pad-api-reference`. Practical implementation of this API is covered in Section :ref:`Application Examples <touch_pad-api-examples>`.

Initialization
^^^^^^^^^^^^^^

Before using a touch pad, you need to initialize the touch pad driver by calling the function :cpp:func:`touch_pad_init`. This function sets several ``.._DEFAULT`` driver parameters listed in :ref:`touch_pad-api-reference` under **Macros**. It also removes the information about which pads have been touched before, if any, and disables interrupts.

If the driver is not required anymore, deinitialize it by calling :cpp:func:`touch_pad_deinit`.

Configuration
^^^^^^^^^^^^^

.. only:: esp32

    Enabling the touch sensor functionality for a particular GPIO is done with :cpp:func:`touch_pad_config()`. The following 10 capacitive touch pads are supported for {IDF_TARGET_NAME}.

    .. list-table::
        :align: center
        :widths: 50 50
        :header-rows: 1

        * - Touch Pad
          - GPIO Pin
        * - T0
          - GPIO4
        * - T1
          - GPIO0
        * - T2
          - GPIO2
        * - T3
          - MTDO
        * - T4
          - MTCK
        * - T5
          - MTDI
        * - T6
          - MTMS
        * - T7
          - GPIO27
        * - T8
          - 32K_XN
        * - T9
          - 32K_XP

.. only:: esp32s2 or esp32s3

    Enabling the touch sensor functionality for a particular GPIO is done with :cpp:func:`touch_pad_config()`. The following 14 capacitive touch pads are supported for {IDF_TARGET_NAME}.

    .. list-table::
        :align: center
        :widths: 50 50
        :header-rows: 1

        * - Touch Pad
          - GPIO Pin
        * - T0
          - Internal channel, not connect to a GPIO
        * - T1
          - GPIO1
        * - T2
          - GPIO2
        * - T3
          - GPIO3
        * - T4
          - GPIO4
        * - T5
          - GPIO5
        * - T6
          - GPIO6
        * - T7
          - GPIO7
        * - T8
          - GPIO8
        * - T9
          - GPIO9
        * - T10
          - GPIO10
        * - T11
          - GPIO11
        * - T12
          - GPIO12
        * - T13
          - GPIO13
        * - T14
          - GPIO14

Use the function :cpp:func:`touch_pad_set_fsm_mode` to select if touch pad measurement (operated by FSM) should be started automatically by a hardware timer, or by software. If software mode is selected, use :cpp:func:`touch_pad_sw_start` to start the FSM.

Touch State Measurements
^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    The following two functions come in handy to read raw or filtered measurements from the sensor:

    * :cpp:func:`touch_pad_read_raw_data`
    * :cpp:func:`touch_pad_read_filtered`

    They can also be used, for example, to evaluate a particular touch pad design by checking the range of sensor readings when a pad is touched or released. This information can be then used to establish a touch threshold.

    .. note::

        Before using :cpp:func:`touch_pad_read_filtered`, you need to initialize and configure the filter by calling specific filter functions described in Section `Filtering of Measurements`_.

.. only:: esp32s2 or esp32s3

    The following function come in handy to read raw measurements from the sensor:

    * :cpp:func:`touch_pad_read_raw_data`

    It can also be used, for example, to evaluate a particular touch pad design by checking the range of sensor readings when a pad is touched or released. This information can be then used to establish a touch threshold.

For the demonstration of how to read the touch pad data, check the application example :example:`peripherals/touch_sensor/touch_sensor_{IDF_TARGET_TOUCH_SENSOR_VERSION}/touch_pad_read`.

Method of Measurements
^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    The touch sensor counts the number of charge/discharge cycles over a fixed period of time (specified by :cpp:func:`touch_pad_set_measurement_clock_cycles`). The count result is the raw data that read from :cpp:func:`touch_pad_read_raw_data`. After finishing one measurement, the touch sensor sleeps until the next measurement start, this interval between two measurements can be set by :cpp:func:`touch_pad_set_measurement_interval`.

    .. note::

        If the specified clock cycles for measurement is too small, the result may be inaccurate, but increasing clock cycles will increase the power consumption as well. Additionally, the response of the touch sensor will slow down if the total time of the interval and measurement is too long.

.. only:: esp32s2 or esp32s3

    The touch sensor records the period of time (i.e., the number of clock cycles) over a fixed charge/discharge cycles (specified by :cpp:func:`touch_pad_set_charge_discharge_times`). The count result is the raw data that read from :cpp:func:`touch_pad_read_raw_data`. After finishing one measurement, the touch sensor sleeps until the next measurement start, this interval between two measurements can be set by :cpp:func:`touch_pad_set_measurement_interval`.

    .. note::

        If the specified charge and discharge cycles for measurement is too small, the result may be inaccurate, but increasing charge and discharge cycles will increase the power consumption as well. Additionally, the response of the touch sensor will slow down if the total time of the interval and measurement is too long.

Optimization of Measurements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A touch sensor has several configurable parameters to match the characteristics of a particular touch pad design. For instance, to sense smaller capacity changes, it is possible to narrow down the reference voltage range within which the touch pads are charged/discharged. The high and low reference voltages are set using the function :cpp:func:`touch_pad_set_voltage`.

.. only:: esp32

    Besides the ability to discern smaller capacity changes, a positive side effect is reduction of power consumption for low power applications. A likely negative effect is an increase in measurement noise. If the dynamic range of obtained readings is still satisfactory, then further reduction of power consumption might be done by reducing the measurement time with :cpp:func:`touch_pad_set_measurement_clock_cycles`.

.. only:: esp32s2 or esp32s3

    Besides the ability to discern smaller capacity changes, a positive side effect is reduction of power consumption for low power applications. A likely negative effect is an increase in measurement noise. If the dynamic range of obtained readings is still satisfactory, then further reduction of power consumption might be done by reducing the measurement time with :cpp:func:`touch_pad_set_charge_discharge_times`.

The following list summarizes available measurement parameters and corresponding 'set' functions:

* Touch pad charge / discharge parameters:

    * voltage range: :cpp:func:`touch_pad_set_voltage`
    * speed (slope): :cpp:func:`touch_pad_set_cnt_mode`

.. only:: esp32

    * Clock cycles of one measurement: :cpp:func:`touch_pad_set_measurement_clock_cycles`

.. only:: esp32s2 or esp32s3

    * Charge and discharge times of one measurement: :cpp:func:`touch_pad_set_charge_discharge_times`

Relationship between the voltage range (high/low reference voltages), speed (slope), and measurement time is shown in the figure below.

.. only:: esp32

    .. figure:: ../../../_static/touch_pad-measurement-parameters.jpg
        :align: center
        :alt: Touch Pad - relationship between measurement parameters
        :figclass: align-center

        Touch pad - relationship between measurement parameters

    The last chart **Output** represents the touch sensor reading, i.e., the count of pulses collected within the measurement time.

.. only:: esp32s2 or esp32s3

    .. figure:: ../../../_static/touch_pad-measurement-parameters-version2.png
        :align: center
        :alt: Touch Pad - relationship between measurement parameters
        :figclass: align-center

        Touch pad - relationship between measurement parameters

    The last chart **Output** represents the touch sensor reading, i.e., the time taken to accumulate the fixed number of cycles.

All functions are provided in pairs to **set** a specific parameter and to **get** the current parameter's value, e.g., :cpp:func:`touch_pad_set_voltage` and :cpp:func:`touch_pad_get_voltage`.

.. _touch_pad-api-filtering-of-measurements:

Filtering of Measurements
^^^^^^^^^^^^^^^^^^^^^^^^^
.. only:: esp32

    If measurements are noisy, you can filter them with provided API functions. Before using the filter, please start it by calling :cpp:func:`touch_pad_filter_start`.

    The filter type is IIR (infinite impulse response), and it has a configurable period that can be set with the function :cpp:func:`touch_pad_set_filter_period`.

    You can stop the filter with :cpp:func:`touch_pad_filter_stop`. If not required anymore, the filter can be deleted by invoking :cpp:func:`touch_pad_filter_delete`.

.. only:: esp32s2 or esp32s3

    If measurements are noisy, you can filter them with provided API functions. The {IDF_TARGET_NAME}'s touch functionality provide two sets of APIs for doing this.

    There is an internal touch channel that is not connected to any external GPIO. The measurements from this denoise pad can be used to filters out interference introduced on all channels, such as noise introduced by the power supply and external EMI.

    The denoise parameters are set with the function :cpp:func:`touch_pad_denoise_set_config` and started by with :cpp:func:`touch_pad_denoise_enable`

    There is also a configurable hardware implemented IIR-filter (infinite impulse response). This IIR-filter is configured with the function :cpp:func:`touch_pad_filter_set_config` and enabled by calling :cpp:func:`touch_pad_filter_enable`

Touch Detection
^^^^^^^^^^^^^^^

Touch detection is implemented in ESP32's hardware based on the user-configured threshold and raw measurements executed by FSM. Use the functions :cpp:func:`touch_pad_get_status` to check which pads have been touched and :cpp:func:`touch_pad_clear_status` to clear the touch status information.

Hardware touch detection can also be wired to interrupts. This is described in the next section.

If measurements are noisy and capacity changes are small, hardware touch detection might be unreliable. To resolve this issue, instead of using hardware detection/provided interrupts, implement measurement filtering and perform touch detection in your own application. For sample implementation of both methods of touch detection, see :example:`peripherals/touch_sensor/touch_sensor_{IDF_TARGET_TOUCH_SENSOR_VERSION}/touch_pad_interrupt`.

Touch Triggered Interrupts
^^^^^^^^^^^^^^^^^^^^^^^^^^

Before enabling an interrupt on a touch detection, you should establish a touch detection threshold. Use the functions described in `Touch State Measurements`_ to read and display sensor measurements when a pad is touched and released. Apply a filter if measurements are noisy and relative capacity changes are small. Depending on your application and environment conditions, test the influence of temperature and power supply voltage changes on measured values.

Once a detection threshold is established, it can be set during initialization with :cpp:func:`touch_pad_config` or at the runtime with :cpp:func:`touch_pad_set_thresh`.

.. only:: esp32

    In the next step, configure how interrupts are triggered. They can be triggered below or above the threshold, which is set with the function :cpp:func:`touch_pad_set_trigger_mode`.

Finally, configure and manage interrupt calls using the following functions:

* :cpp:func:`touch_pad_isr_register` / :cpp:func:`touch_pad_isr_deregister`
* :cpp:func:`touch_pad_intr_enable` / :cpp:func:`touch_pad_intr_disable`

When interrupts are operational, you can obtain the information from which particular pad an interrupt came by invoking :cpp:func:`touch_pad_get_status` and clear the pad status with :cpp:func:`touch_pad_clear_status`.

.. only:: esp32

    .. note::

        Interrupts on touch detection operate on raw/unfiltered measurements checked against user established threshold and are implemented in hardware. Enabling the software filtering API (see :ref:`touch_pad-api-filtering-of-measurements`) does not affect this process.

.. only:: esp32

    Wakeup from Sleep Mode
    ^^^^^^^^^^^^^^^^^^^^^^

    If touch pad interrupts are used to wake up the chip from a sleep mode, you can select a certain configuration of pads (SET1 or both SET1 and SET2) that should be touched to trigger the interrupt and cause the subsequent wakeup. To do so, use the function :cpp:func:`touch_pad_set_trigger_source`.

    Configuration of required bit patterns of pads may be managed for each 'SET' with:

    * :cpp:func:`touch_pad_set_group_mask` / :cpp:func:`touch_pad_get_group_mask`
    * :cpp:func:`touch_pad_clear_group_mask`

.. _touch_pad-api-examples:

Application Examples
--------------------

- Touch sensor read example: :example:`peripherals/touch_sensor/touch_sensor_{IDF_TARGET_TOUCH_SENSOR_VERSION}/touch_pad_read`.
- Touch sensor interrupt example: :example:`peripherals/touch_sensor/touch_sensor_{IDF_TARGET_TOUCH_SENSOR_VERSION}/touch_pad_interrupt`.

.. _touch_pad-api-reference:

API Reference
-------------

.. include-build-file:: inc/touch_sensor.inc
.. include-build-file:: inc/touch_sensor_common.inc

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^

Some useful macros can be used to specified the GPIO number of a touch pad channel, or vice versa. e.g.,

1. ``TOUCH_PAD_NUM5_GPIO_NUM`` is the GPIO number of channel 5 (12);
2. ``TOUCH_PAD_GPIO4_CHANNEL`` is the channel number of GPIO 4 (channel 0).

.. include-build-file:: inc/touch_sensor_channel.inc
.. include-build-file:: inc/touch_sensor_types.inc
