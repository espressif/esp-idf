| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# BLE Peripheral Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example uses the UART transport written by application itself. Refer the file [main/uart_driver.c](main/uart_driver.c).

To write the custom transport in the application, the controller should be disabled and the default uart-transport should be disabled(When the controller is disabled, by default the uart-transport is selected). In order to compile the custom transport in the application, the default uart-transport should be disabled. Refer to the sdkconfig.defaults.

For more information about the application refer to the bleprph [README file](../bleprph/README.md)
To test this demo, any BLE scanner app can be used.

Note :

* To install the dependency packages needed, please refer to the top level [README file](../../../README.md#running-test-python-script-pytest).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Configure the project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `Component config-> Bluetooth` menu:

* Select `controller` to Disabled.
* Disable `Nimble Options-> Host-controller Transport -> Enable Uart Transport`.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output when bleprph_host_only running on esp32 and using controller of esp32c6:

```
I (31) boot: ESP-IDF v5.3-dev-1570-gbc703f3508-dirty 2nd stage bootloader
I (31) boot: compile time Jan 18 2024 17:45:38
I (33) boot: Multicore bootloader
I (37) boot: chip revision: v3.0
I (41) boot.esp32: SPI Speed      : 40MHz
I (46) boot.esp32: SPI Mode       : DIO
I (50) boot.esp32: SPI Flash Size : 2MB
I (55) boot: Enabling RNG early entropy source...
I (60) boot: Partition Table:
I (64) boot: ## Label            Usage          Type ST Offset   Length
I (71) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (79) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (86) boot:  2 factory          factory app      00 00 00010000 00100000
I (94) boot: End of partition table
I (98) esp_image: segment 0: paddr=00010020 vaddr=3f400020 size=19554h (103764) map
I (142) esp_image: segment 1: paddr=0002957c vaddr=3ffb0000 size=0278ch ( 10124) load
I (146) esp_image: segment 2: paddr=0002bd10 vaddr=40080000 size=04308h ( 17160) load
I (155) esp_image: segment 3: paddr=00030020 vaddr=400d0020 size=42800h (272384) map
I (250) esp_image: segment 4: paddr=00072828 vaddr=40084308 size=0a6ech ( 42732) load
I (274) boot: Loaded app from partition at offset 0x10000
I (274) boot: Disabling RNG early entropy source...
I (285) cpu_start: Multicore app
I (294) cpu_start: Pro cpu start user code
I (294) cpu_start: cpu freq: 160000000 Hz
I (294) cpu_start: Application information:
I (297) cpu_start: Project name:     bleprph
I (302) cpu_start: App version:      v5.3-dev-1570-gbc703f3508-dirty
I (309) cpu_start: Compile time:     Jan 18 2024 17:45:29
I (315) cpu_start: ELF file SHA256:  bd62addc6...
I (321) cpu_start: ESP-IDF:          v5.3-dev-1570-gbc703f3508-dirty
I (328) cpu_start: Min chip rev:     v0.0
I (332) cpu_start: Max chip rev:     v3.99
I (337) cpu_start: Chip rev:         v3.0
I (342) heap_init: Initializing. RAM available for dynamic allocation:
I (349) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (355) heap_init: At 3FFB4AA8 len 0002B558 (173 KiB): DRAM
I (362) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (368) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (374) heap_init: At 4008E9F4 len 0001160C (69 KiB): IRAM
I (382) spi_flash: detected chip: generic
I (385) spi_flash: flash io: dio
W (389) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (403) coexist: coex firmware version: c02915e
I (408) main_task: Started on CPU0
I (418) main_task: Calling app_main()
I (428) NimBLE_BLE_PRPH: BLE Host Task Started
I (438) uart: queue free spaces: 8
I (438) main_task: Returned from app_main()
I (478) NimBLE: Device Address:
I (478) NimBLE: 60:55:f9:f6:07:e2
I (478) NimBLE:

I (488) NimBLE: GAP procedure initiated: advertise;
I (488) NimBLE: disc_mode=2
I (488) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (498) NimBLE:
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
