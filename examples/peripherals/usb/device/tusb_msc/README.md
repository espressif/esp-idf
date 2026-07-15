| Supported Targets | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- |

# TinyUSB Mass Storage Device Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Mass Storage Devices are one of the most common USB devices. It use Mass Storage Class (MSC) that allow access to their internal data storage.
This example contains code to make ESP based device recognizable by USB-hosts as a USB Mass Storage Device.
It either allows the embedded application i.e. example to access the partition or Host PC accesses the partition over USB MSC.
They can't be allowed to access the partition at the same time.

This example supports storage media of two types:
1. SPI Flash
2. SD MMC Card

Data is read/written from/to SPI Flash through wear-levelling APIs. Wear leveling is a technique that helps to distribute wear and tear among sectors more evenly without requiring any attention from the user. As a result, it helps in extending the life of each sector of the Flash memory.

As a USB stack, a TinyUSB component is used.

## How to use example

### Scenarios
1. USB which accesses the ESP MSC Partition is unplugged initially and the board is powered-on.
     - Result: Host PC can't access the partition over USB MSC. Application example can perform operations (read, write) on partition.
2. USB which accesses the ESP MSC Partition is already plugged-in at boot time.
     - Result: Host PC recognize it as removable device and can access the partition over USB MSC. Application example can't perform any operation on partition.
3. USB which accesses the ESP MSC Partition is plugged-in at boot-up. After boot-up, it is ejected on Host PC manually by user.
     - Result: Host PC can't access the partition over USB MSC. Application example can perform operations (read, write) on partition.
4. USB which accesses the ESP MSC Partition is plugged-in at boot-up. It is then unplugged(removed) from Host PC manually by user.
     - Result: The behavior is different for bus-powered devices and self-powered devices
          - (a) Bus-Powered devices - Both Host PC as well as application example can't access the partition over USB MSC. Here, the device will be Powered-off.
          - (b) Self-Powered devices - Here, the device can be powered-on even after unplugging the device from Host PC. These behavior can be further categorize in two ways:
               - (i) Self-Powered Devices without VBUS monitoring - Both Host PC as well as application example can't access the partition over USB MSC.
               - (ii) Self-Powered Devices with VBUS monitoring - Host PC can't access the partition over USB MSC. Application example can perform operations (read, write) on partition. Here, in ``tinyusb_config_t`` user must set ``self_powered`` to ``true`` and ``vbus_monitor_io`` to GPIO number (``VBUS_MONITORING_GPIO_NUM``) that will be used for VBUS monitoring.

### Hardware Required

1. If the storage media is SPI Flash, any ESP board that have USB-OTG is supported.
2. If the storage media is SD MMC Card, any ESP board with SD MMC card slot and an SD card is required. For Ex - ESP32-S3-USB-OTG

### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

Next, for Self-Powered Devices with VBUS monitoring, user must set ``self_powered`` to ``true`` and ``vbus_monitor_io`` to GPIO number (``VBUS_MONITORING_GPIO_NUM``) that will be used for VBUS monitoring.

### SD MMC Card Pin Assignments

When using SD MMC Card as storage media, additional pins are required beyond the USB pins. SD MMC card support is available on ESP32-S3, ESP32-P4, and ESP32-S31.

#### SD MMC Configuration

GPIOs and SD bus settings can be configured in two ways:

1. Using menuconfig: Run `idf.py menuconfig` in the project directory, open "TinyUSB MSC Example Configuration", and:
   - Select "SDMMC CARD" for "Storage Media Used"
   - Select "SD/MMC bus width" — "4 lines (D0 - D3)" or "1 line (D0)" (see `CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_1`)
   - On ESP32-S3 and ESP32-P4: set the GPIO numbers for the SD card interface
2. Using target-specific defaults: Pin and LDO defaults for each chip are in ``sdkconfig.defaults.esp32s3``, ``sdkconfig.defaults.esp32p4``, and ``sdkconfig.defaults.esp32s31``. These are applied automatically when you set the target with ``idf.py set-target``.
3. In the source code: See the initialization of ``sdmmc_slot_config_t slot_config`` structure in the example code.

By default, this example uses 4-line SD mode, utilizing 6 pins: CLK, CMD, D0 - D3.

Note that even if card's D3 line is not connected to the ESP chip, it still has to be pulled up, otherwise the card will go into SPI protocol mode.

#### ESP32-S3

On ESP32-S3, the SDMMC peripheral is connected to GPIO pins using the GPIO matrix. This allows arbitrary GPIOs to be used to connect an SD card. The table below lists the default pin assignments.

When using an ESP32-S3-USB-OTG board, this example runs without any extra modifications required. Only an SD card needs to be inserted into the slot.

ESP32-S3 pin  | SD card pin | Notes
--------------|-------------|------------
GPIO36        | CLK         | 10k pullup
GPIO35        | CMD         | 10k pullup
GPIO37        | D0          | 10k pullup
GPIO38        | D1          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO33        | D2          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO34        | D3          | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup

#### ESP32-P4

