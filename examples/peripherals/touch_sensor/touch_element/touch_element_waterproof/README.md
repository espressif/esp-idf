| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# Touch Element waterproof Example

This example demonstrates how to use the Touch Element library of capacitive Touch Sensor and setup the touch elements with touch element waterproof protection.

## How to use example

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

This example's output maybe could not give a strong feeling to user since the waterproof function works
automatically and silently inside the Touch Element library

```
I (331) Touch Element Waterproof Example: Touch Element library install
I (331) Touch Element Waterproof Example: Touch Element waterproof install
I (341) Touch Element Waterproof Example: Touch button install
I (351) Touch Element Waterproof Example: Touch buttons create
I (3191) Touch Element Waterproof Example: Button[7] Press
I (4191) Touch Element Waterproof Example: Button[7] LongPress
I (5191) Touch Element Waterproof Example: Button[7] LongPress
I (5671) Touch Element Waterproof Example: Button[7] Release
I (12561) Touch Element Waterproof Example: Button[9] Press
I (12811) Touch Element Waterproof Example: Button[9] Release
```

See the README.md file in the upper level 'examples' directory for more information about examples.
