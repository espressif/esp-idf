# ESP-BLE-UART Example — NimBLE / Bluedroid

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

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

If your app powers BLE off at runtime, use **one** of the release paths
in [PORTING.md §5.3](../common/ble_uart/PORTING.md#53-lifecycle--bring-up-and-release)
(this example uses Path A from `app_main`):

| Path | When | Calls |
| --- | --- | --- |
| **A — sync** (default) | Shutdown from a normal task (button, Wi-Fi, `app_main`) | `ble_uart_close()` → `ble_uart_uninstall()` |
| **B — async** | Shutdown triggered inside `on_event` / `on_rx` | `close_async()` in callback → `CLOSED` sets flag → **`uninstall()` on a separate app task** (not inside `CLOSED`) |

```c
/* Path A — this example style */
ble_uart_close();
ble_uart_uninstall();
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
(the default in this example). The two flags can be controlled
independently via `cfg.security.mitm` (drops `_AUTHEN`) and the
combined `cfg.security.{sc,bonding,mitm}` set (all OFF drops `_ENC`
too) — see PORTING.md §5.6.

## Files

| File | Lines | Role |
| --- | ---: | --- |
| `main/main.c`            | ~200  | NVS init, install + open with `<prefix>-XXXX` device name (Kconfig prefix + BT MAC suffix), RX echo handler, lifecycle/link-state event sink, bonded-peer dump on boot. Identical for both backends. |
| `main/Kconfig.projbuild` | ~50  | Example-local `EXAMPLE_CUSTOM_ADV_DATA` switch — toggles the `ble_uart_config_t::adv_data` demo path in `main.c`. |
| `CMakeLists.txt` (root) | ~15 | `list(APPEND EXTRA_COMPONENT_DIRS .../common/ble_uart)` before `project()` so `main` can `REQUIRES ble_uart`. |
| `../common/ble_uart/ble_uart.h`        | ~640 | Stack-agnostic public API: configuration struct (preset + per-feature security overrides + custom adv payload + RX/event callbacks) + lifecycle (install/open/close/close_async/uninstall) + TX + pairing replies + bond-management + status + UUID + `BLE_UART_E*` return codes. No NimBLE / Bluedroid types leak through. |
| `../common/ble_uart/ble_uart_nimble.c`     | ~1290 | NimBLE backend: host bring-up, BLE UART GATT service via `ble_gatts_add_svcs`, advertising (default + raw), pairing (incl. Passkey Entry / Numeric Comparison), bond store, async close, install/open/close/uninstall. Active when `CONFIG_BT_NIMBLE_ENABLED=y`. |
| `../common/ble_uart/ble_uart_bluedroid.c`  | ~1660 | Bluedroid backend: controller + host enable, BLE UART GATT service via `esp_ble_gatts_create_attr_tab` (service-table API), advertising (default + raw), pairing (incl. Passkey Entry / Numeric Comparison), bond store, async close, full PREP/EXEC long-write reassembly, install/open/close/uninstall. Active when `CONFIG_BT_BLUEDROID_ENABLED=y`. |
| `../common/ble_uart/Kconfig` | ~30 | Device name prefix + RX scratch size (`menuconfig → Component configuration → ESP-BLE-UART library`). |
| `../common/ble_uart/PORTING.md` | ~1300 | Porting and API guide (integration, CMake, sdkconfig, security model, custom advertising, bond management, thread safety). |
| `sdkconfig.defaults`         | —    | Default: NimBLE backend, MTU 512, SC + bonding + persistent NVS. |
| `sdkconfig.bluedroid`     | —    | Overlay: switch to Bluedroid backend (used via `-D SDKCONFIG_DEFAULTS=...`, see "Choosing the host stack" below). |

## Public API

```c
typedef void (*ble_uart_rx_cb_t) (const uint8_t *data, size_t len);
typedef void (*ble_uart_evt_cb_t)(const ble_uart_evt_t *evt);

typedef struct {
    ble_uart_sec_t    sc;       /* AUTO / OFF / ON */
    ble_uart_sec_t    bonding;
    ble_uart_sec_t    mitm;
    ble_uart_io_cap_t io_cap;   /* AUTO / NO_INPUT_OUTPUT / DISPLAY_ONLY /
                                   KEYBOARD_ONLY / DISPLAY_YES_NO /
                                   KEYBOARD_DISPLAY */
} ble_uart_security_t;

typedef struct {
    bool encrypted;                 /* preset: SC + Bonding + MITM + DisplayOnly */
    ble_uart_security_t security;   /* per-feature overrides; see PORTING.md §5.6 */

    const char *device_name;        /* ≤ BLE_UART_DEVICE_NAME_MAX (26) */
    /* Optional: raw advertising / scan-response bytes (NULL → defaults).
     * Limits: adv_data_len ≤ BLE_UART_ADV_DATA_MAX (28),
     *         scan_rsp_data_len ≤ BLE_UART_SCAN_RSP_DATA_MAX (31).
     * The 3-byte Flags AD element is prepended automatically — don't
     * include it in adv_data. See PORTING.md §5.9 for examples. */
    const uint8_t *adv_data;
    size_t         adv_data_len;
    const uint8_t *scan_rsp_data;
    size_t         scan_rsp_data_len;
    ble_uart_rx_cb_t  ble_uart_on_rx;
    ble_uart_evt_cb_t on_event;     /* lifecycle / link-state events; NULL drops */
} ble_uart_config_t;

typedef struct {
    uint8_t bytes[6];   /* big-endian: bytes[0] is the MSB (AA:BB:CC:DD:EE:FF) */
    uint8_t type;       /* BLE_UART_ADDR_TYPE_PUBLIC | _RANDOM */
} ble_uart_addr_t;

/* Lifecycle */
int  ble_uart_install(const ble_uart_config_t *cfg);  /* host + GATT */
int  ble_uart_open(void);                             /* host task + advertising */
int  ble_uart_close(void);                            /* stop adv / disconnect / halt host */
int  ble_uart_close_async(void);                      /* same, fire-and-forget; safe from inside on_event/on_rx */
int  ble_uart_uninstall(void);                        /* free port + reset state */

/* Data path */
int  ble_uart_tx(const uint8_t *data, size_t len);

/* Pairing replies (call from on_event for input-capable IO caps) */
int  ble_uart_passkey_reply(uint32_t passkey);   /* answer PASSKEY_REQUEST */
int  ble_uart_compare_reply(bool match);         /* answer NUMERIC_COMPARE */

/* Status (best-effort snapshot) */
bool ble_uart_is_connected(void);
bool ble_uart_is_subscribed(void);

/* Bond management (works after install()) */
int  ble_uart_get_bond_count(size_t *out_count);
int  ble_uart_get_bonded_peers(ble_uart_addr_t *out, size_t cap, size_t *out_count);
int  ble_uart_remove_peer(const ble_uart_addr_t *peer);
int  ble_uart_clear_bonds(void);

extern const ble_uart_uuid128_t ble_uart_service_uuid;
```

### Event callback

`on_event` is invoked on the BLE host task (same context as `ble_uart_on_rx`)
with a tagged `ble_uart_evt_t`. Use `LINK_SECURE` — not `is_connected()` —
to gate any application logic that requires the channel to be encrypted /
authenticated:

| `evt->id`                       | Payload                                                 | Fires when |
| ------------------------------- | ------------------------------------------------------- | ---------- |
| `BLE_UART_EVT_CONNECTED`        | `connected.peer`                                        | Physical link up |
| `BLE_UART_EVT_DISCONNECTED`     | `disconnected.reason` (int, stack-specific)           | Physical link down — Bluedroid: `esp_gatt_conn_reason_t`; NimBLE: BLE host return code (`BLE_HS_HCI_ERR()` for HCI) |
| `BLE_UART_EVT_SUBSCRIBED`       | `subscribed.subscribed`                                 | Central writes CCCD on TX (edge-triggered) |
| `BLE_UART_EVT_LINK_SECURE`      | `link_secure.{encrypted,authenticated,bonded,key_size}` | Pairing or bonded reconnect succeeds |
| `BLE_UART_EVT_PASSKEY_DISPLAY`  | `passkey.passkey` (0..999999)                           | SM asks the device to show a passkey |
| `BLE_UART_EVT_PASSKEY_REQUEST`  | —                                                       | SM asks the user to enter a passkey shown by the central — answer with `ble_uart_passkey_reply()` |
| `BLE_UART_EVT_NUMERIC_COMPARE`  | `numeric_compare.passkey` (0..999999)                   | SM asks the user to confirm both sides display the same value — answer with `ble_uart_compare_reply()` |
| `BLE_UART_EVT_PAIRING_FAILED`   | `pairing_failed.reason` (stack-specific)                | Pairing rejected or timed out |
| `BLE_UART_EVT_CLOSED`           | `closed.status` (`BLE_UART_*`)                          | `ble_uart_close_async()` worker has finished; `BLE_UART_OK` means tear-down succeeded |

The default passkey UART banner still prints; the callback is additive so
log-scraping tests stay compatible. Don't block in the callback.

**Callback rules:**

- Do **not** call `ble_uart_close()` or `ble_uart_uninstall()` from
  `on_event` / `on_rx` (host task — deadlocks).
- To start teardown from a callback, call `ble_uart_close_async()` only.
- Call `ble_uart_uninstall()` from a **normal app task** after
  `BLE_UART_EVT_CLOSED` with `closed.status == BLE_UART_OK` (see
  [PORTING.md §5.3.2](../common/ble_uart/PORTING.md#532-path-b--release-after-a-ble-event-close_async)).

Path B sketch (full code in PORTING.md):

```c
case BLE_UART_EVT_PAIRING_FAILED:
    ble_uart_close_async();
    break;
case BLE_UART_EVT_CLOSED:
    if (e->closed.status == BLE_UART_OK) {
        s_ble_closed_ok = true;   /* app task calls uninstall */
    }
    break;
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
#   Component configuration -> ESP-BLE-UART library
#     - BLE device name prefix      (default: BleUart; example appends -XXXX from BT MAC)
#     - RX scratch buffer size      (default: 1024 bytes)
#   BLE UART service example
#     - Use custom advertising data (default: off)
```

Those `BLE_UART_*` options are defined in **`../common/ble_uart/Kconfig`**
(the `ble_uart` component); they appear whenever `ble_uart` is part of the
build (this example pulls it in via `EXTRA_COMPONENT_DIRS` in the root
`CMakeLists.txt`).

`EXAMPLE_CUSTOM_ADV_DATA` is example-local (`main/Kconfig.projbuild`)
and demonstrates `ble_uart_config_t::adv_data` — the field that lets
the application fully control the over-the-air advertising payload
instead of using the library default.

When the option is on, `app_main` hands a static byte array
(`example_adv_payload[]`, top of `main.c`) to `ble_uart_install()`.
The array is just a sequence of `[length][AD type][value]` triplets;
edit it directly to advertise whatever you want — a different Local
Name, Manufacturer Specific Data, custom Service Data, additional
Service UUIDs, etc. The only hard rule is total length ≤
`BLE_UART_ADV_DATA_MAX` (28); the 3-byte Flags AD is added by the
library and does not count against that budget.

The GAP-service Device Name (set via `device_name` in the same
config struct) is independent and is what connected centrals read
post-pair, regardless of `adv_data`.

With the option off the library default is used (Complete Local Name
in the primary packet, 128-bit Service UUID in the scan response).

The two security knobs are set in `sdkconfig.defaults`:

```ini
CONFIG_BT_NIMBLE_SM_SC=y         # LE Secure Connections
CONFIG_BT_NIMBLE_NVS_PERSIST=y   # Bond keys persist across reboots
```

Disable `cfg.encrypted` in `main.c` (set it to `false`) for plaintext
operation in the lab — the GATT characteristics drop their `_ENC`
flags accordingly. Production firmware should keep encryption on.

For finer control without going all-or-nothing — e.g. a displayless
gateway that wants encryption + bonding but no passkey UI, or a
device with a keypad that wants Passkey Entry / Numeric Comparison —
keep `cfg.encrypted = true` and override individual bits via
`cfg.security.{sc,bonding,mitm,io_cap}`. The input-capable IO caps
(`KEYBOARD_ONLY`, `DISPLAY_YES_NO`, `KEYBOARD_DISPLAY`) require an
`on_event` handler that wires `BLE_UART_EVT_PASSKEY_REQUEST` /
`NUMERIC_COMPARE` to `ble_uart_passkey_reply()` /
`ble_uart_compare_reply()`. See PORTING.md §5.6 for the full matrix
and worked examples.

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
I (xxx) ble_uart: advertising as 'BleUart-2233'
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
2. Scan, tap **Connect** on `BleUart-XXXX` (prefix from
   `CONFIG_BLE_UART_DEVICE_NAME_PREFIX`, `XXXX` = last two BT MAC
   bytes). The phone prompts for a 6-digit
   code.
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

To wipe the bond and force a fresh passkey there are three options:

- Call `ble_uart_clear_bonds()` from your app (preserves the rest of NVS)
- Call `ble_uart_remove_peer(&addr)` to drop one peer (use the address
  reported in `BLE_UART_EVT_CONNECTED`, or any address you happen to
  have stored — Bluedroid matches by address only, NimBLE by identity)
- Run `idf.py erase-flash` and re-flash (also wipes WiFi creds, NVS, etc.)

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

## OpenCode Companion

This example serves as the transport layer for the [OpenCode Companion tutorial](OPENCODE_COMPANION.md), which walks through building a physical companion device for OpenCode using ESP-BLE-UART and ESP-VoCat. The tutorial covers:

- **Part 1:** Using ESP-BLE-UART Console to verify the BLE UART data path (Echo Server mode).
- **Part 2:** Using the `ble_uart_service` example on ESP-VoCat with the ESP-BLE-UART Daemon and OpenCode Plugin for session status display and physical permission approval.

See the full guide in English: [OPENCODE_COMPANION.md](OPENCODE_COMPANION.md)
Chinese version: [OPENCODE_COMPANION_CN.md](OPENCODE_COMPANION_CN.md)

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
