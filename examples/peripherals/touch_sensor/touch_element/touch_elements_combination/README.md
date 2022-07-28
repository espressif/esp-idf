| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Touch button example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the Touch Element library of capacitive touch sensor and set up more than one type of touch elements and handle all the event messages in one task.

## How to use example

### Hardware Required

* A development board with ESP32-S2 or ESP32-S3 chip
* A touch extension board like [esp32-s2-touch-devkit-1](https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32s2/esp32-s2-touch-devkit-1/user_guide.html)

### Configure the project

* Set the target of the build by following command, where TARGET can be `esp32s2` or `esp32s3`.
```
idf.py set-target TARGET
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (331) Touch Elements Combination Example: Touch element library installed
I (331) Touch Elements Combination Example: Touch button installed
I (341) Touch Elements Combination Example: Touch buttons created
I (351) Touch Elements Combination Example: Touch slider installed
I (351) Touch Elements Combination Example: Touch slider created
I (361) Touch Elements Combination Example: Touch element library start
I (1841) Touch Elements Combination Example: Button[6] Press
I (1971) Touch Elements Combination Example: Button[6] Release
I (2201) Touch Elements Combination Example: Button[8] Press
I (2351) Touch Elements Combination Example: Button[8] Release
I (2561) Touch Elements Combination Example: Button[10] Press
I (2721) Touch Elements Combination Example: Button[10] Release
I (3431) Touch Elements Combination Example: Slider Press, position: 0
I (3441) Touch Elements Combination Example: Slider Calculate, position: 0
I (3451) Touch Elements Combination Example: Slider Calculate, position: 0
I (3461) Touch Elements Combination Example: Slider Calculate, position: 0
I (3471) Touch Elements Combination Example: Slider Calculate, position: 0
I (3481) Touch Elements Combination Example: Slider Calculate, position: 0
I (3491) Touch Elements Combination Example: Slider Calculate, position: 0
I (3501) Touch Elements Combination Example: Slider Calculate, position: 1
I (3511) Touch Elements Combination Example: Slider Calculate, position: 1
I (3521) Touch Elements Combination Example: Slider Calculate, position: 2
I (3531) Touch Elements Combination Example: Slider Calculate, position: 2
I (3541) Touch Elements Combination Example: Slider Calculate, position: 3
I (3551) Touch Elements Combination Example: Slider Calculate, position: 4
I (3561) Touch Elements Combination Example: Slider Calculate, position: 5
I (3571) Touch Elements Combination Example: Slider Calculate, position: 6
I (3581) Touch Elements Combination Example: Slider Calculate, position: 7
I (3591) Touch Elements Combination Example: Slider Calculate, position: 8
I (3601) Touch Elements Combination Example: Slider Calculate, position: 10
I (3611) Touch Elements Combination Example: Slider Calculate, position: 11
I (3621) Touch Elements Combination Example: Slider Calculate, position: 12
I (3631) Touch Elements Combination Example: Slider Calculate, position: 13
I (3641) Touch Elements Combination Example: Slider Calculate, position: 15
I (3651) Touch Elements Combination Example: Slider Calculate, position: 16
I (3661) Touch Elements Combination Example: Slider Calculate, position: 17
I (3671) Touch Elements Combination Example: Slider Calculate, position: 19
I (3681) Touch Elements Combination Example: Slider Calculate, position: 20
I (3691) Touch Elements Combination Example: Slider Calculate, position: 21
I (3701) Touch Elements Combination Example: Slider Calculate, position: 23
I (3711) Touch Elements Combination Example: Slider Calculate, position: 24
I (3721) Touch Elements Combination Example: Slider Calculate, position: 26
I (3731) Touch Elements Combination Example: Slider Calculate, position: 27
I (3741) Touch Elements Combination Example: Slider Calculate, position: 28
I (3751) Touch Elements Combination Example: Slider Calculate, position: 29
I (3761) Touch Elements Combination Example: Slider Calculate, position: 31
I (3771) Touch Elements Combination Example: Slider Calculate, position: 32
I (3781) Touch Elements Combination Example: Slider Calculate, position: 33
I (3791) Touch Elements Combination Example: Slider Calculate, position: 34
I (3801) Touch Elements Combination Example: Slider Calculate, position: 36
I (3811) Touch Elements Combination Example: Slider Calculate, position: 37
I (3821) Touch Elements Combination Example: Slider Calculate, position: 38
I (3831) Touch Elements Combination Example: Slider Calculate, position: 39
I (3841) Touch Elements Combination Example: Slider Calculate, position: 41
I (3851) Touch Elements Combination Example: Slider Calculate, position: 42
I (3861) Touch Elements Combination Example: Slider Calculate, position: 43
I (3871) Touch Elements Combination Example: Slider Calculate, position: 45
I (3881) Touch Elements Combination Example: Slider Calculate, position: 47
I (3891) Touch Elements Combination Example: Slider Calculate, position: 48
I (3901) Touch Elements Combination Example: Slider Calculate, position: 50
I (3911) Touch Elements Combination Example: Slider Calculate, position: 52
I (3921) Touch Elements Combination Example: Slider Calculate, position: 53
I (3931) Touch Elements Combination Example: Slider Calculate, position: 55
I (3941) Touch Elements Combination Example: Slider Calculate, position: 57
I (3951) Touch Elements Combination Example: Slider Calculate, position: 58
I (3961) Touch Elements Combination Example: Slider Calculate, position: 60
I (3971) Touch Elements Combination Example: Slider Calculate, position: 61
I (3981) Touch Elements Combination Example: Slider Calculate, position: 62
I (3991) Touch Elements Combination Example: Slider Calculate, position: 64
I (4001) Touch Elements Combination Example: Slider Calculate, position: 65
I (4011) Touch Elements Combination Example: Slider Calculate, position: 66
I (4021) Touch Elements Combination Example: Slider Calculate, position: 68
I (4031) Touch Elements Combination Example: Slider Calculate, position: 69
I (4041) Touch Elements Combination Example: Slider Calculate, position: 70
I (4051) Touch Elements Combination Example: Slider Calculate, position: 72
I (4061) Touch Elements Combination Example: Slider Calculate, position: 73
I (4071) Touch Elements Combination Example: Slider Calculate, position: 75
I (4081) Touch Elements Combination Example: Slider Calculate, position: 76
I (4091) Touch Elements Combination Example: Slider Calculate, position: 77
I (4101) Touch Elements Combination Example: Slider Calculate, position: 79
I (4111) Touch Elements Combination Example: Slider Calculate, position: 80
I (4121) Touch Elements Combination Example: Slider Calculate, position: 81
I (4131) Touch Elements Combination Example: Slider Calculate, position: 83
I (4141) Touch Elements Combination Example: Slider Calculate, position: 84
I (4151) Touch Elements Combination Example: Slider Calculate, position: 85
I (4161) Touch Elements Combination Example: Slider Calculate, position: 86
I (4171) Touch Elements Combination Example: Slider Calculate, position: 88
I (4181) Touch Elements Combination Example: Slider Calculate, position: 89
I (4191) Touch Elements Combination Example: Slider Calculate, position: 90
I (4201) Touch Elements Combination Example: Slider Calculate, position: 91
I (4211) Touch Elements Combination Example: Slider Calculate, position: 92
I (4221) Touch Elements Combination Example: Slider Calculate, position: 93
I (4231) Touch Elements Combination Example: Slider Calculate, position: 94
I (4241) Touch Elements Combination Example: Slider Calculate, position: 95
I (4251) Touch Elements Combination Example: Slider Calculate, position: 96
I (4261) Touch Elements Combination Example: Slider Calculate, position: 96
I (4271) Touch Elements Combination Example: Slider Calculate, position: 97
I (4281) Touch Elements Combination Example: Slider Calculate, position: 98
I (4291) Touch Elements Combination Example: Slider Calculate, position: 99
I (4301) Touch Elements Combination Example: Slider Calculate, position: 99
I (4311) Touch Elements Combination Example: Slider Calculate, position: 100
I (4321) Touch Elements Combination Example: Slider Calculate, position: 100
I (4331) Touch Elements Combination Example: Slider Calculate, position: 100
I (4341) Touch Elements Combination Example: Slider Calculate, position: 101
I (4351) Touch Elements Combination Example: Slider Release, position: 101
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
