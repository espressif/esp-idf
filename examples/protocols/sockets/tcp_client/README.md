| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |


# TCP Client example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application creates a TCP socket and tries to connect to the server with predefined IP address and port number. When a connection is successfully established, the application sends message and waits for the answer. After the server's reply, application prints received reply as ASCII text, waits for 2 seconds and sends another message.

## Configure the project
This example can be configured to run on ESP32 and Linux target to communicate over IPv4 and IPv6.

```
idf.py menuconfig
```

Set following parameters under ```Example Configuration``` Options:

* Set `IP version` of example to be IPV4 or IPV6.

* Set `IPV4 Address` in case your chose IP version IPV4 above.

* Set `IPV6 Address` in case your chose IP version IPV6 above.
    * For IPv6 there's an additional option for ```Interface selection```.
    * Enter the name of the interface to explicitly establish communication over a specific interface.
    * On selecting ```Auto``` the example will find the first interface with an IPv6 address and use it.

* Set `Port` number that represents remote port the example will connect to.

Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.


Note: please replace `192.168.0.167 3333` with desired IPV4/IPV6 address (displayed in monitor console) and port number in the following command.


## How to use example:

In order to create TCP server that communicates with TCP Client example, choose one of the following options.

There are many host-side tools which can be used to interact with the UDP/TCP server/client.
One command line tool is [netcat](http://netcat.sourceforge.net) which can send and receive many kinds of packets.

Ref to the [upper level README](../README.md#host-tools) for more information.

### TCP server using netcat
```
nc -l 192.168.0.167 3333
```

## Hardware Required

This example can be run on any commonly available ESP32 development board.
This example can also run on any Linux environment.


## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Troubleshooting

Start server first, to receive data sent from the client (application).
