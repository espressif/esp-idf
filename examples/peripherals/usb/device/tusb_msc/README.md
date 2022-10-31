| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# TinyUSB Mass Storage Device Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Mass Storage Devices are one of the most common USB devices. It use Mass Storage Class (MSC) that allow access to their internal data storage.
This example contains code to make ESP based device recognizable by USB-hosts as a USB Mass Storage Device.
It either allows the embedded application ie example to access the partition or Host PC accesses the partition over USB MSC.
They can't be allowed to access the partition at the same time.
The access to the underlying block device is provided by functions in tusb_msc_storage.c

In this example, data is read/written from/to SPI Flash through wear-levelling APIs. Wear leveling is a technique that helps to distribute wear and tear among sectors more evenly without requiring any attention from the user. As a result, it helps in extending the life of each sector of the Flash memory.

As a USB stack, a TinyUSB component is used.

## How to use example

### Scenarios
1. USB which accesses the ESP MSC Partition is unplugged initially and the board is powered-on.
     - Result: Host PC can't access the partition over USB MSC. Application example can perform operations (read, write) on partition.
2. USB which accesses the ESP MSC Partition is already plugged-in at boot time.
     - Result: Host PC recongnize it as removable device and can access the partition over USB MSC. Application example can't perform any operation on partition.
3. USB which accesses the ESP MSC Partition is plugged-in at boo-up. After boot-up, it is ejected on Host PC manually by user.
     - Result: Host PC can't access the partition over USB MSC. Application example can perform operations (read, write) on partition.
4. USB which accesses the ESP MSC Partition is plugged-in at boot-up. It is then unplugged(removed) from Host PC manually by user.
     - Result: The behaviour is different for bus-powered devices and self-powered devices
          - (a) Bus-Powered devices - Both Host PC as well as application example can't access the partition over USB MSC. Here, the device will be Powered-off.
          - (b) Self-Powered devices - Here, the device can be powered-on even after unplugging the device from Host PC. These behaviour can be further categorize in two ways:
               - (i) Self-Powered Devices without VBUS monitoring - Both Host PC as well as application example can't access the partition over USB MSC.
               - (ii) Self-Powered Devices with VBUS monitoring - Host PC can't access the partition over USB MSC. Application example can perform operations (read, write) on partition. Here, in ``tinyusb_config_t`` user must set ``self_powered`` to ``true`` and ``vbus_monitor_io`` to GPIO number (``VBUS_MONITORING_GPIO_NUM``) that will be used for VBUS monitoring.

### Hardware Required

Any ESP board that have USB-OTG supported.

### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

Next, for Self-Powered Devices with VBUS monitoring, user must set ``self_powered`` to ``true`` and ``vbus_monitor_io`` to GPIO number (``VBUS_MONITORING_GPIO_NUM``) that will be used for VBUS monitoring.

### Build and Flash

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
I (311) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (332) gpio: GPIO[4]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (332) example_msc_main: Initializing storage...
I (342) example_msc_storage: Initializing wear levelling
I (372) example_msc_main: USB MSC initialization
I (372) tusb_desc: 
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
I (532) TinyUSB: TinyUSB Driver installed
I (532) example_msc_main: USB MSC initialization DONE
I (542) example_msc_main: Mount storage...
I (542) example_msc_storage: Initializing FAT
I (552) example_msc_main: 
ls command output:
README.MD
.fseventsd

Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
esp32s3> I (912) example_msc_main: tud_mount_cb MSC START: Expose Over USB
I (912) example_msc_main: Unmount storage...
I (2032) example_msc_main: tud_msc_scsi_cb() invoked: SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL
I (2032) example_msc_main: tud_msc_capacity_cb() size(1024000), sec_size(512)
esp32s3> 
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
E (19102) example_msc_main: storage exposed over USB. Application can't read from storage.
Command returned non-zero error code: 0xffffffff (ESP_FAIL)
esp32s3> write
E (22412) example_msc_main: storage exposed over USB. Application can't write to storage.
Command returned non-zero error code: 0xffffffff (ESP_FAIL)
esp32s3> size
E (24962) example_msc_main: storage exposed over USB. Application can't access storage
Command returned non-zero error code: 0xffffffff (ESP_FAIL)
esp32s3> status
storage exposed over USB: Yes
esp32s3> 
esp32s3> 
esp32s3> I (49692) example_msc_main: tud_msc_scsi_cb() invoked: SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL
I (49692) example_msc_main: tud_msc_start_stop_cb() invoked, power_condition=0, start=0, load_eject=1
I (49702) example_msc_main: tud_msc_start_stop_cb: MSC EJECT: Mount on Example
I (49712) example_msc_main: Mount storage...
I (49712) example_msc_storage: Initializing FAT
I (49712) example_msc_main: 
ls command output:
README.MD
esp32s3> 
esp32s3> 
esp32s3> status
storage exposed over USB: No
esp32s3> read
Mass Storage Devices are one of the most common USB devices. It use Mass Storage Class (MSC) that allow access to their internal data storage.
In this example, ESP chip will be recognised by host (PC) as Mass Storage Device.
Upon connection to USB host (PC), the example application will initialize the storage module and then the storage will be seen as removable device on PC.
esp32s3> write
esp32s3> size
storage size(1024000), sec_size(512)
esp32s3> 
esp32s3> expose
I (76402) example_msc_main: Unmount storage...
esp32s3> I (76772) example_msc_main: tud_msc_scsi_cb() invoked: SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL
I (76772) example_msc_main: tud_msc_capacity_cb() size(1024000), sec_size(512)
esp32s3> 
esp32s3> status
storage exposed over USB: Yes
esp32s3> 
esp32s3>
```
