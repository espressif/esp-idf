| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# Capacity Touch Sensor Example (for hardware version 3)

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

You can see the following output in the monitor if the example runs successfully:

```
W (461) touch: [sample_cfg_id 0] clock precision loss, expect 4000000 hz, got 4006725 hz
W (461) touch: [sample_cfg_id 1] clock precision loss, expect 8000000 hz, got 8013450 hz
W (461) touch: [sample_cfg_id 2] clock precision loss, expect 16000000 hz, got 16026900 hz
Initial benchmark and new threshold are:
[CH 0] 0: 4114, 411      1: 2057, 205    2: 1028, 102
[CH 1] 0: 4643, 464      1: 2322, 232    2: 1160, 116
[CH 2] 0: 4848, 484      1: 2424, 242    2: 1211, 121
[CH 3] 0: 4340, 434      1: 2170, 217    2: 1085, 108
=================================
benchmark [CH 0]: 4115 2056 1028
chan_data [CH 0]: 4115 2056 1028

benchmark [CH 1]: 4644 2322 1160
chan_data [CH 1]: 4644 2322 1160

benchmark [CH 2]: 4848 2423 1211
chan_data [CH 2]: 4848 2423 1211

benchmark [CH 3]: 4337 2168 1084
chan_data [CH 3]: 4337 2168 1084

=================================
benchmark [CH 0]: 4109 2054 1027
chan_data [CH 0]: 4109 2054 1027

benchmark [CH 1]: 4638 2318 1158
chan_data [CH 1]: 4638 2318 1158

benchmark [CH 2]: 4843 2421 1210
chan_data [CH 2]: 4845 2421 1210

benchmark [CH 3]: 4334 2167 1084
chan_data [CH 3]: 4334 2167 1083
...
```

And if you touch and release a button, you will see the following output:

```
...
I (1321) touch_callback: [CH 1] active
=================================
benchmark [CH 0]: 4111 2055 1027
chan_data [CH 0]: 4111 2055 1027

benchmark [CH 1]: 4676 2339 1168
chan_data [CH 1]: 17701 8798 4399

benchmark [CH 2]: 4870 2434 1217
chan_data [CH 2]: 4867 2433 1217

benchmark [CH 3]: 4333 2165 1082
chan_data [CH 3]: 4333 2165 1082

=================================
benchmark [CH 0]: 4109 2053 1027
chan_data [CH 0]: 4108 2053 1027

benchmark [CH 1]: 4676 2339 1168
chan_data [CH 1]: 11256 8817 4363

benchmark [CH 2]: 4868 2434 1217
chan_data [CH 2]: 4862 2429 1214

benchmark [CH 3]: 4332 2165 1082
chan_data [CH 3]: 4330 2164 1081

I (1931) touch_callback: [CH 1] inactive
=================================
benchmark [CH 0]: 4106 2052 1026
chan_data [CH 0]: 4106 2052 1026

benchmark [CH 1]: 4649 2323 1161
chan_data [CH 1]: 4650 2323 1161

benchmark [CH 2]: 4847 2422 1211
chan_data [CH 2]: 4846 2422 1211

benchmark [CH 3]: 4329 2163 1082
chan_data [CH 3]: 4329 2164 1082
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
