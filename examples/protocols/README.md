# Protocols Examples

Implementation of internet communication protocols and services.

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.

## Establishing Wi-Fi or Ethernet Connection

### About the `example_connect()` Function

Protocols examples use a simple helper function, `example_connect()`, to establish Wi-Fi and/or Ethernet connection. This function is implemented in [examples/common_components/protocol_examples_common/include/protocol_examples_common.h](../common_components/protocol_examples_common/include/protocol_examples_common.h), and has a very simple behavior: block until connection is established and IP address is obtained, then return. This function is used to reduce the amount of boilerplate and to keep the example code focused on the protocol or library being demonstrated.

The simple `example_connect()` function does not handle timeouts, does not gracefully handle various error conditions, and is only suited for use in examples. When developing real applications, this helper function needs to be replaced with full Wi-Fi / Ethernet connection handling code. Such code can be found in [examples/wifi/getting_started/](../wifi/getting_started) and [examples/ethernet/basic/](../ethernet/basic) examples.

### Configuring the Example

To configure the example to use Wi-Fi, Ethernet or both connections, open the project configuration menu (`idf.py menuconfig`) and navigate to "Example Connection Configuration" menu. Select either "Wi-Fi" or "Ethernet" or both in the "Connect using" choice.

When connecting using Wi-Fi, enter SSID and password of your Wi-Fi access point into the corresponding fields. If connecting to an open Wi-Fi network, keep the password field empty.

When connecting using Ethernet, set up PHY type and configuration in the provided fields. If using Ethernet for the first time, it is recommended to start with the [Ethernet example readme](../ethernet/basic/README.md), which contains instructions for connecting and configuring the PHY. Once Ethernet example obtains IP address successfully, proceed to the protocols example and set the same configuration options.

### IP protocol handling

By default the examples support any network configuration: IPv4 only, IPv6 only, and dual stack.

The `example_connect()` function waits until Wi-Fi or Ethernet connection is established, and either an IPv4 address or an IPv6 global scope address is obtained. By waiting for the first (of either), this allows it to work in any network.

The behaviour is based on the available network:

| Network | Addresses | DNS | IPv4 destination | IPv6 destination | Dual-stack destination
| -- | -- | -- | -- | -- | -- 
| IPv4 only | IPv4, IPv6 link-local | IPv4 | Yes | Not possible | Uses IPv4
| IPv6 only | IPv6 global, IPv6 link-local | IPv6 | NAT64 if available | Yes | Uses IPv6
| Dual-stack | IPv6 global (*1), IPv4, IPv6 link-local | IPv6, then IPv4 | Yes | Yes | Uses IPv6

(*1) There may be a delay waiting for an IPv6 global address, in which case IPv4 may be used for the first test request (with IPv4 DNS)

#### Disabling protocol versions

Where the target network is unknown, such as this sample, using an adaptable configuration that supports all networks (whatever is available) is best.

However in some cases you may want to restrict network configuration.

For example the Matter standard requires IPv6, so you can reduce application size and simplify development and testing complexity by completely disabling IPv4 (`LWIP_IPV4`), as it is never needed for a Matter device.
