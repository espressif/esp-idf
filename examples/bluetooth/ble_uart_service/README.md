# BLE UART Service Example — NimBLE / Bluedroid

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

A turnkey serial-over-BLE peripheral that implements the de-facto
**BLE UART-over-GATT** layout (RX write, TX notify; fixed 128-bit UUIDs
below), so any widely used BLE-serial central (mobile GATT client apps,
Web Bluetooth examples, your own iOS / Android / Linux / Python scripts)
can talk to it unchanged.

The example ships with **two interchangeable backends** — NimBLE and
Bluedroid — both implementing the same stack-agnostic
`ble_uart.h` API. Pick one with `idf.py menuconfig → Component config →
Bluetooth → Host`; the build system links the matching backend
automatically. Default is NimBLE (smaller footprint).

The whole BLE stack — NVS-backed bond store, NimBLE host, security
manager, advertising, pairing, GAP event handling — is wrapped behind
**two function calls** in `app_main`:

```c
ble_uart_install(&cfg);   // NimBLE host + BLE UART GATT service
ble_uart_open();          // start advertising + auto-encrypt
```

…and two matching tear-down calls if your app ever needs to power
BLE off at runtime:

```c
ble_uart_close();         // stop advertising / disconnect / halt host
ble_uart_uninstall();     // free the NimBLE port + reset state
```

When a central connects, the firmware automatically initiates LE Secure
Connections + Bonding pairing, displays a fresh 6-digit passkey to the
UART monitor, persists the LTK in NVS, and starts delivering received
bytes to the application's `on_rx` callback. The application sends bytes
back with `ble_uart_tx()`.

## GATT layout

|          | UUID                                   | Properties                | Default flags             |
| -------- | -------------------------------------- | ------------------------- | ------------------------- |
| Service  | `6e400001-b5a3-f393-e0a9-e50e24dcca9e` | —                         | —                         |
| RX (in)  | `6e400002-b5a3-f393-e0a9-e50e24dcca9e` | Write, WriteNR            | encrypted, authenticated  |
| TX (out) | `6e400003-b5a3-f393-e0a9-e50e24dcca9e` | Notify (auto-CCCD)        | encrypted, authenticated  |

The `_ENC | _AUTHEN` flags are turned on only when `cfg.encrypted = true`
(the default in this example).

## Files

| File | Lines | Role |
| --- | ---: | --- |
| `main/main.c`            | ~70  | NVS init, MAC-derived device name, install + open, RX echo handler. Identical for both backends. |
| `CMakeLists.txt` (root) | ~15 | `list(APPEND EXTRA_COMPONENT_DIRS .../common/ble_uart)` before `project()` so `main` can `REQUIRES ble_uart`. |
| `../common/ble_uart/ble_uart.h`        | ~155 | Stack-agnostic public API: 3-field config + 4 lifecycle functions + TX/status + UUID + `BLE_UART_E*` return codes. No NimBLE / Bluedroid types leak through. |
| `../common/ble_uart/ble_uart_nimble.c`     | ~650 | NimBLE backend: host bring-up, BLE UART GATT service via `ble_gatts_add_svcs`, advertising, pairing, install/open/close/uninstall. Active when `CONFIG_BT_NIMBLE_ENABLED=y`. |
| `../common/ble_uart/ble_uart_bluedroid.c`  | ~1020 | Bluedroid backend: controller + host enable, BLE UART GATT service via `esp_ble_gatts_create_attr_tab` (service-table API), advertising, pairing, full PREP/EXEC long-write reassembly, install/open/close/uninstall. Active when `CONFIG_BT_BLUEDROID_ENABLED=y`. |
| `../common/ble_uart/Kconfig` | ~30 | Device-name prefix + RX scratch size (`menuconfig → Component configuration → BLE UART library`). |
| `../common/ble_uart/PORTING.md` | ~724 | Porting and API guide (integration, CMake, sdkconfig, thread safety). |
| `sdkconfig.defaults`         | —    | Default: NimBLE backend, MTU 512, SC + bonding + persistent NVS. |
| `sdkconfig.bluedroid`     | —    | Overlay: switch to Bluedroid backend (used via `-D SDKCONFIG_DEFAULTS=...`, see "Choosing the host stack" below). |

## Public API

