| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |


# TCP non-blocking client and server examples

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application aims to demonstrate a simple use of TCP sockets in a nonblocking mode.
It could be configured to run either a TCP server, or a TCP client, or both, in the project configuration settings.

## How to use example

The example is configured by default as the TCP client.

Note that the example uses string representation of IP addresses and ports and thus
could be used on both IPv4 and IPv6 protocols.

### TCP Client

In the client mode, the example connects to a configured hostname or address, sends the specified payload data and waits for a response,
then closes the connection. By default, it connects to a public http server and performs a simple http `GET` request.

### TCP Server

The server example creates a non-blocking TCP socket with the specified port number and polls till
a connection request from the client arrives.
After accepting a request from the client, a connection between server and client is
established, and the application polls for some data to be received from the client.
Received data are printed as ASCII text and retransmitted back to the client.

The server could listen on the specified interface (by the configured bound address) and serves multiple clients.
It resumes to listening for new connections when the client's socket gets closed.

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## Configure the project

```
idf.py menuconfig
```

Set following parameters under Example Configuration Options:

* Set `EXAMPLE_TCP_SERVER` to use the example as a non-blocking TCP server
    * Configure `EXAMPLE_TCP_SERVER_BIND_ADDRESS` to a string representation of the address to bind the server socket to.
    * Configure `EXAMPLE_TCP_SERVER_BIND_PORT` to the port number.

* Set `EXAMPLE_TCP_CLIENT` to use the example as a non-blocking TCP client
    * Configure `EXAMPLE_TCP_CLIENT_CONNECT_ADDRESS` to a string representation of the address to connect the client to.
    * Configure `EXAMPLE_TCP_CLIENT_CONNECT_PORT` to the port number.

* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Troubleshooting

Follow the same troubleshooting instruction as for the standard [TCP sever](../tcp_server/README.md) and the [TCP client](../tcp_client/README.md),
using the host tools and scripts as described in the upper level documentation on [BSD socket examples](../README.md).
