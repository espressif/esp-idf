Analog to Digital Converter
===========================

{IDF_TARGET_ADC1_CH0: default="GPIO 0", esp32="GPIO 36"}
{IDF_TARGET_ADC2_CH7: default="GPIO 0", esp32="GPIO 27"}


Overview
--------

{IDF_TARGET_ADC_DATA_WIDTH:default="12", esp32s2="13"}
{IDF_TARGET_ADC_TOTAL_CHAN:default="20", esp32="18", esp32s2="20", esp32c3="6"}

The {IDF_TARGET_NAME} integrates two {IDF_TARGET_ADC_DATA_WIDTH}-bit SAR (`Successive Approximation Register <https://en.wikipedia.org/wiki/Successive_approximation_ADC>`_) ADCs, supporting a total of {IDF_TARGET_ADC_TOTAL_CHAN} measurement channels (analog enabled pins).

These channels are supported:

.. only:: esp32

    ADC1:
        - 8 channels: GPIO32 - GPIO39
    ADC2:
        - 10 channels: GPIO0, GPIO2, GPIO4, GPIO12 - GPIO15, GOIO25 - GPIO27

.. only:: esp32s2

    ADC1:
        - 10 channels: GPIO1 - GPIO10
    ADC2:
        - 10 channels: GPIO11 - GPIO20

.. only:: esp32c3

    ADC1:
        - 5 channels: GPIO0 - GPIO4
    ADC2:
        - 1 channels: GPIO5


.. _adc_limitations:

ADC Limitations
---------------

.. note::

    .. only:: esp32

        - Some of the ADC2 pins are used as strapping pins (GPIO 0, 2, 15) thus cannot be used freely. Such is the case in the following official Development Kits:
        - :ref:`ESP32 DevKitC <esp-modules-and-boards-esp32-devkitc>`: GPIO 0 cannot be used due to external auto program circuits.
        - :ref:`ESP-WROVER-KIT <esp-modules-and-boards-esp-wrover-kit>`: GPIO 0, 2, 4 and 15 cannot be used due to external connections for different purposes.
        - Since the ADC2 module is also used by the Wi-Fi, only one of them could get the preemption when using together, which means the :cpp:func:`adc2_get_raw` may get blocked until Wi-Fi stops, and vice versa.

    .. only:: not esp32

        - Since the ADC2 module is also used by the Wi-Fi, reading operation of :cpp:func:`adc2_get_raw` may fail between :cpp:func:`esp_wifi_start()` and :cpp:func:`esp_wifi_stop()`. Use the return code to see whether the reading is successful.

    .. only:: esp32c3

        - A specific ADC module can only work under one operating mode at any one time, either Continuous Read Mode or Single Read Mode.
        - ADC1 and ADC2 can not work under Singel Read Mode simultaneously. One of them will get blocked until another one finishes.
        - For continuous (DMA) read mode, the ADC sampling frequency (the ``sample_freq_hz`` member of :cpp:type:`adc_digi_config_t`) should be within ``SOC_ADC_SAMPLE_FREQ_THRES_LOW`` and ``SOC_ADC_SAMPLE_FREQ_THRES_HIGH``.

Driver Usage
------------

Each ADC unit supports two work modes, ADC single read mode and ADC continuous (DMA) mode. ADC single read mode is suitable for low-frequency sampling operations. ADC continuous (DMA) read mode is suitable for high-frequency continuous sampling actions.

.. note::

    ADC readings from a pin not connected to any signal are random.

.. only:: esp32c3

    ADC Continuous (DMA) Read mode
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Please use the ADC continuous read mode driver as the following steps:

    - Initialize the ADC driver by calling the function :cpp:func:`adc_digi_initialize`.
    - Initialize the ADC controller by calling the function :cpp:func:`adc_digi_controller_config`.
    - Start the ADC continuous reading by calling the function :cpp:func:`adc_digi_start`.
    - After starting the ADC, you can get the ADC reading result by calling the function :cpp:func:`adc_digi_read_bytes`. Before stopping the ADC (by calling :cpp:func:`adc_digi_stop`), the driver will keep converting the analog data to digital data.
    - Stop the ADC reading by calling the function :cpp:func:`adc_digi_stop`.
    - Deinitialize the ADC driver by calling the function :cpp:func:`adc_digi_deinitialize`.

    .. note:: See :ref:`adc_limitations` for the limitation of using ADC continuous (DMA) read mode.

ADC Single Read mode
^^^^^^^^^^^^^^^^^^^^

The ADC should be configured before reading is taken.

 - For ADC1, configure desired precision and attenuation by calling functions :cpp:func:`adc1_config_width` and :cpp:func:`adc1_config_channel_atten`.
 - For ADC2, configure the attenuation by :cpp:func:`adc2_config_channel_atten`. The reading width of ADC2 is configured every time you take the reading.

Attenuation configuration is done per channel, see :cpp:type:`adc1_channel_t` and :cpp:type:`adc2_channel_t`, set as a parameter of above functions.

