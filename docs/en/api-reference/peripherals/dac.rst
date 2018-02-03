Digital To Analog Converter
===========================

Overview
--------

ESP32 has two 8-bit DAC (digital to analog converter) channels, connected to GPIO25 (Channel 1) and GPIO26 (Channel 2).

The DAC driver allows these channels to be set to arbitrary voltages.

The DAC channels can also be driven with DMA-style written sample data, via the :doc:`I2S driver <i2s>` when using the "built-in DAC mode".

For other analog output options, see the :doc:`Sigma-delta Modulation module <sigmadelta>` and the :doc:`LED Control module <ledc>`. Both these modules produce high frequency PWM output, which can be hardware low-pass filtered in order to generate a lower frequency analog output.


Application Example
-------------------

Setting DAC channel 1 (GPIO 25) voltage to approx 0.78 of VDD_A voltage (VDD * 200 / 255). For VDD_A 3.3V, this is 2.59V::

  #include <driver/dac.h>

  ...

      dac_output_enable(DAC_CHANNEL_1);
      dac_output_voltage(DAC_CHANNEL_1, 200);

API Reference
-------------

.. include:: /_build/inc/dac.inc

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^
Some useful macros can be used to specified the GPIO number of a DAC channel, or vice versa.
e.g.

1. ``DAC_CHANNEL_1_GPIO_NUM`` is the GPIO number of channel 1 (25);
2. ``DAC_GPIO26_CHANNEL`` is the channel number of GPIO 26 (channel 2).

.. include:: /_build/inc/dac_channel.inc
