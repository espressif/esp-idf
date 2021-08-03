| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |

# Hi-priority IPC example

This example demonstrates how to use Hi-priority IPC, it is based on Hi-priority interrupt (level 4). This feature can be useful in case when need quickly to get the state of the other CPU (consult the [Hi-priority IPC](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ipc.html#hi-priority-ipc)).
In the `asm_funcs.S` file are functions that will be run on the other core. The function should be fairly simple that can be written in assembler.

The first asm function `get_ps_other_cpu()` demonstrates a simple way of usage, it returns the PS register of other core.

The second asm function `extended_ipc_isr_asm()` demonstrates way when need to do some complex operations on other core, for this, save registers to the buffer (regs[]) making them available for use. Then using passed arguments (in[]) do some work and write the result to out[]. At the end recover saved registers.

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
I (344) example: out[1] = (in[0] & in[1] & in[2]) = 0x6
I (354) example: out[2] = in[2] = 0x3
I (354) example: out[3] = PS of other cpu = 0x25
I (364) example: End
```
