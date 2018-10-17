
# UDP Client example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application creates UDP socket and sends message to the predefined port and IP address. After the server's reply, the application prints received reply as ASCII text, waits for 2 seconds and sends another message.

## How to use example

In order to create UDP server that communicates with UDP Client example, choose one of the following options.

There are many host-side tools which can be used to interact with the UDP/TCP server/client. 
One command line tool is [netcat](http://netcat.sourceforge.net) which can send and receive many kinds of packets. 
Note: please replace `192.168.0.167 3333` with desired IPV4/IPV6 address (displayed in monitor console) and port number in the following commands.

In addition to those tools, simple Python scripts can be found under sockets/scripts directory. Every script is designed to interact with one of the examples.

### Send UDP packet via netcat
```
echo "Hello from PC" | nc -w1 -u 192.168.0.167 3333
```

### Receive UDP packet via netcat
```
echo "Hello from PC" | nc -w1 -u 192.168.0.167 3333
```

### UDP server using netcat
```
nc -u -l 192.168.0.167 -p 3333
```

### Python scripts
Script udpserver.py contains configuration for port number and IP version (IPv4 or IPv6) that has to be altered to match the values used by the application. Example:

```
IP_VERSION = 'IPv4'
PORT = 3333;
```

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## Configure the project

```
make menuconfig
```

Set following parameter under Serial Flasher Options:

* Set `Default serial port`.

Set following parameters under Example Configuration Options:

* Set `WiFi SSID` of the Router (Access-Point).

* Set `WiFi Password` of the Router (Access-Point).

* Set `IP version` of example to be IPV4 or IPV6.

* Set `IPV4 Address` in case your chose IP version IPV4 above.

* Set `IPV6 Address` in case your chose IP version IPV6 above.

* Set `Port` number that represents remote port the example will send data and receive data from.

## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Troubleshooting

Start server first, to receive data sent from the client (application).
