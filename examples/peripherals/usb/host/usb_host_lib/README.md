| Supported Targets | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- |

# USB Host Library Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

This example demonstrates the [USB Host Library API](https://docs.espressif.com/projects/esp-usb/en/latest/esp32s2/usb_host.html) using a pseudo class driver and a dedicated USB Host Library task, together with an **interactive console** (ESP-IDF `esp_console` REPL). On boot it:

1. Starts a read-eval-print loop on the configured primary serial console (UART or USB Serial/JTAG, depending on your hardware and project settings).
2. Registers common system commands from the [advanced console example](../../../../system/console/advanced/README.md) (`help`, `tasks`, and sleep-related commands where the chip supports them) plus USB-specific commands (see below).
3. Installs the USB Host stack and runs the class driver task, which waits for a device, prints descriptor information, and handles connect/disconnect.
4. Keeps running until reset; you can uninstall the USB stack from the shell and reinstall it later with `usb_install`.

The example demonstrates the following aspects of the USB Host Library API:

- How to use the Library API to:
    - Install and uninstall the USB Host Library
    - Run the library event handler function and USB Host Library task
    - How to handle library events
- How to use the Client API from a client task to:
    - Register and deregister a client of the USB Host Library
    - Run the client event handler functions
    - How to handle client events via various callbacks (including suspend/resume notifications when supported)
    - Open and close a device
    - Get a device's descriptors
- How to drive **root port power, suspend, and resume** from application code via the console commands

## Console commands (USB)

| Command | Description |
| ------- | ----------- |
| `usb_install` | Install the USB Host stack and start the class driver. On ESP32-P4 it accepts argument `HS\|FS\|both` for USB peripheral selection. |
| `usb_uninstall` | Request teardown of the class client and uninstall the Host Library. |
| `usb_info` | Print host library state (device/client counts, root port state) and bus addresses of connected devices. |
| `usb_suspend` | Suspend the USB root port. On ESP32-P4, only 1 port is suspended. |
| `usb_resume` | Resume the USB root port. On ESP32-P4, only 1 port is resumed. |
| `usb_power` | Set root port power: argument `0` (off) or `1` (on). On ESP32-P4, both ports are turned on/off. |

Type `help` in the monitor for all registered commands.

## How to use example

### Hardware Required

* Development board with USB-OTG support.
* Follow instruction in [examples/usb/README.md](../../README.md) for specific hardware setup.

### Configure the project

```
idf.py menuconfig
```

Under **Example Configuration**:

* **Store command history in flash** — When enabled, mounts a small FAT partition (`storage` in `partitions.csv`) via wear levelling and saves linenoise history to `/data/history.txt`.
* **Maximum command line length** — Upper bound for a single console input line.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output and use the console:

```
idf.py -p PORT flash monitor
```

(Replace `PORT` with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After flashing, you should see startup logs similar to the following (exact ordering and timestamps vary), then the REPL prompt (by default `<IDF_TARGET>>`, e.g. `esp32s3>`):

```
I (305) main_task: Started on CPU0
I (315) main_task: Calling app_main()
I (315) USB host lib: USB host library example
I (325) USB host lib: Command history enabled
I (365) USB host lib: Installing USB Host Library
I (395) USB host lib: USB Host installed with peripheral map 0x1
I (405) USB host lib: USB host stack installed (class driver running)
I (745) CLASS: Registering Client
I (745) CLASS: Opening device at address 1
I (745) CLASS: Getting device information
...
```

When a device is attached, the class driver prints device, configuration, and string descriptors (same style as in previous revisions of this example). On disconnect you may see host stack messages such as:

```
E (6135) USBH: Device 1 gone
```

After `usb_uninstall` (with no devices and the client released), the host task uninstalls the library:

```
I (9545) USB host lib: Get FLAGS_NO_CLIENTS
I (9545) USB host lib: All devices marked as free, no need to wait FLAGS_ALL_FREE event
I (9545) USB host lib: No more clients and devices, uninstall USB Host library
I (9645) USB host lib: USB host stack uninstalled
```

The console keeps running so you can run `usb_install` again or other commands.

## Troubleshooting

To obtain more debug, users should set the [log level](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/system/log.html) to debug via menuconfig.

### Failing Enumeration

```
I (262) cpu_start: Starting scheduler on PRO CPU.
I (268) DAEMON: Installing USB Host Library
I (298) CLASS: Registering Client
E (2748) HUB: Short string desc corrupt
E (2748) HUB: Stage failed: CHECK_SHORT_MANU_STR_DESC
```

The log output demonstrates a device that has failed. The Hub Driver will output some error logs indicating which stage of enumeration has failed.

### Blank String Descriptors

The current USB Host Library will automatically cache the Manufacturer, Product, and Serial Number string descriptors of the device during enumeration. However, when fetching the string descriptors, the USB Host Library will only fetch those string descriptors if they use LANGID code 0x0409 (i.e., English - United States). Therefore, if the example does not print a particular descriptor, it is likely that the string descriptor was not cached during enumeration.
