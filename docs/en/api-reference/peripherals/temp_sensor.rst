Temperature Sensor
===================

Overview
--------

The {IDF_TARGET_NAME} has a built-in temperature sensor used to measure the chip's internal temperature, and hard to measure the environmental temperature accurately. Being built-in means that the temperature sensor should work on any {IDF_TARGET_NAME} regardless of what board the chip is embedded in. The temperature sensor module contains an 8-bit Sigma-Delta ADC and a temperature offset DAC.

The conversion relationship is the first columns of the table below. Among them, offset = 0 is the main measurement option, and other values are extended measurement options.

+--------+------------------------+------------------------+
| offset | measure range(Celsius) | measure error(Celsius) |
+========+========================+========================+
|   -2   |        50 ~ 125        |           < 3          |
+--------+------------------------+------------------------+
|   -1   |        20 ~ 100        |           < 2          |
+--------+------------------------+------------------------+
|    0   |       -10 ~ 80         |           < 1          |
+--------+------------------------+------------------------+
|    1   |       -30 ~ 50         |           < 2          |
+--------+------------------------+------------------------+
|    2   |       -40 ~ 20         |           < 3          |
+--------+------------------------+------------------------+

Driver Usage
------------

1. Initialize the temperature sensor by calling the function :cpp:func:`temp_sensor_set_config` and pass to it a :cpp:type:`temp_sensor_config_t` structure. The :cpp:type:`temp_sensor_config_t` structure should contain all the required parameters. See the example below.

.. code-block:: c

    temp_sensor_config_t temp_sensor = {
        .dac_offset = TSENS_DAC_L2,
        .clk_div = 6,
    };
    temp_sensor_set_config(temp_sensor);

2. Start the temp_sensor by calling :cpp:func:'temp_sensor_start'. The temperature sensor will now measure the temperature.

3. To get the current temperature, take the example below as a reference, the value you get is in Celsius.

.. code-block:: c

    float tsens_out;
    temp_sensor_read_celsius(&tsens_out);

4. To stop the temperature sensor, please call :cpp:func:'temp_sensor_stop'.

.. note::

    If you want dynamic reconfiguration, you need to stop the sensor first (temp_sensor_stop), set the new configuration (temp_sensor_set_config), then start the sensor again (temp_sensor_start).

Application Example
-------------------

Temperature sensor reading example: :example:`peripherals/temp_sensor`.

API Reference - Normal Temp Sensor
----------------------------------

.. include-build-file:: inc/temp_sensor.inc
