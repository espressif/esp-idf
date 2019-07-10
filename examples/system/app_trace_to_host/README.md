# Example: Application Level Tracing - Logging to Host (app_trace_to_host)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example will show how to perform high speed logging by redirecting the log messages to a host instead of UART.

For more description of [logging to host](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#logging-to-host) please refer to **ESP-IDF Programming Guide**, section **Application Level Tracing library**. The following example provides practical implementation of this functionality.


### Use Case

Debugging of time critical functions may not work as desired if log messages are sent through the UART port. Printing out the logs may considerably slow down tested function to the point where it will not operate as expected.

Let's consider a case we are testing implementation of [zero level crossing](https://en.wikipedia.org/wiki/Zero_crossing) detection for a 50 Hz signal with ESP32's ADC. 

We will start by checking if we can read ADC, what is the signal level and how many samples can be collected over 20 ms period by using a code snippet below:

```c
int sampling_period = 20;
int i = 0;
uint32_t sampling_start =  esp_log_timestamp();  //this clock counts miliseconds
do {
    ESP_LOGI(TAG, "Sample:%d, Value:%d", ++i, adc1_get_raw(ADC1_TEST_CHANNEL));
} while (esp_log_timestamp() - sampling_start < sampling_period);
```

Above code is sampling ADC in a loop continuously for 20 ms of time to collect one full period of 50 Hz signal (use 17 ms for 60 Hz signal), and printing out results. If you run this code, result will be likely as below:

```bash
I (4309) example: Sample:1, Value:2224
I (4309) example: Sample:2, Value:840
I (4309) example: Sample:3, Value:3503
I (4319) example: Sample:4, Value:27
I (4319) example: Sample:5, Value:4095
```

As you see we were able to collect only five samples. This seems rather not adequate for zero crossing detection. 

We can remove `ESP_LOGI()` line and sample much faster, but then will not be able to see the values. To see the values we would need to save them in the memory and print out later.

Instead of saving samples to memory, a simple and compelling solution to this issue is sending logs to a host over a JTAG interface. This works much faster than UART. To do so, we need to redirect log messages to JTAG by calling the following function before `ESP_LOGx` line:

```c
esp_log_set_vprintf(esp_apptrace_vprintf);
```

Once time critical messages are sent out, we can redirect `ESP_LOGx` back back to the UART by adding extra two lines of code. 

```c
esp_log_set_vprintf(vprintf);
esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, 100000);
```

Note command `esp_apptrace_flush()` used to empty buffer with pending messages to the host.

If checking the log received by the host, we will see over 400 samples collected!

```bash
...
I (59379) example: Sample:424, Value:298
I (59379) example: Sample:425, Value:345
I (59379) example: Sample:426, Value:386
I (59379) example: Sample:427, Value:432
I (61409) example: Sample:1, Value:2653
```

## How to use example

### Hardware Required

To run this example, you need an ESP32 dev board (e.g. [ESP-WROVER-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v4-1)) which integrates an on-board JTAG adapter. ESP32 core board (e.g. ESP32-DevKitC) can also work as long as you connect it to an external JTAG adapter (e.g. FT2232H, J-LINK). For convenience, here we will take the [ESP-WROVER-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v4-1) for example.

#### Pin Assignment:

**Note:** Connect 50 / 60 Hz sinusoidal signal to ADC1_CHANNEL_6 / GPIO34. The signal range should be from 0V to max 3.1V. Optionally you can bridge GPIO34 with DAC_CHANNEL_1 / GPIO25.

| DAC Output        | ADC Input         |
| ----------------- | ----------------- |
| Channel1 (GPIO25) | Channel6 (GPIO34) |

#### Extra Connections:

1. Connect JTAG interface to ESP32 board, power up both JTAG and ESP32. For details about how to set up JTAG interface, please see [JTAG Debugging](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html).

2. After connecting JTAG interface, you need to [Run OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd). If you are using the [Binary Distribution of OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#jtag-debugging-setup-openocd) and one of versions of [ESP-WROVER-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v4-1), respective command line will look as follows:

    ```bash
    cd ~/esp/openocd-esp32
    bin/openocd -s share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg
    ```

3. Open a separate terminal window and run telnet by entering:

    ```bash
    telnet localhost 4444
    ```

### Configure the project

Open the project configuration menu (`idf.py menuconfig`). Then go into `Example Configuration` menu.

- By default, the DAC will generate 130 Hz signal within 0V..3.1V. To get 50Hz, you need to set non-standard driver of RTC 8MHz clock to lower minimum CW (Cosine Waveform) generator's frequency in `Set custom RTC 8 MHz clock divider to lower CW frequency`.

**Note** To enable application tracing, go to `Component config > Application Level Tracing` and select `(X) Trace memory`. (By default, this option has been switched on in the sdkconfig.defaults).

### Build, Flash and Run

1. Run `idf.py -p PORT flash monitor` to build and flash the project.. (To exit the serial monitor, type ``Ctrl-]``.)
See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

2. In the telnet session window, run the following command. This command will collect 9000 bytes of log data and save them to `adc.log` file in `~/esp/openocd-esp32` folder.

    ```bash
    esp32 apptrace start file://adc.log 0 9000 5 0 0
    ```

3. Decode and print out retrieved log file by executing:

    ```bash
    $IDF_PATH/tools/esp_app_trace/logtrace_proc.py ~/esp/openocd-esp32/adc.log ~/esp/app_trace_to_host/build/app_trace_to_host_test.elf
    ```

    This should provide a similar output:

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

This is the log we have been looking for, complete with timestamps as if printed to UART, but almost two orders of magnitude faster.

## Example Output

Check the full example code [app_trace_to_host](main/app_trace_to_host_test.c) that combines both tests above and runs them in a loop showing instantly the number of samples collected:

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

**note** Above result is generated under CPU running at 240 MHz.

## Conclusion

With this example code we have demonstrated powerful functionality of logging to host with JTAG interface. With standard UART communication baud rate setting of 115200, printing out a single line of a log message takes about 4 ms. This is also the maximum period we can sequentially execute other tasks in between. By providing the same logging over JTAG, we will be able to improve performance of this process over 80 times.

## Troubleshooting
1. I can not flash new firmware when OpenOCD is connected to ESP32.
   * One likely cause would be that you set wrong SPI flash voltage when you start OpenOCD. Suppose you're working with an ESP32 board / module which has a 3.3V powered SPI flash, but you select 
`board/esp32-wrover.cfg` configuration file when start OpenOCD. In this situation, you might not be able to flash ESP32 when OpenOCD is connected. So make sure what the working voltage of the SPI flash is. Currently, for 1.8V flash, we'd like to suggest using `board/esp32-wrover.cfg` and for 3.3V flash, using `board/esp-wroom-32.cfg`. For more information about it, please refer to [ESP32 Modules and Boards](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html) and [Set SPI Flash Voltage](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/tips-and-quirks.html#why-to-set-spi-flash-voltage-in-openocd-configuration).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
