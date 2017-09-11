Analog to Digital Converter
===========================

Overview
--------

ESP32 integrates two 12-bit SAR (`Successive Approximation Register <https://en.wikipedia.org/wiki/Successive_approximation_ADC>`_) ADCs (Analog to Digital Converters) and supports measurements on 18 channels (analog enabled pins). Some of these pins can be used to build a programmable gain amplifier which is used for the measurement of small analog signals.

The ADC driver API currently supports ADC1 (9 channels, attached to GPIOs 32 - 39).

API to support ADC2 is not available yet in ESP-IDF. The reason is that ADC2 is also used by Wi-Fi driver, and the application can only use ADC2 when Wi-Fi driver is not using it (and is not about to use it). This coordination mechanism is work in progress at the moment.

Configuration and Reading ADC
-----------------------------

Taking an ADC reading involves configuring the ADC with the desired precision and attenuation by calling functions :cpp:func:`adc1_config_width` and :cpp:func:`adc1_config_channel_atten`. Configuration is done per channel, see :cpp:type:`adc1_channel_t`, set as a parameter of above functions.

Then it is possible to read ADC conversion result with :cpp:func:`adc1_get_raw`.

It is also possible to read the internal hall effect sensor via ADC1 by calling dedicated function :cpp:func:`hall_sensor_read`. Note that even the hall sensor is internal to ESP32, reading from it uses channels 0 and 3 of ADC1 (GPIO 36 and 39). Do not connect anything else to these pins and do not change their configuration. Otherwise it may affect the measurement of low value signal from the sesnor.

This API provides convenient way to configure ADC1 for reading from :doc:`ULP <../../api-guides/ulp>`. To do so, call function :cpp:func:`adc1_ulp_enable` and then set precision and attenuation as discussed above.

There is another specific function :cpp:func:`adc2_vref_to_gpio` used to route internal reference voltage to a GPIO pin. It comes handy to calibrate ADC reading and this is discussed in section :ref:`adc-api-adc-calibration`.

Application Examples
--------------------

Reading voltage on ADC1 channel 0 (GPIO 36)::

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_12Bit);
        adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_0db);
        int val = adc1_get_raw(ADC1_CHANNEL_0);

The input voltage in above example is from 0 to 1.1V (0 dB attenuation). The input range can be extended by setting higher attenuation, see :cpp:type:`adc_atten_t`.

Reading the internal hall effect sensor::

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_12Bit);
        int val = hall_sensor_read();



The value read in both these examples is 12 bits wide (range 0-4095).

An example of using the ADC driver including calibration (discussed below) is available in esp-idf: :example:`peripherals/adc`

.. _adc-api-adc-calibration:

ADC Calibration
---------------

The :component_file:`esp_adc_cal/include/esp_adc_cal.h` API provides functions to correct for differences in measured voltages caused by non-ideal ADC reference voltages in ESP32s. The ideal ADC reference voltage is 1100 mV however the reference voltage of different ESP32s can range from 1000 mV to 1200 mV. 

Correcting the measured voltage using this API involves referencing a lookup table of voltages. The voltage obtained from the lookup table is then scaled and shifted by a gain and offset factor that is based on the ADC's reference voltage. This is done with function :cpp:func:`esp_adc_cal_get_characteristics`.

The reference voltage of the ADCs can be routed to certain GPIOs and measured manually using the ADC driver’s :cpp:func:`adc2_vref_to_gpio` function.

Example of Reading Calibrated Values
------------------------------------

Reading the ADC and obtaining a result in mV::

    #include <driver/adc.h>
    #include <esp_adc_cal.h>
    
    ...
        #define V_REF 1100  // ADC reference voltage
        
        // Configure ADC
        adc1_config_width(ADC_WIDTH_12Bit);
        adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);
        
        // Calculate ADC characteristics i.e. gain and offset factors
        esp_adc_cal_characteristics_t characteristics;
        esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_11db, ADC_WIDTH_12Bit, &characteristics);
        
        // Read ADC and obtain result in mV
        uint32_t voltage = adc1_to_voltage(ADC1_CHANNEL_6, &characteristics);
        printf("%d mV\n",voltage);
        
        
Routing ADC reference voltage to GPIO, so it can be manually measured and entered in function :cpp:func:`esp_adc_cal_get_characteristics`::

    #include <driver/adc.h>
    #include <driver/gpio.h>
    #include <esp_err.h>

    ...

        esp_err_t status = adc2_vref_to_gpio(GPIO_NUM_25);
        if (status == ESP_OK){
            printf("v_ref routed to GPIO\n");
        }else{
            printf("failed to route v_ref\n");
        }

An example of using the ADC driver and obtaining calibrated measurements is available in esp-idf: :example:`peripherals/adc`

GPIO Lookup Macros
------------------

There are macros available to specify the GPIO number of a ADC channel, or vice versa.
e.g.

1. ``ADC1_CHANNEL_0_GPIO_NUM`` is the GPIO number of ADC1 channel 0 (36);
2. ``ADC1_GPIO32_CHANNEL`` is the ADC1 channel number of GPIO 32 (ADC1 channel 4).

API Reference
-------------

This reference covers three components:

* :ref:`adc-api-reference-adc-driver`
* :ref:`adc-api-reference-adc-calibration`
* :ref:`adc-api-reference-gpio-lookup-macros`


.. _adc-api-reference-adc-driver:

ADC driver
^^^^^^^^^^

.. include:: /_build/inc/adc.inc

.. _adc-api-reference-adc-calibration:

ADC Calibration
^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_adc_cal.inc

.. _adc-api-reference-gpio-lookup-macros:

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/adc_channel.inc
