Analog to Digital Converter (ADC)
=================================

{IDF_TARGET_ADC1_CH0: default="GPIO 0", esp32="GPIO 36"}
{IDF_TARGET_ADC2_CH7: default="GPIO 0", esp32="GPIO 27"}


ADC Channels
------------

{IDF_TARGET_ADC_TOTAL_CHAN:default="20", esp32="18", esp32s2="20", esp32c3="6"}
{IDF_TARGET_ADC_UNIT_NUM:default="2"}

The {IDF_TARGET_NAME} integrates {IDF_TARGET_ADC_UNIT_NUM} SAR (`Successive Approximation Register <https://en.wikipedia.org/wiki/Successive_approximation_ADC>`_) ADCs, supporting a total of {IDF_TARGET_ADC_TOTAL_CHAN} measurement channels (analog enabled pins).

These channels are supported:

.. only:: esp32

    ADC1:
        - 8 channels: GPIO32 - GPIO39
    ADC2:
        - 10 channels: GPIO0, GPIO2, GPIO4, GPIO12 - GPIO15, GOIO25 - GPIO27

.. only:: esp32s2 or esp32s3

    ADC1:
        - 10 channels: GPIO1 - GPIO10
    ADC2:
        - 10 channels: GPIO11 - GPIO20

.. only:: esp32c3

    ADC1:
        - 5 channels: GPIO0 - GPIO4
    ADC2:
        - 1 channels: GPIO5


.. _adc_attenuation:

ADC Attenuation
---------------
{IDF_TARGET_ADC_V_MIN_ATTEN0:default="0", esp32="100"}
{IDF_TARGET_ADC_V_MAX_ATTEN0:default="950", esp32s2="750", esp32c3="750", esp32s3="950"}

{IDF_TARGET_ADC_V_MIN_ATTEN1:default="0", esp32="100"}
{IDF_TARGET_ADC_V_MAX_ATTEN1:default="1250", esp32s2="1050", esp32c3="1050", esp32s3="1250"}

{IDF_TARGET_ADC_V_MIN_ATTEN2:default="0", esp32="150"}
{IDF_TARGET_ADC_V_MAX_ATTEN2:default="1750", esp32s2="1300", esp32c3="1300", esp32s3="1750"}

{IDF_TARGET_ADC_V_MIN_ATTEN3:default="0", esp32="150"}
{IDF_TARGET_ADC_V_MAX_ATTEN3:default="2450", esp32s2="2500", esp32c3="2500", esp32s3="3100"}


Vref is the reference voltage used internally by {IDF_TARGET_NAME} ADCs for measuring the input voltage. The {IDF_TARGET_NAME} ADCs can measure analog voltages from 0 V to Vref. Among different chips, the Vref varies, the median is 1.1 V. In order to convert voltages larger than Vref, input voltages can be attenuated before being input to the ADCs. There are 4 available attenuation options, the higher the attenuation is, the higher the measurable input voltage could be.

=====================  =========================================================================================================
Attenuation            Measurable input voltage range
=====================  =========================================================================================================
``ADC_ATTEN_DB_0``     {IDF_TARGET_ADC_V_MIN_ATTEN0} mV ~ {IDF_TARGET_ADC_V_MAX_ATTEN0} mV
``ADC_ATTEN_DB_2_5``   {IDF_TARGET_ADC_V_MIN_ATTEN1} mV ~ {IDF_TARGET_ADC_V_MAX_ATTEN1} mV
``ADC_ATTEN_DB_6``     {IDF_TARGET_ADC_V_MIN_ATTEN2} mV ~ {IDF_TARGET_ADC_V_MAX_ATTEN2} mV
``ADC_ATTEN_DB_11``    {IDF_TARGET_ADC_V_MIN_ATTEN3} mV ~ {IDF_TARGET_ADC_V_MAX_ATTEN3} mV
=====================  =========================================================================================================


.. _adc_conversion:

ADC Conversion
--------------

