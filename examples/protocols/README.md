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

### Disabling IPv6

By default, `example_connect()` function waits until Wi-Fi or Ethernet connection is established, and IPv4 address and IPv6 link-local address are obtained. In network environments where IPv6 link-local address cannot be obtained, disable "Obtain IPv6 link-local address" option found in "Example Connection Configuration" menu.
