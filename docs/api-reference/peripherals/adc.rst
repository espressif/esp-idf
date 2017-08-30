Analog to Digital Converter
===========================

Overview
--------

ESP32 integrates two 12-bit SAR ("Successive Approximation Register") ADCs (Analog to Digital Converters) and supports measurements on 18 channels (analog enabled pins). Some of these pins can be used to build a programmable gain amplifier which is used for the measurement of small
analog signals.

The ADC driver API currently only supports ADC1 (9 channels, attached to GPIOs 32-39).

Taking an ADC reading involves configuring the ADC with the desired precision and attentuation settings, and then calling adc1_get_raw() to read the channel.

It is also possible to read the internal hall effect sensor via ADC1.

Application Example
-------------------

A full example using the ADC driver and the esp_adc_cal is available in esp-idf: :example:`peripherals/adc`

Reading voltage on ADC1 channel 0 (GPIO 36)::

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_12Bit);
        adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_0db);
        int val = adc1_get_raw(ADC1_CHANNEL_0);

Reading the internal hall effect sensor::

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_12Bit);
        int val = hall_sensor_read();

The value read in both these examples is 12 bits wide (range 0-4095).

API Reference
-------------

.. include:: /_build/inc/adc.inc


ADC Calibration
===============

Overview
--------
The esp_adc_cal API provides functions to correct for differences in measured voltages caused by non-ideal ADC reference voltages in ESP32s. The ideal ADC reference voltage is 1100mV however the reference voltage of different ESP32s can range from 1000mV to 1200mV. 

Correcting the measured voltage using the esp_adc_cal API involves referencing a lookup table of voltages. The voltage obtained from the lookup table is the scaled and shifted by a gain and offset factor that is based on the ADC's reference voltage.

The reference voltage of the ADCs can be routed to certain GPIOs and measured manually using the ADC driver’s adc2_vref_to_gpio() function.

Application Example
-------------------

Reading the ADC and obtaining a result in mV::

    #include <driver/adc.h>
    #include <esp_adc_cal.h>
    
    ...
        #define V_REF 1100  //ADC reference voltage
        
        //Config ADC and characteristics
        adc1_config_width(ADC_WIDTH_12Bit);
        adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);
        
        //Calculate ADC characteristics i.e. gain and offset factors
        esp_adc_cal_characteristics_t characteristics;
        esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_11db, ADC_WIDTH_12Bit, &characteristics);
        
        //Read ADC and obtain result in mV
        uint32_t voltage = adc1_to_voltage(ADC1_CHANNEL_6, &characteristics);
        printf("%d mV\n",voltage);
        
        
Routing ADC reference voltage to GPIO::

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

        
API Reference
-------------

.. include:: /_build/inc/esp_adc_cal.inc