Then it is possible to read ADC conversion result with :cpp:func:`adc1_get_raw` and :cpp:func:`adc2_get_raw`. Reading width of ADC2 should be set as a parameter of :cpp:func:`adc2_get_raw` instead of in the configuration functions.

.. only:: esp32

    It is also possible to read the internal hall effect sensor via ADC1 by calling dedicated function :cpp:func:`hall_sensor_read`. Note that even the hall sensor is internal to ESP32, reading from it uses channels 0 and 3 of ADC1 (GPIO 36 and 39). Do not connect anything else to these pins and do not change their configuration. Otherwise it may affect the measurement of low value signal from the sensor.

.. only:: SOC_ULP_SUPPORTED

    This API provides convenient way to configure ADC1 for reading from :doc:`ULP <../../api-guides/ulp>`. To do so, call function :cpp:func:`adc1_ulp_enable` and then set precision and attenuation as discussed above.

.. only:: esp32 or esp32s2

    There is another specific function :cpp:func:`adc_vref_to_gpio` used to route internal reference voltage to a GPIO pin. It comes handy to calibrate ADC reading and this is discussed in section :ref:`adc-api-adc-calibration`.

.. note:: See :ref:`adc_limitations` for the limitation of using ADC single read mode.


Application Example
-------------------

.. only:: esp32 or esp32s2

    Reading voltage on ADC1 channel 0 ({IDF_TARGET_ADC1_CH0})::

        #include <driver/adc.h>

        ...

            adc1_config_width(ADC_WIDTH_BIT_12);
            adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);
            int val = adc1_get_raw(ADC1_CHANNEL_0);

    The input voltage in the above example is from 0 to 1.1 V (0 dB attenuation). The input range can be extended by setting a higher attenuation, see :cpp:type:`adc_atten_t`.
    An example of using the ADC driver including calibration (discussed below) is available at esp-idf: :example:`peripherals/adc/single_read/adc`

    Reading voltage on ADC2 channel 7 ({IDF_TARGET_ADC2_CH7})::

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

    The reading may fail due to collision with Wi-Fi, if the return value of this API is ``ESP_ERR_INVALID_STATE``, then the reading result is not valid.
    An example using the ADC2 driver to read the output of DAC is available in esp-idf: :example:`peripherals/adc/single_read/adc2`

    .. only:: esp32

        Reading the internal hall effect sensor::

            #include <driver/adc.h>

            ...

                adc1_config_width(ADC_WIDTH_BIT_12);
                int val = hall_sensor_read();


    .. only:: esp32

        The value read in both these examples is 12 bits wide (range 0-4095).

    .. only:: esp32s2

        The value read in both these examples is 13 bits wide (range 0-8191).

    .. _adc-api-adc-calibration:

    Minimizing Noise
    ----------------

    The {IDF_TARGET_NAME} ADC can be sensitive to noise leading to large discrepancies in ADC readings. To minimize noise, users may connect a 0.1 µF capacitor to the ADC input pad in use. Multisampling may also be used to further mitigate the effects of noise.

    .. figure:: ../../../_static/adc-noise-graph.jpg
        :align: center
        :alt: ADC noise mitigation

        Graph illustrating noise mitigation using capacitor and multisampling of 64 samples.

    ADC Calibration
    ---------------

    The :component_file:`esp_adc_cal/include/esp_adc_cal.h` API provides functions to correct for differences in measured voltages caused by variation of ADC reference voltages (Vref) between chips. Per design the ADC reference voltage is 1100 mV, however the true reference voltage can range from 1000 mV to 1200 mV amongst different {IDF_TARGET_NAME}s.

    .. figure:: ../../../_static/adc-vref-graph.jpg
        :align: center
        :alt: ADC reference voltage comparison

        Graph illustrating effect of differing reference voltages on the ADC voltage curve.

    Correcting ADC readings using this API involves characterizing one of the ADCs at a given attenuation to obtain a characteristics curve (ADC-Voltage curve) that takes into account the difference in ADC reference voltage. The characteristics curve is in the form of ``y = coeff_a * x + coeff_b`` and is used to convert ADC readings to voltages in mV. Calculation of the characteristics curve is based on calibration values which can be stored in eFuse or provided by the user.

    Calibration Values
    ^^^^^^^^^^^^^^^^^^

    {IDF_TARGET_ADC_CALI_SOURCE: default="3", esp32="3", esp32s2="1"}

    Calibration values are used to generate characteristic curves that account for the variation of ADC reference voltage of a particular {IDF_TARGET_NAME} chip. There are currently {IDF_TARGET_ADC_CALI_SOURCE} source(s) of calibration values on {IDF_TARGET_NAME}. The availability of these calibration values will depend on the type and production date of the {IDF_TARGET_NAME} chip/module.

    .. only:: esp32

        * **Two Point** values represent each of the ADCs’ readings at 150 mV and 850 mV. To obtain more accurate calibration results these values should be measured by user and burned into eFuse ``BLOCK3``.

        * **eFuse Vref** represents the true ADC reference voltage. This value is measured and burned into eFuse ``BLOCK0`` during factory calibration.

        * **Default Vref** is an estimate of the ADC reference voltage provided by the user as a parameter during characterization. If Two Point or eFuse Vref values are unavailable, **Default Vref** will be used.

            Individual measurement and burning of the **eFuse Vref** has been applied to ESP32-D0WD and ESP32-D0WDQ6 chips produced on/after the 1st week of 2018. Such chips may be recognized by date codes on/later than 012018 (see Line 4 on figure below).

            .. figure:: ../../../_static/chip_surface_marking.png
                :align: center
                :alt: ESP32 Chip Surface Marking

                ESP32 Chip Surface Marking

            If you would like to purchase chips or modules with calibration, double check with distributor or Espressif (sales@espressif.com) directly.

            .. highlight:: none

            If you are unable to check the date code (i.e. the chip may be enclosed inside a canned module, etc.), you can still verify if **eFuse Vref** is present by running the `espefuse.py <https://github.com/espressif/esptool/wiki/espefuse>`_  tool with ``adc_info`` parameter ::

                $IDF_PATH/components/esptool_py/esptool/espefuse.py --port /dev/ttyUSB0 adc_info

            Replace ``/dev/ttyUSB0`` with {IDF_TARGET_NAME} board's port name.

            A chip that has specific **eFuse Vref** value programmed (in this case 1093 mV) will be reported as follows::

                ADC VRef calibration: 1093 mV

            In another example below the **eFuse Vref** is not programmed::

                ADC VRef calibration: None (1100 mV nominal)

            For a chip with two point calibration the message will look similar to::

                ADC VRef calibration: 1149 mV
                ADC readings stored in efuse BLK3:
                    ADC1 Low reading  (150 mV): 306
                    ADC1 High reading (850 mV): 3153
                    ADC2 Low reading  (150 mV): 389
                    ADC2 High reading (850 mV): 3206

    .. only:: esp32s2

        * **eFuse Two Point** values calibrates the ADC output at two different voltages. This value is measured and burned into eFuse ``BLOCK0`` during factory calibration on newly manufactured ESP32-S2 chips and modules. If you would like to purchase chips or modules with calibration, double check with distributor or Espressif (sales@espressif.com) directly.

        .. highlight:: none

        You can verify if **eFuse Two Point** is present by running the `espefuse.py <https://github.com/espressif/esptool/wiki/espefuse>`_  tool with ``adc_info`` parameter ::

            $IDF_PATH/components/esptool_py/esptool/espefuse.py --port /dev/ttyUSB0 adc_info

        Replace ``/dev/ttyUSB0`` with {IDF_TARGET_NAME} board's port name.

