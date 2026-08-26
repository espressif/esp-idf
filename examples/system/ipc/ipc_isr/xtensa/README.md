| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |

# IPC ISR Example

This example demonstrates how to use the IPC ISR feature (which allows an IPC to run in the context of a High Priority Interrupt). The level of the IPC ISR interrupt depends on the `CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` option. The IPC ISR feature can be useful in cases where users need to quickly get the state of the other CPU (consult the [IPC documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ipc.html)). The `asm_funcs.S` file contains the callback that will be run on the other core. The callback should be fairly simple and must be entirely in assembly.

The first assembly callback `get_ps_other_cpu()` demonstrates a callback that simply returns the `PS` register of other core.

The second assembly callback `extended_ipc_isr_asm()` demonstrates a more complex callback that uses a buffer (provided as the callback's argument) to save some registers and return multiple values from the callback. The callback's `void *arg` points to a buffer containing the following:
  - `uint32_t regs[];` that gives the callback an area to save some of the CPUs registers. Saving the registers gives the callback more scratch registers to use.
  - `uint32_t in[];` that gives the callback multiple input arguments
  - `uint32_t out[];` that gives the callback multiple output arguments

The `extended_ipc_isr_asm()` callback will simply save/restore registers to/from `regs[]`, then use the arguments passed by `in[]` to do some work, then write the results to `out[]`.

## How to use example

### Hardware Required

Example should be able to run on any commonly available ESP32 development board. The chip should have two cores.

### Configure the project

- `CONFIG_FREERTOS_UNICORE` - disabled,
- `CONFIG_ESP_IPC_ISR_ENABLE` - enabled.

```
idf.py menuconfig
```

### Build and Flash

```
idf.py build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example output

```
I (304) example: Start
I (304) example: call get_ps_other_cpu
I (314) example: PS_INTLEVEL = 0x5
I (314) example: PS_EXCM = 0x0
I (324) example: PS_UM = 0x1
I (324) example: call extended_ipc_isr_asm
I (324) example: in[0] = 0x1
I (334) example: in[1] = 0x2
I (334) example: in[2] = 0x3
I (334) example: out[0] = (in[0] | in[1] | in[2]) = 0x3
I (344) example: out[1] = (in[0] + in[1] + in[2]) = 0x6
I (354) example: out[2] = in[2] = 0x3
I (354) example: out[3] = PS of other cpu = 0x25
I (364) example: End
```
