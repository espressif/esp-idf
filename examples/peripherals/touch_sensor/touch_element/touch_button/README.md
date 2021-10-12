| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# Touch button example

This example demonstrates how to use the Touch Element library of capacitive touch sensor and set up touch button.

## How to use example

### Configure the project

* Set the target of the build (where `{IDF_TARGET}` stands for the target chip such as `esp32s2`).
* Run `menuconfig` to select a dispatch method for the example.

### Build and Flash

Build the project and flash it to the target board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (331) Touch Button Example: Touch element library installed
I (331) Touch Button Example: Touch button installed
I (341) Touch Button Example: Touch buttons created
I (341) Touch Button Example: Touch element library start
I (1481) Touch Button Example: Button[1] Press
I (1701) Touch Button Example: Button[1] Release
I (2731) Touch Button Example: Button[2] Press
I (2921) Touch Button Example: Button[2] Release
I (3581) Touch Button Example: Button[5] Press
I (3781) Touch Button Example: Button[5] Release
I (3931) Touch Button Example: Button[4] Press
I (4121) Touch Button Example: Button[4] Release
I (4271) Touch Button Example: Button[3] Press
I (4491) Touch Button Example: Button[3] Release
I (4671) Touch Button Example: Button[6] Press
I (4891) Touch Button Example: Button[6] Release
I (5091) Touch Button Example: Button[7] Press
I (5311) Touch Button Example: Button[7] Release
I (5491) Touch Button Example: Button[8] Press
I (5741) Touch Button Example: Button[8] Release
I (5991) Touch Button Example: Button[9] Press
I (7991) Touch Button Example: Button[9] LongPress
I (9991) Touch Button Example: Button[9] LongPress
I (11991) Touch Button Example: Button[9] LongPress
I (12881) Touch Button Example: Button[9] Release
```

See the README.md file in the upper level 'examples' directory for more information about examples.
