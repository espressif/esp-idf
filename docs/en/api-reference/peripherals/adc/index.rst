Analog to Digital Converter (ADC)
=================================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

This guide provides a comprehensive overview of the ADC (Analog to Digital Converter) controller on {IDF_TARGET_NAME}. It begins by introducing core ADC concepts such as conversion principles, raw data resolution, reference voltage, and attenuation. Then it walks through the two supported ADC driver modes — oneshot mode and continuous mode — along with ADC calibration, which helps improve accuracy.

{IDF_TARGET_NAME} integrates {IDF_TARGET_SOC_ADC_PERIPH_NUM} ADC(s) for measuring analog signals from multiple input channels. For details about the number of measurement channels (analog-enabled pins), voltage ranges, and other ADC characteristics, please refer to the `datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`__.


ADC Conversion
---------------

ADC conversion is the process of converting an input analog voltage to a digital value. The results provided by the ADC driver APIs are raw data values that represent the analog input in digital form.

By default, the bit width of these raw ADC results is 12 bits. This means the input voltage range is divided into 4096 (2\ :sup:`12`) discrete levels, which defines the minimum detectable change in input signal.

The voltage ``Vdata`` corresponding to a raw ADC result ``data`` is calculated as:

.. math::

    V_{data} = \frac{data}{2^{bitwidth} - 1} \times V_{ref}

Where:

- ``data`` is the raw ADC result.
- ``bitwidth`` is the resolution of the ADC result (e.g., 12 bits).
- ``Vref`` is the ADC’s reference voltage.

By design, ``Vref`` is set to 1100 mV. However, due to manufacturing variations, the actual value may range between 1000 mV and 1200 mV depending on the chip.

To obtain calibrated and accurate voltage values, refer to the section :doc:`adc_calibration`, which explains how to use the ADC calibration driver to adjust the raw results based on the actual ``Vref`` value.


ADC Attenuation
---------------

The ADC can measure analog voltages from 0 V to ``Vref``. To measure higher voltages, input signals can be attenuated before being passed to the ADC.

The supported attenuation levels are:

- 0 dB (k≈100%)
- 2.5 dB (k≈75%)
- 6 dB (k≈50%)
- 12 dB (k≈25%)

Higher attenuation levels allow the ADC to measure higher input voltages. The voltage ``Vdata`` after applying attenuation can be calculated using:

.. math::

    V_{data} = \frac{V_{ref}}{k}\times{\frac{data}{2^{bitwidth} - 1}}

Where:

- ``k`` is the ratio value corresponding to the attenuation level.
- Other variables are as defined above.

.. only:: not esp32

    For detailed input voltage ranges associated with each attenuation setting, refer to the `datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`__ > Electrical Characteristics > ADC Characteristics.

.. only:: esp32

    For detailed input voltage ranges associated with each attenuation setting, refer to the `datasheet <{IDF_TARGET_DATASHEET_EN_URL}>`__ > Function Description > Analog Peripherals > Analog-to-Digital Converter (ADC).

Driver Usage
------------

.. list::

    - ADC unit supports **oneshot mode**. Oneshot mode is suitable for oneshot sampling: ADC samples one channel at a time.
    :SOC_ADC_DMA_SUPPORTED: - Each ADC unit supports **continuous mode**. Continuous mode is designed for continuous sampling: ADC sequentially samples a group of channels or continuously samples a single channel.

See the guide below for implementation details:

.. toctree::
    :maxdepth: 2

    adc_oneshot
    :SOC_ADC_DMA_SUPPORTED: adc_continuous


ADC Calibration
----------------

The ADC calibration driver corrects deviations through software to obtain more accurate output results.

For more information, refer to the following guide:

.. toctree::
    :maxdepth: 2

    adc_calibration

API Reference
-------------

.. include-build-file:: inc/adc_channel.inc
.. include-build-file:: inc/adc_types.inc
