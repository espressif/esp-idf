Sigma-delta Modulation
======================

{IDF_TARGET_SIGMA_DELTA_MODULATION_CHANNEL_NUM:default="8", esp32c3="4"}

Introduction
------------

{IDF_TARGET_NAME} has a second-order sigma-delta modulation module. This driver configures the channels of the sigma-delta module.

Functionality Overview
----------------------

There are {IDF_TARGET_SIGMA_DELTA_MODULATION_CHANNEL_NUM} independent sigma-delta modulation channels identified with :cpp:type:`sigmadelta_channel_t`. Each channel is capable to output the binary, hardware generated signal with the sigma-delta modulation.

Selected channel should be set up by providing configuration parameters in :cpp:type:`sigmadelta_config_t` and then applying this configuration with :cpp:func:`sigmadelta_config`.

Another option is to call individual functions, that will configure all required parameters one by one:

* **Prescaler** of the sigma-delta generator - :cpp:func:`sigmadelta_set_prescale`
* **Duty** of the output signal - :cpp:func:`sigmadelta_set_duty`
* **GPIO pin** to output modulated signal - :cpp:func:`sigmadelta_set_pin`

The range of the 'duty' input parameter of :cpp:func:`sigmadelta_set_duty` is from -128 to 127 (eight bit signed integer). If zero value is set, then the output signal's duty will be about 50%, see description of :cpp:func:`sigmadelta_set_duty`.


Convert to analog signal (Optional)
-----------------------------------

Typically, if the sigma-delta signal is connected to an LED, you don't have to add any filter between them (because our eyes are a low pass filter naturally). However, if you want to check the real voltage or watch the analog waveform, you need to design an analog low pass filter. Also, it is recommended to use an active filter instead of a passive filter to gain better isolation and not lose too much voltage.

For example, you can take the following `Sallen-Key topology Low Pass Filter`_ as a reference.

.. figure:: ../../../_static/typical_sallenkey_LP_filter.png
    :align: center
    :alt: Sallen-Key Low Pass Filter
    :figclass: align-center

    Sallen-Key Low Pass Filter


Application Example
-------------------

Sigma-delta Modulation example: :example:`peripherals/sigmadelta`.

API Reference
-------------

.. include-build-file:: inc/sigmadelta.inc
.. include-build-file:: inc/sigmadelta_types.inc

.. _Sallen-Key topology Low Pass Filter: https://en.wikipedia.org/wiki/Sallen%E2%80%93Key_topology
