| Supported Targets | ESP32 | ESP32-S31 |
| ----------------- | ----- | --------- |

# ESP-IDF BT-PAN-NAP + Wi-Fi Uplink Demo

## Required components

- [bt_app_core_utils](../common/bt_app_core_utils)

The ESP device acts as **Bluetooth PAN NAP** (like a phone's Bluetooth tethering hotspot, but reversed): PANU devices connect over Bluetooth and reach the Internet through the ESP device's **Wi-Fi STA** link.

## Topology

```
  [Internet]
       |
  [Wi-Fi AP: myssid]     <-- sdkconfig: SSID / password
       |
  [ESP NAP]  IP on Wi-Fi: DHCP from AP
       |  NAPT + IP forward
  Bluetooth PAN (192.168.100.0/24, DHCP from ESP)
       |
  [Phone / PANU client]
```

- **Uplink**: Wi-Fi STA → configurable SSID (default `myssid`, password `mypassword`)
- **Downlink**: PAN NAP, DHCP server for PANU (`192.168.100.1/24`)
- **Forwarding**: lwIP NAPT on the PAN netif after Wi-Fi STA gets IP

## Configure the Project

```
idf.py menuconfig
```

| Menu | Option | Default |
|------|--------|---------|
| Bluetooth → Bluedroid | PAN | enabled |
| PAN NAP Wi-Fi Tether Example | Wi-Fi SSID | `myssid` |
| | Wi-Fi Password | `mypassword` |
| | Bluetooth device name | `ESP_PAN_NAP` |

## Build and Flash

```
idf.py -p PORT flash monitor
```

1. ESP connects to Wi-Fi first (`Wi-Fi uplink IP: ...`).
2. PAN NAP starts, Bluetooth name `ESP_PAN_NAP`.
3. On phone: pair with ESP, enable **Bluetooth tethering / PAN network** (wording varies) or connect as PANU to `ESP_PAN_NAP`.
4. Phone should get IP `192.168.100.x` from ESP and browse via Wi-Fi uplink. NAPT is enabled when the first PANU connects.

## Expected log

```
I PAN_NAP_DEMO: Connecting Wi-Fi SSID "myssid" ...
I PAN_NAP_DEMO: Wi-Fi uplink IP: 192.168.x.x, gw: ...
I PAN_NAP_DEMO: PAN NAP ready, waiting for PANU peers...
I PAN_NAP_DEMO: PANU connected handle=1 peer=[...]
I PAN_NETIF: NAPT enabled on PAN netif (traffic NAT via Wi-Fi uplink)
I PAN_NETIF: DHCP server started for PAN clients
I PAN_NETIF: PANU connected, handle=1, active_links=1
```

## Notes

- Requires **Wi-Fi + Classic BT coexistence** (BR/EDR only + `CONFIG_ESP_COEX_SW_COEXIST_ENABLE`).
- Not all phones expose PANU to third-party NAP; Android with Bluetooth PAN / tethering support works best.
- For phone hotspot use case (ESP as client), see `bt_pan_panu` instead.
- For multi-ESP private LAN without Internet, see `bt_pan_gn`.
