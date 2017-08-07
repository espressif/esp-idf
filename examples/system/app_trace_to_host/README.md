# Example: Application Level Tracing - Logging to Host (app_trace_to_host)

This test code shows how to perform high speed logging by redirecting the log messages to a host instead of UART.

For description of [logging to a host](http://esp-idf.readthedocs.io/en/latest/api-guides/app_trace.html#logging-to-host) please refer to **ESP32 Programming Guide**, section **Application Level Tracing library**. The following example provides practical implementation of this functionality.


## Use Case

Debugging of time critical functions may not work as desired if log messages are sent though the UART port. Printing out the logs may considerably slow down tested function to the point where it will not operate as expected.

Let's consider a case we are testing implementation of [zero level crossing](https://en.wikipedia.org/wiki/Zero_crossing) detection for a 50 Hz signal with ESP32's ADC. 

We will start by checking if we can read ADC, what is the signal level and how many samples can be collected over 20 ms period by using a code snippet below:

```
int sampling_period = 20;
int i = 0;
uint32_t sampling_start =  esp_log_timestamp();  //this clock counts miliseconds
do {
    ESP_LOGI(TAG, "Sample:%d, Value:%d", ++i, adc1_get_raw(ADC1_TEST_CHANNEL));
} while (esp_log_timestamp() - sampling_start < sampling_period);
```

Above code is sampling ADC in a loop continuously for 20 ms of time to collect one full period of 50 Hz signal (use 17 ms for 60 Hz signal), and printing out results. If you run this code, result will be likely as below:

```
I (4309) example: Sample:1, Value:2224
I (4309) example: Sample:2, Value:840
I (4309) example: Sample:3, Value:3503
I (4319) example: Sample:4, Value:27
I (4319) example: Sample:5, Value:4095
```

As you see we were able to collect only five samples. This seems rather not adequate for zero crossing detection. 

We can remove `ESP_LOGI()` line and sample much faster, but then will not be able to see the values. To see the values we would need to save them in the memory and print out later.

Instead of saving samples to memory, a simple and compelling solution to this issue is sending logs to a host over a JTAG interface. This works much faster than with the UART. To do so, we need to redirect log messages to JTAG by calling the following function before `ESP_LOGx` line:

```
esp_log_set_vprintf(esp_apptrace_vprintf);
```

Once time critical messages are sent out, we can redirect `ESP_LOGx` back back to the UART by adding extra two lines of code. 

```
esp_log_set_vprintf(vprintf);
esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, 100000);
```

Note command `esp_apptrace_flush()` used to empty buffer with pending messages to the host.

If checking the log received by the host, we will see over 400 samples collected!

```
...
I (59379) example: Sample:424, Value:298
I (59379) example: Sample:425, Value:345
I (59379) example: Sample:426, Value:386
I (59379) example: Sample:427, Value:432
I (61409) example: Sample:1, Value:2653
```


## Example in Action

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


## Do it Yourself

To run the example and retrieve the log from the host, do the following:

1.  Connect JTAG interface to ESP32 board, power up both JTAG and ESP32. For details how to setup JTAG interface see [JTAG Debugging](http://esp-idf.readthedocs.io/en/latest/api-guides/jtag-debugging/index.html). 

2.  [Run OpenOCD](http://esp-idf.readthedocs.io/en/latest/api-guides/jtag-debugging/index.html#run-openocd). If you are using the [binary distribution of OpenOCD](http://esp-idf.readthedocs.io/en/latest/api-guides/jtag-debugging/index.html#jtag-debugging-setup-openocd) and one of versions of [ESP-WROVER-KIT](http://esp-idf.readthedocs.io/en/latest/hw-reference/modules-and-boards.html#esp32-wrover-kit-v1-esp32-devkitj-v1), respective command line will look as follows:

    ```
    cd ~/esp/openocd-esp32
    bin/openocd -s share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg
    ```

3.  Compile and load the example. Note to enable application tracing in menuconfig by going to `Component config > Application Level Tracing` and selecting `(X) Trace memory` 

4.  Connect 50 Hz sinusoidal signal to ADC1_CHANNEL_6 / GPIO34. The signal range should be from 0V to max 3.1V. Optionally bridge GPIO34 with DAC_CHANNEL_1 / GPIO25, that generates 130 Hz signal within 0V..3.1V. To get 50 Hz, you need to set non standard divider of RTC 8 MHz clock to lower the minimum CW (Cosine Waveform) generator's frequency. You can do it in menuconfig by going to `Component config > Example configuration > Set custom RTC 8 MHz clock divider to lower CW frequency`.

5.  Open a separate terminal window and run telnet by entering:

    ```
    telnet localhost 4444
    ``` 

6.  In telnet execute the following command:

    ```
    esp32 apptrace start file://adc0.log file://adc1.log 0 9000 5 0 0
    ```

    This command should collect 9000 bytes of log data and save them to `adc0.log` file in `~/esp/openocd-esp32` folder. The `adc1.log` file will be empty / is not used.

7.  Decode and print out retrieved log file by executing:

    ```
    $IDF_PATH/tools/esp_app_trace/logtrace_proc.py ~/esp/openocd-esp32/adc0.log ~/esp/app_trace_to_host/build/app_trace_to_host_test.elf
    ```

    This should provide a similar output:

    ```
    Parse trace file '/user-home/esp/openocd-esp32/adc0.log'...
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


## Conclusion

With this example code we have demonstrated powerful functionality of logging to host with JTAG interface. With standard UART communication speed setting of 115200 BPS, printing out a single line of a log message takes about 4 ms. This is also the maximum period we can sequentially execute other tasks in between. By providing the same logging over JTAG, we were able improve performance of this process over 80 times.

