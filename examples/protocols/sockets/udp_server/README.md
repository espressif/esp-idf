
# UDP Server example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application creates UDP socket with the specified port number and waits for the data to be received. Received data are printed as ASCII text and retransmitted back to the client.

## How to use example

In order to create UDP client that communicates with UDP server example, choose one of the following options.

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

### UDP client using netcat
```
nc -u 192.168.0.167 3333
```

### Python scripts
Script udpclient.py contains configuration for port number, IP version (IPv4 or IPv6) and IP address that has to be altered to match the values used by the application. Example:

```
PORT = 3333;
IP_VERSION = 'IPv4'
IPV4 = '192.168.0.167'
IPV6 = 'FE80::32AE:A4FF:FE80:5288'
```

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## Configure the project

```
idf.py menuconfig
```

Set following parameter under Serial Flasher Options:

* Set `Default serial port`.

Set following parameters under Example Configuration Options:

* Set `IP version` of the example to be IPV4 or IPV6.

* Set `Port` number that represents remote port the example will create.

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
