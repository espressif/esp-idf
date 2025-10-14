| Supported Targets | ESP32 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# Touch Sensor Sleep Wake up Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is going to demonstrate how to wake up the chip from light/deep sleep by the touch sensor.

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* A USB cable for power supply and programming
* (Optional) Touch board with touch buttons on it.
    - If you don't have a touch board, you can connect the touch pins with male jump wires and touch it directly for testing.

### Configure the Project

You can determine the touch channel number by ``EXAMPLE_TOUCH_CHANNEL_NUM`` in the example. And adjust the active threshold by ``s_thresh2bm_ratio``.

You can run `idf.py menuconfig` to configure the example

- `Example Configuration > Touch Sensor wakeup level`: Select `Light sleep wakeup` or `Deep sleep wakeup` mode
- `Example Configuration > Allow touch sensor power down during the deep sleep`: Choose whether to allow the touch sensor power down during the deep sleep. If enabled, the touch sensor will be powered down during the deep sleep, only one specified touch channel can wakeup the chip from deep sleep. If disabled, the touch sensor will keep working during the deep sleep. Any enabled touch channel can wakeup the chip from deep sleep.


### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Light Sleep Log

```
...
Touch [CH 7] enabled on GPIO9
Touch [CH 8] enabled on GPIO10
Touch [CH 9] enabled on GPIO11
Initial benchmark and new threshold are:
Touch [CH 7] 0: 4884, 97
Touch [CH 8] 0: 4993, 99
Touch [CH 9] 0: 4913, 98
I (399) touch_wakeup: touch wakeup source is ready
Entering light sleep in:
5 sec...
4 sec...
3 sec...
2 sec...
1 sec...
W (5399) touch callback: ch 8 active
I (5399) touch_wakeup: Wake up by touch

Entering light sleep in:
5 sec...
W (5479) touch callback: ch 8 inactive
4 sec...
3 sec...
2 sec...
1 sec...
```

### Deep Sleep Log

```
...
Touch [CH 7] enabled on GPIO9
Touch [CH 8] enabled on GPIO10
Touch [CH 9] enabled on GPIO11
Initial benchmark and new threshold are:
Touch [CH 7] 0: 4897, 97
Touch [CH 8] 0: 4879, 97
Touch [CH 9] 0: 4954, 99
I (405) touch_wakeup: touch wakeup source is ready
Entering deep sleep in:
5 sec...
4 sec...
3 sec...
2 sec...
1 sec...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
