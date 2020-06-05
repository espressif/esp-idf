# Asio SSL client/server example

Simple Asio client and server with SSL/TLS transport

## How to Use Example

### Hardware Required

This example can be executed on any ESP platform board. No external connection is required, it is recommended though
to connect to internet or a local network via WiFi or Ethernet to easily exercise features of this example.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
* Enable the ASIO client and set server's host name to examine client's functionality. 
The ASIO client connects to the configured server and sends default payload string "GET / HTTP/1.1" 
* Enable the ASIO server to examine server's functionality. The ASIO server listens to connection and echos back what was received.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Client connecting to public server

The below output illustrates the client connecting to a public https server.

```
I (1267) example_connect: Waiting for IP(s)
I (2587) wifi:new:<11,0>, old:<1,0>, ap:<255,255>, sta:<11,0>, prof:1
I (3367) wifi:state: init -> auth (b0)
I (3377) wifi:state: auth -> assoc (0)
I (3387) wifi:state: assoc -> run (10)
I (3397) wifi:security type: 3, phy: bgn, rssi: -49
I (3397) wifi:pm start, type: 1
I (3457) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (4747) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:260a:xxxx:xxxx:xxxx, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5247) esp_netif_handlers: example_connect: sta ip: 192.168.32.69, mask: 255.255.252.0, gw: 192.168.32.3
I (5247) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.32.69
I (5257) example_connect: Connected to example_connect: sta
I (5257) example_connect: - IPv4 address: 192.168.32.69
I (5267) example_connect: - IPv6 address: fe80:0000:0000:0000:260a:xxxx:xxxx:xxxx, type: ESP_IP6_ADDR_IS_LINK_LOCAL
W (5277) esp32_asio_pthread: pthread_condattr_setclock: not yet supported!
W (5297) esp32_asio_pthread: pthread_condattr_setclock: not yet supported!
Reply: HTTP/1.1 200 OK
D
```
### Both server and client enabled

The below output demonstrates the client connecting to the ASIO server via loopback interface, so no WiFi, nor Ethernet connection
was established. 
```
I (0) cpu_start: App cpu up.
I (495) heap_init: Initializing. RAM available for dynamic allocation:
I (502) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (508) heap_init: At 3FFB5400 len 0002AC00 (171 KiB): DRAM
I (515) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (521) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (527) heap_init: At 4008BB80 len 00014480 (81 KiB): IRAM
I (534) cpu_start: Pro cpu start user code
I (556) spi_flash: detected chip: gd
I (556) spi_flash: flash io: dio
W (556) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (566) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (600) example_connect: Waiting for IP(s)
W (600) esp32_asio_pthread: pthread_condattr_setclock: not yet supported!
W (1610) esp32_asio_pthread: pthread_condattr_setclock: not yet supported!
W (1610) esp32_asio_pthread: pthread_condattr_setclock: not yet supported!
Server received: GET / HTTP/1.1


Reply: GET / HTTP/1.1
```
See the README.md file in the upper level 'examples' directory for more information about examples.
