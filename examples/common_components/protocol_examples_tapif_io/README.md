# tapif-io Component

This component implements a tap networking interface that provides connectivity to host network using `tuntap` interface in Linux.
It could be used to route lwip traffic to host side network, typically when working with the **Linux target**.

## How to use this component

### Usage of the API

1) Add the path to this component to as a dependency to the `main` component of your project using the following idf_component.yml file:
```yaml
dependencies:
  tapif_io:
    path: ${IDF_PATH}/examples/common_components/tapif_io
```
2) Include lwip and linux side of the configuration
```cpp
#include "esp_netif.h"      // esp-netif
#include "tapio.h"          // esp-netif's driver side
#include "lwip/tapif.h"     // esp-netif's network stack side
```
3) Configure the esp-netif
   a) setup the linux tap I/O config
```cpp
    esp_netif_driver_ifconfig_t driver_cfg = {
            .handle = tapio_create(),
            .transmit = tapio_output,
    };
```

  b) configure the lwip netif for the tap interface
```cpp
    struct esp_netif_netstack_config stack_cfg = {
        .lwip = {
            .init_fn = lwip_tapif_init,
            .input_fn = lwip_tapif_input,
        }
    };
```

  c) configure the esp-netif basic parameters
```cpp
    esp_netif_inherent_config_t base_cfg = {
        .if_key = "TAP",                    // unique name of the interface
        .flags = ESP_NETIF_FLAG_AUTOUP,     // no dhcp client, starts when it's set up
        .ip_info = &ip_info,                // add static IP info
        .route_prio = 100                   // priority for setting default gateway
    };
```


4) Initialize and attach the esp_netif to the I/O handle
```cpp
    esp_netif_t *tap_netif = esp_netif_new(&cfg);
    esp_netif_attach(tap_netif, driver_cfg.handle);
```

### Host side networking

1) Create a new tun/tap interface type named `tap0`
   a) You can run the script `./make_tap_netif`
   b) Update the IP address of the interface to correspond to the configured static IP in previous step

2) Start the application and send/receive the packets via `tap0` interface
  * it is possible to create server or client test application listening or connecting to this interface.
  * it is also possible to route these packets to external network (using routing rules or simply by ip forwarding if using the same subnet)

#### Common networking/routing examples

##### Isolated internal connection

Is useful to experiment with one interface with no intention to connect to internet or external facilities.
Typically, when we want to create a server listening on the `tap0` interface and run a client in lwip, e.g. the default `tcp_client` socket example in IDF.
* Create the tap interface using `./make_tap_netif` and set the IP address **not to overlap** with any other IPv4 network range (e.g. `ip addr add 192.168.5.1/24 dev tap0`)
* Configure the `tapif_io` component to use static address from that range (e.g. `192.168.5.x`)
* Configure the `tcp_client` example to connect to the tap interface IP  address (e.g. `192.168.5.1`)
* Execute a tcp server listening on the tap interface and the configured port (e.g. `nc -l 3333`)
* Build and run the `tcp_client` example to send and receive data between the server created in the previous step.

##### Connecting to the external network using IP forwarding

This allows using full-featured network facilities of your host network, but a care must be taken to the selected IP addresses to avoid potential conflicts.
* Set the IP address of the `tap0` interface from the range used by your host system's default gateway (e.g. `ip addr add 192.168.0.123/24 dev tap0`, assuming the default netif is `eth0` with IP range of `192.168.0.x` and this address doesn't overlap with any other IP address in this network)
* Configure the `tapif_io` with another address from the same range, e.g.
```text
CONFIG_EXAMPLE_CONNECT_TAPIF_IP_ADDR="192.168.0.100"
CONFIG_EXAMPLE_CONNECT_TAPIF_NETMASK="255.255.255.0"
CONFIG_EXAMPLE_CONNECT_TAPIF_GW="192.168.0.1"
```
assuming that the default gateway of your host network is configured to `192.168.0.1`
* Build and run the lwip example to interact with the host network, e.g, to send an HTTP request to a publicly available http server (if the server is reachable from your host network)

(Note, that the IP forwarding must be enabled in the host system:
```bash
echo 1 > /proc/sys/net/ipv4/ip_forward
```
)

##### Routing the internal interface to the host network with IP tables

Uses an isolated interface with routing and NAT-ing between interfaces
* Configure the `tap0` interface address **not to overlap** with any other IPv4 network range.
* Setup `MASQUERADE` target to route network traffic between `tap0` and your default network interface (`eth0` in the below example). 
```bash
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
sudo iptables -A FORWARD -i eth0 -o tap0 -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -A FORWARD -i tap0 -o eth0 -j ACCEPT
```

##### Using DHCP

It's also possible to configure the lwip interface to use DHCP client (common setup for most default network interfaces, such as Ethernet or WiFi station)
and set up a DHCP server on the host machine to assign the IP address dynamically.

This component sets up a DHCP client if `CONFIG_EXAMPLE_CONNECT_WAIT_FOR_IP` is enabled and waits for assigning an IP address. See below the description of DHCP client workflow for tap interface:

1) **Configure and set the `esp-netif` up**

* Same as in [API usage](#Usage-of-the-API), but update the base esp-netif config `3c)` to enable DHCP client
```cpp
    esp_netif_inherent_config_t base_cfg = {
            .if_key = "TAP",
            .flags = (esp_netif_flags_t)(ESP_NETIF_DHCP_CLIENT | ESP_NETIF_FLAG_EVENT_IP_MODIFIED | ESP_NETIF_FLAG_AUTOUP),
            .route_prio = 100
    };
```
* After starting the netif, tell the lwIP that we're connected
```cpp
    esp_netif_action_connected(tap_netif, 0, 0, 0);
```
* Wait for the IP address to be assigned.
This could be implemented using an event handler
```cpp
    esp_netif_inherent_config_t base_cfg = {
        ...
       .get_ip_event = TAP0_GOT_IP,
       ...
    };
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, TAP0_GOT_IP, event_handler, NULL));
    // wait for the IP event (e.g. using signalling semaphores from the handler)
    // ...
    esp_netif_ip_info_t ip_info = {};
    ESP_LOGI("tap-init", "Assigned IP address:"IPSTR ",", IP2STR(&ip_info.ip));
```

2) **Configure forwarding/routing** if needed based on the previous sections.

3) **Configure the DHCP server on the host machine**

Example for `isc-dhcp-server`

```bash
INTERFACES="tap0";
authoritative;

subnet 192.168.5.0 netmask 255.255.255.0 {
  range 192.168.5.2 192.168.5.200;
  option routers 192.168.5.1;
  option domain-name-servers 8.8.8.8;
}
```
