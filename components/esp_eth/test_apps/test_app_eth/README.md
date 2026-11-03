| Supported Targets | ESP32 | ESP32-P4 | ESP32-S31 |
| ----------------- | ----- | -------- | --------- |

# Ethernet Driver Test

Generic Ethernet tests that apply to all Ethernet drivers. This app uses the [`espressif/eth_test_app`](https://components.espressif.com/components/espressif/eth_test_app) component, which initializes hardware through [`espressif/ethernet_init`](https://components.espressif.com/components/espressif/ethernet_init).

Internal ESP EMAC-specific tests live in `../test_app_emac`.

## Prerequisites

Install third-party Python packages:

```bash
pip install scapy
```

Have permission to use RAW sockets, or set:

```bash
sudo setcap 'CAP_NET_RAW+eip' $(readlink -f $(which python))
```

## Chip-Specific Configuration

PHY-specific Kconfig options (10 Mbps loopback, 1000 Mbps, MAC address class, and similar) come from `eth_test_app` and are set in the `sdkconfig.ci.*` presets. See the [eth_test_app README](https://github.com/espressif/esp-eth-drivers/blob/master/eth_test_app/README.md) for the full list.
