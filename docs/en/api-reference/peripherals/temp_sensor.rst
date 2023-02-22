Temperature Sensor
===================

Overview
--------

The {IDF_TARGET_NAME} has a built-in temperature sensor used to measure the chip's internal temperature, and hard to measure the environmental temperature accurately. Being built-in means that the temperature sensor should work on any {IDF_TARGET_NAME} regardless of what board the chip is embedded in. The temperature sensor module contains an 8-bit Sigma-Delta ADC and a temperature offset DAC.

The conversion relationship is the first columns of the table below. Among them, Offset = 0 is the default measurement option, and other values are extended measurement options.

.. list-table::
   :header-rows: 1
   :widths: 25 50 50

   * - Offset
     - Measurement Range (°C)
     - Measurement Error (°C)
   * - -2
     - 50 ~ 125
     - < 3
   * - -1
     - 20 ~ 100
     - < 2
   * - 0
     - -10 ~ 80
     - < 1
   * - 1
     - -30 ~ 50
     - < 2
   * - 2
     - -40 ~ 20
     - < 3

Driver Usage
------------

1. Initialize the temperature sensor by calling the function :cpp:func:`temp_sensor_set_config` and pass to it a :cpp:type:`temp_sensor_config_t` structure. The :cpp:type:`temp_sensor_config_t` structure should contain all the required parameters. See the example below.

  .. code-block:: c

      temp_sensor_config_t temp_sensor = {
          .dac_offset = TSENS_DAC_L2,
          .clk_div = 6,
      };
      temp_sensor_set_config(temp_sensor);

1. Start the temperature sensor by calling :cpp:func:`temp_sensor_start`, and then the sensor will start to measure the temperature.

2. To get the current temperature, you can take the example below as a reference, and the temperatures you get are in Celsius.

  .. code-block:: c

      float tsens_out;
      temp_sensor_read_celsius(&tsens_out);

1. To stop the temperature sensor, please call :cpp:func:`temp_sensor_stop`.

.. note::

    To realize dynamic reconfiguration, you need to stop the sensor first with :cpp:func:`temp_sensor_stop`, then set the new configuration with :cpp:func:`temp_sensor_set_config`, and then re-start the sensor with :cpp:func:`temp_sensor_start`.

Application Example
-------------------

For examples of the temperature sensor, please refer to :example:`peripherals/temp_sensor`.

API Reference - Normal Temp Sensor
----------------------------------

.. include-build-file:: inc/temp_sensor.inc
