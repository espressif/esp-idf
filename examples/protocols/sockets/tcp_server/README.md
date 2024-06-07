| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |


# TCP Server example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application creates a TCP socket with the specified port number and waits for a connection request from the client. After accepting a request from the client, connection between server and client is established and the application waits for some data to be received from the client. Received data are printed as ASCII text and retransmitted back to the client.

## How to use example

In order to create TCP client that communicates with TCP server example, choose one of the following options.

There are many host-side tools which can be used to interact with the UDP/TCP server/client.
One command line tool is [netcat](http://netcat.sourceforge.net) which can send and receive many kinds of packets.
Note: please replace `192.168.0.167 3333` with desired IPV4/IPV6 address (displayed in monitor console) and port number in the following command.

Ref to the [upper level README](../README.md#host-tools) for more information.

### TCP client using netcat
```
nc 192.168.0.167 3333
```

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## Configure the project

```
idf.py menuconfig
```

Set following parameters under Example Configuration Options:

* Set `IP version` of the example to be IPV4 or IPV6.

* Set `Port` number of the socket, that server example will create.

* Set `TCP keep-alive idle time(s)` value of TCP keep alive idle time. This time is the time between the last data transmission.

* Set `TCP keep-alive interval time(s)` value of TCP keep alive interval time. This time is the interval time of keepalive probe packets.

* Set `TCP keep-alive packet retry send counts` value of TCP keep alive packet retry send counts. This is the number of retries of the keepalive probe packet.

Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Troubleshooting

Start server first, to receive data sent from the client (application).
