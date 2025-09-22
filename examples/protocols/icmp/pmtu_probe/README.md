| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Path MTU Probe (ICMP)

This example probes a safe Path MTU (PMTU) using ICMP echo requests and applies it to the active network interface at runtime via `esp_netif_set_mtu()`.

It performs a simple binary search over the ICMP payload size to find the largest size that receives a reply within the timeout, then estimates the interface MTU as `payload + 28` (20-byte IPv4 header + 8-byte ICMP header) and sets that MTU on the default interface.

Notes:
- This is a pragmatic approach for constrained networks (e.g., LTE) where fragmentation or ICMP handling can be problematic. It is not a full standards PMTUD/PLPMTUD implementation.
- For IPv6 targets, lwIP’s ND6 manages per-destination PMTU internally. The interface MTU cap still bounds TCP effective MSS.
- DHCP or PPP may renegotiate MTU; re-run the probe or reapply the MTU after reconnect events if needed.

## How to use

1) Configure network and probe target:
- `idf.py menuconfig` → Example Connection Configuration → select Wi-Fi or Ethernet and credentials.
- `idf.py menuconfig` → Example Configuration → set "Probe Hostname/IP" (default: `www.espressif.com`).

2) Build/flash/monitor:
- `idf.py -p PORT flash monitor`

3) Example output:
```
I (xxx) pmtu_probe: Connecting network...
I (xxx) pmtu_probe: Probing PMTU to host www.espressif.com (IPv4)
I (xxx) pmtu_probe: Search range payload=[0, 1472]
I (xxx) pmtu_probe: Best payload=1392 -> MTU=1420
I (xxx) pmtu_probe: Applying MTU 1420 to default netif
```

## Implementation details

- Uses `esp_ping` (ping_sock) to attempt 1 echo per payload size and waits synchronously for success/timeout.
- Bounds the search using the current interface MTU when available, otherwise falls back to `1472`.
- Applies MTU via new API:
  - `esp_netif_set_mtu(esp_netif_t *netif, uint16_t mtu)`
  - `esp_netif_get_mtu(esp_netif_t *netif, uint16_t *mtu)`

