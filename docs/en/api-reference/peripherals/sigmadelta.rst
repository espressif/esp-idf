Sigma-delta Modulation
======================

Introduction
------------

ESP32 has a second-order sigma-delta modulation module. This driver configures the channels of the sigma-delta module.

Functionality Overview 
----------------------

There are eight independent sigma-delta modulation channels identified with :cpp:type:`sigmadelta_channel_t`. Each channel is capable to output the binary, hardware generated signal with the sigma-delta modulation.

Selected channel should be set up by providing configuration parameters in :cpp:type:`sigmadelta_config_t` and then applying this configuration with :cpp:func:`sigmadelta_config`.

Another option is to call individual functions, that will configure all required parameters one by one:

* **Prescaler** of the sigma-delta generator - :cpp:func:`sigmadelta_set_prescale`
* **Duty** of the output signal - :cpp:func:`sigmadelta_set_duty`
* **GPIO pin** to output modulated signal - :cpp:func:`sigmadelta_set_pin`

The range of the 'duty' input parameter of :cpp:func:`sigmadelta_set_duty` is from -128 to 127 (eight bit signed integer). If zero value is set, then the output signal's duty will be about 50%, see description of :cpp:func:`sigmadelta_set_duty`.

Application Example
-------------------

Sigma-delta Modulation example: :example:`peripherals/sigmadelta`.

API Reference
-------------

.. include:: /_build/inc/sigmadelta.inc
