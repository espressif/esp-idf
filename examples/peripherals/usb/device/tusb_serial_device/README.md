| Supported Targets | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- |

# TinyUSB Serial Device Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up an ESP chip as a USB CDC ACM serial device. The device logs received data and echoes it back to the host.

The example uses the `esp_tinyusb` component. On ESP32-P4 and ESP32-S31, it also demonstrates the component's power-management integration: the chip can enter automatic light sleep while the USB bus is suspended and wake when USB activity resumes.

## How to Use the Example

This example can also be configured as a dual serial device. Run `idf.py menuconfig` and set the number of serial devices under `Component config → TinyUSB Stack → Communication Device Class (CDC) → CDC Channel Count`.

### USB Wakeup from Light Sleep

For ESP32-P4 and ESP32-S31, `sdkconfig.defaults.esp32p4` and `sdkconfig.defaults.esp32s31` enable:

- `CONFIG_PM_ENABLE` and `CONFIG_FREERTOS_USE_TICKLESS_IDLE` for automatic light sleep.
- `CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP` and `CONFIG_ESP_SLEEP_EVENT_CALLBACKS` for peripheral power-down and USB PHY restoration.
- `CONFIG_TINYUSB_USB_OTG_WAKEUP` and `CONFIG_TINYUSB_PM` for USB wakeup and the esp_tinyusb PM lock.

With these options, the component:

- Registers the USB peripheral as a light-sleep wakeup source.
- Prepares and restores the USB PHY when the system enters and exits light sleep.
- Holds an `ESP_PM_NO_LIGHT_SLEEP` lock while the USB bus is active.
- Releases the lock when the host suspends the USB device, allowing ESP-IDF to enter automatic light sleep.
- Reacquires the lock after a USB wakeup before normal USB processing resumes.

The application enables automatic light sleep with `esp_pm_configure()`, keeps `ESP_PD_DOMAIN_CNNT` powered during light sleep, and enables the esp_tinyusb PM lock. It reports suspend and resume through `TINYUSB_EVENT_SUSPENDED` and `TINYUSB_EVENT_RESUMED`.

The USB light-sleep wakeup source requires a USB-OTG high-speed port. It is not available on full-speed-only ports or through the USB Serial/JTAG peripheral. `TINYUSB_DEFAULT_CONFIG()` selects the high-speed port on supported targets.

### Hardware Required

Any ESP board with USB-OTG support. To test USB wakeup on ESP32-P4 or ESP32-S31, connect the USB-OTG high-speed port to the host and use a separate UART connection for the ESP-IDF monitor.

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

After flashing, you should see output similar to the following for a single CDC channel:

```
I (...) example: USB initialization
I (...) TinyUSB: TinyUSB Driver installed on port ...
I (...) example: USB initialization DONE
```

Connect to the serial port (e.g. on Linux, it should be `/dev/ttyACM0`) by any terminal application (e.g. `picocom /dev/ttyACM0`).
Now you can send data strings to the device, the device will echo back the same data string.

The monitor tool will also print the communication process:

```
I (...) example: Data from channel 0:
I (...) example: 0x3ffbfea0   45 73 70 72 65 73 73 69  66 0d                    |Espressif.|
```

On ESP32-P4 and ESP32-S31, suspending and resuming the USB bus also produces:

```
I (...) example: USB suspended with remote wakeup enabled; automatic light sleep is now allowed
I (...) example: USB resumed
```

The remote-wakeup state depends on whether the host enabled this USB feature, so the message may report it as disabled.

To trigger USB suspend from the host, disable or suspend the CDC ACM device on the host side. On Windows, open Device Manager, find the COM port for the device, and disable it. On Linux, you can unbind the CDC ACM driver for the device or put the USB device into autosuspend. On macOS, disconnecting the terminal application and letting the host suspend the interface can also trigger suspend depending on the host power policy.

Automatic light sleep can run multiple short sleep cycles while USB remains suspended. The application reports the USB state transition instead of logging every light-sleep entry and exit.
