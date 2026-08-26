| Supported Targets | ESP32 | ESP32-S31 |
| ----------------- | ----- | --------- |

# ESP-IDF BT-PAN-GN Demo

## Required components

- [bt_app_core_utils](../common/bt_app_core_utils)

This example runs as a **Group Network (GN)** hub only. It stays discoverable, accepts incoming **PANU** connections (up to 4 in this example), and bridges Ethernet frames at the Bluetooth PAN layer so connected clients can reach the hub and each other.

## Topology

```
                    +------------------+
                    |  ESP32 (GN hub)  |
                    |  BT: ESP_PAN_GN  |
                    |  IP: 192.168.100.1|
                    +--------+---------+
                             |
              +--------------+--------------+
              | PANU         | PANU         |
    +---------+----+   +-----+--------+
    | Client A     |   | Client B     |
    | .100.10      |   | .100.11      |
    +--------------+   +--------------+
```

- **This example**: GN role only; static IPv4 `192.168.100.1/24` (no DHCP server).
- **PANU clients**: any peer that connects as PANU → GN (e.g. another ESP app, or a host that supports Bluetooth PAN). Clients need addresses on `192.168.100.0/24` (static IP if the peer has no DHCP).
- **L2 bridging** between clients is done inside the Bluetooth PAN stack on the hub.

## Configure the Project

```
idf.py menuconfig
```

1. Enable PAN: `Component config → Bluetooth → Bluedroid Options → PAN`
2. Optional: `PAN GN Example Configuration → GN Hub Bluetooth name` (default `ESP_PAN_GN`)

## Build and Flash

```
idf.py -p PORT flash monitor
```

Power on the hub first, then connect PANU clients to `ESP_PAN_GN` with `esp_pan_connect(bda, ESP_PAN_ROLE_PANU, ESP_PAN_ROLE_GN)`.

## Expected log

```
I PAN_GN_DEMO: GN hub ready, waiting for PANU clients...
I PAN_GN_DEMO: PANU connected handle=1 peer=[...]
I PAN_NETIF: PAN link up, handle=1, links=1
```

## Notes

- Private PAN LAN only; no Internet uplink. For phone tethering see `bt_pan_panu` / `bt_pan_nap`.
- Pairing may be requested on first connect; the example auto-accepts SSP confirm.
