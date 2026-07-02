| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |
# ULP-LP-Core simple example with GPIO Interrupt:

This example demonstrates how to program the LP-Core coprocessor to wake up from a RTC IO interrupt, instead of waking periodically from the ULP timer.

ULP program written in C can be found across `ulp/main.c`. The build system compiles and links this program, converts it into binary format, and embeds it into the .rodata section of the ESP-IDF application.

At runtime, the main code running on the ESP (found in lp_core_gpio_wake_up_example_main.c) loads ULP program into the `RTC_SLOW_MEM` memory region using `ulp_lp_core_load_binary` function. The main code then configures the ULP GPIO wakeup source and starts the coprocessor by using `ulp_lp_core_run` followed by putting the chip into deep sleep mode.

When the wakeup source pin is pulled low the LP-Core coprocessor is woken up, sends a wakeup signal to the main CPU and goes back to sleep again.

In this example the input signal is connected to GPIO2. To change the pin number, check the Chip Pin List document and adjust `WAKEUP_PIN` variable in main.c.


## Example output

```
Not a LP-Core wakeup, initializing it!
Entering deep sleep

...

LP-Core woke up the main CPU!
Entering deep sleep
```
