| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# ULP FSM Build System Example

This example demonstrates a separate ULP FSM project built with the CMake v2 ULP project helpers. It programs the ULP FSM coprocessor to increment a counter while the main CPUs are in deep sleep, then wake the main CPU after a fixed number of timer iterations.

ULP program written in assembly can be found across `main/ulp/main/counter.S` and `main/ulp/main/wake_up.S` (demonstrating multiple ULP source files). The build system assembles and links this program, converts it into binary format, and embeds it into the .rodata section of the ESP-IDF application.

At runtime, the main code running on the ESP (found in main.c) loads ULP program into the `RTC_SLOW_MEM` memory region using `ulp_load_binary` function. Main code configures the ULP program by setting up values of some variables and then starts it using `ulp_run`. Once the ULP program is started, it runs periodically, with the period set by the main program. The main program enables ULP wakeup source and puts the chip into deep sleep mode.

On each timer iteration, the ULP program increments `iteration_count`. Once this counter reaches `wakeup_iteration` (set by the main program), the ULP triggers wake up from deep sleep. Upon wakeup, the main program stops the ULP timer, prints the final counter value, and exits the example.

In this example, the `CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` Kconfig option is used, which allows you to reduce the boot time of the bootloader during waking up from deep sleep. The bootloader stores in rtc memory the address of a running partition and uses it when it wakes up. This example allows you to skip all image checks and speed up the boot.

## Example output

```
Not ULP wakeup, initializing ULP
Entering deep sleep

ULP FSM woke up the main CPU
ULP FSM iteration count: 5
Example finished
```
