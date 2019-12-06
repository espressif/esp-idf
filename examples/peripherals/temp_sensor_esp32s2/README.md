# ESP32-S2 Temperature Sensor Example

The ESP32-S2 has a built-in temperature sensor. The temperature sensor module contains an 8-bit Sigma-Delta ADC and a temperature offset DAC.    

The conversion relationship is the first two columns of the table below. Among them, `offset = 0`(default) is the main measurement option, and other values are extended measurement options.  

DAC level | offset | measure range(℃) | measure error(℃)
    :-:   |   :-:  |        :-:        |       :-:
     0    |   -2   |      50 ~ 125     |       < 3
     1    |   -1   |      20 ~ 100     |       < 2
     2    |    0   |     -10 ~ 80      |       < 1
     3    |    1   |     -30 ~ 50      |       < 2
     4    |    2   |     -40 ~ 20      |       < 3

* Log output :

```
I (243) TempSensor: Initializing Temperature sensor
I (243) TempSensor: default dac 2, clk_div 6
I (243) TempSensor: Config temperature range [-10°C ~ 80°C], error < 1°C
I (253) TempSensor: Temperature sensor started
I (1253) TempSensor: Temperature out celsius 27.287399°C
I (2253) TempSensor: Temperature out celsius 26.848801°C
I (3253) TempSensor: Temperature out celsius 26.848801°C
I (4253) TempSensor: Temperature out celsius 27.287399°C
I (5253) TempSensor: Temperature out celsius 27.287399°C
```