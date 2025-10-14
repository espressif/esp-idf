| Supported Targets | ESP32 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# Capacity Touch Sensor Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is going to demonstrate how to register the touch channels and read the data.

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* A USB cable for power supply and programming
* (Optional) Touch board with touch buttons on it.
    - If you don't have a touch board, you can connect the touch pins with male jump wires and touch it directly for testing.

### Configure the Project

You can determine the touch channel number by ``EXAMPLE_TOUCH_CHANNEL_NUM`` in the example. And adjust the active threshold by ``s_thresh2bm_ratio``.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

You can see the following output in the monitor if the example runs successfully (take ESP32-P4 for example):

```
Touch [CH 0] enabled on GPIO2
Touch [CH 1] enabled on GPIO3
Touch [CH 2] enabled on GPIO4
Touch [CH 3] enabled on GPIO5
=================================
Initial benchmark and new threshold are:
Touch [CH 0] 0: 5161, 77         1: 5121, 76     2: 2533, 37
Touch [CH 1] 0: 5007, 75         1: 5036, 75     2: 2464, 36
Touch [CH 2] 0: 5086, 76         1: 5056, 75     2: 2487, 37
Touch [CH 3] 0: 4965, 74         1: 4989, 74     2: 2433, 36
=================================
benchmark [CH 0]: 5160 5121 2533
smooth    [CH 0]: 5160 5122 2533

benchmark [CH 1]: 5007 5036 2464
smooth    [CH 1]: 5007 5036 2464

benchmark [CH 2]: 5086 5056 2487
smooth    [CH 2]: 5086 5056 2487

benchmark [CH 3]: 4964 4989 2433
smooth    [CH 3]: 4964 4990 2433

=================================
benchmark [CH 0]: 5159 5121 2533
smooth    [CH 0]: 5160 5121 2533

benchmark [CH 1]: 5005 5036 2464
smooth    [CH 1]: 5006 5035 2464

benchmark [CH 2]: 5085 5056 2487
smooth    [CH 2]: 5086 5056 2488

benchmark [CH 3]: 4964 4990 2433
smooth    [CH 3]: 4964 4990 2433
...
```

And if you touch and release a button, you will see the following output:

```
...
I (2861) touch_callback: [CH 0] active
=================================
benchmark [CH 0]: 5755 5425 2762
smooth    [CH 0]: 5997 5666 2841

benchmark [CH 1]: 5025 5049 2473
smooth    [CH 1]: 5025 5050 2473

benchmark [CH 2]: 5104 5066 2495
smooth    [CH 2]: 5105 5066 2495

benchmark [CH 3]: 4982 5002 2441
smooth    [CH 3]: 4982 5001 2441

I (3021) touch_callback: [CH 0] inactive
=================================
benchmark [CH 0]: 5756 5428 2763
smooth    [CH 0]: 5756 5428 2764

benchmark [CH 1]: 5025 5048 2473
smooth    [CH 1]: 5026 5048 2474

benchmark [CH 2]: 5104 5066 2495
smooth    [CH 2]: 5104 5066 2495

benchmark [CH 3]: 4981 5002 2441
smooth    [CH 3]: 4981 5002 2441
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
