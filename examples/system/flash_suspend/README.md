| Supported Targets | ESP32-C3 |
| ----------------- | -------- |

# Flash Suspend Example

## Overview

One problem in IDF is that during Flash operations (via either ESP Flash, NVS or Partition APIs), concurrent execution of code residing in Flash will lead to a crash. Consequently, all ISRs and their callbacks need to be placed in internal RAM.

Background: In IDF, executable code (instructions) are usually placed in Flash.

However, by using the auto-suspend feature, it is not necessary to put ISRs and their callbacks into internal RAM anymore.

Here we define two sets of Flash operations:
SET1:  Operations where CPU fetches data and instructions from Flash.
SET2: `ESP Flash` driver operations and other operations from drivers based on `ESP Flash` (NVS, Partition drivers, etc.).

- During Erase Operation (SET 2), if there is a SET 1 request, the ongoing SET 2 Flash operation will be suspended automatically, the Erase Operation is interrupted, CPU will get a chance to read from Flash. After CPU's access, the Flash chip will be resumed automatically and the Erase Operation will continue.

- During other Flash Operations (SET 2), if there is a SET 1 request, CPU will usually wait until the SET 2 operation finishes, then get the chance to read from Flash.


## Example Process

As mentioned above, in this example we go through the following steps:

`General Steps`:
1. Create a partition for Flash Erase Operation
2. Configure a GP timer to trigger an alarm once
3. Register a timer alarm event callback

`Misc Steps`:
4. Make the callback call a function (`s_function_in_flash`) in Flash
5. Start the timer
6. Start a Flash Erase Operation
7. Pause the timer

`IRAM Steps`:
8. Make the callback call a function (`s_function_in_iram`) in Internal RAM
9. Start the timer
10. Start a Flash Erase Operation

## Example Result

You will see an ISR happening during each of the Flash Erase Operations (SET 2).

### `Misc Steps`

The ISR callback calls a function, and its main body is in `.flash.text`. You may use
```
riscv32-esp-elf-objdump
```
to see the concrete instructions of the function. It contains around 10 instructions. The Cache will read from the Flash, if these instructions are not cached yet. CPU then fetches these instructions from the Cache. For ESP32C3, the time between the call to `s_function_in_flash`, and its execution, is around 32 us, when the CPU frequency is 160MHz. Apart from the context switch time, instruction execution time (please refer to the example comments to know the function body), and other hardware overheads, there is still around 28.5us, which should be the time for the Cache synchronisation (see note 1.).

----

*note 1*
**Cache synchronisation:**
In ESP-IDF, the cache is a small memory system which can provide low-latency access to a block of data, residing in Flash or PSRAM. The CPU accesses Flash and PSRAM via the cache. When the CPU requested data that is not in the cache, we have a cache miss and the data will be synchronized to cache from either Flash or PSRAM.
In this example, for `Misc Steps`, the involved cache synchronization time is a bit longer, since a SET 2 Flash Erase Operation is ongoing. The ESP chip will send a suspend command to the Flash chip, to suspend the ongoing Erase Operation, and let the Cache access the Flash chip.

*The Cache synchronisation time would be even longer, if more instructions need to be fetched.*

### `IRAM Steps`

The ISR callback calls a function, and its main body is in `.iram0.text`. For ESP32C3, the time between `s_function_in_iram` is called, and its execution, is around 3.8 us, when the CPU frequency is 160MHz. Under this circumstance, around 400 times instruction execution will cost about 2.5us. Apart from the context switch and other overheads, these instructions in Internal RAM can be fetched within 1us.

## Conclusion for Code Placement

For ESP-IDF drivers, we usually expect the ISR callbacks to be short, simple and useful. It's more like a way to inform users of a hardware event. And the event handling routines should be delegated to the main program.

What should not be used in an ISR callback:
- printf (there is mutex underneath)
- long log print (even using `esp_rom_printf`)
- blocking logic

Based on these principles,
- If the `CONFIG_SPI_FLASH_AUTO_SUSPEND` is not enabled/supported, usually an ISR and its callbacks should be put in Internal RAM, when you also need to operate the Flash via `ESP Flash` driver and the drivers based on `ESP Flash`.
- If the `CONFIG_SPI_FLASH_AUTO_SUSPEND` is enabled, the ISR function and its callbacks **can** be put in Flash. But their maximum response time will be **prolonged**.
- If the `CONFIG_SPI_FLASH_AUTO_SUSPEND` is enabled, a Flash Erase Operation would be interrupted by the CPU / Cache access request to the Flash. Therefore, when the ISR happens frequently, the Flash Erase Operation may be **prolonged** a lot.


### Configure the project

To change the `Auto suspend long erase/write operations` (`CONFIG_SPI_FLASH_AUTO_SUSPEND`), open the project configuration menu (`idf.py menuconfig`).

Navigate to `Component config -> SPI Flash driver` menu.


### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (285) Example: found partition 'storage1' at offset 0x110000 with size 0x80000
I (5615) Example: Flash Driver Erase Operation finishes, duration:
		2635600.01 us
I (5615) Example: During Erase, ISR callback function(in flash) response time:
		32.26 us
I (8295) Example: Flash Driver Erase Operation finishes, duration:
		2626830.69 us
I (8295) Example: During Erase, ISR callback function(in iram) response time:
		3.83 us
I (8295) Example: Finish
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
