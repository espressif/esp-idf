# I2C Self-Test Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of I2C driver by running two tasks on I2C bus:

1. Read external I2C sensor, here we take the BH1750 ambient light sensor (GY-30 module) for an example.
2. Use one of ESP32’s I2C port (master mode) to read and write another I2C port (slave mode) in ESP32.

If you have a new I2C application to go (for example, read the temperature data from external sensor with I2C interface), try this as a basic template, then add your own code.

## How to use example

### Hardware Required

To run this example, you should have one ESP32 dev board (e.g. ESP32-WROVER Kit) or ESP32 core board (e.g. ESP32-DevKitC). Optionally, you can also connect an external sensor, here we choose the BH1750 just for an example. BH1750 is a digital ambient light sensor, for more information about it, you can read the [PDF](http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1721fvc-e.pdf) of this sensor.

#### Pin Assignment:

**Note:** The following pin assignments are used by default, yout can change these  in the `menuconfig` .

|                  | SDA    | SCL    |
| ---------------- | ------ | ------ |
| ESP32 I2C Master | GPIO18 | GPIO19 |
| ESP32 I2C Slave  | GPIO25 | GPIO26 |
| BH1750 Sensor    | SDA    | SCL    |

- slave:
  - GPIO25 is assigned as the data signal of I2C slave port
  - GPIO26 is assigned as the clock signal of I2C slave port
- master:
  - GPIO18 is assigned as the data signal of I2C master port
  - GPIO19 is assigned as the clock signal of I2C master port

- Connection:
  - connect GPIO18 with GPIO25
  - connect GPIO19 with GPIO26
  - connect SDA/SCL of BH1750 sensor with GPIO18/GPIO19

**Note: ** There’s no need to add an external pull-up resistors for SDA/SCL pin, because the driver will enable the internal pull-up resistors.

### Configure the project

Enter `make menuconfig` if you are using GNU Make based build system or enter `idf.py menuconfig` if you are using CMake based build system. Then go into `Example Configuration` menu.

- In the `I2C Master` submenu, you can set the pin number of SDA/SCL according to your board. Also you can modify the I2C port number and freauency of the master.
- In the `I2C Slave` submenu, you can set the pin number of SDA/SCL according to your board. Also you can modify the I2C port number and address of the slave.
- In the `BH1750 Sensor` submenu, you can choose the slave address of BH1750 accroding to the pin level of ADDR pin (if the pin level of ADDR is low then the address is `0x23`, otherwise it is `0x5c`). Here you can also control the operation mode of BH1750, each mode has a different resolution and measurement time. For example, in the `One Time L-Resolution` mode, the resolution is 4 Lux and measurement time is typically 16ms (higher resolution means longer measurement time). For more information, you can consult the datasheet of BH1750.


### Build and Flash

Enter `make -j4 flash monitor` if you are using GNU Make based build system or enter `idf.py build flash monitor` if you' are using CMake based build system.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (6495) i2c-example: TASK[1] test cnt: 1
*******************
TASK[1]  MASTER READ SENSOR( BH1750 )
*******************
data_h: 01
data_l: d0
sensor val: 386.67 [Lux]
I (6695) i2c-example: TASK[0] test cnt: 2
*******************
TASK[0]  MASTER READ SENSOR( BH1750 )
*******************
data_h: 01
data_l: d0
sensor val: 386.67 [Lux]
*******************
TASK[0]  MASTER READ FROM SLAVE
*******************
====TASK[0] Slave buffer data ====
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 
70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 

====TASK[0] Master read ====
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 
70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 

*******************
TASK[1]  MASTER READ FROM SLAVE
*******************
====TASK[1] Slave buffer data ====
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 
70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 

====TASK[1] Master read ====
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 
70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 

*******************
TASK[0]  MASTER WRITE TO SLAVE
*******************
----TASK[0] Master write ----
0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 
1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 
2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 
3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 
4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 
5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 
6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 
7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 

----TASK[0] Slave read: [128] bytes ----
0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 
1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 
2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 
3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 
4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 
5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 
6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 
7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 
```

## Troubleshooting

- BH1750 has two I2C address, which is decided by the voltage level of `ADDR` pin at start up. Make sure to check your schemetic before run this example.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
