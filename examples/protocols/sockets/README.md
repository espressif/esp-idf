
# BSD Socket API Examples

This directory contains simple examples demonstrating BSD Socket API. 
Each example, contains README.md file with mode detailed informations about that particular example.
For more general informations about all examples, see the README.md file in the upper level 'examples' directory.
Examples:

* UDP Client - The application creates UDP socket and sends message to the predefined port and IP address. After the server's reply, the application prints received reply as ASCII text, waits for 2 seconds and sends another message.

* UDP Server - The application creates UDP socket with the specified port number and waits for the data to be received. Received data are printed as ASCII text and retransmitted back to the client.

* TCP Client - The application creates a TCP socket and tries to connect to the server with predefined IP address and port number. When a connection is successfully established, the application sends message and waits for the answer. After the server's reply, application prints received reply as ASCII text, waits for 2 seconds and sends another message.

* TCP Server - The application creates a TCP socket with the specified port number and waits for a connection request from the client. After accepting a request from the client, connection between server and client is established and the application waits for some data to be received from the client. Received data are printed as ASCII text and retransmitted back to the client.

* UDP Multicast - The application shows how to use the IPV4 & IPV6 UDP multicast features via the BSD-style sockets interface.

Standard BSD API documentation:
http://pubs.opengroup.org/onlinepubs/007908799/xnsix.html

Other references:
https://csperkins.org/teaching/2007-2008/networked-systems/lecture04.pdf
http://wiki.treck.com/Introduction_to_BSD_Sockets


## Host tools

There are many host-side tools which can be used to interact with the UDP/TCP server/client example. 
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

### UDP server using netcat
```
nc -u -l 192.168.0.167 -p 3333
```

### TCP client using netcat
```
nc 192.168.0.167 3333
```

### TCP server using netcat
```
nc -l 192.168.0.167 -p 3333
```

### Python scripts
Each script contains port number, IP version (IPv4 or IPv6) and IP address (only clients) that has to be altered to match the values used by the application. Example:

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
make menuconfig
```

* Set serial port under Serial Flasher Options.

* Specific configuration for each example can be found in its README.md file.

## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.



