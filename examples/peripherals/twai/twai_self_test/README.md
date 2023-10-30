| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# TWAI Self Test Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The TWAI Self Test Example demonstrates how a node can transmit TWAI messages to itself using the TWAI driver's "No Acknowledgement" mode and Self Reception Requests. The Self Test Example can be run as a simple test to determine whether a target (ESP32, ESP32-S2, ESP32-S3 or ESP32-C3) is properly connected to a working external transceiver.

## How to use example

### Hardware Required

This example requires only a single target (e.g., an ESP32 or ESP32-S2). The target must be connected to an external transceiver (e.g., a SN65HVD23X transceiver). This connection usually consists of a TX and an RX signal.

Note: If you don't have an external transceiver, this example can still be run by simply connecting the TX GPIO and RX GPIO with a jumper.

### Configure the project

* Set the target of the build (where `{IDF_TARGET}` stands for the target chip such as `esp32` or `esp32s2`).
* Then run `menuconfig` to configure the example.

```sh
idf.py set-target {IDF_TARGET}
idf.py menuconfig
```

* Under `Example Configuration`, configure the pin assignments using the options `TX GPIO Number` and `RX GPIO Number` according to how the target was connected to the transceiver. By default, `TX GPIO Number` and `RX GPIO Number` are set to the following values:
  * On the ESP32, `TX GPIO Number` and `RX GPIO Number` default to `21` and `22` respectively
  * On other chips, `TX GPIO Number` and `RX GPIO Number` default to `0` and `2` respectively

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```sh
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
I (345) TWAI Self Test: Driver installed
I (345) TWAI Self Test: Driver started
I (355) TWAI Self Test: Msg received - Data = 0
...
I (1335) TWAI Self Test: Msg received - Data = 99
I (1335) TWAI Self Test: Driver stopped
I (1435) TWAI Self Test: Driver started
I (1435) TWAI Self Test: Msg received - Data = 0
...
I (2425) TWAI Self Test: Msg received - Data = 99
I (2425) TWAI Self Test: Driver stopped
I (2525) TWAI Self Test: Driver started
I (2525) TWAI Self Test: Msg received - Data = 0
...
I (3515) TWAI Self Test: Msg received - Data = 99
I (3515) TWAI Self Test: Driver stopped
I (3615) TWAI Self Test: Driver uninstalled
```

## Troubleshooting

```text
I (345) TWAI Self Test: Driver installed
I (345) TWAI Self Test: Driver started
```

If the TWAI driver is installed and started but no messages are received, check that the target is correctly connected to the external transceiver, and that the external transceiver is operating properly (i.e., properly powered and not in sleep mode).

## Example Breakdown

The TWAI Self Test Example will do multiple iterations of the following steps:

1. Install the TWAI driver
2. Start the TWAI driver
3. Simultaneously transmit and receive multiple messages using the self reception request.
4. Stop the TWAI driver
5. Repeat steps 2 to 4 for multiple iterations
6. Uninstall the TWAI driver
