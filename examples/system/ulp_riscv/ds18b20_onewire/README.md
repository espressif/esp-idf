| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# ULP-RISC-V DS18B20 Temperature Sensor OneWire Communication

This example demonstrates how to program the ULP-RISC-V co-processor to read temperature from a [DS18B20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf) temperature sensor over 1-Wire.
When the measurement exceeds the limit set(32.5 degrees) the co-processor will wake up the main CPU from deep-sleep and the main CPU will print the temperature measurement.

### Hardware Required

* A development board with a SOC which has a RISC-V ULP coprocessor (e.g., ESP32-S2 Saola)
* A USB cable for power supply and programming
* A DS18B20 temperature sensor

Example connection :

| ESP Dev-kit  | DS18B20 |
| ------------ | ------- |
| GPIO4        | DQ      |
| VCC 3.3V     | VDD     |
| GND          | GND     |


## Example output

```
Not a ULP wakeup, initializing it!
Entering in deep sleep

...

ULP-RISC-V woke up the main CPU, temperature is above set limit!
ULP-RISC-V read temperature is 32.562500
Entering in deep sleep

...

ULP-RISC-V woke up the main CPU, temperature is above set limit!
ULP-RISC-V read temperature is 33.000000
Entering in deep sleep

```