{IDF_TARGET_ADC_SINGLE_MAX_WIDTH:default="12", esp32s2="13}
{IDF_TARGET_ADC_SINGLE_RAW_MAX:default="4095", esp32s2="8191"}
{IDF_TARGET_ADC_CONTINUOUS_MAX_WIDTH:default="12", esp32s3="13}
{IDF_TARGET_ADC_CONTINUOUS_RAW_MAX:default="4095", esp32s3="8191"}


An ADC conversion is to convert the input analog voltage to a digital value. The ADC conversion results provided by the ADC driver APIs are raw data. Resolution of {IDF_TARGET_NAME} ADC raw results under Single Read mode is {IDF_TARGET_ADC_SINGLE_MAX_WIDTH}-bit.

- :cpp:func:`adc1_get_raw`
- :cpp:func:`adc2_get_raw`

.. only:: esp32c3

    - :cpp:func:`adc_digi_read_bytes`

To calculate the voltage based on the ADC raw results, this formula can be used:

.. parsed-literal::

    Vout = Dout * Vmax / Dmax       (1)

where:

======  =============================================================
Vout    Digital output result, standing for the voltage.
Dout    ADC raw digital reading result.
Vmax    Maximum measurable input analog voltage, see :ref:`adc_attenuation`.
Dmax    Maximum of the output ADC raw digital reading result, which is {IDF_TARGET_ADC_SINGLE_RAW_MAX} under Single Read mode, {IDF_TARGET_ADC_CONTINUOUS_RAW_MAX} under Continuous Read mode.
======  =============================================================

For boards with eFuse ADC calibration bits, :cpp:func:`esp_adc_cal_raw_to_voltage` can be used to get the calibrated conversion results. These results stand for the actual voltage (in mV). No need to transform these data via the formula (1).
If ADC calibration APIs are used on boards without eFuse ADC calibration bits, warnings will be generated. See :ref:`adc_calibration`.


.. _adc_limitations:

ADC Limitations
---------------

.. note::

    .. only:: esp32

        - Some of the ADC2 pins are used as strapping pins (GPIO 0, 2, 15) thus cannot be used freely. Such is the case in the following official Development Kits:
        - ESP32 DevKitC: GPIO 0 cannot be used due to external auto program circuits.
        - ESP-WROVER-KIT: GPIO 0, 2, 4 and 15 cannot be used due to external connections for different purposes.
        - Since the ADC2 module is also used by the Wi-Fi, only one of them could get the preemption when using together, which means the :cpp:func:`adc2_get_raw` may get blocked until Wi-Fi stops, and vice versa.

    .. only:: not esp32

        - Since the ADC2 module is also used by the Wi-Fi, reading operation of :cpp:func:`adc2_get_raw` may fail between :cpp:func:`esp_wifi_start()` and :cpp:func:`esp_wifi_stop()`. Use the return code to see whether the reading is successful.

    .. only:: esp32c3

        - A specific ADC module can only work under one operating mode at any one time, either Continuous Read Mode or Single Read Mode.
        - ADC1 and ADC2 can not work under Singel Read Mode simultaneously. One of them will get blocked until another one finishes.
        - For continuous (DMA) read mode, the ADC sampling frequency (the ``sample_freq_hz`` member of :cpp:type:`adc_digi_config_t`) should be within ``SOC_ADC_SAMPLE_FREQ_THRES_LOW`` and ``SOC_ADC_SAMPLE_FREQ_THRES_HIGH``.

Driver Usage
------------

.. only:: esp32c3

    Each ADC unit supports two work modes, ADC single read mode and ADC continuous (DMA) mode. ADC single read mode is suitable for low-frequency sampling operations. ADC continuous (DMA) read mode is suitable for high-frequency continuous sampling actions.

.. only:: not esp32c3

    Both of the ADC units support single read mode, which is suitable for low-frequency sampling operations.

.. note::

    ADC readings from a pin not connected to any signal are random.

.. only:: esp32c3

    ADC Continuous (DMA) Read mode
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    To use the ADC continuous read mode driver, execute the following steps:

        1. Initialize the ADC driver by calling the function :cpp:func:`adc_digi_initialize`.
        2. Initialize the ADC controller by calling the function :cpp:func:`adc_digi_controller_config`.
        3. Start the ADC continuous reading by calling the function :cpp:func:`adc_digi_start`.
        4. After starting the ADC, you can get the ADC reading result by calling the function :cpp:func:`adc_digi_read_bytes`. Before stopping the ADC (by calling :cpp:func:`adc_digi_stop`), the driver will keep converting the analog data to digital data.
        5. Stop the ADC reading by calling the function :cpp:func:`adc_digi_stop`.
        6. Deinitialize the ADC driver by calling the function :cpp:func:`adc_digi_deinitialize`.


    .. only:: esp32c3

    The code example for using ADC continuous (DMA) read mode can be found in the :example:`peripherals/adc/dma_read` directory of ESP-IDF examples.

    .. note:: See :ref:`adc_limitations` for the limitation of using ADC continuous (DMA) read mode.

ADC Single Read mode
^^^^^^^^^^^^^^^^^^^^

The ADC should be configured before reading is taken.

 - For ADC1, configure desired precision and attenuation by calling functions :cpp:func:`adc1_config_width` and :cpp:func:`adc1_config_channel_atten`.
 - For ADC2, configure the attenuation by :cpp:func:`adc2_config_channel_atten`. The reading width of ADC2 is configured every time you take the reading.

Attenuation configuration is done per channel, see :cpp:type:`adc1_channel_t` and :cpp:type:`adc2_channel_t`, set as a parameter of above functions.

Then it is possible to read ADC conversion result with :cpp:func:`adc1_get_raw` and :cpp:func:`adc2_get_raw`. Reading width of ADC2 should be set as a parameter of :cpp:func:`adc2_get_raw` instead of in the configuration functions.

Single Read mode ADC example can be found in :example:`peripherals/adc/single_read` directory of ESP-IDF examples.

.. only:: esp32

    It is also possible to read the internal hall effect sensor via ADC1 by calling dedicated function :cpp:func:`hall_sensor_read`. Note that even the hall sensor is internal to ESP32, reading from it uses channels 0 and 3 of ADC1 (GPIO 36 and 39). Do not connect anything else to these pins and do not change their configuration. Otherwise it may affect the measurement of low value signal from the sensor.

.. only:: SOC_ULP_SUPPORTED

    This API provides convenient way to configure ADC1 for reading from :doc:`ULP <../../api-guides/ulp>`. To do so, call function :cpp:func:`adc1_ulp_enable` and then set precision and attenuation as discussed above.

.. only:: esp32 or esp32s2

    There is another specific function :cpp:func:`adc_vref_to_gpio` used to route internal reference voltage to a GPIO pin. It comes handy to calibrate ADC reading and this is discussed in section :ref:`adc_calibration`.


.. note:: See :ref:`adc_limitations` for the limitation of using ADC single read mode.


Minimizing Noise
----------------

The {IDF_TARGET_NAME} ADC can be sensitive to noise leading to large discrepancies in ADC readings. Depending on the usage scenario, users may connect a bypass capacitor (e.g. a 100 nF ceramic capacitor) to the ADC input pad in use, to minimize noise. Besides, multisampling may also be used to further mitigate the effects of noise.

.. only:: esp32

    .. figure:: ../../../_static/diagrams/adc/adc-noise-graph.jpg
        :align: center
        :alt: ADC noise mitigation

        Graph illustrating noise mitigation using capacitor and multisampling of 64 samples.


.. _adc_calibration:

ADC Calibration
---------------

.. only:: esp32 or esp32s2

    The :component_file:`esp_adc_cal/include/esp_adc_cal.h` API provides functions to correct for differences in measured voltages caused by variation of ADC reference voltages (Vref) between chips. Per design the ADC reference voltage is 1100 mV, however the true reference voltage can range from 1000 mV to 1200 mV amongst different {IDF_TARGET_NAME}s.

    .. figure:: ../../../_static/diagrams/adc/adc-vref-graph.jpg
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



.. only:: esp32c3 or esp32s3

    {IDF_TARGET_NAME} ADC Calibration contains 2 steps: Hardware Calibration and Software Calibration.


    Hardware Calibration
    ^^^^^^^^^^^^^^^^^^^^

    Based on series of comparisons with the reference voltage, {IDF_TARGET_NAME} ADC determines each bit of the output digital result. Per design the {IDF_TARGET_NAME} ADC reference voltage is 1100 mV, however the true reference voltage can range from 1000 mV to 1200 mV among different chips. To minimize this difference, hardware calibration is introduced.

    Hardware calibration contains 2 steps:

        1. Set an auto-calibration parameter of bandgap voltage reference. In this way, the difference mentioned above can be minimized.
        2. Correct the offset of the ADC Vin-Dout characteristics. ADC characteristics is generally a function: f(x) = A * x + B, where B is the offset.

        .. only:: esp32c3

            An uncalibrated ADC characteristics is as follows:

            .. figure:: ../../../_static/diagrams/adc/adc-uncali-raw-c3.png
                :align: center
                :alt: ADC uncalibrated conversion result

        .. only:: esp32s3

            An uncalibrated ADC characteristics is as follows:

            .. figure:: ../../../_static/diagrams/adc/adc-uncali-raw-s3.png
                :align: center
                :alt: ADC uncalibrated conversion result


    The offset in the uncalibrated characteristics is significant. Step 2 is to correct the offset to 0.

        .. only:: esp32c3

            After hardware calibration, the ADC characteristics would be like:

            .. figure:: ../../../_static/diagrams/adc/adc-hw-cali-c3.png
                :align: center
                :alt: ADC conversion results after hardware calibration

        .. only:: esp32s3

            After hardware calibration, the ADC characteristics would be like:

            .. figure:: ../../../_static/diagrams/adc/adc-hw-cali-s3.png
                :align: center
                :alt: ADC conversion results after hardware calibration

    Hardware calibration is done internally by the ADC driver. The consequent results are raw data. A transformation is needed to get the final result, see :ref:`adc_conversion`.


    Software Calibration
    ^^^^^^^^^^^^^^^^^^^^

    To convert ADC raw data to calibrated digital data, following steps should be followed:

        1. Check the eFuse to know if the software calibration is supported via :cpp:func:`esp_adc_cal_check_efuse`.
        2. Calculate the ADC calibration characteristics via :cpp:func:`esp_adc_cal_characterize`. The ADC software calibration characteristics are per ADC module and per attenuation. For example, characteristics of ADC1 channel 0 under 11 dB attenuation are the same as characteristics of ADC1 channel 2 under 11 dB attenuation. But characteristics of ADC1 channel 0 under 11 dB attenuation are different with characteristics of ADC2 channel 0 under 11 dB attenuation. Also characteristics of ADC1 channel 0 under 11 dB attenuation are different with characteristics of ADC1 channel 0 under 6 dB attenuation.
        3. Get the actual voltage value via :cpp:func:`esp_adc_cal_raw_to_voltage`.

    .. only:: esp32c3

        After software calibration, the ADC characteristics would be like:

            .. figure:: ../../../_static/diagrams/adc/adc-all-cali-c3.png
                :align: center
                :alt: ADC conversion results after hardware calibration

    .. only:: esp32s3

        After software calibration, the ADC characteristics would be like:

            .. figure:: ../../../_static/diagrams/adc/adc-all-cali-s3.png
                :align: center
                :alt: ADC conversion results after hardware calibration


    The results provided by the ADC calibration APIs indicate the actual voltage values. ADC software calibration example can be found in :example:`peripherals/adc/single_read` directory of ESP-IDF examples.


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

.. _adc-api-reference-adc-calibration:

ADC Calibration
^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_adc_cal.inc

.. _adc-api-reference-gpio-lookup-macros:

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/adc_channel.inc
