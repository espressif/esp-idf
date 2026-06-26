# ESP-BLE-UART Porting & API Guide

> **Naming convention:** Use **ESP-BLE-UART** for Espressif-owned product names (Bridge, Console, Daemon, Echo Server, the `ble_uart` component, and the `ble_uart_service` example). Use **BLE UART** for the generic GATT service convention, transport layer, and compatible third-party devices. This follows the same pattern as ESP-BLE-MESH.

This document lives in **`examples/bluetooth/common/ble_uart/`** next to the
`ble_uart` component sources (`ble_uart.h`, backend `.c` files).

**Reference application:** use the **`examples/bluetooth/ble_uart_service`**
example as the working template. Its root `CMakeLists.txt` appends this
directory to **`EXTRA_COMPONENT_DIRS`** so `main` can `REQUIRES ble_uart`;
`main/main.c` initializes NVS, calls `ble_uart_install()` /
`ble_uart_open()` with the Kconfig-supplied GAP name and the default
encrypted UART-over-BLE echo path, and the tree ships
`sdkconfig.defaults` plus the Bluedroid overlay
(`sdkconfig.bluedroid`). Clone or diff that project when adapting to a new
target or host stack.

A complete guide to integrating `ble_uart` into any ESP-IDF project.
**Either** `EXTRA_COMPONENT_DIRS` pointing at this component **or** a few
copied source files **plus** the glue steps below are enough to bring an
encrypted BLE serial peripheral up in a fresh project — the same
`ble_uart.h` API works on top of either NimBLE or Bluedroid; pick the
host with a Kconfig knob.

This guide uses **NimBLE** as the running example because it is the
default on every ESP32 family target. The Bluedroid path is identical
from the application's point of view; the only differences are the
sdkconfig knobs called out in §4.3 and a few stack-specific notes
flagged inline.

---

## 1. What `ble_uart` Provides

| Capability | Description |
| --- | --- |
| Widely used BLE UART-over-GATT (RX/TX) | Interoperates with every generic BLE-serial tool (mobile GATT clients, Web Bluetooth, custom scripts) |
| LE Secure Connections + Bonding pairing | Single switch; when enabled, a fresh 6-digit passkey is printed to UART |
| Auto-reconnect | After a bonded central disconnects, advertising restarts immediately and the LTK is reused — no passkey prompt |
| Raw byte pass-through | RX is delivered via a callback; TX is exposed as `ble_uart_tx` |
| Auto-fragmentation | TX is sliced according to the negotiated ATT MTU |
| Fully wrapped | The user's `app_main` only calls two functions: `install` + `open` |

`ble_uart` is agnostic of any application-layer protocol (no JSON, no
line framing). It only delivers bytes — **what you do with those bytes
is entirely up to you**.

---

## 2. Prerequisites

| Requirement | Notes |
| --- | --- |
| ESP-IDF v5.0+ | v5.x or v6.x recommended |
| BT controller | Must support BLE (ESP32 / C2 / C3 / C5 / C6 / C61 / H2 / S3 / …) |
| Host stack | Exactly one of `CONFIG_BT_NIMBLE_ENABLED=y` (default, smaller) or `CONFIG_BT_BLUEDROID_ENABLED=y` in sdkconfig (covered in detail below) |
| Flash size | At least 2 MB (the default partition table is plenty) |

---

## 3. File Inventory

Canonical sources live under **`$IDF_PATH/examples/bluetooth/common/ble_uart/`**
(component name `ble_uart`): `ble_uart.h`, `ble_uart_nimble.c`,
`ble_uart_bluedroid.c`, `CMakeLists.txt`, and `Kconfig` (device name + RX scratch;
`menuconfig → Component configuration → ESP-BLE-UART library`). When reusing
outside this tree, copy the whole `common/ble_uart/` directory or at least
merge `Kconfig` into your component so the same `CONFIG_BLE_UART_*` symbols
exist.

**Option A — depend on the in-tree component (no copy):** add the component
directory to **`EXTRA_COMPONENT_DIRS` in the project root `CMakeLists.txt`
before `include($ENV{IDF_PATH}/tools/cmake/project.cmake)` / `project()`**,
then use `REQUIRES ble_uart` from `main/CMakeLists.txt` (see
`examples/bluetooth/ble_uart_service/CMakeLists.txt`). This ensures the
`ble_uart` target exists when CMake expands `main`'s requirements.

Kconfig options appear under
`menuconfig → Component configuration → ESP-BLE-UART library`.

> A `main/idf_component.yml` path dependency alone is **not** sufficient if
> `main/CMakeLists.txt` lists `REQUIRES ble_uart`: the early requirement scan
> runs before the component manager injects that dependency, so CMake fails
> with *unknown component `ble_uart`*. Prefer `EXTRA_COMPONENT_DIRS` (as in the
> reference example) or copy the sources into a normal project component.

**Option B — copy into your project:** pick the backend you want and
copy that pair plus the public header (or copy both backends; each `.c`
gates on its Kconfig symbol):

```
your_project/main/
├── ble_uart.h              ← copy from .../common/ble_uart/
├── ble_uart_nimble.c       ← if you'll set CONFIG_BT_NIMBLE_ENABLED=y
└── ble_uart_bluedroid.c    ← if you'll set CONFIG_BT_BLUEDROID_ENABLED=y
```

Optional: copy `Kconfig` from `common/ble_uart/` into your component (or merge
its symbols into your own `Kconfig`) if you want `BLE_UART_*` in `menuconfig`;
otherwise hard-code the device name and rely on the 1024-byte fallback for RX
scratch.

---

## 4. Step-by-Step Integration

Assume you already have an ESP-IDF project (`my_project/`).

### 4.1 Copy the files

```bash
cd my_project/main
BLE_UART_SRC="$IDF_PATH/examples/bluetooth/common/ble_uart"
# Stack-agnostic public header — always.
cp "$BLE_UART_SRC/ble_uart.h" .
# Pick one (or copy both — the inactive one compiles to nothing).
cp "$BLE_UART_SRC/ble_uart_nimble.c" .
cp "$BLE_UART_SRC/ble_uart_bluedroid.c" .
```

### 4.2 Edit `main/CMakeLists.txt`

```cmake
# List both backends; each .c file is gated on its matching Kconfig
# symbol, so only the active one contributes code.
idf_component_register(SRCS "main.c"
                            "ble_uart_nimble.c"
                            "ble_uart_bluedroid.c"
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
```

### 4.3 Edit `sdkconfig.defaults` (the 7 critical lines)

**NimBLE backend (default, smaller footprint):**

```ini
# Enable NimBLE
CONFIG_BT_ENABLED=y
CONFIG_BTDM_CTRL_MODE_BLE_ONLY=y     # only needed on classic ESP32; C3/S3/C6/... will warn "unknown" — safe to ignore
CONFIG_BT_BLUEDROID_ENABLED=n
CONFIG_BT_NIMBLE_ENABLED=y

# Encryption + persistent bonds
CONFIG_BT_NIMBLE_SM_SC=y             # LE Secure Connections
CONFIG_BT_NIMBLE_NVS_PERSIST=y       # persist LTKs in NVS — passkey-free reconnects
```

`CONFIG_BT_NIMBLE_ATT_PREFERRED_MTU` is optional; the default (256) is
fine. Bumping it to 512 lets TX push larger chunks per notification, but
the central must support it.

**Bluedroid backend (drop-in alternative):**

Use the **`examples/bluetooth/ble_uart_service/sdkconfig.bluedroid`** file as
the authoritative Kconfig overlay: it enables the host stack, SMP, GATTS
(service-table API), and the BLE-only advertising knobs that
`ble_uart_bluedroid.c` expects. Either merge those lines into your own
`sdkconfig.defaults`, or pass them as a second defaults file:

```bash
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.bluedroid" reconfigure
```

(Paths are relative to the example project root; copy `sdkconfig.bluedroid`
into your tree if you are not starting from `ble_uart_service`.)

A minimal inline sketch (may drift from IDF defaults — **diff against
`sdkconfig.bluedroid` after each IDF upgrade**):

```ini
CONFIG_BT_ENABLED=y
CONFIG_BT_NIMBLE_ENABLED=n
CONFIG_BT_BLUEDROID_ENABLED=y

# LE Secure Connections + bonding (Bluedroid persists LTKs by default)
CONFIG_BT_BLE_SMP_ENABLE=y

# Optional: bigger MTU (when supported by your IDF target / menuconfig)
# CONFIG_BT_GATT_MAX_MTU_SIZE=512

# BLE-only feature set (saves flash on classic-BT-capable parts)
CONFIG_BT_BLE_42_FEATURES_SUPPORTED=y
CONFIG_BT_BLE_42_ADV_EN=y
```

### 4.4 Write `app_main` (template)

Minimal working template:

