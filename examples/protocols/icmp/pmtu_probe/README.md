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
I (8236) example_common: Connected to example_netif_sta
I (8246) example_common: - IPv4 address: 192.168.0.35,
I (8276) pmtu_probe: Probing PMTU to host www.espressif.com (IPv4)
I (8276) pmtu_probe: Search range payload=[0, 1472]
I (8286) pmtu_probe: Trying payload once with size 736
I (8436) pmtu_probe: Trying payload once with size 1104
I (8586) pmtu_probe: Trying payload once with size 1288
I (8736) pmtu_probe: Trying payload once with size 1380
I (8886) pmtu_probe: Trying payload once with size 1426
I (9036) pmtu_probe: Trying payload once with size 1449
I (9186) pmtu_probe: Trying payload once with size 1461
I (9336) pmtu_probe: Trying payload once with size 1467
I (9486) pmtu_probe: Trying payload once with size 1470
I (9636) pmtu_probe: Trying payload once with size 1471
I (9786) pmtu_probe: Trying payload once with size 1472
I (9936) pmtu_probe: Best payload=1472 -> MTU=1500
I (9936) pmtu_probe: Applying MTU 1500 to default netif
I (9936) main_task: Returned from app_main()
```

## Implementation details

- Uses `esp_ping` (ping_sock) to attempt 1 echo per payload size and waits synchronously for success/timeout.
- Bounds the search using the current interface MTU when available, otherwise falls back to `1472`.
- Applies MTU via these API:
  - `esp_netif_set_mtu(esp_netif_t *netif, uint16_t mtu)`
  - `esp_netif_get_mtu(esp_netif_t *netif, uint16_t *mtu)`
