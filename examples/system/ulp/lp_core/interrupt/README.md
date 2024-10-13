| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |

# LP-Core example with interrupt triggered from HP-Core:

This example demonstrates how to program the ULP coprocessor to receive an interrupt triggered by the HP-Core

ULP program written in C can be found across `lp_core/main.c`. The build system compiles and links this program, converts it into binary format, and embeds it into the .rodata section of the ESP-IDF application.

At runtime, the application running inside the main CPU loads ULP program into the `RTC_SLOW_MEM` memory region using `ulp_lp_core_load_binary` function. The main code then configures the ULP and starts the coprocessor by using `ulp_lp_core_run`. Once the ULP program is started, it runs continuously, waiting for interrupts. The main program will periodically trigger interrupts on the LP-Core.

After triggering a certain amount of interrupts, the main core will read and print the number of interrupts received as reported by the LP-Core.

## Example output

```
LP core loaded with firmware and running successfully
Triggered 10 interrupts on the LP-Core, LP-Core received 10 interrupts
```