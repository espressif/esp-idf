| Supported Targets | ESP32 | ESP32-C5 | ESP32-C61 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# FreeRTOS PSRAM Stack Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

This example demonstrates `CONFIG_FREERTOS_PLACE_TASK_STACKS_IN_EXT_RAM`, which causes `xTaskCreate()` and `xTaskCreatePinnedToCore()` to allocate task stacks from PSRAM by default (with internal RAM as a fallback).  TCBs are always kept in internal DRAM.

It also shows how to use the `esp_flash_dispatcher` component so that flash operations called from PSRAM-stacked tasks remain safe.

Any code path that disables the cache (flash erase/write, OTA, NVS) must run on a task whose stack is in internal RAM, or must go through `esp_flash_dispatcher`.

## What is being demonstrated

1. Three worker tasks are created with `xTaskCreate()`.  Each task prints the address of a local variable to confirm its stack resides in PSRAM.
2. Each worker calls `esp_partition_read()` from its PSRAM stack.  The `esp_flash_dispatcher` component intercepts the call and executes it on a dedicated internal-RAM task, avoiding a crash when the cache is temporarily disabled.

## How to use example

### Hardware Required

A board with a PSRAM-capable SoC: ESP32-S2, ESP32-S3, or ESP32-P4.

### Configure the project

```
idf.py menuconfig
```

The `sdkconfig.defaults` already enables `CONFIG_SPIRAM`, `CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM`, and `CONFIG_FREERTOS_PLACE_TASK_STACKS_IN_EXT_RAM`.  No additional configuration is required.

### Build and Flash

```
idf.py build flash monitor
```

### Example output

```
I (xxx) psram_stack: Initialising flash dispatcher (routes flash ops off PSRAM-stacked tasks)
I (xxx) psram_stack: worker[0]: stack @ 0x3c0xxxxx  -> PSRAM
I (xxx) psram_stack: worker[0]: flash read OK (first bytes: e9 04 ...)
I (xxx) psram_stack: worker[1]: stack @ 0x3c0xxxxx  -> PSRAM
I (xxx) psram_stack: worker[1]: flash read OK (first bytes: e9 04 ...)
I (xxx) psram_stack: worker[2]: stack @ 0x3c0xxxxx  -> PSRAM
I (xxx) psram_stack: worker[2]: flash read OK (first bytes: e9 04 ...)
I (xxx) psram_stack: Example complete
```

## Further reading

- `CONFIG_FREERTOS_PLACE_TASK_STACKS_IN_EXT_RAM` in `menuconfig → Component config → FreeRTOS → Extra`
- [External RAM](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/external-ram.html) — limitations of tasks with PSRAM stacks
- `espressif/esp_flash_dispatcher` on the [ESP Component Registry](https://components.espressif.com/components/espressif/esp_flash_dispatcher)
