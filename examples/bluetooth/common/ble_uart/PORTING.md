# ESP-BLE-UART Porting & API Guide

> **Naming convention:** Use **ESP-BLE-UART** for Espressif-owned product names (Bridge, Console, Daemon, Echo Server, the `ble_uart` component, and the `ble_uart_service` example). Use **BLE UART** for the generic GATT service convention, transport layer, and compatible third-party devices. This follows the same pattern as ESP-BLE-MESH.

This document lives in **`examples/bluetooth/common/ble_uart/`** next to the
`ble_uart` component sources (`ble_uart.h`, backend `.c` files).

**Reference application:** use the **`examples/bluetooth/ble_uart_service`**
example as the working template. Its root `CMakeLists.txt` appends this
directory to **`EXTRA_COMPONENT_DIRS`** so `main` can `REQUIRES ble_uart`;
`main/main.c` initializes NVS and a MAC-derived GAP name, calls
`ble_uart_install()` / `ble_uart_open()` with the default encrypted UART-over-BLE echo
path, and the tree ships `sdkconfig.defaults` plus the Bluedroid overlay
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
`ble_uart_bluedroid.c`, `CMakeLists.txt`, and `Kconfig` (prefix + RX scratch;
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
    bool        encrypted;          /* Master switch for SC + Bonding + MITM */
    const char *device_name;        /* GAP device name; NULL uses the NimBLE default */
    ble_uart_rx_cb_t ble_uart_on_rx;/* RX byte callback */
} ble_uart_config_t;
```

| Field | Type | Required | Default / meaning |
| --- | --- | --- | --- |
| `encrypted` | `bool` | yes | `true` = SC + Bonding + MITM + DisplayOnly + encrypted GATT chars; `false` = fully plaintext (sniffable, lab use only) |
| `device_name` | `const char *` | recommended | Any string. Mind the 31-byte primary advertising packet limit: flags(3) + tx_pwr(3) + name(2 + length) + 128-bit UUID(18) → keep the name ≤ 8 bytes |
| `ble_uart_on_rx` | callback | optional | `NULL` discards every received byte |

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

- The callback runs in the **NimBLE host task** context — **do not
  block**; offload heavy work to your own task.
- A single callback may carry only **part** of an upper-layer frame
  (the central slices on ATT MTU). Framing logic (line / TLV /
  length-prefixed) is your responsibility.
- The data carries **no `ctx` argument**. If your callback needs state,
  use a file-scope `static` or a global.

### 5.3 Lifecycle functions

```c
int  ble_uart_install(const ble_uart_config_t *cfg);
int  ble_uart_open(void);
int  ble_uart_close(void);
int  ble_uart_uninstall(void);
```

| Function | What it does (NimBLE) | What it does (Bluedroid) | When to call | Blocking? |
| --- | --- | --- | --- | --- |
| `install` | `nimble_port_init` + `ble_hs_cfg` + SM + SIG services + UART GATT | `controller_init/enable` + `bluedroid_init/enable` + SM + `esp_ble_gatts_create_attr_tab` (waits ≤500 ms for the attr-table event) | After `nvs_flash_init()`, before `open` | No, ~50 ms (NimBLE) / ~150 ms (Bluedroid) |
| `open` | Bond store + spawn host task + start advertising once synced | Configure adv data + scan rsp + start advertising | After `install` | No, host runs in the background |
| `close` | Stop adv → graceful disconnect (LL_TERMINATE_IND, waits ≤500 ms for the disconnect event) → `nimble_port_stop()` | Stop adv → graceful disconnect (`esp_ble_gap_disconnect`, waits ≤500 ms) | After `open`, before `uninstall` | Yes, up to ~500 ms while waiting for the peer disconnect |
| `uninstall` | Calls `close` if still open, then `nimble_port_deinit()` and resets module state | Calls `close` if still open, then `bluedroid_disable+deinit` + `controller_disable+deinit` | After `close` (or directly — `uninstall` cascades into `close` on its own) | Yes, follows the same wait window as `close` |

Call order:

```
nvs_flash_init
    └── ble_uart_install
            └── ble_uart_open      ← BLE is live
                    └── ble_uart_close
                            └── ble_uart_uninstall   ← clean state, can install again
