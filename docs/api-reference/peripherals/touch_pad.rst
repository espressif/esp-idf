Touch Sensor
============

Overview
--------

A touch-sensor system is built on a substrate which carries electrodes and relevant connections under a protective flat surface. When a user touches the surface, the capacitance variation is triggered and a binary signal is generated to indicate whether the touch is valid.

ESP32 can provide up to 10 capacitive touch pads / GPIOs. The sensing pads can be arranged in different combinations, so that a larger area or more points can be detected. The touch pad sensing process is under the control of a hardware-implemented finite-state machine (FSM) which is initiated by software or a dedicated hardware timer.

Application Examples
--------------------

- Touch sensor read example: :example:`peripherals/touch_pad_read`.
- Touch sensor interrupt example: :example:`peripherals/touch_pad_interrupt`.

API Reference
-------------

.. include:: /_build/inc/touch_pad.inc

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^
Some useful macros can be used to specified the GPIO number of a touchpad channel, or vice versa.
e.g.

1. ``TOUCH_PAD_NUM5_GPIO_NUM`` is the GPIO number of channel 5 (12);
2. ``TOUCH_PAD_GPIO4_CHANNEL`` is the channel number of GPIO 4 (channel 0).

.. include:: /_build/inc/touch_channel.inc