```c
typedef void (*ble_uart_rx_cb_t)(const uint8_t *data, size_t len);

typedef struct {
    bool encrypted;                 /* SC + Bonding + MITM in one knob */
    const char *device_name;
    ble_uart_rx_cb_t ble_uart_on_rx;
} ble_uart_config_t;

/* Lifecycle */
int  ble_uart_install(const ble_uart_config_t *cfg);  /* NimBLE host + GATT */
int  ble_uart_open(void);                             /* host task + advertising */
int  ble_uart_close(void);                            /* stop adv / disconnect / halt host */
int  ble_uart_uninstall(void);                        /* free NimBLE port + reset state */

/* Data path */
int  ble_uart_tx(const uint8_t *data, size_t len);

/* Status (best-effort snapshot) */
bool ble_uart_is_connected(void);
bool ble_uart_is_subscribed(void);

extern const ble_uart_uuid128_t ble_uart_service_uuid;
```

## Choosing the host stack

The same `ble_uart.h` API is implemented twice — once on top of NimBLE
(`ble_uart_nimble.c`) and once on top of Bluedroid
(`ble_uart_bluedroid.c`). The shared `ble_uart` component's
`CMakeLists.txt` registers both files; each guards its body with
`#if CONFIG_BT_NIMBLE_ENABLED` / `#if CONFIG_BT_BLUEDROID_ENABLED`, so
exactly one becomes live at compile time.

Two ways to switch:

```bash
# A. Flip the Kconfig knob interactively
idf.py menuconfig
#   Component config -> Bluetooth -> Host -> NimBLE / Bluedroid

# B. Apply the Bluedroid overlay non-interactively (scripts / reproducible builds)
idf.py -B build_bd \
       -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.bluedroid" \
       reconfigure
idf.py -B build_bd build flash monitor
```

When neither is enabled the build fails up-front with a clear error.

### Differences callers should know about

| | NimBLE backend | Bluedroid backend |
| --- | --- | --- |
| Long-write (PREP+EXEC) reassembly | done by NimBLE itself | done in `ble_uart_bluedroid.c`, capped at `CONFIG_BLE_UART_RX_SCRATCH_SIZE` |
| TX congestion behaviour | `mbuf` pool is generous; rarely returns ENOMEM | `esp_ble_gatts_send_indicate` may return ENOMEM under load → caller should back off |
| Passkey origin | generated locally with `esp_random()` | generated by the Bluedroid SM, surfaced via `PASSKEY_NOTIF_EVT` |
| Bond persistence | needs `CONFIG_BT_NIMBLE_NVS_PERSIST=y` | persisted by default |
| `install` blocking time | ~50 ms | ~150 ms (waits for `CREAT_ATTR_TAB_EVT`) |
| `uninstall` thoroughness | `nimble_port_deinit()` releases everything | `bluedroid_disable+deinit` + `controller_disable+deinit` releases everything |

## How to use

### Configure

```bash
idf.py set-target esp32c3      # or esp32, esp32s3, esp32c6, esp32h2 ...
idf.py menuconfig              # optional
#   Component configuration -> BLE UART library
#     - BLE device name prefix    (default: BleUart)
#     - RX scratch buffer size      (default: 1024 bytes)
```

Those `BLE_UART_*` options are defined in **`../common/ble_uart/Kconfig`**
(the `ble_uart` component); they appear whenever `ble_uart` is part of the
build (this example pulls it in via `EXTRA_COMPONENT_DIRS` in the root
`CMakeLists.txt`).

The two security knobs are set in `sdkconfig.defaults`:

```ini
CONFIG_BT_NIMBLE_SM_SC=y         # LE Secure Connections
CONFIG_BT_NIMBLE_NVS_PERSIST=y   # Bond keys persist across reboots
```

Disable `cfg.encrypted` in `main.c` (set it to `false`) for plaintext
operation in the lab — the GATT characteristics drop their `_ENC`
flags accordingly. Production firmware should keep encryption on.

### Build & flash

```bash
idf.py build flash monitor
```

Expected boot log (NimBLE backend — the per-characteristic register
lines are NimBLE-specific; Bluedroid prints the four UART-service handles in a
single line, see below):

