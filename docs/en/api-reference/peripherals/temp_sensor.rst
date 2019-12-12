ESP32-S2 Temperature Sensor
===========================

Overview
--------

The ESP32-S2 has a built-in temperature sensor. The temperature sensor module contains an 8-bit Sigma-Delta ADC and a temperature offset DAC.  
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

Application Example
-------------------

Temperature sensor reading example: :example:`peripherals/temp_sensor`.

API Reference - Normal Temp Sensor
----------------------------------

.. include:: /_build/inc/temp_sensor.inc
