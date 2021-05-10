Digital To Analog Converter (DAC)
=================================

{IDF_TARGET_DAC_CH_1: default = "GPIO25", esp32 = "GPIO25", esp32s2 = "GPIO17"}
{IDF_TARGET_DAC_CH_2: default = "GPIO26", esp32 = "GPIO26", esp32s2 = "GPIO18"}

Overview
--------

{IDF_TARGET_NAME} has two 8-bit DAC (digital to analog converter) channels, connected to {IDF_TARGET_DAC_CH_1} (Channel 1) and {IDF_TARGET_DAC_CH_2} (Channel 2).



The DAC driver allows these channels to be set to arbitrary voltages.

.. only:: esp32

    The DAC channels can also be driven with DMA-style written sample data by the digital controller, via the :doc:`I2S driver <i2s>` when using the "built-in DAC mode".

.. only:: esp32s2

    The DAC channels can also be driven with DMA-style written sample data by the digital controller, please check the Application Example(DMA part) to get more information.


For other analog output options, see the :doc:`Sigma-delta Modulation module <sdm>` and the :doc:`LED Control module <ledc>`. Both these modules produce high frequency PDM/PWM output, which can be hardware low-pass filtered in order to generate a lower frequency analog output.


Application Example
-------------------

Setting DAC channel 1 ({IDF_TARGET_DAC_CH_1}) voltage to approx 0.78 of VDD_A voltage (VDD * 200 / 255). For VDD_A 3.3V, this is 2.59V.

.. code:: c

    #include <driver/dac.h>

    ...

    dac_output_enable(DAC_CHANNEL_1);
    dac_output_voltage(DAC_CHANNEL_1, 200);


.. only:: esp32s2

For {IDF_TARGET_NAME}, DAC support to use DMA to send digital data to convert. Here is the example:

.. code-block:: c

    #include <driver/dac.h>

    const dac_digi_config_t cfg = {
        .mode = mode,
        .interval = 100,
        .dig_clk.use_apll = false,  // APB clk
        .dig_clk.div_num = 79,      // See comments `adc_digi_clk_t`
        .dig_clk.div_b = 1,
        .dig_clk.div_a = 0,
        .dac_dma_cnt = 1,           // The dac dma link number for your project
        .dac_dma_length = 512,      // The dac dam link length for your project. Should fit the buffer you prepared with dac_dma_cnt.
        .dac_dma_link_type = DAC_DMA_LINK_RECURSIVE,  //The link type.
    };

    dac_digi_initialize(&cfg);
    dac_output_enable(DAC_CHANNEL_X);
    dac_digi_start();
    dac_digi_write_bytes((uint8_t*)buffer_you_prepared);
    dac_digi_stop();
    dac_digi_deinitialize();

    

API Reference
-------------

.. include-build-file:: inc/dac.inc
.. include-build-file:: inc/dac_common.inc

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^
Some useful macros can be used to specified the GPIO number of a DAC channel, or vice versa.
e.g.

1. ``DAC_CHANNEL_1_GPIO_NUM`` is the GPIO number of channel 1 ({IDF_TARGET_DAC_CH_1});
2. ``DAC_{IDF_TARGET_DAC_CH_2}_CHANNEL`` is the channel number of GPIO 26 (channel 2).

.. include-build-file:: inc/dac_channel.inc
.. include-build-file:: inc/dac_types.inc
