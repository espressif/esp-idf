| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# TWAI Alert and Recovery Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the alert and bus recovery features of the TWAI driver. The alert feature allows the TWAI driver to notify the application layer of certain TWAI driver or bus events. The bus recovery feature is used to recover the TWAI driver after it has entered the Bus-Off state. See the TWAI driver reference for more details.

## How to use example

### Hardware Required

This example requires only a single target (e.g., an ESP32 or ESP32-S2). The target must be connected to an external transceiver (e.g., a SN65HVD23X transceiver). This connection usually consists of a TX and an RX signal.

Note: If you don't have an external transceiver, this example can still be run by simply connecting the TX GPIO and RX GPIO with a jumper.

### Configure the project

* Set the target of the build (where `{IDF_TARGET}` stands for the target chip such as `esp32`, `esp32s2`, `esp32s2` or `esp32c3`).
* Then run `menuconfig` to configure the example.

```
idf.py set-target {IDF_TARGET}
idf.py menuconfig
```

* Under `Example Configuration`, configure the pin assignments using the options `TX GPIO Number` and `RX GPIO Number` according to how the target was connected to the transceiver. By default, `TX GPIO Number` and `RX GPIO Number` are set to the following values:
    * On the ESP32, `TX GPIO Number` and `RX GPIO Number` default to `21` and `22` respectively
    * On the ESP32-S2, `TX GPIO Number` and `RX GPIO Number` default to `20` and `21` respectively
    * On the ESP32-S3, `TX GPIO Number` and `RX GPIO Number` default to `4` and `5` respectively
    * On the ESP32-C3, `TX GPIO Number` and `RX GPIO Number` default to `2` and `3` respectively

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
I (330) TWAI Alert and Recovery: Driver installed
I (340) TWAI Alert and Recovery: Driver started
I (340) TWAI Alert and Recovery: Starting transmissions
W (350) TWAI Alert and Recovery: Trigger TX errors in 3
W (1350) TWAI Alert and Recovery: Trigger TX errors in 2
W (2350) TWAI Alert and Recovery: Trigger TX errors in 1
I (3350) TWAI Alert and Recovery: Trigger errors
I (3650) TWAI Alert and Recovery: Surpassed Error Warning Limit
I (3650) TWAI Alert and Recovery: Entered Error Passive state
I (4300) TWAI Alert and Recovery: Bus Off state
W (4300) TWAI Alert and Recovery: Initiate bus recovery in 3
W (5300) TWAI Alert and Recovery: Initiate bus recovery in 2
W (6300) TWAI Alert and Recovery: Initiate bus recovery in 1
I (7300) TWAI Alert and Recovery: Initiate bus recovery
I (7350) TWAI Alert and Recovery: Bus Recovered
I (7350) TWAI Alert and Recovery: Driver uninstalled
```

## Troubleshooting

```
I (3350) TWAI Alert and Recovery: Trigger errors
```

If the example does not progress pass triggering errors, check that the target is correctly connected to the transceiver.

```
I (3350) TWAI Alert and Recovery: Trigger errors
I (3650) TWAI Alert and Recovery: Surpassed Error Warning Limit
I (3650) TWAI Alert and Recovery: Entered Error Passive state
```

If the example is able to trigger errors but does not enter the bus off state (i.e., stays in the error passive state), check that the triggering of the bit error is properly set to the examples operating bit rate. By default, the example runs at a bit rate of 125kbits/sec, and the bit error should be triggered after the arbitration phase of each transmitted message.

## Example Breakdown

The TWAI Alert and Recovery Example will do the following...

1. Initialize the TWAI driver in No Acknowledgement mode (so that another node is not required).
2. Create a transmit task to handle message transmission, and a control task to handle alerts.
3. Control task starts the TWAI driver, then reconfigures the alerts to trigger when the error passive or bus off state is entered. The control task then waits for those alerts.
4. The transmit repeatedly transmits single shot messages (i.e., message won't be retried if an error occurs).
5. When a message is being transmitted, the transmit task will purposely invert the TX pin to trigger a bit error. **Note that the triggering of the bit error is timed to occur after the arbitration phase of the transmitted message**.
6. The triggering of a bit error on each transmitted message eventually puts the TWAI driver into the Bus-Off state.
7. Control tasks detects the Bus-Off state via an alert, and triggers the Bus-Off recovery process after a short delay. Alerts are also reconfigured to trigger on the completion of Bus-Off recovery.
8. Once the Bus-Off recovery completion alert is detected by the control task, the TWAI driver is stopped and uninstalled.
