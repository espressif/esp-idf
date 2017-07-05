Pulse Counter
=============

Overview
--------

The PCNT (Pulse Counter) module is designed to count the number of rising and/or falling edges of an input signal. Each pulse counter unit has a 16-bit signed counter register and two channels that can be configured to either increment or decrement the counter. Each channel has a signal input that accepts signal edges to be detected, as well as a control input that can be used to enable or disable the signal input. The inputs have optional filters that can be used to discard unwanted glitches in the signal.

Application Example
-------------------

Pulse counter with control signal and event interrupt example: :example:`peripherals/pcnt`.

API Reference
-------------

.. include:: /_build/inc/pcnt.inc

