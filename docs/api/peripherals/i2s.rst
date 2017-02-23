I2S
===

Overview
--------

ESP32 contains two I2S peripherals. These peripherals can be configured to input and output sample data via the I2S driver.

The I2S peripheral supports DMA meaning it can stream sample data without requiring each sample to be read or written by the CPU.

I2S output can also be routed directly to the Digital/Analog Converter output channels (GPIO 25 & GPIO 26) to produce analog output directly, rather than via an external I2S codec.

Application Example
-------------------

A full I2S example is available in esp-idf: :example:`peripherals/i2s`.

Short example of I2S configuration::

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
         .dma_buf_len = 64
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
         .bits_per_sample = 8, /* must be 8 for built-in DAC */
         .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
         .communication_format = I2S_COMM_FORMAT_I2S_MSB,
         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
         .dma_buf_count = 8,
         .dma_buf_len = 64
    };

    ...

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver

        i2s_set_pin(i2s_num, NULL); //for internal DAC

        i2s_set_sample_rates(i2s_num, 22050); //set sample rates

        i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `components/driver/include/driver/i2s.h`

Data Structures
^^^^^^^^^^^^^^^

  .. doxygenstruct:: i2s_config_t
     :members:
  .. doxygenstruct:: i2s_event_t
     :members:
  .. doxygenstruct:: i2s_pin_config_t
     :members:

Macros
^^^^^^

  .. doxygendefine:: I2S_PIN_NO_CHANGE

Enumerations
^^^^^^^^^^^^

  .. doxygenenum:: i2s_bits_per_sample_t
  .. doxygenenum:: i2s_comm_format_t
  .. doxygenenum:: i2s_channel_fmt_t
  .. doxygenenum:: pdm_sample_rate_ratio_t
  .. doxygenenum:: pdm_pcm_conv_t
  .. doxygenenum:: i2s_port_t
  .. doxygenenum:: i2s_mode_t
  .. doxygenenum:: i2s_event_type_t

Functions
^^^^^^^^^

  .. doxygenfunction:: i2s_set_pin
  .. doxygenfunction:: i2s_driver_install
  .. doxygenfunction:: i2s_driver_uninstall
  .. doxygenfunction:: i2s_write_bytes
  .. doxygenfunction:: i2s_read_bytes
  .. doxygenfunction:: i2s_push_sample
  .. doxygenfunction:: i2s_pop_sample
  .. doxygenfunction:: i2s_set_sample_rates
  .. doxygenfunction:: i2s_start
  .. doxygenfunction:: i2s_stop
  .. doxygenfunction:: i2s_zero_dma_buffer


