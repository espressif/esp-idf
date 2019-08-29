# Application Level Tracing Example (Logging to Host)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the [Application Level Tracing Library](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#) (henceforth referred to as **App Trace**) to log messages to a host via JTAG instead of the normal method of logging via UART.

UART logs are time consuming and can significantly slow down the function that calls it. Therefore, it is generally a bad idea to use UART logs in time-critical functions. Logging to host via JTAG is significantly faster and can be used in time-critical functions. For more details regarding logging to host via JTAG, refer to the [Logging to Host Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#app-trace-logging-to-host).

This example demonstrates JTAG logging to host in the context of polling for a [zero crossing](https://en.wikipedia.org/wiki/Zero_crossing). The ESP32 will continuously sample a 50 to 60 Hz sinusoidal signal (using the ADC) and log the sampled values (via JTAG). Due to the higher speed of JTAG logging, the polling rate of the ADC should be high enough to detect a zero crossing.

This example utilizes the following ESP-IDF features:
* [DAC driver](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/dac.html) to generate the 50 Hz sinusoidal signal.
* [ADC driver](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/adc.html) to sample the sinusoidal signal.
* [Application Level Tracing Library](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#) to log ADC samples to host.
* [OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#setup-of-openocd) to interface with the ESP32 and receive the log output over JTAG.

## How to use example

### Hardware Required

To run this example, you need an ESP32 dev board connected to a JTAG adapter, which can come in the following forms:

* [ESP-WROVER-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v4-1) which integrates an on-board JTAG adapter. Ensure that the [required jumpers to enable JTAG are connected](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/get-started-wrover-kit.html#setup-options) on the WROVER-KIT.
* ESP32 core board (e.g. ESP32-DevKitC) can also work as long as you connect it to an external JTAG adapter (e.g. FT2232H, J-LINK).

This example will assume that that an ESP-WROVER-KIT is used.

#### Pin Assignment:

The sinusoidal signal of 50 to 60 Hz ranging from 0 V ~ 3.1 V should be input into `GPIO34` (`ADC1_CHANNEL_6`). Users may provide this signal themselves, our use the DAC generated signal by bridging GPIO34 with `GPIO25` (`DAC_CHANNEL_1`).

| DAC Output         | ADC Input          |
| ------------------ | ------------------ |
| Channel 1 (GPIO25) | Channel 6 (GPIO34) |

#### Extra Connections:

1. Connect the JTAG interface to ESP32 board, and power up both the JTAG and ESP32. For details about how to set up JTAG interface, please see [JTAG Debugging](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html).

2. After connecting JTAG interface, you need to [Run OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd).

3. Open a separate terminal window and run telnet by entering the command below. The telnet terminal window is used to feed commands to OpenOCD:

    ```bash
    telnet localhost 4444
    ```

### Configure the project

```
idf.py menuconfig
```

* By default, the DAC will generate 130 Hz signal ranging from 0 V ~ 3.1 V. To generate a 50 Hz signal, the RTC 8 MHz clock will need to use a non-standard divider. This is achieved by enabling the `Example Configuration > Set custom RTC 8 MHz clock divider to lower CW frequency` configuration option.

* To enable application tracing, select the `(X) Trace memory` option under `Component config > Application Level Tracing`. This option should have been selected by default.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

**Start App Trace:** In the telnet session window, trigger OpenOCD to start App Trace on the ESP32 by entering the command below. This command will collect 9000 bytes of JTAG log data and save them to `adc.log` file in `~/esp/openocd-esp32` folder.

```bash
esp32 apptrace start file://adc.log 0 9000 5 0 0
```

**Note:** For more details on OpenOCD commands regarding App Trace, refer to the [OpenOCD Application Level Tracing Commands](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#openocd-application-level-tracing-commands)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example will continuously sample the ADC for 20ms per iteration, and will alternate between JTAG and UART logging per iteration. However, the JTAG logs should be captured by OpenOCD, thus will not appear in the monitor's output. Therefore, the monitor should only display the iterations where UART logging was used (i.e. every alternate iteration) such as the following:

```
I (4289) example: Sampling ADC and sending data to the host...
I (4309) example: Collected 427 samples in 20 ms.

I (4309) example: Sampling ADC and sending data to the UART...
I (4309) example: Sample:1, Value:2224
I (4309) example: Sample:2, Value:840
I (4309) example: Sample:3, Value:3503
I (4319) example: Sample:4, Value:27
I (4319) example: Sample:5, Value:4095
I (4329) example: Collected 5 samples in 20 ms.
```

**Note:** The UART log above was produced with the CPU running at 240 MHz.

To access the JTAG logs, the `adc.log` file should be decoded. This can be done by using the `logtrace_proc.py` script  as such:

```bash
$IDF_PATH/tools/esp_app_trace/logtrace_proc.py ~/esp/openocd-esp32/adc.log ~/esp/app_trace_to_host/build/app_trace_to_host_test.elf
```

The `logtrace_proc.py` script should produce the following output when decoding:

```
Parse trace file '/user-home/esp/openocd-esp32/adc.log'...
Unprocessed 7 bytes of log record args!
Parsing completed.
====================================================================
I (59369) example: Sample:1, Value:3717
I (59369) example: Sample:2, Value:3647
I (59369) example: Sample:3, Value:3575
I (59369) example: Sample:4, Value:3491
...

I (59379) example: Sample:398, Value:78
I (59379) example: Sample:399, Value:58
I (59379) example: Sample:400, Value:22
I (59379) example: Sample:401, Value:14
I (59379) example: Sample:402, Value:0
I (59379) example: Sample:403, Value:0
I (59379) example: Sample:404, Value:0
I (59379) example: Sample:405, Value:0
I (59379) example: Sample:406, Value:0
I (59379) example: Sample:407, Value:0
I (59379) example: Sample:408, Value:0
I (59379) example: Sample:409, Value:0
I (59379) example: Sample:410, Value:0
I (59379) example: Sample:411, Value:0
I (59379) example: Sample:412, Value:0
I (59379) example: Sample:413, Value:0
I (59379) example: Sample:414, Value:16
I (59379) example: Sample:415, Value:32
I (59379) example: Sample:416, Value:40
I (59379) example: Sample:417, Value:74
I (59379) example: Sample:418, Value:89
I (59379) example: Sample:419, Value:113
I (59379) example: Sample:420, Value:160
I (59379) example: Sample:421, Value:192
I (59379) example: Sample:422, Value:221
I (59379) example: Sample:423, Value:256
I (59379) example: Sample:424, Value:298
I (59379) example: Sample:425, Value:345
I (59379) example: Sample:426, Value:386
I (59379) example: Sample:427, Value:432
I (61409) example: Sample:1, Value:2653

====================================================================

Log records count: 428
```

## Troubleshooting

### Unable to flash when OpenOCD is connected to ESP32

One likely cause would be an incorrect SPI flash voltage when starting OpenOCD. Suppose an ESP32 board/module with a 3.3 V powered SPI flash is being used, but the `board/esp32-wrover.cfg` configuration file is selected when starting OpenOCD which can set the SPI flash voltage to 1.8 V. In this situation, the SPI flash will not work after OpenOCD connects to the ESP32 as OpenOCD has changed the SPI flash voltage. Therefore, you might not be able to flash ESP32 when OpenOCD is connected. 

To work around this issue, users are suggested to use `board/esp32-wrover.cfg` for ESP32 boards/modules operating with an SPI flash voltage of 1.8 V, and `board/esp-wroom-32.cfg` for 3.3 V. Refer to [ESP32 Modules and Boards](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html) and [Set SPI Flash Voltage](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/tips-and-quirks.html#why-to-set-spi-flash-voltage-in-openocd-configuration) for more details.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)

The log should be identical to those printed via UART (complete with timestamps), but almost two orders of magnitude faster.

## Example Breakdown

The following code snippet demonstrates a loop of the sampling and logging the ADC over a 20 ms period in order to capture one full period of a 50 Hz signal.

```c
int sampling_period = 20;
int i = 0;
uint32_t sampling_start =  esp_log_timestamp();  //this clock counts milliseconds
do {
    ESP_LOGI(TAG, "Sample:%d, Value:%d", ++i, adc1_get_raw(ADC1_TEST_CHANNEL));
} while (esp_log_timestamp() - sampling_start < sampling_period);
```

If `ESP_LOGI()` is routed via UART (occurs by default), the log output produced will likely resemble the output shown below. Notice that due to UART logging is time consuming, thus the ADC is only sampled five times, which is too infrequent to consistently detect a zero crossing (where the zero crossing is `4096/2 = 2048` i.e., the mid point of the 12-bit ADC).

```bash
I (4309) example: Sample:1, Value:2224
I (4309) example: Sample:2, Value:840
I (4309) example: Sample:3, Value:3503
I (4319) example: Sample:4, Value:27
I (4319) example: Sample:5, Value:4095
I (4329) example: Collected 5 samples in 20 ms.
```

However, by logging via JTAG, the logging is much quicker hence allows a much higher sampling frequency (over 400 times) as shown the the log output below thus would be able to detect a zero crossing more consistently.

```c
esp_log_set_vprintf(esp_apptrace_vprintf);
```

```bash
...

I (59379) example: Sample:423, Value:256
I (59379) example: Sample:424, Value:298
I (59379) example: Sample:425, Value:345
I (59379) example: Sample:426, Value:386
I (59379) example: Sample:427, Value:432
I (61409) example: Sample:1, Value:2653

====================================================================

Log records count: 428

```

This example has demonstrated powerful functionality of logging to host via JTAG interface. With standard UART communication at a baud rate of 115200, printing out a single line log message takes approximately 4 ms. This also means that logged tasks cannot run more frequently than every 4 ms. By providing the same logging over JTAG, logging performance is improved 80 fold.