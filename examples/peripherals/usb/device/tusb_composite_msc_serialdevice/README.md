| Supported Targets | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- |

# TinyUSB Composite Device (MSC + Serial) Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

A USB device can provide multiple functions that are active simultaneously. Such multi-function devices are also known as composite devices. This example configures an ESP chip as both a CDC ACM serial device and an MSC device backed by SPI flash.

The example uses esp_tinyusb and enables asynchronous MSC storage I/O by default.

### Asynchronous MSC I/O

`CONFIG_TINYUSB_MSC_ASYNC_IO=y` moves MSC storage reads and writes from the TinyUSB task to a dedicated worker task. This separation is especially useful for a composite device: CDC ACM traffic remains responsive while the host accesses slower SPI flash storage. Asynchronous I/O also reports failed storage writes to the USB host; without it, write failures are only logged by the device.

The worker uses the existing `CONFIG_TINYUSB_MSC_BUFSIZE` storage buffer rather than allocating a second data buffer. With the default 4096-byte worker stack, it uses approximately 4.5 KB of additional heap, plus approximately 100 bytes for each additional LUN.

This example uses the default worker configuration:

- Stack size: 4096 bytes
- Priority: 5
- Core affinity: no affinity

To customize the worker, install the MSC driver before creating the SPI flash storage:

```c
tinyusb_msc_driver_config_t driver_cfg = { 0 };
driver_cfg.async_io_task = TINYUSB_TASK_CUSTOM(2048, 5, tskNO_AFFINITY);
ESP_ERROR_CHECK(tinyusb_msc_install_driver(&driver_cfg));
```

The required stack size depends on the storage medium and its driver. Measure the worker task's peak stack usage during representative reads, writes, and write failures before reducing the default stack.

## How to use example

### Hardware Required

Any ESP board that supports the USB-OTG peripheral.

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
I (...) main_task: Calling app_main()
I (...) example_main: Initializing storage...
I (...) example_main: Initializing wear levelling
I (...) tinyusb_msc_storage: Async IO task started (prio 5, core 0)
I (...) example_main: USB Composite initialization
...
I (...) TinyUSB: TinyUSB Driver installed on port 1
I (...) example_main: USB Composite initialization DONE
```
