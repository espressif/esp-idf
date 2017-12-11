Analog to Digital Converter
===========================

Overview
--------

ESP32 integrates two 12-bit SAR (`Successive Approximation Register <https://en.wikipedia.org/wiki/Successive_approximation_ADC>`_) ADCs (Analog to Digital Converters) and supports measurements on 18 channels (analog enabled pins). Some of these pins can be used to build a programmable gain amplifier which is used for the measurement of small analog signals.

The ADC driver API supports ADC1 (9 channels, attached to GPIOs 32 - 39), and ADC2 (10 channels, attached to GPIOs 0, 2, 4, 12 - 15 and 25 - 27).
However, there're some restrictions for the application to use ADC2:

1. The application can use ADC2 only when Wi-Fi driver is not started, since the ADC is also used by the Wi-Fi driver, which has higher priority.
2. Some of the ADC2 pins are used as strapping pins (GPIO 0, 2, 15), so they cannot be used freely. For examples, for official Develop Kits:

  - `ESP32 Core Board V2 / ESP32 DevKitC <http://esp-idf.readthedocs.io/en/latest/hw-reference/modules-and-boards.html#esp32-core-board-v2-esp32-devkitc>`_: GPIO 0 cannot be used due to external auto program circuits.
  - `ESP-WROVER-KIT V3 <http://esp-idf.readthedocs.io/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v3>`_: GPIO 0, 2, 4 and 15 cannot be used due to external connections for different purposes.

Configuration and Reading ADC
-----------------------------

The ADC should be configured before reading is taken.

 - For ADC1, configure desired precision and attenuation by calling functions :cpp:func:`adc1_config_width` and :cpp:func:`adc1_config_channel_atten`. 
 - For ADC2, configure the attenuation by :cpp:func:`adc2_config_channel_atten`. The reading width of ADC2 is configured every time you take the reading.
 
Attenuation configuration is done per channel, see :cpp:type:`adc1_channel_t` and :cpp:type:`adc2_channel_t`, set as a parameter of above functions.

Then it is possible to read ADC conversion result with :cpp:func:`adc1_get_raw` and :cpp:func:`adc2_get_raw`. Reading width of ADC2 should be set as a parameter of :cpp:func:`adc2_get_raw` instead of in the configuration functions.

.. note:: Since the ADC2 is shared with the WIFI module, which has higher priority, reading operation of :cpp:func:`adc2_get_raw` will fail between :cpp:func:`esp_wifi_start()` and :cpp:func:`esp_wifi_stop()`. Use the return code to see whether the reading is successful.

It is also possible to read the internal hall effect sensor via ADC1 by calling dedicated function :cpp:func:`hall_sensor_read`. Note that even the hall sensor is internal to ESP32, reading from it uses channels 0 and 3 of ADC1 (GPIO 36 and 39). Do not connect anything else to these pins and do not change their configuration. Otherwise it may affect the measurement of low value signal from the sesnor.

This API provides convenient way to configure ADC1 for reading from :doc:`ULP <../../api-guides/ulp>`. To do so, call function :cpp:func:`adc1_ulp_enable` and then set precision and attenuation as discussed above.

There is another specific function :cpp:func:`adc2_vref_to_gpio` used to route internal reference voltage to a GPIO pin. It comes handy to calibrate ADC reading and this is discussed in section :ref:`adc-api-adc-calibration`.

Application Examples
--------------------

Reading voltage on ADC1 channel 0 (GPIO 36)::

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);
        int val = adc1_get_raw(ADC1_CHANNEL_0);

The input voltage in above example is from 0 to 1.1V (0 dB attenuation). The input range can be extended by setting higher attenuation, see :cpp:type:`adc_atten_t`.
An example using the ADC driver including calibration (discussed below) is available in esp-idf: :example:`peripherals/adc`

Reading voltage on ADC2 channel 7 (GPIO 27)::

    #include <driver/adc.h>

    ...
        
        int read_raw;
        adc2_config_channel_atten( ADC2_CHANNEL_7, ADC_ATTEN_0db );

        esp_err_t r = adc2_get_raw( ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &read_raw);
        if ( r == ESP_OK ) {
            printf("%d\n", read_raw );
        } else if ( r == ESP_ERR_TIMEOUT ) {
            printf("ADC2 used by Wi-Fi.\n");
        }

The reading may fail due to collision with Wi-Fi, should check it.
An example using the ADC2 driver to read the output of DAC is available in esp-idf: :example:`peripherals/adc2`

Reading the internal hall effect sensor::

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_BIT_12);
        int val = hall_sensor_read();



The value read in both these examples is 12 bits wide (range 0-4095).

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
        esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, &characteristics);
        
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
