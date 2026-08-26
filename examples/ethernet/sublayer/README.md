| Supported Targets | ESP32 | ESP32-P4 | ESP32-S31 |
| ----------------- | ----- | -------- | --------- |

# Ethernet Sublayer Example

Demonstrates the experimental Ethernet netif sublayer (`esp_eth_sublayer`): one physical Ethernet driver shared by an untagged `esp_netif` (DHCP) and a tagged 802.1Q VLAN `esp_netif` (static IP).

Uses the [ethernet_init](https://components.espressif.com/components/espressif/ethernet_init/) component for driver setup.

## How to use example

### Configure the project

```
idf.py menuconfig
```

See common configurations for Ethernet examples from [upper level](../README.md#common-configurations).

This example requires enabling `CONFIG_IDF_EXPERIMENTAL_FEATURES` and `CONFIG_ETH_SUBLAYER_SUPPORT`.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

### Setup VLAN interface on Linux

Create a tagged 802.1Q interface on the host so you can reach the ESP VLAN netif (defaults: VID `20`, IP `192.168.20.10`):

```bash
# Replace eth0 with your host Ethernet interface (e.g. enp3s0)
sudo ip link add link eth0 name eth0.20 type vlan id 20
sudo ip addr add 192.168.20.1/24 dev eth0.20
sudo ip link set eth0.20 up

ping 192.168.20.10
```

To remove it later:

```bash
sudo ip link delete eth0.20
```

Match `id` / address to **Example Configuration** if you changed the defaults. The physical link between the host and the board must carry tagged frames (direct cable, or a switch port in trunk/hybrid mode).

See [upper-level Ethernet README](../README.md) for hardware notes and common troubleshooting.