```

Each call returns `BLE_HS_EALREADY` if the corresponding state is
already true (e.g. `open` called twice, or `close` called when the
radio is already down). It is therefore safe to call `close` /
`uninstall` defensively at shutdown without checking the current state
yourself.

**Do NOT call `close` / `uninstall` from inside `ble_uart_on_rx`** —
that callback runs on the NimBLE host task, and `close` blocks on
`nimble_port_stop()` which expects the host task to exit. Self-stop
deadlocks. Forward the request to a normal FreeRTOS task instead.

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
| `0` | Success (notification handed to the stack) |
| `BLE_HS_ENOTCONN` | No central connected; **this is normal — typically ignore** |
| `BLE_HS_EINVAL` | `data == NULL` or `len == 0` |
| `BLE_HS_ENOMEM` | Stack mbuf pool exhausted |
| other | Internal stack error — see `ble_hs.h` |

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

### 5.6 Service UUID constant

```c
extern const ble_uart_uuid128_t ble_uart_service_uuid;
```

Always `6e400001-b5a3-f393-e0a9-e50e24dcca9e` (the de-facto BLE UART service UUID). It is
already inserted into the scan response, so the **application normally
does not touch it**. You only need it if you take over advertising
yourself (see 6.3).

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
table. **Caveat**: this must happen before `ble_uart_open()`, otherwise
the host task is already running and the GATT table is locked.

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

### 6.4 Configuring the device-name prefix via Kconfig

If you use the shared `ble_uart` component, options are already in
`menuconfig → Component configuration → ESP-BLE-UART library`. If you copied only
the `.c` / `.h` files into `main/`, copy `Kconfig` from `common/ble_uart/` as
well (or merge its symbols into your own `Kconfig.projbuild`), then:

```c
char name[24];
snprintf(name, sizeof(name), "%s-%02X%02X",
         CONFIG_BLE_UART_DEVICE_NAME_PREFIX, mac[4], mac[5]);

ble_uart_install(&(ble_uart_config_t){
    .encrypted = true,
    .device_name = name,
    .ble_uart_on_rx = on_rx,
});
```

Edit the default through `menuconfig → Component configuration → ESP-BLE-UART
library → BLE device name prefix`.

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

When nobody is subscribed, `ble_uart_tx` returns `BLE_HS_ENOTCONN` —
**just ignore it**.

---

## 7. Calling Context & Thread Safety

| Function | Calling context | Thread-safe |
| --- | --- | --- |
| `ble_uart_install` | Any task; once per uninstall cycle | One-shot until `uninstall` |
| `ble_uart_open` | Any task; after `install` | One-shot until `close` |
| `ble_uart_close` | Any task **except the BLE host task** (NimBLE host task / Bluedroid BTC task) | Idempotent; second call returns `EALREADY` |
| `ble_uart_uninstall` | Any task **except the BLE host task** | Idempotent; cascades into `close` if needed |
| `ble_uart_tx` | Any FreeRTOS task | Yes — multi-task concurrent |
| `ble_uart_is_connected` / `is_subscribed` | Any context | Yes (bool read; best-effort snapshot) |
| `ble_uart_on_rx` callback | BLE host task (NimBLE host task / Bluedroid BTC task) | Your code must not block, **must not call `close` / `uninstall`** |
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
| Device not discoverable | Device name exceeds the advertising packet limit (drop the tx_pwr field or shorten the name) |
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
typedef void (*ble_uart_rx_cb_t)(const uint8_t *data, size_t len);

typedef struct {
    bool encrypted;
    const char *device_name;
    ble_uart_rx_cb_t ble_uart_on_rx;
} ble_uart_config_t;

/* === Lifecycle === */
int  ble_uart_install(const ble_uart_config_t *cfg);   /* host + GATT */
int  ble_uart_open(void);                              /* start advertising (NimBLE: spawn host task) */
int  ble_uart_close(void);                             /* stop adv / disconnect / quiesce host */
int  ble_uart_uninstall(void);                         /* tear down host + reset state */

/* === Send (callable from any task) === */
int  ble_uart_tx(const uint8_t *data, size_t len);

/* === Receive === */
/* Via the cfg.ble_uart_on_rx callback, signature:
 * void cb(const uint8_t *data, size_t len);  */

/* === Status === */
bool ble_uart_is_connected(void);
bool ble_uart_is_subscribed(void);

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
