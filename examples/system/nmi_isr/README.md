| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# NMI ISR Example

This example demonstrates how to allocate and use non-maskable interrupt (NMI) on Xtensa-based targets. The `asm_funcs.S` file contains the ISR that will be run on the core that installed the NMI. The callback should be fairly simple and must be entirely written in assembly.

Defining an NMI handler can be done by defining a routine named `xt_nmi`. That routine will be called via `call0` instruction, as such, before returning from the ISR, the return address register, `a0`, must be restored thanks to the instruction:

```
rsr a0, EXCSAVE + XCHAL_NMILEVEL
```

## How to use example

### Hardware Required

Example can run on any Xtensa-based ESP32 development board. Since the example uses GPIO19 as a bi-directional pin, make sure not to connect it to anything.

### Configure the project

No particular configuration is required to run this example, the default one is suitable.

### Build and Flash

```
idf.py build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example output

```
example: Start
example: Success
```
