Analog to Digital Converter
===========================

Overview
--------

ESP32 integrates two 12-bit SAR ("Successive Approximation Register") ADCs (Analog to Digital Converters) and supports measurements on 18 channels (analog enabled pins). Some of these pins can be used to build a programmable gain amplifier which is used for the measurement of small
analog signals.

The ADC driver API currently only supports ADC1 (9 channels, attached to GPIOs 32-39).

Taking an ADC reading involves configuring the ADC with the desired precision and attentuation settings, and then calling adc1_get_voltage() to read the channel.

It is also possible to read the internal hall effect sensor via ADC1.

Application Example
-------------------

Reading voltage on ADC1 channel 0 (GPIO 36)::

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_12Bit);
        adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_0db);
        int val = adc1_get_voltage(ADC1_CHANNEL_0);

Reading the internal hall effect sensor::

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_12Bit);
        int val = hall_sensor_read();

The value read in both these examples is 12 bits wide (range 0-4095).

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `components/driver/include/driver/adc.h`

Enumerations
^^^^^^^^^^^^

  .. doxygenenum:: adc1_channel_t
  .. doxygenenum:: adc_atten_t
  .. doxygenenum:: adc_bits_width_t

Functions
^^^^^^^^^

  .. doxygenfunction:: adc1_config_width
  .. doxygenfunction:: adc1_config_channel_atten
  .. doxygenfunction:: adc1_get_voltage
  .. doxygenfunction:: hall_sensor_read

