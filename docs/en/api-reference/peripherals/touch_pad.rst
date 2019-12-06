Touch Sensor
============

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

A touch sensor system is built on a substrate which carries electrodes and relevant connections under a protective flat surface. When a user touches the surface, the capacitance variation is used to evaluate if the touch was valid.

ESP32 can handle up to 10 capacitive touch pads / GPIOs. The sensing pads can be arranged in different combinations (e.g., matrix, slider), so that a larger area or more points can be detected. The touch pad sensing process is under the control of a hardware-implemented finite-state machine (FSM) which is initiated by software or a dedicated hardware timer.

Design, operation, and control registers of a touch sensor are discussed in `ESP32 Technical Reference Manual <https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>`_ (PDF). Please refer to this manual for additional details on how this subsystem works.

In-depth design details of touch sensors and firmware development guidelines for ESP32 are available in `Touch Sensor Application Note <https://github.com/espressif/esp-iot-solution/blob/master/documents/touch_pad_solution/touch_sensor_design_en.md>`_. If you want to test touch sensors in various configurations without building them on your own, check the `Guide for ESP32-Sense Development Kit <https://github.com/espressif/esp-iot-solution/blob/master/documents/evaluation_boards/esp32_sense_kit_guide_en.md>`_.


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

For detailed description of a particular function, please go to Section :ref:`touch_pad-api-reference`. Practical implementation of this API is covered in Section :ref:`touch_pad-api-examples`.


Initialization
^^^^^^^^^^^^^^

Before using a touch pad, you need to initialize the touch pad driver by calling the function :cpp:func:`touch_pad_init`. This function sets several ``.._DEFAULT`` driver parameters listed in :ref:`touch_pad-api-reference` under *Macros*. It also removes the information about which pads have been touched before, if any, and disables interrupts. 

If the driver is not required anymore, deinitialize it by calling :cpp:func:`touch_pad_deinit`.


Configuration
^^^^^^^^^^^^^

Enabling the touch sensor functionality for a particular GPIO is done with :cpp:func:`touch_pad_config`. 

Use the function :cpp:func:`touch_pad_set_fsm_mode` to select if touch pad measurement (operated by FSM) should be started automatically by a hardware timer, or by software. If software mode is selected, use :cpp:func:`touch_pad_sw_start` to start the FSM.


Touch State Measurements
^^^^^^^^^^^^^^^^^^^^^^^^

The following two functions come in handy to read raw or filtered measurements from the sensor:

* :cpp:func:`touch_pad_read`
* :cpp:func:`touch_pad_read_filtered`

They can also be used, for example, to evaluate a particular touch pad design by checking the range of sensor readings when a pad is touched or released. This information can be then used to establish a touch threshold.

.. note::

    Before using :cpp:func:`touch_pad_read_filtered`, you need to initialize and configure the filter by calling specific filter functions described in Section `Filtering of Measurements`_.

For the demonstration of how to use both read functions, check the application example :example:`peripherals/touch_pad_read`.


Optimization of Measurements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A touch sensor has several configurable parameters to match the characteristics of a particular touch pad design. For instance, to sense smaller capacity changes, it is possible to narrow down the reference voltage range within which the touch pads are charged / discharged. The high and low reference voltages are set using the function :cpp:func:`touch_pad_set_voltage`.

Besides the ability to discern smaller capacity changes, a positive side effect is reduction of power consumption for low power applications. A likely negative effect is an increase in measurement noise. If the dynamic range of obtained readings is still satisfactory, then further reduction of power consumption might be done by reducing the measurement time with :cpp:func:`touch_pad_set_meas_time`.

The following list summarizes available measurement parameters and corresponding 'set' functions:

* Touch pad charge / discharge parameters:

    * voltage range: :cpp:func:`touch_pad_set_voltage`
    * speed (slope): :cpp:func:`touch_pad_set_cnt_mode`

* Measurement time: :cpp:func:`touch_pad_set_meas_time`

Relationship between the voltage range (high / low reference voltages), speed (slope), and measurement time is shown in the figure below. 

.. figure:: ../../../_static/touch_pad-measurement-parameters.jpg
    :align: center
    :alt: Touch Pad - relationship between measurement parameters 
    :figclass: align-center

    Touch pad - relationship between measurement parameters 

The last chart *Output* represents the touch sensor reading, i.e., the count of pulses collected within the measurement time.