```c
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"

#include "ble_uart.h"

static const char *TAG = "app";

/* What to do with received bytes — up to you */
static void ble_uart_on_rx(const uint8_t *data, size_t len)
{
    ESP_LOGI(TAG, "rx %u bytes", (unsigned)len);
    /* echo it back as a demo */
    ble_uart_tx(data, len);
}

void app_main(void)
{
    /* 1. NVS: NimBLE uses it for PHY calibration and bond storage */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* 2. Bring up ESP-BLE-UART */
    ESP_ERROR_CHECK(ble_uart_install(&(ble_uart_config_t){
        .encrypted      = true,
        .device_name    = "MyDevice",
        .ble_uart_on_rx = ble_uart_on_rx,
    }));

    /* 3. Take off */
    ESP_ERROR_CHECK(ble_uart_open());
}
```

### 4.5 Build & flash

```bash
idf.py set-target esp32s3   # or whichever target you use
idf.py build flash monitor
```

Once flashed, the UART monitor should show (NimBLE backend):

```
I (xxx) ble_uart: registered service 6e400001-... handle=14
I (xxx) ble_uart: registered chr 6e400002-... def=15 val=16
I (xxx) ble_uart: registered chr 6e400003-... def=17 val=18
I (xxx) ble_uart: addr=...
I (xxx) ble_uart: BLE host task started
I (xxx) ble_uart: advertising as 'MyDevice'
```

…or with the Bluedroid backend:

```
I (xxx) ble_uart: gatts reg status=0 app_id=85 gatts_if=3
I (xxx) ble_uart: registered service svc_handle=40 rx=42 tx=44 cccd=45
I (xxx) ble_uart: advertising started
```

A phone GATT client app discovers `MyDevice`; connect, enter the
passkey, subscribe to TX, write to RX, and you will see the echo come
back.

---

## 5. API Reference

### 5.1 Configuration struct

```c
typedef struct {
    bool                 encrypted;  /* Preset shortcut for SC + Bonding + MITM */
    ble_uart_security_t  security;   /* Per-feature overrides — see §5.6 */

    const char  *device_name;        /* GAP service device name (UUID 0x2A00) */

    /* Custom advertising bytes — see §5.9. NULL keeps the default
     * payload. ble_uart prepends the 3-byte Flags AD itself; you don't. */
    const uint8_t *adv_data;
    size_t         adv_data_len;     /* ≤ BLE_UART_ADV_DATA_MAX (28) */
    const uint8_t *scan_rsp_data;
    size_t         scan_rsp_data_len;/* ≤ BLE_UART_SCAN_RSP_DATA_MAX (31) */

    ble_uart_rx_cb_t  ble_uart_on_rx;/* RX byte callback */
    ble_uart_evt_cb_t on_event;      /* Lifecycle / link-state events */
} ble_uart_config_t;
```

| Field | Type | Required | Default / meaning |
| --- | --- | --- | --- |
| `encrypted` | `bool` | yes | One-line preset for the override fields under `security`: `true` = SC + Bonding + MITM + DisplayOnly + encrypted+authenticated GATT chars; `false` = fully plaintext (sniffable, lab use only). Override individual bits via `security.*` — see §5.6. |
| `security` | `ble_uart_security_t` | optional | A zero-initialised member (`security.{sc,bonding,mitm,io_cap} = AUTO`) inherits everything from `encrypted`. Set any sub-field to `OFF`/`ON` (or pick a specific `io_cap`) to override just that bit. Out-of-range enum values, or impossible combos like `mitm=ON` with `io_cap=NO_INPUT_OUTPUT`, fail `ble_uart_install()` with `BLE_UART_EINVAL`. Full reference in §5.6. |
| `device_name` | `const char *` | recommended | Set as the GAP-service Device Name (UUID 0x2A00). With the **default** advertising payload it is also placed in the primary adv as the Complete Local Name; with custom `adv_data` (see §5.9) it is **not** auto-included — the application owns the adv bytes. Length must be ≤ **`BLE_UART_DEVICE_NAME_MAX` = 26** (sized so the default Flags + Name AD layout always fits in a 31-byte primary packet). Longer names fail `ble_uart_install()` synchronously with `BLE_UART_EINVAL`. |
| `adv_data` / `adv_data_len` | bytes + length | optional | Application-controlled raw advertisement data. NULL keeps the built-in default (Complete Local Name only). Max length **`BLE_UART_ADV_DATA_MAX` = 28** (the 31-byte primary packet minus our 3-byte Flags AD). Buffer is copied in `install`; the pointer doesn't need to outlive the call. See §5.9. |
| `scan_rsp_data` / `scan_rsp_data_len` | bytes + length | optional | Application-controlled raw scan-response data. NULL keeps the built-in default (128-bit BLE UART service UUID). Max length **`BLE_UART_SCAN_RSP_DATA_MAX` = 31** (no Flags element here). Same copy semantics as `adv_data`. |
| `ble_uart_on_rx` | callback | optional | `NULL` discards every received byte |
| `on_event`       | callback | optional | `NULL` drops every event (see §5.2.1). **Not required** for the default preset (`encrypted=true`, all `security.*` AUTO → Passkey Display): the port logs the 6-digit passkey to UART and completes pairing without a callback. **Required** when `io_cap` is `KEYBOARD_ONLY`, `DISPLAY_YES_NO`, or `KEYBOARD_DISPLAY` — otherwise `ble_uart_install()` returns `BLE_UART_EINVAL`. |

### 5.2 RX callback signature

```c
typedef void (*ble_uart_rx_cb_t)(const uint8_t *data, size_t len);

static void my_handler(const uint8_t *data, size_t len)
{
    /* `data` is reused after the callback returns; memcpy into your own
     * buffer if you need to keep it. */
}
```

**Caveats**:

- The callback runs on the BLE host task (NimBLE host task /
  Bluedroid BTC task) — **do not block**; offload heavy work to your
  own task.
- A single callback may carry only **part** of an upper-layer frame
  (the central slices on ATT MTU). Framing logic (line / TLV /
  length-prefixed) is your responsibility.
- The data carries **no `ctx` argument**. If your callback needs state,
  use a file-scope `static` or a global.

### 5.2.1 Event callback

```c
typedef void (*ble_uart_evt_cb_t)(const ble_uart_evt_t *evt);

static void on_event(const ble_uart_evt_t *e)
{
    switch (e->id) {
    case BLE_UART_EVT_CONNECTED:        /* link up */ break;
    case BLE_UART_EVT_DISCONNECTED:     /* e->disconnected.reason */ break;
    case BLE_UART_EVT_SUBSCRIBED:       /* e->subscribed.subscribed */ break;
    case BLE_UART_EVT_LINK_SECURE:
        if (e->link_secure.encrypted && e->link_secure.authenticated) {
            /* Safe to forward sensitive payloads now */
        }
        break;
    case BLE_UART_EVT_PASSKEY_DISPLAY:  /* e->passkey.passkey */ break;
    case BLE_UART_EVT_PASSKEY_REQUEST:  /* user types peer's 6-digit;
                                           ble_uart_passkey_reply(d) */ break;
    case BLE_UART_EVT_NUMERIC_COMPARE:  /* e->numeric_compare.passkey,
                                           ble_uart_compare_reply(b)  */ break;
    case BLE_UART_EVT_PAIRING_FAILED:   /* e->pairing_failed.reason */ break;
    }
}
```

