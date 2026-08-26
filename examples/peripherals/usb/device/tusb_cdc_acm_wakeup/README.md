| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# TinyUSB CDC ACM Wakeup Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up an ESP chip as a USB CDC ACM device that enters light sleep when the USB bus is suspended and wakes up from USB activity.
The wakeup source is available only on USB-OTG capable targets with High-Speed USB support. It is not supported by USB Serial/JTAG-only peripherals.

As a USB stack, a TinyUSB component is used.

## How to use example

The example implements a USB CDC ACM echo device. When the host suspends the USB bus, the `tud_suspend_cb` callback configures the USB peripheral suspend state and enters light sleep. USB bus activity from the host wakes the chip and the CDC ACM echo device continues running.

### Hardware Required

Any ESP board that supports High-Speed USB-OTG wakeup from light sleep.

#### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see this output:

```
I (285) tusb_cdc_acm_wakeup: USB initialization
I (455) TinyUSB: TinyUSB Driver installed
I (465) tusb_cdc_acm_wakeup: USB initialization DONE
```

Connect to the serial port (e.g. on Linux, it should be `/dev/ttyACM0`) by any terminal application (e.g. `picocom /dev/ttyACM0`).
Now you can send data strings to the device, the device will echo back the same data string.

To trigger USB suspend from the host, disable or suspend the CDC ACM device on the host side. On Windows, open Device Manager, find the COM port for the device, and disable it. On Linux, you can unbind the CDC ACM driver for the device or put the USB device into autosuspend. On macOS, disconnecting the terminal application and letting the host suspend the interface can also trigger suspend depending on the host power policy.

When the USB host suspends and resumes the bus, the monitor output should include:

```
I (122142) tusb_cdc_acm_wakeup: 0x4ff3afa4   48 65 6c 6c 6f                                    |Hello|
I (122272) tusb_cdc_acm_wakeup: Data from channel 0:
I (122272) tusb_cdc_acm_wakeup: 0x4ff3afa4   48 65 6c 6c 6f                                    |Hello|
I (122402) tusb_cdc_acm_wakeup: Data from channel 0:
I (122402) tusb_cdc_acm_wakeup: 0x4ff3afa4   48 65 6c 6c 6f                                    |Hello|
I (122622) tusb_cdc_acm_wakeup: Data from channel 0:
I (122622) tusb_cdc_acm_wakeup: 0x4ff3afa4   48 65 6c 6c 6f                                    |Hello|
I (127432) tusb_cdc_acm_wakeup: Line state changed on channel 0: DTR:0, RTS:0
I (132402) tusb_cdc_acm_wakeup: USB suspended, entering light sleep
I (132422) tusb_cdc_acm_wakeup: Woke up from: USB
I (132422) tusb_cdc_acm_wakeup: USB resumed
I (145762) tusb_cdc_acm_wakeup: Line state changed on channel 0: DTR:0, RTS:0
I (145812) tusb_cdc_acm_wakeup: Line state changed on channel 0: DTR:0, RTS:0
I (145812) tusb_cdc_acm_wakeup: Line state changed on channel 0: DTR:0, RTS:0
I (145822) tusb_cdc_acm_wakeup: Line state changed on channel 0: DTR:0, RTS:0
I (145822) tusb_cdc_acm_wakeup: Line state changed on channel 0: DTR:0, RTS:0
I (145832) tusb_cdc_acm_wakeup: Line state changed on channel 0: DTR:0, RTS:0
I (148762) tusb_cdc_acm_wakeup: Data from channel 0:
I (148762) tusb_cdc_acm_wakeup: 0x4ff3afa4   48 65 6c 6c 6f                                    |Hello|
I (148902) tusb_cdc_acm_wakeup: Data from channel 0:
I (148902) tusb_cdc_acm_wakeup: 0x4ff3afa4   48 65 6c 6c 6f                                    |Hello|
I (149032) tusb_cdc_acm_wakeup: Data from channel 0:
I (149032) tusb_cdc_acm_wakeup: 0x4ff3afa4   48 65 6c 6c 6f                                    |Hello|
I (149252) tusb_cdc_acm_wakeup: Data from channel 0:
I (149252) tusb_cdc_acm_wakeup: 0x4ff3afa4   48 65 6c 6c 6f                                    |Hello|
I (151382) tusb_cdc_acm_wakeup: Line state changed on channel 0: DTR:0, RTS:0
I (155312) tusb_cdc_acm_wakeup: USB suspended, entering light sleep
```