```
I (xxx) ble_uart: registered service 6e400001-... handle=14
I (xxx) ble_uart: registered chr 6e400002-... def=15 val=16
I (xxx) ble_uart: registered chr 6e400003-... def=17 val=18
I (xxx) ble_uart: addr=80:7d:3a:11:22:33
I (xxx) ble_uart: BLE host task started
I (xxx) ble_uart: advertising as 'BleUart-XXXX'
```

Expected boot log (Bluedroid backend):

```
I (xxx) ble_uart: gatts reg status=0 app_id=85 gatts_if=3
I (xxx) ble_uart: registered service svc_handle=40 rx=42 tx=44 cccd=45
I (xxx) ble_uart: advertising started
```

## Pairing & demo

1. On a phone, install **a BLE GATT client app** that supports scanning,
   pairing, characteristic write, and notify/CCCD (many mobile “BLE tools”
   or serial-over-BLE utilities qualify).
2. Scan, tap **Connect** on `BleUart-XXXX`. The phone prompts for a
   6-digit code.
3. The device prints a fresh code in a banner on UART:

   ```
   W (xxx) ble_uart:     +-----------------------------+
   W (xxx) ble_uart:     |  BLE PAIRING PASSKEY:       |
   W (xxx) ble_uart:     |       427183                |
   W (xxx) ble_uart:     +-----------------------------+
   ```
4. Type that code on the phone; pairing completes. The link is now
   AES-CCM-encrypted and the LTK is stored to NVS.
5. Open the **UART service** (UUID `6e400001-…`), subscribe to TX (the down-arrow
   icon), then write any bytes to RX (the up-arrow icon). The device
   logs them to UART and **echoes them right back** through TX.
6. Disconnect and reconnect: no passkey prompt — the bond resumes
   automatically.

To wipe the bond and force a fresh passkey, run `idf.py erase-flash`
and re-flash.

## Adapting to your application

Replace the `ble_uart_on_rx` body in `main.c` with your own protocol
parser (line / TLV / length-prefixed framing — `ble_uart` delivers raw
bytes with no framing assumptions). Send replies with `ble_uart_tx()`.

## Reusing `ble_uart` in your own project

**Recommended (no copy):** register the shared component directory **before**
`project()` so CMake can resolve `REQUIRES ble_uart` from `main/` (same pattern
as this example's root `CMakeLists.txt`):

```cmake
cmake_minimum_required(VERSION 3.22)
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../path/to/common/ble_uart")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(my_app)
```

Then in `main/CMakeLists.txt` use `REQUIRES ble_uart nvs_flash` and
`#include "ble_uart.h"`.

Adjust the `EXTRA_COMPONENT_DIRS` path if you vendor `common/ble_uart` elsewhere
(e.g. `${CMAKE_CURRENT_LIST_DIR}/components/ble_uart`).

**Alternative:** copy the whole `examples/bluetooth/common/ble_uart/` directory
into your tree (or only the `.h` / `.c` files into `main/`) and add `bt nvs_flash`
to that component's `REQUIRES`, then in your `app_main`:

```c
nvs_flash_init();

ble_uart_install(&(ble_uart_config_t){
    .encrypted      = true,
    .device_name    = "MyDevice",
    .ble_uart_on_rx = my_handler,
});
ble_uart_open();
```

That's it — encrypted serial-over-BLE in 4 lines.

## Troubleshooting

- **Phone shows "pairing failed"** — the central asked for "Just Works"
  and our SM rejected it because MITM is required when
  `cfg.encrypted = true`. Pick a phone / app that supports passkey entry.
- **No passkey appears in UART** — verify `CONFIG_BT_NIMBLE_SM_SC=y` in
  your sdkconfig and that you didn't toggle `cfg.encrypted` to `false`.
- **`enc_change status=13 encrypted=1 ...`** — `13` is `BLE_HS_ETIMEOUT`,
  triggered by a benign race between our `ble_gap_security_initiate()`
  and the central's own auto-encryption on a bonded reconnect. Status
  is non-zero but the link is fully encrypted; safe to ignore.
- **Notifications missing after a reconnect** — `ble_uart_tx` deliberately
  doesn't gate on the CCCD-subscribe state because bonded reconnects
  often skip the CCCD write. Bytes are still pushed; the central
  delivers them based on its remembered subscription.
