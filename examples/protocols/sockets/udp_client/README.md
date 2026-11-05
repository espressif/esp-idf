| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |


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

This example can run on the host using the lwIP port for Linux and the FreeRTOS simulator. Socket APIs call into the real lwIP stack (not the host OS TCP/IP stack). `examples/protocols/sockets/tcp_client` on Linux uses native host sockets instead. See also [Running lwIP on Linux](../../../../docs/en/api-guides/lwip.rst) in the lwIP API guide.

To configure, build, and run the example on Linux, follow the steps below.

1. Configure the target (the Linux target is currently available only in `preview` stage):
```
idf.py --preview set-target linux
```
2. Configure the project:
```
idf.py menuconfig
```
Choose connection capabilities in `Example Connection Configuration`:

* `EXAMPLE_CONNECT_LWIP_TAPIF` is enabled by default. `example_connect()` then attaches lwIP to a host TAP interface named `tap0`.
    * Create that interface with the `./make_tap_netif` script in the `protocol_examples_tapif_io` component directory.
    * `EXAMPLE_CONNECT_WAIT_FOR_IP` is also enabled by default, so the example runs a DHCP client and waits for an address. Provide a DHCP server on `tap0`, or disable this option and set a static IP address, gateway, and netmask.
    * Use an IP range that does not overlap with other host interfaces. The static-IP defaults in `tapif_io` use `192.168.5.x`; keep the same range on `tap0` (the script defaults are fine unless another interface already uses that range).
    * Optionally set input or output packet loss to simulate a lossy link.
    * See the [`tapif_io` component documentation](../../../common_components/protocol_examples_tapif_io/README.md) for host routing, NAT, and DHCP server examples.
* `sdkconfig.defaults.linux` disables `EXAMPLE_CONNECT_LWIP_TAPIF`. `example_connect()` is then a no-op, and traffic stays on lwIP-internal interfaces such as loopback (`lo`).
* Enable `EXAMPLE_CONNECT_LWIP_TAPIF` to attach lwIP to a host TAP interface named `tap0`.
    * Create that interface with the `./make_tap_netif` script in the `protocol_examples_tapif_io` component directory.
    * `EXAMPLE_CONNECT_WAIT_FOR_IP` is enabled by default in that case, so the example runs a DHCP client and waits for an address. Provide a DHCP server on `tap0`, or disable this option and set a static IP address, gateway, and netmask.
    * Use an IP range that does not overlap with other host interfaces. The static-IP defaults in `tapif_io` use `192.168.5.x`; keep the same range on `tap0` (the script defaults are fine unless another interface already uses that range).
    * Optionally set input or output packet loss to simulate a lossy link.
    * See the [`tapif_io` component documentation](../../../common_components/protocol_examples_tapif_io/README.md) for host routing, NAT, and DHCP server examples.

3. Generate the partition table for the application:
```
idf.py partition-table
```

4. Build and run the example as usual (flash is not used on Linux):
```
idf.py build
idf.py monitor
```
