
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

In addition to those tools, There are some python scripts under `examples/protocols/sockets/scripts`. 
And scripts for automated tests named `pytest_xxx.py` can be found under each example directory. 


### Python Scripts Socket Tools

Python scripts under `examples/protocols/sockets/scripts` could be used to exercise the socket communication. 
Command line arguments such as IP version and IP address shall be supplied. Use `python xxxx.py --help` to see how to use these scripts. 

Examples:
```bash
# python run_tcp_client.py --help
python run_tcp_client.py 192.168.1.2 [--port=3333] [--message="Data to ESP"]
python run_tcp_client.py fe80::2%eth0 [--port=3333] [--message="Data to ESP"]
# python run_tcp_server.py --help
python run_tcp_server.py [--port=3333] [--ipv6]
```

### Python Scripts For Automated Tests

Script named `pytest_xxxx` in the application directory can be used for automated tests. 
They can also be run locally. Ref: [ESP-IDF Tests with Pytest Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/esp-idf-tests-with-pytest.html).

Example:
```bash
$ cd $IDF_PATH
$ bash install.sh --enable-pytest
$ . ./export.sh
$ cd examples/protocols/sockets/tcp_client
$ python $IDF_PATH/tools/ci/ci_build_apps.py . --target esp32 -vv --pytest-apps
$ pytest --target esp32
```

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

### Note about IPv6 addresses

Examples are configured to obtain multiple IPv6 addresses. The actual behavior may differ depending on the local network, typically the ESP gets assigned these two addresses

* Local Link address

* Unique Local address

The value and type of the IPv6 address is displayed in the terminal, for example:

Please make sure that when using the Local Link address, an interface id is included in the configuration:

* In the embedded code
```
    dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(esp_netif_instance);
```
* On the host

   - Interface name suffix is present when passing the address as a string, for example `fe80::260a:XXX:XXX:XXX%en0`
   - The interface id is present when passing the endpoint as tupple, for example `socket.connect(('fd00::260a:XXXX:XXXX:XXXX', 3333, 0, 3))`

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## Configure the project

```
idf.py menuconfig
```

* Specific configuration for each example can be found in its README.md file.

## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.