.. only:: esp32c3

    The code example for using ADC single read mode and ADC continuous (DMA) read mode can be found in the :example:`peripherals/adc/esp32c3` directory of ESP-IDF examples.

.. only:: esp32 or esp32s2

    Application Extensions
    ----------------------

    For a full example see esp-idf: :example:`peripherals/adc/single_read`

    Characterizing an ADC at a particular attenuation::

        #include "driver/adc.h"
        #include "esp_adc_cal.h"

        ...

            //Characterize ADC at particular atten
            esp_adc_cal_characteristics_t *adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
            esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
            //Check type of calibration value used to characterize ADC
            if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
                printf("eFuse Vref");
            } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
                printf("Two Point");
            } else {
                printf("Default");
            }

    Reading an ADC then converting the reading to a voltage::

        #include "driver/adc.h"
        #include "esp_adc_cal.h"

        ...
            uint32_t reading =  adc1_get_raw(ADC1_CHANNEL_5);
            uint32_t voltage = esp_adc_cal_raw_to_voltage(reading, adc_chars);

    Routing ADC reference voltage to GPIO, so it can be manually measured (for **Default Vref**)::

        #include "driver/adc.h"

        ...

            esp_err_t status = adc_vref_to_gpio(ADC_UNIT_1, GPIO_NUM_25);
            if (status == ESP_OK) {
                printf("v_ref routed to GPIO\n");
            } else {
                printf("failed to route v_ref\n");
            }

GPIO Lookup Macros
------------------

There are macros available to specify the GPIO number of a ADC channel, or vice versa.
e.g.

1. ``ADC1_CHANNEL_0_GPIO_NUM`` is the GPIO number of ADC1 channel 0.
2. ``ADC1_GPIOn_CHANNEL`` is the ADC1 channel number of GPIO n.

API Reference
-------------

This reference covers three components:

* :ref:`adc-api-reference-adc-driver`
* :ref:`adc-api-reference-adc-calibration`
* :ref:`adc-api-reference-gpio-lookup-macros`


.. _adc-api-reference-adc-driver:

ADC driver
^^^^^^^^^^

.. include-build-file:: inc/adc.inc

.. include-build-file:: inc/adc_types.inc

.. include-build-file:: inc/adc_common.inc

.. _adc-api-reference-adc-calibration:

ADC Calibration
^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_adc_cal.inc

.. _adc-api-reference-gpio-lookup-macros:

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/adc_channel.inc
