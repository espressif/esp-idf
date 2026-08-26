| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# ULP FSM and RISC-V Combined Example

This example demonstrates how to use both ULP FSM and ULP RISC-V coprocessors sequentially in the same application.

## Overview

The example implements a counter that is incremented in three stages:

1. **ULP FSM Stage**: The FSM coprocessor increments the counter from 0 to 100
2. **ULP RISC-V Stage**: The RISC-V coprocessor continues incrementing from 100 to 500
3. **Main CPU Stage**: The main processor completes the count from 500 to 1500

This demonstrates:
- Enabling both `CONFIG_ULP_COPROC_TYPE_FSM` and `CONFIG_ULP_COPROC_TYPE_RISCV` for sequential use
- Using the `TYPE` parameter in `ulp_embed_binary()` to specify which toolchain to use for each ULP program
- Coordinating execution between FSM ULP, RISC-V ULP, and the main CPU
- Sharing data between different coprocessors and the main CPU

## How to Use Example

### Hardware Required

* ESP32-S2 or ESP32-S3 development board

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use)

(To exit the serial monitor, type ``Ctrl-]``)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
ULP FSM and RISC-V Combined Example
====================================

Step 1: Starting ULP FSM to count from 0 to 100...
HP core going to sleep, will be woken by ULP FSM when counting completes...
HP core woken up by: ULP
ULP FSM completed. Counter value: 100

Step 2: Starting ULP RISC-V to count from 100 to 500...
HP core going to sleep, will be woken by ULP RISC-V when counting completes...
HP core woken up by: ULP
ULP RISC-V completed. Counter value: 500

Step 3: Main CPU counting from 500 to 1500...
Main CPU completed. Final counter value: 1500

====================================
All stages completed successfully!
FSM: 0 -> 100
RISC-V: 100 -> 500
Main CPU: 500 -> 1500
```

## Troubleshooting

* If you see an error about missing ULP type when building, ensure both `CONFIG_ULP_COPROC_TYPE_FSM` and `CONFIG_ULP_COPROC_TYPE_RISCV` are enabled in menuconfig.

* The `TYPE` parameter in `ulp_embed_binary()` is mandatory when both ULP types are enabled. Check `main/CMakeLists.txt` to see the correct syntax:
  ```cmake
  ulp_embed_binary(ulp_fsm_main "${ulp_fsm_sources}" "" TYPE fsm)
  ulp_embed_binary(ulp_riscv_main "${ulp_riscv_sources}" "" TYPE riscv)
  ```

## Implementation Details

### CMakeLists.txt

The main component's CMakeLists.txt shows how to embed both FSM and RISC-V ULP binaries:

```cmake
# ULP FSM
set(ulp_fsm_app_name ulp_fsm_main)
set(ulp_fsm_sources "ulp_fsm/main.S")
ulp_embed_binary(${ulp_fsm_app_name} "${ulp_fsm_sources}" "" TYPE fsm)

# ULP RISC-V
set(ulp_riscv_app_name ulp_riscv_main)
set(ulp_riscv_sources "ulp_riscv/main.c")
ulp_embed_binary(${ulp_riscv_app_name} "${ulp_riscv_sources}" "" TYPE riscv)
```

The `TYPE` parameter is crucial - it tells the build system which toolchain to use for each ULP program.

### ULP FSM Program

The FSM program (`ulp_fsm/main.S`) is written in assembly and:
- Maintains a counter in RTC slow memory
- Increments it on each wakeup
- Halts when reaching 100

### ULP RISC-V Program

The RISC-V program (`ulp_riscv/main.c`) is written in C and:
- Continues from where FSM left off
- Increments the counter on each wakeup
- Halts when reaching 500

### Main Application

The main application coordinates the three stages:
1. Loads and starts the FSM ULP program
2. Waits for FSM to complete
3. Transfers the counter value and starts the RISC-V ULP program
4. Waits for RISC-V to complete
5. Completes the final counting stage on the main CPU
