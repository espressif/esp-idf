# protocol_example_connect

This component implements the most common connection methods for ESP32 boards. It should be used mainly in examples of ESP-IDF to demonstrate functionality of network protocols and other libraries, that need the connection step as a prerequisite.

## How to use this component

Choose the preferred interface (WiFi, Ethernet, PPPoS) to connect to the network and configure the interface.

It is possible to enable multiple interfaces simultaneously making the connection phase to block until all the chosen interfaces acquire IP addresses.
It is also possible to disable all interfaces, skipping the connection phase altogether.

### WiFi

Choose WiFi connection method (for chipsets that support it) and configure basic WiFi connection properties:
* WiFi SSID
* WiFI password
* Maximum connection retry (connection would be aborted if it doesn't succeed after specified number of retries)
* WiFi scan method (including RSSI and authorization mode threshold)



### Ethernet

Choose Ethernet connection if your board supports it. The most common settings is using Espressif Ethernet Kit, which is also the recommended HW for this selection. You can also select an SPI ethernet device (if your chipset doesn't support internal EMAC or if you prefer). It is also possible to use OpenCores Ethernet MAC if you're running the example under QEMU.

### PPP

Point to point connection method creates a simple IP tunnel to the counterpart device (running PPP server), typically a Linux machine with pppd service. We currently support only PPP over Serial (using UART or USB CDC). This is useful for simple testing of networking layers, but with some additional configuration on the server side, we could simulate standard model of internet connectivity. The PPP server could be also represented by a cellular modem device with pre-configured connectivity and already switched to PPP mode (this setup is not very flexible though, so we suggest using a standard modem library implementing commands and modes, e.g. [esp_modem](https://components.espressif.com/component/espressif/esp_modem) ).

> [!Note]
> Note that if you choose USB device, you have to manually add a dependency on `esp_tinyusb` component. This step is necessary to keep the `protocol_example_connect` component simple and dependency free. Please run this command from your project location to add the dependency:
> ```bash
> idf.py add-dependency espressif/esp_tinyusb^1
> ```

#### Setup a PPP server

Connect the board using UART or USB and note the device name, which would be typically:
* `/dev/ttyACMx` for USB devices
* `/dev/ttyUSBx` for UART devices

Run the pppd server:

```bash
sudo pppd /dev/ttyACM0 115200 192.168.11.1:192.168.11.2 ms-dns 8.8.8.8 modem local noauth debug nocrtscts nodetach +ipv6
```

Please update the parameters with the correct serial device, baud rate, IP addresses, DNS server, use `+ipv6` if `EXAMPLE_CONNECT_IPV6=y`.

#### Connection to outside

In order to access other network endpoints, we have to configure some IP/translation rules. The easiest method is to setup a masquerade of the PPPD created interface (`ppp0`) to your default networking interface (`${ETH0}`). Here is an example of such rule:

```bash
sudo iptables -t nat -A POSTROUTING -o ${ETH0} -j MASQUERADE
sudo iptables -A FORWARD -i ${ETH0} -o ppp0 -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -A FORWARD -i ppp0 -o ${ETH0} -j ACCEPT
```
