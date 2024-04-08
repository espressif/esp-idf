| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Application Level Tracing Example (Basic)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the [Application Level Tracing Library](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#) (henceforth referred to as **App Trace**) to log messages to a host via JTAG instead of the normal method of logging via UART.

UART logs are time consuming and can significantly slow down the function that calls it. Therefore, it is generally a bad idea to use UART logs in time-critical functions. Logging to host via JTAG is significantly faster and can be used in time-critical functions. For more details regarding logging to host via JTAG, refer to the [Logging to Host Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#app-trace-logging-to-host).

### Hardware Required

To run this example, you need a supported target dev board connected to a JTAG adapter, which can come in the following forms:

* [ESP-WROVER-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v4-1) which integrates an on-board JTAG adapter. Ensure that the [required jumpers to enable JTAG are connected](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/get-started-wrover-kit.html#setup-options) on the WROVER-KIT.
* ESP32, ESP32-S2 or ESP32-C2 core board (e.g. ESP32-DevKitC, [ESP32-S2-Saola-1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/hw-reference/esp32s2/user-guide-saola-1-v1.2.html)) can also work as long as you connect it to an external JTAG adapter (e.g. FT2232H, J-LINK).

This example will assume that an ESP-WROVER-KIT is used.

#### Connections:

1. Connect the JTAG interface to the target board. For details about how to set up JTAG interface, please see [JTAG Debugging](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html). Power up both the JTAG debugger and target board.

2. After connecting JTAG interface, you need to [Run OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd).

3. Open a separate terminal window and run telnet by entering the command below. The telnet terminal window is used to feed commands to OpenOCD:

    ```bash
    telnet localhost 4444
    ```

### Configure the project

```
idf.py menuconfig
```

* To enable application tracing, select the `(X) Trace memory` option under `Component config > Application Level Tracing`. This option should have been selected by default.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

**Start App Trace:** In the telnet session window, trigger OpenOCD to start App Trace on the target by entering the command below. This command will collect 9000 bytes of JTAG log data and save them to the file `file://apptrace.log` (note `file://` depends on
where OpenOCD was started). Assuming that OpenOCD was started in this example's directory, `apptrace.log` will be saved here as well.


```bash
esp apptrace start file://apptrace.log 0 2000 3 0 0
```

**Note:** For more details on OpenOCD commands regarding App Trace, refer to the [OpenOCD Application Level Tracing Commands](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#openocd-application-level-tracing-commands)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

### Unable to flash when OpenOCD is connected to the target

On ESP32 boards, one likely cause would be an incorrect SPI flash voltage when starting OpenOCD. Suppose a target board/module with a 3.3 V powered SPI flash is being used, but the configuration file (ex. `board/esp32-wrover.cfg` for ESP32) is selected when starting OpenOCD which can set the SPI flash voltage to 1.8 V. In this situation, the SPI flash will not work after OpenOCD connects to the target as OpenOCD has changed the SPI flash voltage. Therefore, you might not be able to flash to the target when OpenOCD is connected.

To work around this issue, users are suggested to use `board/esp32-wrover.cfg` for ESP32 boards/modules operating with an SPI flash voltage of 1.8 V, and `board/esp-wroom-32.cfg` for 3.3 V. Refer to [ESP32 Modules and Boards](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html) and [Set SPI Flash Voltage](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/tips-and-quirks.html#why-to-set-spi-flash-voltage-in-openocd-configuration) for more details.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
