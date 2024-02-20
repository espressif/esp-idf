| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# IPC ISR Example

This example demonstrates how to use the IPC ISR feature (which allows an IPC to run in the context of a High Priority Interrupt). The level of the IPC ISR interrupt depends on the `CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` option. The IPC ISR feature can be useful in cases where users need to quickly get the state of the other CPU (consult the [IPC documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ipc.html)). Unlike ESP32 or ESP32S3, for this chip, the callback function can be written in C.

The first callback `get_mstatus_other_cpu()` demonstrates a callback that simply returns the `MSTATUS` register of other core.

The second callback `extended_ipc_isr_func()` demonstrates how to return multiple values from the callback. The callback's `void *arg` points to a structure that contains the following:
  - `uint32_t in[];` that gives the callback multiple input arguments
  - `uint32_t out[];` that gives the callback multiple output arguments

The `extended_ipc_isr_func()` callback uses the `in[]` arguments does some work and then writes to `out[]`.

## How to use example

### Hardware Required

Example should be able to run on any commonly available ESP32P4 development board. The chip should have two cores.

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
I (411) example: Start
I (421) example: call get_mstatus_other_cpu
I (421) example: MSTATUS = 0x3880
I (421) example: call extended_ipc_isr_func
I (431) example: in[0] = 0x1
I (431) example: in[1] = 0x2
I (441) example: in[2] = 0x3
I (441) example: out[0] = (in[0] | in[1] | in[2]) = 0x3
I (451) example: out[1] = (in[0] + in[1] + in[2]) = 0x6
I (451) example: out[2] = MCAUSE of other cpu = 0xb800002c
I (461) example: out[3] = MSTATUS of other cpu = 0x3880
I (461) example: End
```
