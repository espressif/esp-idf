| Supported Targets | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- |

# LP Core Debugging Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates how to debug application running on the LP core.

## How to use example

### Hardware Required

To run this example, you should have an ESP32-C6 based development board.

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Debugging Session

1) Run OpenOCD `openocd -f board/esp32c6-lpcore-builtin.cfg`.
2) Run GDB `riscv32-esp-elf-gdb -x gdbinit build/lp_debugging_example.elf`
3) `gdbinit` file will tell GDB to load debug info and symbols and set a number of breakpoints.
4) Type `c` upon hitting every breakpoint.
5) Finally LP core application should stop in `abort()`.

### LP Core Debugging Specifics

1) Add `-O0` compile option for ULP app in its CMakeLists.txt.
```
target_compile_options(${ULP_APP_NAME} PRIVATE -O0)
```
2) LP core supports limited set of HW exceptions, so, for example, writing at address `0x0` will not cause a panic as it would be for the code running on HP core. This can be overcome to some extent by enabling undefined behavior sanitizer for LP core application, so `ubsan` can help to catch some errors. But note that it will increase code size significantly and it can happen that application won't fit into RTC RAM. To enable `ubsan` for ULP app add `-fsanitize=undefined -fno-sanitize=shift-base` compile option to its CMakeLists.txt.
```
target_compile_options(${ULP_APP_NAME} PRIVATE -fsanitize=undefined -fno-sanitize=shift-base)
```
3) To be able to debug program running on LP core debug info and symbols need to be loaded to GDB. So there is special GDB command in `gdbinit`:
```
add-symbol build/esp-idf/main/ulp_debug_example/ulp_debug_example.elf
```
4) Upon startup LP core application is loaded into RAM, so all SW breakpoints set before that moment will get overwritten. The best moment to set breakpoints for LP core application is to do this when LP core program reaches `main` function. So `gdbinit` file used in this example sets temporary HW breakpoint on `main` and then set a bunch of other breakpoints when it hit.
```
thb main
commands
b main
b do_crash
b do_things
b ulp_lp_core_delay_us
c
end
```

## Limitations

1) Currently debugging is not supported when either HP or LP core enters any sleep mode. So this limits debugging scenarios.
2) FreeRTOS support in OpenOCD is disabled when debugging LP core, so you won't be able to see tasks running in the system. Instead there will be two threads representing HP ('esp32c6.cpu0') and LP ('esp32c6.cpu1') cores:
```
(gdb) info thread
  Id   Target Id                                                          Frame
  1    Thread 1 "esp32c6.cpu0" (Name: esp32c6.cpu0, state: debug-request) 0x40803772 in esp_cpu_wait_for_intr ()
    at /home/user/projects/esp/esp-idf/components/esp_hw_support/cpu.c:64
* 2    Thread 2 "esp32c6.cpu1" (Name: esp32c6.cpu1, state: breakpoint)    do_things (max=1000000000)
    at /home/user/projects/esp/esp-idf/examples/system/ulp/lp_core/debugging/main/lp_core/main.c:21
```
3) When setting HW breakpoint in GDB it is set on both cores, so the number of available HW breakpoints is limited to the number of them supported by LP core (2 for ESP32-C6).
4) OpenOCD flash support is disabled. It does not matter for LP core application because it is run completely from RAM and GDB can use SW breakpoints for it. But if you want to set a breakpoint on function from flash used by the code running on HP core (e.g. `app_main`) you should request to set HW breakpoint explicitly via `hb`/`thb` GDB commands.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