All functions are provided in pairs to *set* a specific parameter and to *get* the current parameter's value, e.g., :cpp:func:`touch_pad_set_voltage` and :cpp:func:`touch_pad_get_voltage`.

.. _touch_pad-api-filtering-of-measurements:

Filtering of Measurements
^^^^^^^^^^^^^^^^^^^^^^^^^

If measurements are noisy, you can filter them with provided API functions. Before using the filter, please start it by calling :cpp:func:`touch_pad_filter_start`.

The filter type is IIR (infinite impulse response), and it has a configurable period that can be set with the function :cpp:func:`touch_pad_set_filter_period`.

You can stop the filter with :cpp:func:`touch_pad_filter_stop`. If not required anymore, the filter can be deleted by invoking :cpp:func:`touch_pad_filter_delete`.


Touch Detection
^^^^^^^^^^^^^^^

Touch detection is implemented in ESP32's hardware based on the user-configured threshold and raw measurements executed by FSM. Use the functions :cpp:func:`touch_pad_get_status` to check which pads have been touched and :cpp:func:`touch_pad_clear_status` to clear the touch status information. 

Hardware touch detection can also be wired to interrupts. This is described in the next section.

If measurements are noisy and capacity changes are small, hardware touch detection might be unreliable. To resolve this issue, instead of using hardware detection / provided interrupts, implement measurement filtering and perform touch detection in your own application. For sample implementation of both methods of touch detection, see :example:`peripherals/touch_pad_interrupt`.


Touch Triggered Interrupts
^^^^^^^^^^^^^^^^^^^^^^^^^^

Before enabling an interrupt on a touch detection, you should establish a touch detection threshold. Use the functions described in `Touch State Measurements`_ to read and display sensor measurements when a pad is touched and released. Apply a filter if measurements are noisy and relative capacity changes are small. Depending on your application and environment conditions, test the influence of temperature and power supply voltage changes on measured values.

Once a detection threshold is established, it can be set during initialization with :cpp:func:`touch_pad_config` or at the runtime with :cpp:func:`touch_pad_set_thresh`.

In the next step, configure how interrupts are triggered. They can be triggered below or above the threshold, which is set with the function :cpp:func:`touch_pad_set_trigger_mode`.

Finally, configure and manage interrupt calls using the following functions:

* :cpp:func:`touch_pad_isr_register` / :cpp:func:`touch_pad_isr_deregister`
* :cpp:func:`touch_pad_intr_enable` / :cpp:func:`touch_pad_intr_disable`

When interrupts are operational, you can obtain the information from which particular pad an interrupt came by invoking :cpp:func:`touch_pad_get_status` and clear the pad status with :cpp:func:`touch_pad_clear_status`.

.. note::

    Interrupts on touch detection operate on raw / unfiltered measurements checked against user established threshold and are implemented in hardware. Enabling the software filtering API (see :ref:`touch_pad-api-filtering-of-measurements`) does not affect this process.


Wakeup from Sleep Mode
^^^^^^^^^^^^^^^^^^^^^^

If touch pad interrupts are used to wake up the chip from a sleep mode, you can select a certain configuration of pads (SET1 or both SET1 and SET2) that should be touched to trigger the interrupt and cause the subsequent wakeup. To do so, use the function :cpp:func:`touch_pad_set_trigger_source`.

Configuration of required bit patterns of pads may be managed for each 'SET' with:

* :cpp:func:`touch_pad_set_group_mask` / :cpp:func:`touch_pad_get_group_mask`
* :cpp:func:`touch_pad_clear_group_mask`


.. _touch_pad-api-examples:

Application Examples
--------------------

- Touch sensor read example: :example:`peripherals/touch_pad_read`.
- Touch sensor interrupt example: :example:`peripherals/touch_pad_interrupt`.


.. _touch_pad-api-reference:

API Reference
-------------

.. include:: /_build/inc/touch_pad.inc

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^
Some useful macros can be used to specified the GPIO number of a touch pad channel, or vice versa.
e.g.

1. ``TOUCH_PAD_NUM5_GPIO_NUM`` is the GPIO number of channel 5 (12);
2. ``TOUCH_PAD_GPIO4_CHANNEL`` is the channel number of GPIO 4 (channel 0).

.. include:: /_build/inc/touch_channel.inc

