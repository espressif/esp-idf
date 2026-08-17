| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# OpenThread + Zigbee Multipan Example

This example starts the OpenThread and Zigbee stacks side-by-side on a single host SoC, sharing one 802.15.4 RCP over UART. The host runs both stacks; the RCP firmware multiplexes them via Spinel IIDs.

## What It Demonstrates

- Shared RCP backend initialization via `esp_radio_spinel_multipan_init()` before either stack starts. This opens the UART RCP once; OpenThread and Zigbee then attach as clients.
- OpenThread stack startup via `esp_openthread_start()`
- Zigbee 2.0 stack startup via `esp_zigbee_init()` + `esp_zigbee_start()`
- Shared UART-RCP usage with OpenThread multipan (`CONFIG_OPENTHREAD_MULTIPAN_HOST_ENABLE=y`)
- Auto-start of the OpenThread Border Router on top of multipan

## Hardware Setup

You need two chips:

- Host chip running this example
- 802.15.4 RCP chip running `examples/openthread/ot_rcp`, built with:
  - `CONFIG_IEEE802154_MULTI_PAN_ENABLE=y`
  - `CONFIG_OPENTHREAD_MULTIPAN_RCP_ENABLE=y`

Default UART wiring:

| Host pin | RCP pin |
| -------- | ------- |
| `GPIO4`  | `TX`    |
| `GPIO5`  | `RX`    |
| `GND`    | `GND`   |

The current RCP cannot receive on two 802.15.4 channels at once, so Zigbee and OpenThread must use the same channel. This example forms Zigbee on channel 13. Shared UART pins/port/baud and OT/ZB defaults are in [`main/esp_multipan_config.h`](main/esp_multipan_config.h).

## Build and Flash

Flash the RCP first, then this example on the host:

```bash
# RCP — enable CONFIG_IEEE802154_MULTI_PAN_ENABLE and CONFIG_OPENTHREAD_MULTIPAN_RCP_ENABLE
cd $IDF_PATH/examples/openthread/ot_rcp
idf.py set-target <rcp-target>
idf.py menuconfig
# Component config → IEEE 802.15.4 → Enable multi-pan feature for frame filter
# Component config → OpenThread → Thread Core Features → Enable Multipan RCP
idf.py build flash

# Host
cd $IDF_PATH/examples/openthread/ot_zb_multipan
idf.py set-target <your-target>
idf.py build flash monitor
```

## Default Behaviour

`sdkconfig.defaults` ships with:

- `CONFIG_OPENTHREAD_MULTIPAN_HOST_ENABLE=y`
- `CONFIG_OPENTHREAD_BORDER_ROUTER=y`
- `CONFIG_ZB_ZCZR=y` (Zigbee coordinator/router)
- `CONFIG_ZB_RADIO_SPINEL_UART=y` (Zigbee uses the shared UART RCP)

On boot you should see, in order:

1. `OpenThread stack started`.
2. `Zigbee stack started` followed by `Zigbee network formed: pan_id=...`.

## Testing Zigbee Join

Flash another ESP32 (capable of 802.15.4) with `examples/zigbee/light_sample/HA_on_off_light`. After this host forms the Zigbee network and opens the network for 180 s, the light sample should join automatically.

Then bring up Wi-Fi and Thread the usual way:

```
> ot wifi connect -s <ssid> -p <password>
> ot dataset init new
> ot dataset channel 13
> ot dataset commit active
> ot ifconfig up
> ot thread start
```
