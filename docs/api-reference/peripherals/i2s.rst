I2S
===

Overview
--------

ESP32 contains two I2S peripherals. These peripherals can be configured to input and output sample data via the I2S driver.

The I2S peripheral supports DMA meaning it can stream sample data without requiring each sample to be read or written by the CPU.

I2S output can also be routed directly to the Digital/Analog Converter output channels (GPIO 25 & GPIO 26) to produce analog output directly, rather than via an external I2S codec.

.. note:: For high accuracy clock applications, APLL clock source can be used with `.use_apll = 1` and ESP32 will automatic caculate APLL parameter. 

Application Example
-------------------

A full I2S example is available in esp-idf: :example:`peripherals/i2s`.

Short example of I2S configuration:

.. highlight:: c

::

    #include "driver/i2s.h"
    #include "freertos/queue.h"

    static const int i2s_num = 0; // i2s port number

    static const i2s_config_t i2s_config = {
         .mode = I2S_MODE_MASTER | I2S_MODE_TX,
         .sample_rate = 44100,
         .bits_per_sample = 16,
         .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
         .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
         .dma_buf_count = 8,
         .dma_buf_len = 64,
         .use_apll = 0,
         .apll_param = I2S_APLL_NONE
    };

    static const i2s_pin_config_t pin_config = {
        .bck_io_num = 26,
        .ws_io_num = 25,
        .data_out_num = 22,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    ...

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver

        i2s_set_pin(i2s_num, &pin_config);

        i2s_set_sample_rates(i2s_num, 22050); //set sample rates

        i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

Short example configuring I2S to use internal DAC for analog output::

    #include "driver/i2s.h"
    #include "freertos/queue.h"

    static const int i2s_num = 0; // i2s port number

    static const i2s_config_t i2s_config = {
         .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
         .sample_rate = 44100,
         .bits_per_sample = 16, /* the DAC module will only take the 8bits from MSB */
         .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
         .communication_format = I2S_COMM_FORMAT_I2S_MSB,
         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
         .dma_buf_count = 8,
         .dma_buf_len = 64
    };

    ...

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver

        i2s_set_pin(i2s_num, NULL); //for internal DAC, this will enable both of the internal channels
        
        //You can call i2s_set_dac_mode to set built-in DAC output mode.
        //i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN); 

        i2s_set_sample_rates(i2s_num, 22050); //set sample rates

        i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

API Reference
-------------

.. include:: /_build/inc/i2s.inc

