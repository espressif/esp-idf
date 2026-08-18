| Supported Targets | ESP32 | ESP32-S31 |
| ----------------- | ----- | --------- |

# ESP-IDF BT-PAN-PANU Demo

## Required components

- [bt_app_core_utils](../common/bt_app_core_utils)

This example shows how to use the **Personal Area Networking (PAN)** profile as **PANU** (PAN User) to connect to a **NAP** (Network Access Point).

By default it looks for the Bluetooth name `ESP_PAN_NAP`, which matches the companion example `bt_pan_nap`. You can also connect to a phone that shares Internet via Bluetooth tethering.

The ESP32 discovers the NAP by Bluetooth name, pairs with it, establishes a PAN connection, obtains an IP address via DHCP through lwIP, and **pings a configurable target** to verify connectivity.

## Prerequisites

### Option A: ESP32 NAP (`bt_pan_nap`)

1. Flash and run `examples/bluetooth/bluedroid/classic_bt/bt_pan_nap` on another ESP32.
2. Ensure its Bluetooth name is `ESP_PAN_NAP` (the default).
3. Keep the default `Target NAP Bluetooth Name` in this example, or set it to match.

### Option B: Phone NAP

1. Pair the ESP32 with the phone in system Bluetooth settings (recommended), or let pairing happen during connection.
2. Enable **Bluetooth tethering** / **Bluetooth PAN** / **Internet sharing via Bluetooth** (wording varies by vendor).
3. Set the phone's Bluetooth name in menuconfig (`Target NAP Bluetooth Name`).

On Android: Settings → Network → Hotspot & tethering → Bluetooth tethering.

On iPhone: Settings → Personal Hotspot → enable *Allow Others to Join*, and ensure Bluetooth is on.

## Configure the Project

```
idf.py menuconfig
```

1. Enable PAN: `Component config → Bluetooth → Bluedroid Options → PAN (Personal Area Networking)`
2. Set NAP name: `PAN PANU Example Configuration → Target NAP Bluetooth Name` (default `ESP_PAN_NAP`)
3. Set ping target: `PAN PANU Example Configuration → Ping Target` (default `8.8.8.8`)

## Build and Flash

```
idf.py -p PORT flash monitor
```

## Example Flow

1. Initialize Classic Bluetooth, lwIP (`esp_netif`), and PAN (PANU role only).
2. Start GAP inquiry for the configured NAP name.
3. Call `esp_pan_connect()` as PANU to the peer's NAP service.
4. On `ESP_PAN_OPEN_EVT`, bring up the PAN netif and start DHCP client.
5. Feed received PAN Ethernet frames into lwIP; transmit lwIP frames via `esp_pan_write()`.
6. On `IP_EVENT_ETH_GOT_IP`, automatically ping the configured target.

## Expected log

```
I (1234) PAN_PANU_DEMO: Looking for NAP "ESP_PAN_NAP"
I (1234) PAN_PANU_DEMO: Will ping 8.8.8.8 after DHCP
I (5678) PAN_PANU_DEMO: PAN connected, handle=1, peer=[...], local_role=0x1, peer_role=0x4
I (5680) PAN_NETIF: PAN link up, DHCP client started
I (6200) PAN_NETIF: Got IP: 192.168.100.2, mask: 255.255.255.0, gw: 192.168.100.1
I (6200) PAN_NETIF: Start ping 8.8.8.8 (20 packets)
I (6450) PAN_NETIF: 64 bytes from 8.8.8.8 icmp_seq=1 ttl=118 time=180 ms
I (6700) PAN_NETIF: --- 8.8.8.8 ping statistics ---
I (6700) PAN_NETIF: 20 packets transmitted, 20 received, 0% packet loss, time 3200 ms
```

## Troubleshooting

| Symptom | Suggestion |
| ------- | ---------- |
| NAP not found | Check Bluetooth name in menuconfig (default `ESP_PAN_NAP`); move devices closer |
| Connect failed | Confirm NAP is running / phone tethering is on; delete old pairing and retry |
| PAN connected but no IP | Wait a few seconds; some NAPs delay DHCP |
| Ping timeout | Try `8.8.8.8` first; ensure uplink Internet is available |
| Pairing failed | Accept pairing on peer; check SSP/passkey logs |
