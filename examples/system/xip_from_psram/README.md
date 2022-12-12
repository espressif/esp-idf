| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# XIP (Execute-In-Place) From PSRAM Example

This example illustrates a typical usage of XIP (Execute-In-Place) From PSRAM. With this feature:
- You can optimize internal RAM usage
- System can avoid disabling the Cache during an SPI1 Flash operation.

## Overview

Here we define two sets of operations related to external memory:
SET1:  Operations where CPU fetches data and instructions from external memory. 
SET2: `ESP Flash` driver operations and other operations from drivers based on `ESP Flash` (NVS, Partition drivers, etc.).

By default, during `SET2` operations, concurrent access requests to the Flash and PSRAM (`SET1` operations) will be disabled otherwise both the `SET1` and `SET2` operations are not guaranteed to be safe (this is an undefined behaviour).

Only ISRs in internal RAM will get executed during `SET2` operations. Besides, if any functions or data are accessed in these ISRs (usually this happens in ISR callbacks), they need to be placed into internal RAM as well. For interrupt handlers which need to execute when the cache is disabled (e.g., for low latency operations), you need to set the ESP_INTR_FLAG_IRAM flag when the interrupt handler is registered.

When **CONFIG_SPIRAM_FETCH_INSTRUCTIONS** and **CONFIG_SPIRAM_RODATA** are both enabled,  the `flash.text` sections (for instructions) and the `.rodata` section (read only data) will be moved to PSRAM. Corresponding virtual memory range will be re-mapped to PSRAM. Under this condition, ESP-IDF won't disable concurrent accesses to external memory (`SET1` operations) anymore.

By using this feature, during `SET2` operations, placement of ISRs, ISR callbacks, and related data are no longer limited to internal RAM.

## Example Process

To show this feature, in this example we go through the following steps:

`General Steps`:
1. Create a partition for Flash Erase Operation
2. Create an esp_timer in one-shot mode 

`PSRAM Steps`:
3. Do a Flash erase operation, and start the timer
4. ESP-Timer callback is dispatched and it calls a function in PSRAM during the flash erase operation 
5. The Flash erase operation finishes
6. Show the result about the callback(in PSRAM) response and execute time

`IRAM Steps`:
7. Do a Flash erase operation, and start the timer
8. ESP-Timer callback is dispatched and it calls a function in IRAM during the flash erase operation 
9. The flash erase operation finishes
10. Show the result about the callback(in IRAM) response and execute time

### Timeline

Initialization and config -> Flash erase start -> ESP-Timer callback(in PSRAM) appear -> Flash erase finish -> Flash erase start -> ESP-Timer callback(in IRAM) appear -> Flash erase finish

                       ISR         CPU
                        |           | 
                        |           |
                        |           |           
                        |           * <----flash operation starts 
                        |           *
      callback starts   * --------> *              
         (in PSRAM)     *           *              
      callback finishes * <-------- *       
                        |           *
                        |           *
                        |           * <----flash operation finishes
                        |           |
                        |           |
                        |           |
                        |           | 
                        |           |
                        |           |           
                        |           * <----flash operation starts 
                        |           *
      callback starts   * --------> *              
         (in IRAM)      *           *              
      callback finishes * <-------- *       
                        |           *
                        |           *
                        |           * <----flash operation finishes
                        |           |
                        |           |

## Example Result

The ISR which call a function in IRAM happening during Flash erase operations. CPU fetches instructions and data from internal RAM.
The ISR which call a function in PSRAM happening during Flash erase operations and its response time is longer than calling a function in IRAM. That's because fetching instructions from PSRAM takes more time than fetching from IRAM.

## Configure the project

Open the project configuration menu (`idf.py menuconfig`).

1. Set the `Partition Table -> Custom partition table CSV` to `y`.
2. Set the `Component config -> High resolution timer -> Support ISR dispatch method` to `y`.
3. Set the `Component config -> ESP PSRAM -> SPI RAM config` to `y` then set `Cache fetch instructions from SPI RAM` and `Cache load read only data from SPI RAM` to `y`.


## Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (742) esp_psram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (742) example: found partition 'storage1' at offset 0x110000 with size 0x10000

I (1152) example: callback(in PSRAM) response time: 7 us
I (1362) example: callback(in IRAM) response time: 5 us

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
