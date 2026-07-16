| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# ULP RISC-V Build System Example

This example demonstrates a separate ULP RISC-V project built with the CMake v2 ULP project helpers. It programs the ULP RISC-V coprocessor to increment a counter while the main CPUs are in deep sleep, then wake the main CPU after a fixed number of timer iterations.

ULP program written in C can be found in `main/ulp/main/main.c`. The build system compiles and links this program, converts it into binary format, and embeds it into the .rodata section of the ESP-IDF application.

At runtime, the application running inside the main CPU loads ULP program into the `RTC_SLOW_MEM` memory region using `ulp_riscv_load_binary` function. The main code then configures the ULP wakeup period and starts the coprocessor by using `ulp_riscv_run`. Once the ULP program is started, it runs periodically, with the period set by the main program. The main program enables ULP wakeup source and puts the chip into deep sleep mode.

On each timer iteration, the ULP program increments `iteration_count`. Once this counter reaches `wakeup_iteration` (set by the main program), the ULP sends a wakeup signal to the main CPU. Upon wakeup, the main program stops the ULP timer, prints the final counter value, and exits the example.

## Example output

```
Not a ULP-RISC-V wakeup, initializing it!
Entering deep sleep

ULP-RISC-V woke up the main CPU!
ULP-RISC-V iteration count: 5
Example finished
```
