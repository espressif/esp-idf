| Supported Targets | ESP32-H2 |
| ----------------- | -------- |

# OpenThread radio co-processor (RCP) example

## Overview

This example demonstrates an openthread radio co-processor.
The RCP can receive/transmit 15.4-type packets, encode and send the packet to the host device.
You can use the RCP to implement an Openthread sniffer. You cani also connect the RCP to the host device which do not have a 15.4-radio to join in an Openthread network.

To run this example, an ESP32-H2 board is required.

### Config ESP32-H2 uart

Modify the file `esp_ot_config.h`.

In the file `esp_ot_config.h`, there are three parameters. 
As for `ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG` and `ESP_OPENTHREAD_DEFAULT_PORT_CONFIG`, modification is not recommended.
As for the parameters in `ESP_OPENTHREAD_DEFAULT_HOST_CONFIG`, you can refer to ESP uart example `example/peripherals/uart/uart_echo/README.md`

### Build, Flash, and Run

Build the project and flash it to the board

```
idf.py -p <PORT> build flash
```

Now you'll get an ot-rcp (for this example, there is no output in console).

# OpenThread sniffer example (Linux or macOS based)

## Overview

This example is about how to run a sniffer to capture Thread packet, which refers to [openthread.io](https://openthread.io/guides/pyspinel/sniffer). 
Openthread group just supports Linux and macOS.

## How to use example

### Hardware connection

To run this example, you need an ESP32-H2 borad flashed with ot-rcp application.

### Set up the sniffer environment

The sniffer tool connects to an openthread RCP device and converts it into a promiscuous packet sniffer. RCP
will encode and send the packet to host after capturing a 15.4-type packet. On the host, Pyspinel should be run for
decoding the packet and generating a pcap (packet capture) stream to be saved or piped directly into Wireshark.

Clone and install Pyspinel and dependencies, refer to [openthread.io](https://openthread.io/guides/pyspinel/sniffer#installation).

### Install and config Wireshark

For installing the Wireshark, refer to [Espressif Wireshark User Guide](
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wireshark-user-guide.html).

You need to config Wireshark with some properties for the Thread network.
For Wireshark configuration, refer to [openthread.io](https://openthread.io/guides/pyspinel/sniffer#thread_network_properties).

### Run the sniffer

For example, to sniff on Channel 15 using a device mounted at /dev/ttyUSB0 with the baudrate 115200 in the Wireshark output:

```
cd ~/pyspinel
python sniffer.py -c 15 -u /dev/ttyUSB0 --crc -b 115200 | wireshark -k -i -
```