On ESP32-P4, Slot 1 of the SDMMC peripheral is connected to GPIO pins using the GPIO matrix. This allows arbitrary GPIOs to be used to connect an SD card. The table below lists the default pin assignments.

ESP32-P4 pin  | SD card pin | Notes
--------------|-------------|------------
GPIO43        | CLK         | 10k pullup
GPIO44        | CMD         | 10k pullup
GPIO39        | D0          | 10k pullup
GPIO40        | D1          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO41        | D2          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO42        | D3          | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup

Default dedicated pins on ESP32-P4 are able to connect to an ultra high-speed SD card (UHS-I) which requires 1.8V switching (instead of the regular 3.3V). This means the user has to provide an external LDO power supply to use them, or enable and configure the internal LDO via `idf.py menuconfig` -> `TinyUSB MSC Example Configuration` -> `SDMMC powered by internal LDO` and `LDO IO`.

When using different GPIO pins this is not required and the `SDMMC powered by internal LDO` setting can be disabled.

#### ESP32-S31

On ESP32-S31, the SDMMC peripheral is connected to specific GPIO pins using the IO MUX. GPIO pins cannot be customized. The table below lists the pin connections.

ESP32-S31 pin | SD card pin | Notes
--------------|-------------|------------
GPIO24        | CLK         | 10k pullup
GPIO25        | CMD         | 10k pullup
GPIO20        | D0          | 10k pullup
GPIO21        | D1          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO22        | D2          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO23        | D3          | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup

### Build and Flash

1. By default, the example will compile to access SPI Flash as storage media. Here, SPI Flash Wear Levelling WL_SECTOR_SIZE is set to 512 and WL_SECTOR_MODE is set to PERF in Menuconfig.
2. In order to access SD MMC card as storage media, change the configuration as described in [SD MMC Configuration](#sd-mmc-configuration) above.

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see the output at idf monitor:

```
I (329) app_start: Starting scheduler on CPU0
I (334) app_start: Starting scheduler on CPU1
I (334) main_task: Started on CPU0
I (344) main_task: Calling app_main()
I (344) gpio: GPIO[4]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (354) example_main: Initializing storage...
I (364) example_main: Initializing wear levelling
I (374) example_main: Mount storage...
I (374) example_main:
ls command output:
.fseventsd
_pic.jpg
.__pic.jpg
README.MD
I (384) example_main: USB MSC initialization
I (384) tusb_desc:
┌─────────────────────────────────┐
│  USB Device Descriptor Summary  │
├───────────────────┬─────────────┤
│bDeviceClass       │ 239         │
├───────────────────┼─────────────┤
│bDeviceSubClass    │ 2           │
├───────────────────┼─────────────┤
│bDeviceProtocol    │ 1           │
├───────────────────┼─────────────┤
│bMaxPacketSize0    │ 64          │
├───────────────────┼─────────────┤
│idVendor           │ 0x303a      │
├───────────────────┼─────────────┤
│idProduct          │ 0x4002      │
├───────────────────┼─────────────┤
│bcdDevice          │ 0x100       │
├───────────────────┼─────────────┤
│iManufacturer      │ 0x1         │
├───────────────────┼─────────────┤
│iProduct           │ 0x2         │
├───────────────────┼─────────────┤
│iSerialNumber      │ 0x3         │
├───────────────────┼─────────────┤
│bNumConfigurations │ 0x1         │
└───────────────────┴─────────────┘
I (554) TinyUSB: TinyUSB Driver installed
I (564) example_main: USB MSC initialization DONE

Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
I (724) main_task: Returned from app_main()
esp32s3>
esp32s3> help
help
  Print the list of registered commands

read
  read BASE_PATH/README.MD and print its contents

write
  create file BASE_PATH/README.MD if it does not exist

size
  show storage size and sector size

expose
  Expose Storage to Host

status
  Status of storage exposure over USB

exit
  exit from application

esp32s3>
esp32s3> read
E (80054) example_main: storage exposed over USB. Application can't read from storage.
Command returned non-zero error code: 0xffffffff (ESP_FAIL)
esp32s3> write
E (83134) example_main: storage exposed over USB. Application can't write to storage.
Command returned non-zero error code: 0xffffffff (ESP_FAIL)
esp32s3> size
E (85354) example_main: storage exposed over USB. Application can't access storage
Command returned non-zero error code: 0xffffffff (ESP_FAIL)
esp32s3> status
storage exposed over USB: Yes
esp32s3> expose
E (108344) example_main: storage is already exposed
Command returned non-zero error code: 0xffffffff (ESP_FAIL)
esp32s3>
esp32s3>
esp32s3> read
Mass Storage Devices are one of the most common USB devices. It use Mass Storage Class (MSC) that allow access to their internal data storage.
In this example, ESP chip will be recognised by host (PC) as Mass Storage Device.
Upon connection to USB host (PC), the example application will initialize the storage module and then the storage will be seen as removable device on PC.
esp32s3> write
esp32s3> size
Storage Capacity 0MB
esp32s3> status
storage exposed over USB: No
esp32s3> expose
I (181224) example_main: Unmount storage...
esp32s3> status
storage exposed over USB: Yes
esp32s3>
esp32s3>
```
