| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 | ESP32-H2 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# GPIO Wakeup Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example illustrates usage of gpio wakeup and light sleep.  Instead of using the `esp_light_sleep_start()` function, the example uses the `power management` api, which implements the auto light sleep. The gpio can be used to decide whether or not to auto light sleep.

Due to the difference between socs, the default io pin used by different socs may be different, the default io pin used are as follows:

| SOC      | IO pin |
| -------- | ------ |
| ESP32    | GPIO4  |
| ESP32-C2 | GPIO6  |
| ESP32-C3 | GPIO6  |
| ESP32-C6 | GPIO6  |
| ESP32-S2 | GPIO5  |
| ESP32-S3 | GPIO6  |
| ESP32-H2 | GPIO5  |

When the io pin input low level, the system will not enter auto light sleep state, when the io pin input high level, turn on the auto light sleep function, the system may enter light sleep state.



## How to Use Example

### Hardware Required

This example can be used with any ESP32 development board.

### Set Target

```
idf.py set-target esp32xx
```

(Use specific soc model instead of xx)

### Configure the project

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.
