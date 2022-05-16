| Supported Targets | ESP32 | ESP32-S2 | ESP32-C3 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |
# IR Protocol Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example illustrates how to encode and decode RMT signals with/to common IR protocols (e.g. NEC and RC5).

[NEC](https://www.sbprojects.net/knowledge/ir/nec.php) and [RC5](https://www.sbprojects.net/knowledge/ir/rc5.php) have different encoding rules, but both can be compatible to RMT data format.

The example supports building and parsing both normal and extended NEC/RC5 protocol. And also supports `repeat code` which would be sent out if one remote key got pressed for a specific long time.

## How to Use Example

### Hardware Required

* A development board with supported SoC mentioned in the above `Supported Targets` table
* An USB cable for power supply and programming
* A 5mm infrared LED (e.g. IR333C) used to transmit encoded IR signals
* An infrared receiver module (e.g. IRM-3638T), which integrates a demodulator and AGC circuit.

Example connection :

| ESP chip                    | IR333C | IRM-3638T |
| --------------------------- | ------ | --------- |
| CONFIG_EXAMPLE_RMT_TX_GPIO  | Tx     | ×         |
| CONFIG_EXAMPLE_RMT_RX_GPIO  | ×      | Rx        |
| VCC 5V                      | √      | ×         |
| VCC 3.3V                    | ×      | √         |
| GND                         | GND    | GND       |


### Configure the Project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Select the infrared protocol used in the example under `Infrared Protocol` option.
* Set the GPIO number used for transmitting the IR signal under `RMT TX GPIO` option.
* Set the GPIO number used for receiving the demodulated IR signal under `RMT RX GPIO` option.
* Set the RMT TX channel number under `RMT TX Channel Number` option.
* Set the RMT RX channel number under `RMT RX Channel Number` option.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Run this example, you will see the following output log (for NEC protocol):
```
I (2000) example: Send command 0x20 to address 0x10
I (2070) example: Scan Code  --- addr: 0x0010 cmd: 0x0020
I (2220) example: Scan Code (repeat) --- addr: 0x0010 cmd: 0x0020
I (4240) example: Send command 0x21 to address 0x10
I (4310) example: Scan Code  --- addr: 0x0010 cmd: 0x0021
I (4460) example: Scan Code (repeat) --- addr: 0x0010 cmd: 0x0021
I (6480) example: Send command 0x22 to address 0x10
I (6550) example: Scan Code  --- addr: 0x0010 cmd: 0x0022
I (6700) example: Scan Code (repeat) --- addr: 0x0010 cmd: 0x0022
I (8720) example: Send command 0x23 to address 0x10
I (8790) example: Scan Code  --- addr: 0x0010 cmd: 0x0023
I (8940) example: Scan Code (repeat) --- addr: 0x0010 cmd: 0x0023
I (10960) example: Send command 0x24 to address 0x10
I (11030) example: Scan Code  --- addr: 0x0010 cmd: 0x0024
I (11180) example: Scan Code (repeat) --- addr: 0x0010 cmd: 0x0024
```

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
