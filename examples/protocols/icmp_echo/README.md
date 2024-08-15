| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ICMP Echo-Reply (Ping) example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Ping is a useful network utility used to test if a remote host is reachable on the IP network. It measures the round-trip time for messages sent from the source host to a destination target that are echoed back to the source.

Ping operates by sending Internet Control Message Protocol (ICMP) echo request packets to the target host and waiting for an ICMP echo reply.

This example implements a simple ping command line util based on the [console component](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/console.html).

**Notes:** Currently this example only supports IPv4.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

### Configure the project

```
idf.py menuconfig
```

In the `Example Connection Configuration` menu:

* Choose the network interface (Wi-Fi or Ethernet) used by this example under `Connect using`.
* If Wi-Fi interface is selected, you also have to set:
  * Wi-Fi SSID and Wi-Fi password that your board will connect to.
* If Ethernet interface is selected, you also have to set:
  * Select Ethernet type under `Ethernet Type`, for example, `Internal EMAC` or `SPI Ethernet Module`.
  * Select Ethernet PHY chip model under `Ethernet PHY Device`, for example, `IP101`.
  * You might also have to set other Ethernet driver specific parameters under `Component Config > Ethernet`, for example, EMAC Clock mode, GPIO used by SMI, and etc.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

* Run `help` to get manual of all supported commands.

```bash
esp32> help
help
  Print the list of registered commands

ping  [-W <t>] [-i <t>] [-s <n>] [-c <n>] [-Q <n>] <host>
  send ICMP ECHO_REQUEST to network hosts
  -W, --timeout=<t>  Time to wait for a response, in seconds
  -i, --interval=<t>  Wait interval seconds between sending each packet
  -s, --size=<n>  Specify the number of data bytes to be sent
  -c, --count=<n>  Stop after sending count packets
  -Q, --tos=<n>  Set Type of Service related bits in IP datagrams
        <host>  Host address
```

* Run `ping` command to test reachable of remote server.

```bash
esp32> ping www.espressif.com
64 bytes from 119.9.92.99 icmp_seq=1 ttl=51 time=36 ms
64 bytes from 119.9.92.99 icmp_seq=2 ttl=51 time=34 ms
64 bytes from 119.9.92.99 icmp_seq=3 ttl=51 time=37 ms
64 bytes from 119.9.92.99 icmp_seq=4 ttl=51 time=36 ms
64 bytes from 119.9.92.99 icmp_seq=5 ttl=51 time=33 ms

--- 119.9.92.99 ping statistics ---
5 packets transmitted, 5 received, 0% packet loss, time 176ms
```

* Run `ping` with a wrong domain name
```bash
esp32> ping www.hello-world.io
ping: unknown host www.hello-world.io
Command returned non-zero error code: 0x1 (ERROR)
```

* Run `ping` with an unreachable server
```bash
esp32> ping www.zoom.us
From 69.171.230.18 icmp_seq=1 timeout
From 69.171.230.18 icmp_seq=2 timeout
From 69.171.230.18 icmp_seq=3 timeout
From 69.171.230.18 icmp_seq=4 timeout
From 69.171.230.18 icmp_seq=5 timeout

--- 69.171.230.18 ping statistics ---
5 packets transmitted, 0 received, 100% packet loss, time 4996ms
```
