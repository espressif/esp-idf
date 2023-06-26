| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |


# UDP Client example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application creates UDP socket and sends message to the predefined port and IP address. After the server's reply, the application prints received reply as ASCII text, waits for 2 seconds and sends another message.

## How to use example

In order to create UDP server that communicates with UDP Client example, choose one of the following options.

There are many host-side tools which can be used to interact with the UDP/TCP server/client.
One command line tool is [netcat](http://netcat.sourceforge.net) which can send and receive many kinds of packets.
Note: please replace `192.168.0.167 3333` with desired IPV4/IPV6 address (displayed in monitor console) and port number in the following commands.

Ref to the [upper level README](../README.md#host-tools) for more information.

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
nc -u -l 192.168.0.167 3333
```

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## Configure the project

```
idf.py menuconfig
```

Set following parameters under Example Configuration Options:

* Set `IP version` of example to be IPV4 or IPV6.

* Set `IPV4 Address` in case your chose IP version IPV4 above.

* Set `IPV6 Address` in case your chose IP version IPV6 above.

* Set `Port` number that represents remote port the example will send data and receive data from.

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

## Running the example for Linux target

This example could be executed on host system, using lwIP port for linux and FreeRTOS simulator on linux. The socket API used in this example directly calls lwIP implementation. Follow the steps below to configure, build and run the example on linux operating system.

1. First configure the target (please note that using linux target is currently available only in `preview` stage)
```
idf.py --preview set-target linux
```
2. Configure the project
```
idf.py menuconfig
```
Choose connection capabilities in `Example Connection Configuration` menu:

* By default, the `example_connect()` function returns as a no-op, expecting that the connection is already available. This option is preferred when we don't have to interact with outside networking layers and use only lwIP internal interface, such as loopback netif (`lo`).
* If you want to connect lwIP network interface to the host system networking, set `EXAMPLE_CONNECT_LWIP_TAPIF`.
    * Configure the interface address information (IP address, GW address and netmask).
    * Create a host network interface named `tap0` of *TAP* type. You can use the `./make_tap_netif` script located in the `tapif_io` component directory.
    * Optionally set input or output packet loss rate to simulate loosing data of physical interfaces.
    * Note about the host side networking:
    * This example uses static IP address configured in `tapif_io` component configuration.
    * Use the IP ranges that do not overlap with any other IP range of the host system.
    * Make sure that the same IP range is configured in `tap0` interface created by tge `./make_tap_netif` script.
    * You can leave the defaults in the script and `tapif_io` settings unless any other host network interface uses `192.168.5.x` range.
    * Read more about host-side networking in the [`tapif_io` component documentation](../../../common_components/protocol_examples_tapif_io/README.md).

3. Generate partition table for the application:
```
idf.by partition-table
```

4. Build and run the example the usual way (Note that the flash step is left out)
```
idf.by build
idf.py monitor
```