| `evt->id` | Payload (anonymous-union member) | Fires when |
| --- | --- | --- |
| `BLE_UART_EVT_CONNECTED`        | —                                                       | Physical link up |
| `BLE_UART_EVT_DISCONNECTED`     | `disconnected.reason` (int, stack-specific)           | Physical link down — Bluedroid: `esp_gatt_conn_reason_t`; NimBLE: BLE host return code (`BLE_HS_HCI_ERR()` for HCI) |
| `BLE_UART_EVT_SUBSCRIBED`       | `subscribed.subscribed`                                 | CCCD on TX changed (edge-triggered) |
| `BLE_UART_EVT_LINK_SECURE`      | `link_secure.{encrypted,authenticated,bonded,key_size}` | Pairing or bonded reconnect succeeds |
| `BLE_UART_EVT_PASSKEY_DISPLAY`  | `passkey.passkey` (0..999999)                           | SM generated a passkey for the central to type (Passkey Display). **Optional** — the port always prints a banner to UART; with `on_event == NULL` the event is dropped and pairing still completes (NimBLE injects the passkey internally; Bluedroid needs no app reply). Register `on_event` only if you want a custom UI in addition to the log line. |
| `BLE_UART_EVT_PASSKEY_REQUEST`  | —                                                       | SM asks the user to enter a passkey shown by the central — application **must** reply via `ble_uart_passkey_reply()` (see §5.6.1). Requires `on_event != NULL` at install time. |
| `BLE_UART_EVT_NUMERIC_COMPARE`  | `numeric_compare.passkey` (0..999999)                   | SM asks the user to confirm the displayed value matches the central — application **must** reply via `ble_uart_compare_reply()` (see §5.6.1). Requires `on_event != NULL` at install time. |
| `BLE_UART_EVT_PAIRING_FAILED`   | `pairing_failed.reason`                                 | Pairing rejected or timed out (including no application reply for `PASSKEY_REQUEST` / `NUMERIC_COMPARE` before the SM's pairing timeout) |
| `BLE_UART_EVT_CLOSED`           | `closed.status` (`BLE_UART_*` from the worker's `ble_uart_close()`) | `ble_uart_close_async()` worker finished — then `uninstall` on an app task (§5.3.2) |

**Use `LINK_SECURE`, not `is_connected()`, to gate any logic that
requires the link to be encrypted / authenticated** — bare
`is_connected()` returns `true` while the link is still plaintext, and
inferring security from `encrypted` / `authenticated` separately on the
caller side is exactly the kind of leak the callback is designed to
plug.

**Threading**: same context and rules as `ble_uart_on_rx` (NimBLE host
task / Bluedroid BTC task). Don't block, don't call `ble_uart_close` /
`ble_uart_uninstall` from inside the callback — use
`ble_uart_close_async()` (§5.3.2) if you need to teardown in response
to an event.

**Exception — `BLE_UART_EVT_CLOSED`**: this single event fires from
the close-async worker task instead of the BLE host task; by the time
it runs the host task is already gone. Keep the handler short: set a
flag or notify an app task — do **not** call `ble_uart_uninstall()`
here (see §5.3.2). The worker clears `s_closing` only after your
handler returns.

**Ordering contracts (both backends)**:

- A single CCCD value change fires exactly one `SUBSCRIBED` event
  (edge-triggered — repeating the same write is a no-op).
- If the central was subscribed at the moment the link drops, you get
  `SUBSCRIBED(false)` **before** `DISCONNECTED`. NimBLE does this
  natively (`BLE_GAP_SUBSCRIBE_REASON_TERM`); the Bluedroid backend
  synthesizes the same sequence so consumers can write a single state
  machine that works on either host.
- `LINK_SECURE` always arrives after `CONNECTED` — pairing can't run
  without a link.
- `BLE_UART_EVT_CLOSED` always arrives **after**
  `BLE_UART_EVT_DISCONNECTED` (when there was a peer) — the
  close-async worker calls the same disconnect+wait sequence as the
  synchronous `ble_uart_close()` before firing CLOSED.

**Backend differences**:

- `BLE_UART_EVT_LINK_SECURE.key_size`: NimBLE reports the negotiated
  size (7..16); Bluedroid surfaces a fixed 16 — Bluedroid sets
  `ESP_BLE_SM_MAX_KEY_SIZE=16` at install time and does not expose the
  negotiated size on `AUTH_CMPL`.
- Bonded reconnects: NimBLE re-fires `LINK_SECURE` on every encryption
  change; Bluedroid only fires `AUTH_CMPL_EVT` when the SM exchange
  actually runs, so a pure LTK-restart may not refire the event.
- CCCD persistence on bonded reconnect: NimBLE re-fires
  `SUBSCRIBED(true)` automatically (via `BLE_GAP_SUBSCRIBE_REASON_RESTORE`)
  when the bonded peer reconnects; Bluedroid does not persist CCCD
  across connections, so the central has to write CCCD again to
  resubscribe.

### 5.3 Lifecycle — bring-up and release

#### API summary

```c
int  ble_uart_install(const ble_uart_config_t *cfg);
int  ble_uart_open(void);
int  ble_uart_close(void);
int  ble_uart_close_async(void);   /* fire-and-forget, see §5.3.2–5.3.4 */
int  ble_uart_uninstall(void);
```

| Function | What it does (NimBLE) | What it does (Bluedroid) | When to call | Blocking? |
| --- | --- | --- | --- | --- |
| `install` | `nimble_port_init` + `ble_hs_cfg` + SM + SIG services + UART GATT | `controller_init/enable` + `bluedroid_init/enable` + SM + `esp_ble_gatts_create_attr_tab` (waits ≤500 ms for the attr-table event) | After `nvs_flash_init()`, before `open` | No, ~50 ms (NimBLE) / ~150 ms (Bluedroid) |
| `open` | Spawn host task + `ble_hs_start` (first time via `BLE_HS_AUTO_START`, later via `ble_hs_sched_start`) + advertising once synced; after a prior `close`, re-queues GAP/GATT/UART svc defs (§5.3.1a) | Configure adv data + scan rsp + start advertising (GATT table from `install` stays up) | After `install` | No, host runs in the background |
| `close` | Stop adv → graceful disconnect (≤500 ms) → `nimble_port_stop()` → `ble_gatts_reset()` | Stop adv → graceful disconnect (≤500 ms); host + GATT table stay up | After `open`, before `uninstall`; **not** from host-task callbacks (§5.3.2) | Yes, up to ~500 ms (NimBLE) |
| `close_async` | Worker runs the same body as `close`, then `BLE_UART_EVT_CLOSED` | Same | From `on_event` / `on_rx` (host task) when sync `close` would deadlock | No (returns once worker is spawned) |
| `uninstall` | `close` if still open (+ poll in-flight `close_async` ≤~5 s), then `nimble_port_deinit`, wipe module state | Same + controller deinit | After the radio is fully closed (§5.3.2); **not** from host-task callbacks | Yes |

**Bring-up** (every product):

```text
nvs_flash_init()
    └── ble_uart_install(&cfg)     /* once per uninstall cycle */
            └── ble_uart_open()    /* advertising + pairing; BLE is live */
```

Run-forever firmware can stop here — no `close` / `uninstall` required.

**Release** — pick **one** path below. `close` stops the radio but keeps
`install` state (you can `open()` again). `uninstall` tears the host +
controller down so `install()` can run from scratch.

| Goal | Call sequence | Who calls `close` / `uninstall` |
| --- | --- | --- |
| Power BLE off from a **normal app task** (button, Wi-Fi, `app_main` shutdown) | `ble_uart_close()` → `ble_uart_uninstall()` | That app task only |
| Power BLE off **because of a BLE event** (RX command, failed pairing, policy) | `ble_uart_close_async()` in `on_event` / `on_rx` → wait for `BLE_UART_EVT_CLOSED` → `ble_uart_uninstall()` on an **app task** (§5.3.2) | `close_async` in callback; `uninstall` deferred |

Each API returns `BLE_UART_EALREADY` when the module is already in the
target state, so defensive `close` / `uninstall` at shutdown without
manual state checks is fine **as long as** you follow the release path
for your scenario.

#### 5.3.1 Path A — synchronous release (recommended default)

Use when teardown is **not** triggered from inside `on_event` /
`on_rx` (NimBLE host task / Bluedroid BTC task). This is what the
`ble_uart_service` example does.

```c
void shutdown_ble_from_app_task(void)
{
    int rc;

    rc = ble_uart_close();
    if (rc != BLE_UART_OK && rc != BLE_UART_EALREADY) {
        ESP_LOGE(TAG, "ble_uart_close rc=%d", rc);
    }

    rc = ble_uart_uninstall();
    if (rc != BLE_UART_OK && rc != BLE_UART_EALREADY) {
        ESP_LOGE(TAG, "ble_uart_uninstall rc=%d", rc);
    }
    /* BLE UART fully released — safe to ble_uart_install() again */
}
```

```text
ble_uart_open()          /* running */
        │
        ▼
ble_uart_close()         /* same app task; not from on_event / on_rx */
        │
        ▼
ble_uart_uninstall()
```

- `uninstall` may call `close` internally if you skipped `close` — still
  call both explicitly so return codes are obvious in your logs.
- Do **not** call `close` or `uninstall` from `on_event` / `on_rx` — use
  Path B instead.

#### 5.3.1a Pausing and resuming (`close` then `open` again)

`install` state is preserved across `close()` — you may call `open()`
again without `uninstall()`. This is what the `ble_uart_service` example
exercises in `app_main` (open → close → open) to prove the cycle.

**NimBLE backend**

| Topic | Behaviour |
| --- | --- |
| GATT services | Same set as after `install`: GAP (`0x1800`), GATT (`0x1801`), BLE UART (NUS). `close()` calls the public `ble_gatts_reset()`; the next `open()` re-runs `ble_svc_gap_init()`, `ble_svc_gatt_init()`, and re-adds the UART service. |
| ATT handles | **Not stable** — centrals must run a full service discovery after each reconnect; do not cache handles across a `close`/`open` cycle. |
| Subscriptions | Cleared — the central must re-enable TX notifications (CCCD). |
| Bonds | NVS bond store is unchanged (still configured at `install()`). |
| First vs later `open` | With default `BLE_HS_AUTO_START`, the first `open()` consumes the one-shot auto-start queued by `nimble_port_init()`; every later `open()` must call `ble_hs_sched_start()` (handled inside `ble_uart_open()`). |

**Bluedroid backend**

`close()` only stops advertising and disconnects; the host and attribute
table created at `install()` stay registered. A second `open()` restarts
advertising. GATT handles are typically unchanged.

**Extra GATT services (§6.3)**

Services you register with `ble_gatts_add_svcs()` / `ble_svc_*_init()`
at `install()` time are **not** automatically re-registered by
`ble_uart` on a later `open()` after `close()` (NimBLE only re-adds
GAP, GATT, and UART). Either call your init/add functions again inside
your own `open()` hook after `ble_uart_close()`, or use
`close()` → `uninstall()` → `install()` → `open()` for a full rebuild.

#### 5.3.2 Path B — release after a BLE event (`close_async`)

Use when the **reason** to shut down arrives on the host task (e.g.
`BLE_UART_EVT_PAIRING_FAILED`, an RX “power off” byte, or
`LINK_SECURE` policy). Synchronous `close()` deadlocks there; use
`close_async()` and **defer** `uninstall()` to a normal task.

```c
static volatile bool s_ble_closed_ok;

static void on_event(const ble_uart_evt_t *e)
{
    switch (e->id) {
    case BLE_UART_EVT_PAIRING_FAILED:
        ble_uart_close_async();   /* OK: host-task context */
        break;

    case BLE_UART_EVT_CLOSED:
        /* Runs on the close-async worker — keep this short. Do NOT call
         * ble_uart_uninstall() here (s_closing is still set; see §5.3.3). */
        if (e->closed.status == BLE_UART_OK) {
            s_ble_closed_ok = true;   /* or xTaskNotifyGive / queue */
        }
        break;
    default:
        break;
    }
}

void ble_shutdown_task(void *arg)
{
    (void)arg;
    for (;;) {
        if (s_ble_closed_ok) {
            s_ble_closed_ok = false;
            ble_uart_uninstall();     /* normal app task */
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}
```

```text
on_event / on_rx (host task):
    ble_uart_close_async()
            │
            ▼
    [worker: do_close ≈ sync close]
            │
            ├── BLE_UART_EVT_DISCONNECTED  (if peer was connected)
            └── BLE_UART_EVT_CLOSED        (worker task; set flag only)
            │
            ▼
app task (not host, not inside CLOSED handler):
    ble_uart_uninstall()
```

- `close_async` returns `BLE_UART_OK` once the worker is **spawned**, not
  when close finishes.
- Only `BLE_UART_EVT_CLOSED` with `.closed.status == BLE_UART_OK` means
  the same quiesced state as `ble_uart_close()` — then it is safe to
  `uninstall()` from your app task.
- On failure (`BLE_UART_EFAIL`, etc.) the port may still be open; retry
  `ble_uart_close()` / `ble_uart_close_async()` from an app task.

#### 5.3.3 `close_async` + `uninstall` — rules and pitfalls

`ble_uart_uninstall()` **polls** an in-flight `close_async` worker for
up to **~5 s**. If the worker has not exited it logs
`uninstall: close_async worker still running, tearing down anyway` and
continues anyway — treat that as an application bug, not a supported
path.

| Do | Don't |
| --- | --- |
| `close_async()` in `on_event` / `on_rx`; `uninstall()` later on **one** app task after `CLOSED` + `BLE_UART_OK` | `uninstall()` in the same task right after `close_async()` without waiting |
| Set a flag / queue in `BLE_UART_EVT_CLOSED`; return immediately | `ble_uart_uninstall()` **inside** `BLE_UART_EVT_CLOSED` (worker still holds `s_closing`) |
| Sync `close` + `uninstall` from a button / network task | `close` / `uninstall` from host-task callbacks |
| Keep `on_event` / `on_rx` short while a close is in flight | Multi-second blocking in callbacks during `close_async` |
| After a timeout log, fix ordering before `install()` again | Immediate `install()` + `open()` + `close_async()` after a wedged teardown |

If you see `uninstall: close_async worker still running, tearing down
anyway`, fix call ordering (§5.3.2) before calling `install()` again.

#### 5.3.4 `ble_uart_close_async()` — reference

Some applications need to teardown the radio in response to a BLE
event — examples: a "shutdown" command on RX, a `LINK_SECURE` whose
flags don't meet the application's policy, or a `PAIRING_FAILED` from
a peer that's been blacklisted. Because the synchronous `close()` is
called *from* the host task it would normally run on, calling it
inline would deadlock. `close_async()` papers over that: it spawns a
small worker task (~3 KB stack, idle+2 priority) that runs the same
close body, then signals completion via the event callback.

**Behaviour** (see §5.3.2 for the full release flow):

- `close_async` returns `BLE_UART_OK` once the worker has been spawned.
- `BLE_UART_EVT_DISCONNECTED` (if connected) then `BLE_UART_EVT_CLOSED`
  with `.closed.status` — same ≤500 ms disconnect window as sync `close`.
- Second call while draining → `BLE_UART_EALREADY`; before `open` →
  `BLE_UART_EALREADY`; spawn failure → `BLE_UART_ENOMEM` (latch reset).

### 5.4 TX interface

```c
int ble_uart_tx(const uint8_t *data, size_t len);
```

For formatted output, format into your own buffer with `snprintf` first
and pass it to `ble_uart_tx`:

```c
char line[64];
int n = snprintf(line, sizeof(line), "temp=%d.%d\n", t / 10, t % 10);
ble_uart_tx((const uint8_t *)line, (size_t)n);
```

**Return values**:

| Return | Meaning |
| --- | --- |
| `BLE_UART_OK` | Success (notification handed to the stack) |
| `BLE_UART_ENOTCONN` | No central connected; **this is normal — typically ignore** |
| `BLE_UART_EINVAL` | `data == NULL` or `len == 0` |
| `BLE_UART_ENOMEM` | Stack mbuf pool exhausted |
| `BLE_UART_EFAIL` | Internal stack error — see logs |

**Calling context**: any FreeRTOS task at any priority. **Not callable
from an ISR** — push the data to a queue from the ISR and let a task
call `ble_uart_tx`.

**Auto-fragmentation**: regardless of buffer size, the implementation
splits the payload into successive notifications of `(MTU - 3)` bytes.
The central receives them in transmission order.

### 5.5 Status queries

```c
bool ble_uart_is_connected(void);
bool ble_uart_is_subscribed(void);
```

- `is_connected()`: a central is connected (it may not be paired yet).
- `is_subscribed()`: the central has subscribed to TX notifications
  (note: bonded reconnects often skip CCCD writes).

You usually **don't need** to query these up-front — `ble_uart_tx`
returns `ENOTCONN` to tell you.

### 5.6 Security configuration

`cfg.encrypted` is a one-line **preset** that turns on every part of
the stack's security toolbox at once — LE Secure Connections, bonding
(LTK persisted in NVS), MITM protection, DisplayOnly IO, and the
`_ENC | _AUTHEN` flags on the GATT characteristics. It maps to the
older two-state behaviour and is what the "secure by default" template
in §4.4 picks.

For applications that need finer control — a displayless gateway that
still wants encrypted bonding, a one-shot encrypted session that
doesn't keep an LTK, an interop test build that disables only MITM —
each component of the preset can be flipped individually through the
`cfg.security` sub-struct:

```c
typedef enum {
    BLE_UART_SEC_AUTO = 0,   /* follow cfg.encrypted */
    BLE_UART_SEC_OFF  = 1,
    BLE_UART_SEC_ON   = 2,
} ble_uart_sec_t;

typedef enum {
    BLE_UART_IO_CAP_AUTO             = 0,  /* DisplayOnly when MITM is on;
                                              NoInputNoOutput when off.
                                              Passkey Display needs no on_event */
    BLE_UART_IO_CAP_NO_INPUT_OUTPUT  = 1,  /* Just Works only             */
    BLE_UART_IO_CAP_DISPLAY_ONLY     = 2,  /* Passkey Display — UART banner
                                              + optional PASSKEY_DISPLAY;
                                              no on_event required          */
    BLE_UART_IO_CAP_KEYBOARD_ONLY    = 3,  /* Passkey Entry — PASSKEY_REQUEST;
                                              on_event required             */
    BLE_UART_IO_CAP_DISPLAY_YES_NO   = 4,  /* Numeric Comparison;
                                              on_event required             */
    BLE_UART_IO_CAP_KEYBOARD_DISPLAY = 5,  /* PASSKEY_REQUEST or NUMERIC_COMPARE;
                                              on_event required             */
} ble_uart_io_cap_t;

typedef struct {
    ble_uart_sec_t    sc;       /* tri-state */
    ble_uart_sec_t    bonding;  /* tri-state */
    ble_uart_sec_t    mitm;     /* tri-state */
    ble_uart_io_cap_t io_cap;   /* AUTO + the five IO caps above */
} ble_uart_security_t;
```

Each of `cfg.security.{sc,bonding,mitm}` is a tri-state. `AUTO`
(the value of any zero-initialised member) inherits from
`cfg.encrypted`; `OFF` / `ON` override that specific bit only. The
resolution table:

| `cfg.encrypted` | Override field | Resolved bit |
| --- | --- | --- |
| `true`  | `AUTO`              | ON  |
| `true`  | `OFF`               | OFF |
| `true`  | `ON`                | ON  |
| `false` | `AUTO`              | OFF |
| `false` | `OFF`               | OFF |
| `false` | `ON`                | ON  |

`cfg.security.io_cap` follows the same `AUTO` / explicit pattern.
The application picks an IO cap matching its UI; the SM combines it
with the central's IO cap to elect the pairing model (see BT Core
Spec §2.3.5.1) and ble_uart fires the matching event:

| Pairing model | Trigger event | Application response |
| --- | --- | --- |
| Just Works                | (none — pairs silently)        | — |
| Passkey Display (we show) | `BLE_UART_EVT_PASSKEY_DISPLAY` (optional; UART banner always) | (none — port handles SM reply; central types the digits) |
| Passkey Entry (user types)| `BLE_UART_EVT_PASSKEY_REQUEST` | `ble_uart_passkey_reply(d)` — **`on_event` required** |
| Numeric Comparison        | `BLE_UART_EVT_NUMERIC_COMPARE` | `ble_uart_compare_reply(b)` — **`on_event` required** |

Numeric Comparison additionally requires LE Secure Connections on
both sides (legacy SM doesn't support it); against a legacy peer a
`DISPLAY_YES_NO` / `KEYBOARD_DISPLAY` IO cap falls back to either
Passkey Entry (with our keypad) or Just Works.

#### What is checked synchronously

`ble_uart_install()` rejects the following with `BLE_UART_EINVAL`
**before** bringing the host stack up, so misconfigured applications
fail fast and predictably:

- `cfg.security.{sc,bonding,mitm}` outside `{AUTO, OFF, ON}`
- `cfg.security.io_cap` outside the six values listed above
- Resolved `mitm == ON` together with resolved
  `io_cap == NO_INPUT_OUTPUT` — Just Works cannot satisfy MITM and
  the SM would otherwise reject pairing in flight
- `cfg.on_event == NULL` together with a **configured** (not resolved)
  input-capable `io_cap` — only `KEYBOARD_ONLY`, `DISPLAY_YES_NO`, and
  `KEYBOARD_DISPLAY`. Without an event sink the application would never
  see `PASSKEY_REQUEST` / `NUMERIC_COMPARE` and pairing would silently
  stall until the SM times out. **`AUTO` (even when it resolves to
  DisplayOnly because `mitm=ON`), `DISPLAY_ONLY`, and `NO_INPUT_OUTPUT`
  do not require `on_event`** — Passkey Display is satisfied inside the
  port (UART log + internal SM reply); `PASSKEY_DISPLAY` via `on_event`
  is additive only.

#### How the resolved policy is applied

| Component | Effect |
| --- | --- |
| Resolved `sc` / `bonding` / `mitm` (any ON) | SM is enabled; `ble_gap_security_initiate` (NimBLE) / `esp_ble_set_encryption` (Bluedroid) runs on connect |
| Resolved `mitm` | `ESP_BLE_SEC_ENCRYPT_MITM` vs `_NO_MITM` (Bluedroid); `_AUTHEN` flag added to GATT chars |
| Any of the three on | Encrypted GATT permission flags (`_ENC` on NimBLE, `_ENCRYPTED` on Bluedroid) |
| All three off | Plain `READ`/`WRITE` permissions; SM disabled |
| Resolved `io_cap` | `BLE_HS_IO_*` (NimBLE) / `ESP_IO_CAP_*` (Bluedroid) |

#### Common combinations

```c
/* (a) Default — secure-by-default UART. SC + Bonding + MITM, DisplayOnly.
 *     on_event may be NULL: passkey is printed to UART and pairing
 *     completes without PASSKEY_DISPLAY / reply callbacks. */
ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    /* security.{sc,bonding,mitm,io_cap} all AUTO → all ON. */
    /* .on_event = NULL  — valid for this preset */
});

/* (b) Displayless gateway. SC + Bonding + Just Works (no passkey UI). */
ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    .security  = {
        .mitm   = BLE_UART_SEC_OFF,
        .io_cap = BLE_UART_IO_CAP_NO_INPUT_OUTPUT,
    },
});

/* (c) Encrypted but ephemeral. Re-pair every reconnect, no NVS bond. */
ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    .security  = { .bonding = BLE_UART_SEC_OFF },
});

/* (d) Plaintext lab build. */
ble_uart_install(&(ble_uart_config_t){
    .encrypted = false,
    /* security.* all AUTO → all OFF. */
});

/* (e) Interop test — keep encryption + bonding, drop MITM only. */
ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    .security  = { .mitm = BLE_UART_SEC_OFF },
    /* security.io_cap AUTO → NoInputNoOutput once MITM is gone. */
});

/* (f) Passkey Entry — peripheral has a keypad, central has a display.
 *     User reads the 6-digit code off the central and types it here.
 *     on_event MUST be set; the application wires PASSKEY_REQUEST to
 *     a UI prompt and feeds the digits to ble_uart_passkey_reply(). */
ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    .security  = { .io_cap = BLE_UART_IO_CAP_KEYBOARD_ONLY },
    .on_event  = on_event,
    ...
});

/* (g) Numeric Comparison — peripheral has display + yes/no button.
 *     Both sides see the same 6-digit value; user confirms match.
 *     Requires LE Secure Connections (so .sc must be ON, which it is
 *     by default with .encrypted=true). on_event MUST be set. */
ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    .security  = { .io_cap = BLE_UART_IO_CAP_DISPLAY_YES_NO },
    .on_event  = on_event,
    ...
});

/* (h) Touchscreen UI — full keypad+display. The SM elects either
 *     Passkey Entry or Numeric Comparison depending on the central;
 *     wire BOTH events. */
ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    .security  = { .io_cap = BLE_UART_IO_CAP_KEYBOARD_DISPLAY },
    .on_event  = on_event,
    ...
});
```

#### 5.6.1 Pairing reply API

**Passkey Display (default / `DISPLAY_ONLY` / `AUTO` + `mitm=ON`)** does
not use the reply APIs. The port generates the 6-digit value, logs it,
and drives the SM (NimBLE: `ble_sm_inject_io` on `BLE_SM_IOACT_DISP`;
Bluedroid: no `esp_ble_passkey_reply` needed on `PASSKEY_NOTIF`). You
only need `ble_uart_passkey_reply()` / `ble_uart_compare_reply()` for
the interactive models below.

`Passkey Entry` and `Numeric Comparison` are interactive — the SM
suspends pairing until the application reports the user's input.
`ble_uart` exposes one reply call per flavour:

```c
int ble_uart_passkey_reply(uint32_t passkey);   /* 0..999999 */
int ble_uart_compare_reply(bool match);
```

Both are safe from any task, return immediately, and accept exactly
one reply per request. Subsequent calls (or calls with no request in
flight) return `BLE_UART_ENOTCONN`. `passkey > 999999` returns
`BLE_UART_EINVAL`. If the user fails to reply before the SM's pairing
timeout (controller default ≈ 30 s), the link surfaces
`BLE_UART_EVT_PAIRING_FAILED` and any later reply is silently dropped.

```c
static void on_event(const ble_uart_evt_t *e)
{
    switch (e->id) {
    case BLE_UART_EVT_PASSKEY_REQUEST:
        /* Prompt the user; once digits are entered: */
        ble_uart_passkey_reply(user_input);   /* 0..999999 */
        break;

    case BLE_UART_EVT_NUMERIC_COMPARE:
        ESP_LOGI(TAG, "compare %06" PRIu32, e->numeric_compare.passkey);
        /* Once the user confirms: */
        ble_uart_compare_reply(true /* or false on mismatch */);
        break;

    default: break;
    }
}
```

A `false` reply to `compare_reply()` makes pairing fail with a
numeric-comparison mismatch — surfaced as
`BLE_UART_EVT_PAIRING_FAILED`. To cancel `PASSKEY_REQUEST` without a
mismatch event, just don't call `passkey_reply()`; the SM will time
out the pairing.

#### Backend differences

- **Passkey Display without `on_event`**: both backends complete pairing;
  only `PASSKEY_DISPLAY` is suppressed when the callback is `NULL`. The
  UART banner (`show_passkey`) is always emitted for log-scraping tests.
- **Numeric Comparison edge case**: if `io_cap` resolved to DisplayOnly
  but the central still negotiates NC (rare), Bluedroid rejects the
  request when `on_event == NULL`; NimBLE may stall until the SM times
  out — use `DISPLAY_YES_NO` / `KEYBOARD_DISPLAY` with a registered
  `on_event` if you need NC.
- **NimBLE** lets the application observe the negotiated `key_size`
  on `BLE_UART_EVT_LINK_SECURE`; **Bluedroid** surfaces a fixed `16`
  (the value forced via `ESP_BLE_SM_MAX_KEY_SIZE` at install time —
  Bluedroid does not expose the negotiated size on `AUTH_CMPL`).
- With `mitm=OFF`, NimBLE pairs with `_AUTHEN` permissions still
  off on the chars; Bluedroid uses `ESP_GATT_PERM_*_ENCRYPTED`
  (the encryption-without-MITM tier) to match.
- `cfg.encrypted=false` plus any `cfg.security.*=ON` override is
  allowed — it partially enables the SM, e.g.
  `cfg.encrypted=false, cfg.security.sc=ON` is "SC pairing without
  MITM and without persisted bond". Useful only for lab interop
  tests; production firmware should keep `cfg.encrypted = true` and
  only override surgically.

### 5.7 Bond management

```c
/* All three are usable as soon as ble_uart_install() returns; they
 * do not require ble_uart_open() to have been called yet — clearing
 * stale bonds before the first advertising window is the canonical
 * use case. */
int  ble_uart_get_bond_count(size_t *out_count);
int  ble_uart_get_bonded_peers(ble_uart_addr_t *out, size_t cap, size_t *out_count);
int  ble_uart_remove_peer(const ble_uart_addr_t *peer);
int  ble_uart_clear_bonds(void);

/* Address type used by remove_peer and BLE_UART_EVT_CONNECTED. */
typedef struct {
    uint8_t bytes[6];   /* big-endian: bytes[0] is the MSB octet */
    uint8_t type;       /* BLE_UART_ADDR_TYPE_PUBLIC or _RANDOM */
} ble_uart_addr_t;
```

| Function | Effect |
| --- | --- |
| `ble_uart_get_bond_count`   | Number of peers in the persistent store; 0 means "no bonds yet". Pass `cap == 0` to `get_bonded_peers` for the same count without an address buffer. |
| `ble_uart_get_bonded_peers` | List bonded peer addresses; writes up to `cap`, reports total in `*out_count` (caller may re-call with a larger buffer if total > cap). `cap == 0` returns the count only. |
| `ble_uart_remove_peer`      | Drop one peer's LTK / IRK / persisted CCCD. **Idempotent** — returns `BLE_UART_OK` even when the peer is not in the store (NimBLE: `ble_store_util_delete_peer` treats `BLE_HS_ENOENT` as success; Bluedroid: `esp_ble_remove_bond_device` does not fail on a missing entry). Call `get_bonded_peers()` first if you need to tell "removed" from "was never bonded". |
| `ble_uart_clear_bonds`      | Drop *all* of the above; equivalent to a factory reset of the bond store, but does not touch any other NVS namespace |

`ble_uart_remove_peer` and `ble_uart_clear_bonds` do **not** actively
disconnect the current link (encrypted or not). Call `ble_uart_close()`
first if you need an immediate disconnect and re-pair.

**Where do I get the address?** From `BLE_UART_EVT_CONNECTED.connected.peer`
(see §5.2.1). Save it from your event handler the first time you see
each new peer, then pass it to `ble_uart_remove_peer` later when you
want to forget it.

**Backend notes**:

- Bluedroid matches bonds by BD address alone — `peer->type` is
  ignored by `remove_peer`. If the peer first connected as
  `address_A` and bonding succeeded, CONNECT and `get_bonded_peers()`
  keep reporting `address_A` on later reconnects even when the
  peer's over-the-air address has changed (e.g. a new RPA).
- NimBLE matches by `(type, identity-address)` — for an RPA peer this
  is the resolved identity, **not** the random address you saw on the
  wire. `BLE_UART_EVT_CONNECTED` reports the resolved identity when
  it's known (post-pairing reconnect of a bonded RPA peer); on first
  pair it equals the OTA random address, so the bond is recorded
  under that random address and `remove_peer` works either way.
- Neither backend reports "peer not found" from `remove_peer` — a
  wrong `(type, bytes)` pair that does not match any stored bond
  still returns `BLE_UART_OK`. This mirrors the underlying stacks'
  delete-if-present semantics, not a lookup-then-delete API.
- `ble_uart_clear_bonds` on Bluedroid iterates the bond list and
  removes each entry; on NimBLE it calls `ble_store_clear()`, which
  also wipes the local LTK and any persisted CCCD.
- **NimBLE** `get_bond_count` / `get_bonded_peers(cap=0)` heap-allocate a
  scratch buffer sized to `BLE_STORE_MAX_BONDS` (not the caller's stack),
  so they are safe from small-stack tasks regardless of
  `CONFIG_BT_NIMBLE_MAX_BONDS`.

### 5.8 Service UUID constant

```c
extern const ble_uart_uuid128_t ble_uart_service_uuid;
```

Always `6e400001-b5a3-f393-e0a9-e50e24dcca9e` (the de-facto BLE UART service UUID). It is
already inserted into the scan response **by the default payload**, so
the application only needs to reference it when it takes over the adv
bytes itself (see §5.9) or otherwise replaces our advertising (see §6.3).

### 5.9 Custom advertising payloads

`ble_uart` builds a sensible default for both the primary advertisement
and the scan response:

| Packet | Default content | Why |
| --- | --- | --- |
| Primary adv (31 B max) | Flags AD + Complete Local Name (`device_name`) | Phones show the name; everything else in the 31 bytes is left for the application to add via `adv_data` |
| Scan response (31 B max) | Complete 128-bit BLE UART service UUID (18 B element) | The 128-bit UUID alone is too big to share the primary packet with a typical name |

Set `adv_data` / `scan_rsp_data` in the config to override **everything
the application sees** — only the 3-byte Flags AD element of the primary
packet stays library-controlled (the BT spec mandates a Flags element,
and a few of its bits — General Discoverable / BR-EDR Not Supported —
are state we already negotiated with the controller).

```c
/*  +-- 31-byte primary advertisement packet ---------------------+
 *  | [02 01 06]  ← Flags AD prepended by ble_uart (3 bytes)      |
 *  | <up to BLE_UART_ADV_DATA_MAX = 28 bytes from your buffer>   |
 *  +-------------------------------------------------------------+
 *
 *  +-- 31-byte scan-response packet -----------------------------+
 *  | <up to BLE_UART_SCAN_RSP_DATA_MAX = 31 bytes from your buf> |
 *  +-------------------------------------------------------------+
 */
```

`adv_data_len` is checked at install time; oversized buffers fail with
`BLE_UART_EINVAL`. Both buffers are copied into module-private storage,
so the caller's pointers do not need to outlive the call.

**Format**: a sequence of standard BT Core "AD structure" triplets —
`[length(1)] [AD type(1)] [value(length-1)]`. AD-type values are
defined in the *Bluetooth Assigned Numbers* document
([Generic Access Profile, §1](https://www.bluetooth.com/specifications/assigned-numbers/)).
Common ones:

| Type | Name | Value format |
| --- | --- | --- |
| `0x09` | Complete Local Name | UTF-8 bytes |
| `0x08` | Shortened Local Name | UTF-8 bytes |
| `0x0A` | TX Power Level | 1 signed byte (dBm) |
| `0x07` | Complete List of 128-bit Service UUIDs | 16 bytes per UUID |
| `0xFF` | Manufacturer Specific Data | 2-byte company ID + payload |

**Example — replace the default with name + UUID + 4 bytes of vendor data**

```c
static const uint8_t adv_payload[] = {
    /* Complete Local Name "MyDev"  (1 + 1 + 5 = 7 bytes) */
    0x06, 0x09, 'M', 'y', 'D', 'e', 'v',

    /* Complete 128-bit Service UUID — bytes are little-endian on air,
     * matching ble_uart_service_uuid.bytes[]. (1 + 1 + 16 = 18 bytes) */
    0x11, 0x07,
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e,
    /* total = 7 + 18 = 25 bytes  (≤ BLE_UART_ADV_DATA_MAX = 28) */
};

static const uint8_t scan_rsp_payload[] = {
    /* Manufacturer Specific Data: Espressif Systems (0x02E5) + 4 bytes */
    0x07, 0xFF, 0xE5, 0x02, 0xDE, 0xAD, 0xBE, 0xEF,
};

ble_uart_install(&(ble_uart_config_t){
    .encrypted         = true,
    .device_name       = "MyDev",        /* GAP service value, peer-readable */
    .adv_data          = adv_payload,
    .adv_data_len      = sizeof(adv_payload),
    .scan_rsp_data     = scan_rsp_payload,
    .scan_rsp_data_len = sizeof(scan_rsp_payload),
    .ble_uart_on_rx    = on_rx,
    .on_event          = on_event,
});
```

**Notes**:

- `device_name` and `adv_data` are independent. The first is the GAP
  service value that any connected peer can read over GATT; the second
  is what scanners see before connecting. If you want the device name
  visible during a scan, include a Complete-Local-Name AD element
  (`0x09`) in `adv_data` yourself — providing custom `adv_data`
  disables the auto-include path.
- The 31-byte packet limit is BLE 4.x legacy advertising. Extended
  advertising (BLE 5.0) is **not** wired through this API — both
  backends fall back to legacy advertising for portability.
- Set only one half if you want the other to keep its default — e.g.
  custom `adv_data` with `scan_rsp_data = NULL` keeps the default
  service-UUID scan response.

---

## 6. Advanced Usage

### 6.1 Different RX framing strategies

**A. Split on `\n` (suits ASCII protocols / JSON)**

```c
static uint8_t  s_buf[1024];
static size_t   s_len;

static void on_rx(const uint8_t *d, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (d[i] == '\n') { handle_line(s_buf, s_len); s_len = 0; }
        else if (s_len < sizeof s_buf) s_buf[s_len++] = d[i];
    }
}
```

**B. Length-prefixed binary frames**

```c
static void on_rx(const uint8_t *d, size_t n)
{
    static uint16_t need = 0;
    static uint8_t  frame[256];
    static size_t   got = 0;

    for (size_t i = 0; i < n; i++) {
        if (need == 0) { need = d[i]; got = 0; continue; }
        frame[got++] = d[i];
        if (got == need) { handle_frame(frame, got); need = 0; }
    }
}
```

**C. Forward straight to UART**

```c
static void on_rx(const uint8_t *d, size_t n)
{
    uart_write_bytes(UART_NUM_1, (const char *)d, n);
}
```

### 6.2 Disabling encryption (lab scenarios)

```c
ble_uart_install(&(ble_uart_config_t){
    .encrypted = false,        /* ← turn it off */
    .device_name = "OpenDev",
    .ble_uart_on_rx = ...,
});
```

Effect:
- GATT characteristics drop the `_ENC | _AUTHEN` flags.
- Any central can read/write — no pairing required.
- No passkey prompt.
- Data is sniffable by any nearby BLE sniffer or compromised radio in range.

**Do not ship this in production firmware.**

### 6.3 Coexisting with other GATT services

> The snippet below is for the **NimBLE backend**. With Bluedroid, register
> additional profiles via `esp_ble_gatts_app_register()` before calling
> `ble_uart_open()` — the gating rule is the same: extra services must
> be in place before advertising starts.

`ble_uart` registers its own service; you can call `ble_gatts_add_svcs()`
**multiple times** and NimBLE will build all of them into the GATT
table. **Caveat**: this must happen before the **first** `ble_uart_open()`
for that `install()` cycle, otherwise the host task is already running
and the GATT table is locked. If you use `ble_uart_close()` and later
`ble_uart_open()` without `uninstall()`, you must call your extra
`ble_svc_*_init()` / `ble_gatts_add_svcs()` again before that second
`open()` — see §5.3.1a.

```c
ble_uart_install(&cfg);

/* Register your extra services before open() */
ble_svc_dis_init();              /* Device Information Service */
my_battery_service_init();       /* your own battery service */

ble_uart_open();
```

> If your service must appear in the **advertising packet**, you have
> to bypass `ble_uart`'s internal advertising logic — override
> `ble_hs_cfg.sync_cb` with your own implementation after
> `ble_uart_install`, then call `ble_uart_open()`. Note that
> `ble_uart`'s internal `start_advertising` will not run, so you must
> call `ble_gap_adv_start` yourself. In that case, just fork
> `ble_uart_nimble.c` (or the matching `ble_uart_bluedroid.c`).

### 6.4 Configuring the device name via Kconfig

If you use the shared `ble_uart` component, options are already in
`menuconfig → Component configuration → ESP-BLE-UART library`. If you copied only
the `.c` / `.h` files into `main/`, copy `Kconfig` from `common/ble_uart/` as
well (or merge its symbols into your own `Kconfig.projbuild`), then:

The bundled example builds a per-unit name as `<prefix>-XXXX` where
`XXXX` is the last two BT MAC bytes in hex:

```c
uint8_t mac[6] = {0};
esp_read_mac(mac, ESP_MAC_BT);
char name[BLE_UART_DEVICE_NAME_MAX + 1];
snprintf(name, sizeof(name), "%s-%02X%02X",
         CONFIG_BLE_UART_DEVICE_NAME_PREFIX, mac[4], mac[5]);

ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    .device_name = name,
    .ble_uart_on_rx = on_rx,
});
```

Edit the prefix through `menuconfig → Component configuration →
ESP-BLE-UART library → BLE device name prefix`.

For a fixed name on every unit, skip the MAC suffix and pass any
string ≤ `BLE_UART_DEVICE_NAME_MAX` directly to `device_name`.

### 6.5 Pushing data proactively

You can call TX from any task:

```c
/* A periodic sensor-reporting task */
static void sensor_task(void *arg)
{
    char line[64];
    while (1) {
        int t = read_temperature();
        int n = snprintf(line, sizeof(line), "temp=%d.%d\n", t / 10, t % 10);
        ble_uart_tx((const uint8_t *)line, (size_t)n);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Spawn it from app_main */
xTaskCreate(sensor_task, "sensor", 3072, NULL, 5, NULL);
```

When nobody is subscribed, `ble_uart_tx` returns `BLE_UART_ENOTCONN` —
**just ignore it**.

---

## 7. Calling Context & Thread Safety

| Function | Calling context | Thread-safe |
| --- | --- | --- |
| `ble_uart_install` | Any task; once per uninstall cycle | One-shot until `uninstall` |
| `ble_uart_open` | Any task; after `install` | One-shot until `close` |
| `ble_uart_close` | Any task **except the BLE host task** (NimBLE host task / Bluedroid BTC task) | Idempotent; second call returns `EALREADY` |
| `ble_uart_close_async` | Any task — including the BLE host task (use this from inside `on_rx` / `on_event`) | Idempotent; second call while a worker is draining returns `EALREADY` |
| `ble_uart_uninstall` | Any task **except the BLE host task** | Idempotent; see §5.3 release paths; polls in-flight `close_async` ≤~5 s. Best-effort teardown: returns the **first** `BLE_UART_*` failure (`ble_uart_close` or translated `esp_err_t`) but always wipes module state so a retry is possible. |
| `ble_uart_tx` | Any FreeRTOS task | Yes — multi-task concurrent |
| `ble_uart_is_connected` / `is_subscribed` | Any context | Yes (bool read; best-effort snapshot) |
| `ble_uart_on_rx` / `on_event` callback | BLE host task (NimBLE host task / Bluedroid BTC task); **`BLE_UART_EVT_CLOSED` is the lone exception — fires on the close-async worker task** | Your code must not block, **must not call `close` / `uninstall`** — use `ble_uart_close_async()` instead |
| **Calling any `ble_uart` API from an ISR** | not allowed | Neither host stack supports it |

---

## 8. Memory / Performance

| Item | Footprint |
| --- | --- |
| Code segment (`ble_uart_nimble.c.o`) | ~14 KB (with `-Os`) |
| Code segment (`ble_uart_bluedroid.c.o`) | ~22 KB (with `-Os`; larger because long-write reassembly is open-coded) |
| Static RAM (globals + RX buffer) | ~1.1 KB (the bulk is `CONFIG_BLE_UART_RX_SCRATCH_SIZE`, default 1024 B) |
| Host task stack (NimBLE host / Bluedroid BTC) | 4 KB (default) |
| Controller task stack | ~3 KB (default) |
| Bond store (NVS) | ~80 bytes per bonded peer |
| ATT MTU | Negotiated; whatever you set in sdkconfig (247 / 256 / 512) |

Measured throughput (ESP32-S3, iPhone 14 Pro central, MTU 247):
- TX (notify): ~25 KB/s
- RX (write): ~20 KB/s

---

## 9. FAQ

| Symptom | Cause / fix |
| --- | --- |
| `nimble_port_init rc=...` | NVS not initialised, or BT controller not enabled |
| Compile error: `host/ble_hs.h` not found | `REQUIRES bt` is missing from CMakeLists |
| `ble_uart_install()` returns `BLE_UART_EINVAL` | Buffer too long (`device_name` / `adv_data` / `scan_rsp` limits in §5.9), impossible security (`mitm=ON` + `io_cap=NO_INPUT_OUTPUT`), **`io_cap` in `{KEYBOARD_ONLY, DISPLAY_YES_NO, KEYBOARD_DISPLAY}` with `on_event=NULL`** (note: default `AUTO` + `encrypted=true` and explicit `DISPLAY_ONLY` **do** allow `on_event=NULL`), or out-of-range `sc`/`bonding`/`mitm`/`io_cap`. See §5.6. |
| Pairing fails | Central uses "Just Works" but we require MITM (`encrypted=true`). Use a central that supports passkey entry |
| `enc_change status=13 encrypted=1 bonded=1` | `13 = BLE_HS_ETIMEOUT`. Bonded-reconnect race; **the link is actually encrypted — safe to ignore** |
| Notifications missing after a reconnect | Bonded centrals often skip the CCCD write; our TX path doesn't gate on subscription state, so notifications still go out — make sure the central side has its callback registered |
| Second connection rejected | `MAX_CONNECTIONS = 1` by default. For multi-connection support, bump the sdkconfig value and turn `s_conn_handle` (NimBLE backend) / `s_conn_id` (Bluedroid backend) into an array |
| Flash fills up | Bond entries accumulate. Periodically run `idf.py erase-flash`, or call `ble_store_clear()` in code |

---

## 10. Differences from This Example

If you **build directly on top of this example**:

| You already have | No further work needed |
| --- | --- |
| `main.c` echo template | Replace with your own `on_rx` body |
| `sdkconfig.defaults` | Reuse as-is |
| `sdkconfig.bluedroid` | Only if you switch to Bluedroid host — reuse as-is (see §4.3); omit for default NimBLE |
| Root `CMakeLists.txt` (`EXTRA_COMPONENT_DIRS` → `../common/ble_uart`) | Reuse as-is (or follow §3 option B) |
| `CMakeLists.txt` (root + main) | Reuse as-is |

If you **start from an empty project**:

| What you need to do | Source |
| --- | --- |
| Add `EXTRA_COMPONENT_DIRS` for `examples/bluetooth/common/ble_uart` in root `CMakeLists.txt`, **or** copy `ble_uart.h` + at least one of `ble_uart_nimble.c` / `ble_uart_bluedroid.c` into `main/` | §3 of this guide |
| Copy the key lines of `sdkconfig.defaults` | §4.3 of this guide |
| Add `REQUIRES ble_uart nvs_flash` (after `EXTRA_COMPONENT_DIRS`) **or** SRC + `REQUIRES bt nvs_flash` (copied sources) to `main/CMakeLists.txt` | §4.2 of this guide |
| Write `install` + `open` in `app_main` | §4.4 of this guide |

---

## 11. API Cheat Sheet (print and pin to the wall)

```c
#include "ble_uart.h"

/* === Types === */
typedef void (*ble_uart_rx_cb_t) (const uint8_t *data, size_t len);
typedef void (*ble_uart_evt_cb_t)(const ble_uart_evt_t *evt);

typedef struct {
    ble_uart_sec_t    sc;       /* AUTO / OFF / ON — follow `encrypted` when AUTO */
    ble_uart_sec_t    bonding;
    ble_uart_sec_t    mitm;
    ble_uart_io_cap_t io_cap;   /* AUTO / NO_INPUT_OUTPUT / DISPLAY_ONLY /
                                   KEYBOARD_ONLY / DISPLAY_YES_NO /
                                   KEYBOARD_DISPLAY */
} ble_uart_security_t;

typedef struct {
    bool encrypted;                 /* preset: SC + Bonding + MITM + DisplayOnly */
    ble_uart_security_t security;   /* per-feature overrides; see §5.6 */

    const char *device_name;        /* ≤ BLE_UART_DEVICE_NAME_MAX (26) */
    /* Custom adv payloads (NULL → defaults).
     * Limits: adv_data_len ≤ BLE_UART_ADV_DATA_MAX (28),
     *         scan_rsp_data_len ≤ BLE_UART_SCAN_RSP_DATA_MAX (31). */
    const uint8_t *adv_data;
    size_t         adv_data_len;
    const uint8_t *scan_rsp_data;
    size_t         scan_rsp_data_len;
    ble_uart_rx_cb_t  ble_uart_on_rx;
    ble_uart_evt_cb_t on_event;     /* optional for default Passkey Display;
                                       required for KEYBOARD_ONLY /
                                       DISPLAY_YES_NO / KEYBOARD_DISPLAY */
} ble_uart_config_t;

/* === Lifecycle === */
int  ble_uart_install(const ble_uart_config_t *cfg);   /* host + GATT */
int  ble_uart_open(void);                              /* start advertising (NimBLE: spawn host task) */
int  ble_uart_close(void);                             /* stop adv / disconnect / quiesce host */
int  ble_uart_close_async(void);                       /* same, fire-and-forget; signals BLE_UART_EVT_CLOSED on completion */
int  ble_uart_uninstall(void);                         /* best-effort teardown; first error, state always cleared */

/* === Send (callable from any task) === */
int  ble_uart_tx(const uint8_t *data, size_t len);

/* === Receive === */
/* Via the cfg.ble_uart_on_rx callback, signature:
 * void cb(const uint8_t *data, size_t len);  */

/* === Pairing replies (PASSKEY_REQUEST / NUMERIC_COMPARE only) === */
int  ble_uart_passkey_reply(uint32_t passkey);   /* answer PASSKEY_REQUEST  */
int  ble_uart_compare_reply(bool match);         /* answer NUMERIC_COMPARE */

/* === Status === */
bool ble_uart_is_connected(void);
bool ble_uart_is_subscribed(void);

/* === Bond management (works after install) === */
typedef struct {
    uint8_t bytes[6];   /* big-endian: bytes[0] is MSB */
    uint8_t type;       /* BLE_UART_ADDR_TYPE_PUBLIC or _RANDOM */
} ble_uart_addr_t;

int  ble_uart_get_bond_count(size_t *out_count);
int  ble_uart_get_bonded_peers(ble_uart_addr_t *out, size_t cap, size_t *out_count);
int  ble_uart_remove_peer(const ble_uart_addr_t *peer);
int  ble_uart_clear_bonds(void);

/* === Service UUID (for advertising; usually no need to touch) === */
extern const ble_uart_uuid128_t ble_uart_service_uuid;
```

---

## 12. Minimal Project Template (ready to flash)

A complete flashable project takes a handful of files. The inactive
backend `.c` compiles to nothing if you ship both.

**Using the shared component (fewer copies):**

```
my_ble_uart_project/
├── CMakeLists.txt              ← EXTRA_COMPONENT_DIRS → …/common/ble_uart (before project())
├── sdkconfig.defaults
└── main/
    ├── CMakeLists.txt          ← REQUIRES ble_uart nvs_flash; SRCS main.c only
    └── main.c
```

**Copying sources into `main/` (classic layout):**

```
my_ble_uart_project/
├── CMakeLists.txt
├── sdkconfig.defaults
└── main/
    ├── CMakeLists.txt
    ├── ble_uart.h              ← from $IDF_PATH/examples/bluetooth/common/ble_uart/
    ├── ble_uart_nimble.c
    ├── ble_uart_bluedroid.c    ← optional second backend
    └── main.c
```

**Root `CMakeLists.txt`** (shared `ble_uart` via `EXTRA_COMPONENT_DIRS`):
```cmake
cmake_minimum_required(VERSION 3.16)
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../path/to/common/ble_uart")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(my_ble_uart)
```

**`main/CMakeLists.txt`** (shared component — no `.c` copies in `main/`):
```cmake
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES ble_uart nvs_flash)
```

**`main/CMakeLists.txt`** (classic copy layout — both backends in `main/`):
```cmake
idf_component_register(SRCS "main.c"
                            "ble_uart_nimble.c"
                            "ble_uart_bluedroid.c"
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
```

**`sdkconfig.defaults`** (7 lines, NimBLE backend):
```ini
CONFIG_BT_ENABLED=y
CONFIG_BTDM_CTRL_MODE_BLE_ONLY=y
CONFIG_BT_BLUEDROID_ENABLED=n
CONFIG_BT_NIMBLE_ENABLED=y
CONFIG_BT_NIMBLE_SM_SC=y
CONFIG_BT_NIMBLE_NVS_PERSIST=y
CONFIG_BT_NIMBLE_ATT_PREFERRED_MTU=512
```

**Bluedroid host instead of NimBLE:** copy
`examples/bluetooth/ble_uart_service/sdkconfig.bluedroid` next to your
`sdkconfig.defaults` and pass
`-D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.bluedroid"` (see §4.3).

**`main/main.c`** — copy the §4.4 template verbatim.

Flash:

```bash
idf.py set-target esp32s3
idf.py build flash monitor
```

Done.
