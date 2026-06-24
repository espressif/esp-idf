| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# CherryUSB Serial Device Example

Starts a CDC Device, and sends back any received data from the host.

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to use example

This example can also be configured to act as double serial device.
Run `idf.py menuconfig` and in `Example Configuration → Enable usb cdc acm two channel` enable.

### Hardware Required

Any ESP board that have USB-OTG supported.

#### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

When running, the example will print the following output:

```
I (271) main_task: Started on CPU0
I (291) main_task: Calling app_main()
[I/USB] cherryusb, version: v1.4.3
[I/USB] ========== dwc2 udc params ==========
[I/USB] CID:00000000
[I/USB] GSNPSID:4f54400a
[I/USB] GHWCFG1:00000000
[I/USB] GHWCFG2:224dd930
[I/USB] GHWCFG3:00c804b5
[I/USB] GHWCFG4:d3f0a030
[I/USB] dwc2 fsphy type:1, hsphy type:0, dma support:2
[I/USB] dwc2 has 7 endpoints and dfifo depth(32-bit words) is 200, default config: 7 endpoints
[I/USB] =================================
[I/USB] fifo0 size:0010, offset:0050
[I/USB] fifo1 size:0010, offset:0060
[I/USB] fifo2 size:0010, offset:0070
[I/USB] fifo3 size:0010, offset:0080
[I/USB] fifo4 size:0010, offset:0090
[I/USB] fifo5 size:0010, offset:00a0
[I/USB] fifo6 size:0010, offset:00b0
I (391) device_cdc_main: usb cdc acm init done
I (721) device_cdc_main: intf:0, dtr:0
I (721) device_cdc_main: intf:0, rts:0
I (2981) device_cdc_main: intf:0, dtr:0
I (2981) device_cdc_main: intf:0, rts:0
I (2981) device_cdc_main: intf:0, dtr:0
I (2981) device_cdc_main: intf:0, rts:0
I (2981) device_cdc_main: intf:0, dtr:0
I (2981) device_cdc_main: intf:0, rts:0
I (2991) device_cdc_main: intf:0, dtr:0
I (2991) device_cdc_main: intf:0, rts:0
I (2991) device_cdc_main: intf:0, dtr:0
I (2991) device_cdc_main: intf:0, rts:0
I (3001) device_cdc_main: intf:0, dtr:0
I (3001) device_cdc_main: intf:0, rts:0
I (6011) device_cdc_main: actual out len:13
I (6021) device_cdc_main: actual in len:13
I (10411) device_cdc_main: actual out len:13
I (10421) device_cdc_main: actual in len:13
I (11111) device_cdc_main: intf:0, dtr:0
I (11111) device_cdc_main: intf:0, rts:0
```
