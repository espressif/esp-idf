| Supported Targets | ESP32-C5 | ESP32-P4 |
| ----------------- | -------- | -------- |

# BitScrambler Loopback Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to load and control the BitScrambler. Specifically, the loopback mode of the BitScrambler is used to transform a buffer of data into a different format.

## How to Use Example

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
I (305) bs_example: BitScrambler example main
BitScrambler program complete. Input 40, output 40 bytes:
FF 00 00 00 00 00 00 00
80 80 80 80 80 80 80 80
01 02 04 08 10 20 40 80
00 FF 00 FF 00 FF 00 FF
FF 00 FF 00 FF 00 FF 00
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
