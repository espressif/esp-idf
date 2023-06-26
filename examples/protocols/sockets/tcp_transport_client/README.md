| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |


# TCP TRANSPORT Client example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application creates a TCP connection using `tcp_transport` component and tries to connect to the server with predefined IP address and port number. When a connection is successfully established, the application sends message and waits for the answer. After the server's reply, application prints received reply as ASCII text, waits for 2 seconds and sends another message.

It's possible to enable SOCKS proxy support to make the connection to go through a proxy server. 

## How to use example

In order to create TCP server that communicates with TCP TRANSPORT Client example, it's possible to choose some host-side tool or one of the scripts available in the example parent directory.

There are many host-side tools which can be used to interact with the UDP/TCP server/client. 
One command line tool is [netcat](http://netcat.sourceforge.net) which can send and receive many kinds of packets. 

In addition to those tools, simple Python scripts can be found under the example parent directory. Every script is designed to interact with one of the examples.

### TCP server using netcat
```
nc -l 0.0.0.0 -p 3333
```

In this scenario netcat doesn't send any data back. If the python scripts are used to test the monitor shows the message sent from the server. 

### Using a proxy server

A simple way of testing the usage of the proxy client on Linux systems is 
to use ssh as a proxy:

```
ssh -N -v -D 0.0.0.0:1080 localhost
```

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## Configure the project

```
idf.py menuconfig
```

Set following parameters under Example Configuration Options:

* Set `Target server Address` that represents remote host the example will connect to.

* Set `Target Port` number that represents remote port the example will connect to.

Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### Enabling proxy client

Set `Enable proxy client`
Set `Proxy server address` that represents the proxy server the example will connect to.
Set `Proxy server port` number that represents the proxy server port the example will connect to.

## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Troubleshooting

Start server first, to receive data sent from the client (application).
