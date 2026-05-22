/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * ESP-BLE-UART — turnkey serial-over-BLE peripheral.
 *
 * Implements the de-facto BLE UART-over-GATT layout (RX write, TX notify;
 * fixed 128-bit UUIDs below) on top of either NimBLE or Bluedroid; the
 * backend is picked at compile time via CONFIG_BT_NIMBLE_ENABLED /
 * CONFIG_BT_BLUEDROID_ENABLED.
 *
 * Lifecycle — bring-up:
 *
 *     nvs_flash_init();
 *     ble_uart_install(&cfg);   // host + GATT service (once per uninstall)
 *     ble_uart_open();          // advertising + pairing
 *
 * Run-forever apps stop after open().
 *
 * Lifecycle — release (pick one path; see PORTING.md §5.3):
 *
 *   Path A — from a normal app task (not on_event / on_rx):
 *     ble_uart_close();         // stop adv / disconnect / halt host
 *     ble_uart_uninstall();     // free port + reset state
 *
 *   Path B — teardown triggered by a BLE event on the host task:
 *     ble_uart_close_async();   // in on_event / on_rx only
 *     // wait for BLE_UART_EVT_CLOSED (.closed.status == BLE_UART_OK)
 *     ble_uart_uninstall();     // on an app task — NOT inside CLOSED
 *
 * GATT layout (UUIDs are the widely used fixed 128-bit values):
 *
 *     Service: 6e400001-b5a3-f393-e0a9-e50e24dcca9e
 *     RX     : 6e400002-b5a3-f393-e0a9-e50e24dcca9e   write
 *     TX     : 6e400003-b5a3-f393-e0a9-e50e24dcca9e   notify
 *
 * See PORTING.md in this component directory for the integration guide.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----- Return codes --------------------------------------------------- */

/** All ble_uart_* APIs return one of these stack-neutral codes. */
#define BLE_UART_OK         0   /* Success */
#define BLE_UART_EINVAL    -1   /* Bad argument or unsupported op */
#define BLE_UART_ENOTCONN  -2   /* No central currently connected */
#define BLE_UART_ENOMEM    -3   /* Out of mbufs / send queue full */
#define BLE_UART_EALREADY  -4   /* Lifecycle already in this state */
#define BLE_UART_EFAIL     -5   /* Backend internal failure (see logs) */

/* ----- 128-bit UUID helper -------------------------------------------- */

/** Stack-agnostic 128-bit UUID in little-endian (wire) order. */
typedef struct {
    uint8_t bytes[16];
} ble_uart_uuid128_t;

/* ----- BLE address ---------------------------------------------------- */

/** Address type, mirroring the BT Core spec values. */
#define BLE_UART_ADDR_TYPE_PUBLIC   0
#define BLE_UART_ADDR_TYPE_RANDOM   1

/** 6-octet BLE device address.
 *
 *  `bytes` is in big-endian order — `bytes[0]` is the MSB octet, the
 *  way addresses are usually printed (`AA:BB:CC:DD:EE:FF`). Both
 *  backends marshal between this representation and their own native
 *  byte order internally, so callers never need to flip bytes. */
typedef struct {
    uint8_t bytes[6];
    uint8_t type;     /* BLE_UART_ADDR_TYPE_PUBLIC or _RANDOM */
} ble_uart_addr_t;

/* ----- Configuration -------------------------------------------------- */

/** RX byte callback. Invoked from the BLE host task whenever bytes
 *  arrive on the RX characteristic. The buffer is owned by the stack
 *  and reused after return — copy what you need to keep.
 *
 *  Don't block here; offload heavy work to your own task.
 *
 *  Long-write (PREP/EXEC) reassembly is handled transparently — you
 *  always see one contiguous payload, capped by
 *  CONFIG_BLE_UART_RX_SCRATCH_SIZE (default 1024). Oversized writes
 *  are rejected with ATT error 0x0d. */
typedef void (*ble_uart_rx_cb_t)(const uint8_t *data, size_t len);

/* ----- Event callback ------------------------------------------------- */

/** Lifecycle / link-state events delivered to ble_uart_config_t::on_event.
 *
 *  All events fire from the BLE host task context (NimBLE host task /
 *  Bluedroid BTC task), with one documented exception:
 *  BLE_UART_EVT_CLOSED is fired by the close-async worker task, after
 *  the host stack has been torn down — there is no host task left to
 *  deliver it from. See ble_uart_close_async().
 *
 *  The same threading rules as ble_uart_on_rx apply: don't block, and
 *  don't call ble_uart_close() / ble_uart_uninstall() (use the async
 *  variant if you need to teardown from inside an event handler). */
typedef enum {
    /** Physical link established. Payload: .connected.peer.
     *  Type is always BLE_UART_ADDR_TYPE_PUBLIC or _RANDOM (each
     *  backend's wider addr-type enum is collapsed before delivery).
     *
     *  Backend semantics differ:
     *  - NimBLE: peer identity address (`peer_id_addr`). On first
     *    connect this equals the over-the-air address; on a bonded
     *    RPA reconnect it is the resolved identity, not the random
     *    address currently on the wire.
     *  - Bluedroid: the BD address recorded at bond time. If the
     *    peer connected as address_A and bonding succeeded, later
     *    reconnects still report address_A in CONNECT even when the
     *    peer's over-the-air address has changed (e.g. a new RPA).
     *    Matches `get_bonded_peers()` / `remove_peer` (`bytes` only). */
    BLE_UART_EVT_CONNECTED,

    /** Physical link torn down. Payload: .disconnected.reason
     *  (stack-specific disconnect code — esp_gatt_conn_reason_t on
     *  Bluedroid, NimBLE BLE host return code on NimBLE; see
     *  BLE_HS_HCI_ERR() / BLE_HS_ERR_HCI_BASE for HCI encoding). */
    BLE_UART_EVT_DISCONNECTED,

    /** CCCD on the TX characteristic changed. Payload:
     *  .subscribed.subscribed (true = notifications enabled). */
    BLE_UART_EVT_SUBSCRIBED,

    /** Link reached the encrypted+authenticated state requested at
     *  install time. Payload: .link_secure.{encrypted, authenticated,
     *  bonded, key_size}. Use this — not is_connected() — to gate any
     *  application logic that requires the channel to be secure. */
    BLE_UART_EVT_LINK_SECURE,

    /** SM asks the application to display a 6-digit passkey.
     *  Payload: .passkey.passkey (0..999999). The default banner on
     *  UART still prints; this callback is additive so a UI / test
     *  harness can avoid scraping logs. */
    BLE_UART_EVT_PASSKEY_DISPLAY,

    /** SM asks the application to collect a 6-digit passkey from the
     *  user (the central displays it; the user types it into this
     *  device). No payload.
     *
     *  The application MUST respond by calling ble_uart_passkey_reply()
     *  with the 6 digits the user entered. Until the reply arrives —
     *  or until the SM's pairing timeout fires (the controller's
     *  default ~30 s) — pairing is suspended; on timeout the link
     *  surfaces BLE_UART_EVT_PAIRING_FAILED.
     *
     *  Only fires when cfg.security.io_cap is one of the input-capable
     *  values (KEYBOARD_ONLY / KEYBOARD_DISPLAY) and the central asks
     *  for Passkey Entry. */
    BLE_UART_EVT_PASSKEY_REQUEST,

    /** SM asks the application to display a 6-digit value and let the
     *  user confirm whether the same value appears on the central.
     *  Payload: .numeric_compare.passkey (0..999999).
     *
     *  The application MUST respond by calling ble_uart_compare_reply()
     *  with the user's verdict (true = match). Same suspend-and-time-
     *  out semantics as BLE_UART_EVT_PASSKEY_REQUEST.
     *
     *  Only fires when cfg.security.io_cap is one of the
     *  comparison-capable values (DISPLAY_YES_NO / KEYBOARD_DISPLAY)
     *  and the central asks for Numeric Comparison (which itself
     *  requires LE Secure Connections on both sides). */
    BLE_UART_EVT_NUMERIC_COMPARE,

    /** Pairing failed or was rejected. Payload: .pairing_failed.reason
     *  (NimBLE BLE_HS_E* / Bluedroid esp_ble_auth_fail_rsn_t). */
    BLE_UART_EVT_PAIRING_FAILED,

    /** Async-close completion — fired only by ble_uart_close_async()
     *  after the worker task has finished the equivalent of a
     *  synchronous ble_uart_close(). Payload: .closed.status — the
     *  return code from that close (BLE_UART_OK on success).
     *
     *  When .closed.status is BLE_UART_OK the host stack is fully
     *  quiesced — same state as right after ble_uart_close() returns.
     *  Defer ble_uart_uninstall() to a normal app task (set a flag /
     *  queue here); do not call uninstall from this handler — see
     *  PORTING.md §5.3.2. On failure (e.g. BLE_UART_EFAIL) the port
     *  may still be open; retry ble_uart_close() / ble_uart_close_async()
     *  from an app task.
     *
     *  Unlike every other event in this enum, this one runs on the
     *  close-async worker task, NOT on the BLE host task — by the
     *  time it fires the host task is already gone. Keep the handler
     *  short; the worker clears s_closing after it returns. */
    BLE_UART_EVT_CLOSED,
} ble_uart_evt_id_t;

/** Tagged union delivered to ble_uart_config_t::on_event. */
typedef struct {
    ble_uart_evt_id_t id;
    union {
        struct {
            ble_uart_addr_t peer;
        } connected;

        struct {
            int reason;            /* stack-specific disconnect code */
        } disconnected;

        struct {
            bool subscribed;
        } subscribed;

        struct {
            bool    encrypted;     /* 1 = link is AES-CCM encrypted */
            bool    authenticated; /* 1 = pairing used MITM protection */
            bool    bonded;        /* 1 = LTK persisted in NVS */
            uint8_t key_size;      /* 7..16 (octets) */
        } link_secure;

        struct {
            uint32_t passkey;      /* 0..999999 */
        } passkey;

        struct {
            uint32_t passkey;      /* 0..999999 — the value to display */
        } numeric_compare;

        struct {
            int reason;            /* stack-specific status code */
        } pairing_failed;

        struct {
            int status;            /* BLE_UART_* from async close worker */
        } closed;
    };
} ble_uart_evt_t;

/** Event callback. May be NULL — events are silently dropped then. */
typedef void (*ble_uart_evt_cb_t)(const ble_uart_evt_t *evt);

/* ----- Security configuration ---------------------------------------- */

/** Tri-state knob for the per-feature security overrides in
 *  ble_uart_config_t (`sc`, `bonding`, `mitm`).
 *
 *  AUTO (= 0, the value of a zero-initialised struct member) means
 *  "use whatever cfg.encrypted implies":
 *
 *      encrypted = true  →  AUTO behaves as ON
 *      encrypted = false →  AUTO behaves as OFF
 *
 *  OFF / ON force the bit regardless of the preset, letting the
 *  caller mix the preset with one or two surgical overrides without
 *  spelling out every other field. */
typedef enum {
    BLE_UART_SEC_AUTO = 0,
    BLE_UART_SEC_OFF  = 1,
    BLE_UART_SEC_ON   = 2,
} ble_uart_sec_t;

/** SM Input/Output capability — combines with the central's IO cap and
 *  the resolved `mitm` bit to pick the pairing model (Just Works /
 *  Passkey Display / Passkey Entry / Numeric Comparison — see BT Core
 *  Spec §2.3.5.1). The application doesn't decide the method directly;
 *  it picks the IO cap that matches its UI and ble_uart fires the right
 *  event when the SM negotiates a method.
 *
 *  Passing an out-of-range integer makes ble_uart_install() return
 *  BLE_UART_EINVAL. Only the input-capable values (KEYBOARD_ONLY,
 *  DISPLAY_YES_NO, KEYBOARD_DISPLAY) require cfg.on_event to be
 *  non-NULL — pairing would otherwise stall on unanswered
 *  BLE_UART_EVT_PASSKEY_REQUEST / NUMERIC_COMPARE. AUTO (resolves to
 *  DisplayOnly when MITM is ON), DISPLAY_ONLY, and NO_INPUT_OUTPUT do
 *  not require on_event; Passkey Display is handled internally. */
typedef enum {
    /** Default: DisplayOnly when the resolved MITM bit is ON;
     *  NoInputNoOutput when it is OFF. */
    BLE_UART_IO_CAP_AUTO             = 0,

    /** Device has no UI; pairing always uses Just Works. Cannot
     *  satisfy MITM — combining this with mitm=ON makes
     *  ble_uart_install() return BLE_UART_EINVAL. */
    BLE_UART_IO_CAP_NO_INPUT_OUTPUT  = 1,

    /** Device shows a 6-digit passkey on a display; the central
     *  enters it. Generates a fresh passkey for every pairing,
     *  surfaced via BLE_UART_EVT_PASSKEY_DISPLAY (no reply call
     *  needed — the central does the typing). */
    BLE_UART_IO_CAP_DISPLAY_ONLY     = 2,

    /** Device has keys (or some other way to feed digits to the
     *  library) but no display; the central displays a 6-digit
     *  passkey, the user reads it from there and types it in.
     *
     *  ble_uart fires BLE_UART_EVT_PASSKEY_REQUEST and waits for
     *  ble_uart_passkey_reply(). Requires cfg.on_event != NULL. */
    BLE_UART_IO_CAP_KEYBOARD_ONLY    = 3,

    /** Device has a display + a yes/no confirmation control. With a
     *  similarly-equipped LE Secure Connections central this elects
     *  Numeric Comparison: ble_uart fires BLE_UART_EVT_NUMERIC_COMPARE
     *  with the 6-digit value to display, and waits for
     *  ble_uart_compare_reply().
     *
     *  Falls back to Just Works against legacy or NoInput peers.
     *  Requires cfg.on_event != NULL. */
    BLE_UART_IO_CAP_DISPLAY_YES_NO   = 4,

    /** Device has a display AND a keypad (covers both Numeric
     *  Comparison and Passkey Entry). Best fit for a touchscreen UI
     *  that wants to handle every MITM-capable peer.
     *
     *  ble_uart fires either BLE_UART_EVT_PASSKEY_REQUEST or
     *  BLE_UART_EVT_NUMERIC_COMPARE depending on what the SM
     *  negotiates with the central; respond with the matching reply
     *  API. Requires cfg.on_event != NULL. */
    BLE_UART_IO_CAP_KEYBOARD_DISPLAY = 5,
} ble_uart_io_cap_t;

/** Per-feature security overrides, embedded in ble_uart_config_t.
 *
 *  Each tri-state field defaults to AUTO (= 0, the value of any
 *  zero-initialised member), inheriting its bit from
 *  ble_uart_config_t::encrypted:
 *
 *      encrypted = true  →  AUTO behaves as ON
 *      encrypted = false →  AUTO behaves as OFF
 *
 *  Set any field to OFF / ON to override that single bit while the
 *  rest still follow the preset. Common patterns are listed in
 *  PORTING.md §5.6 (e.g. encrypted=true with mitm=OFF +
 *  io_cap=NO_INPUT_OUTPUT for a displayless gateway).
 *
 *  Combinations the SM cannot satisfy — io_cap=NO_INPUT_OUTPUT
 *  together with the resolved mitm=ON, or an out-of-range enum value
 *  — make ble_uart_install() return BLE_UART_EINVAL up front, before
 *  the host stack is brought up. */
typedef struct {
    /** Override LE Secure Connections (the BT 4.2+ pairing method
     *  that uses ECDH for the LTK). */
    ble_uart_sec_t sc;

    /** Override bonding (persistence of the LTK / IRK / persisted
     *  CCCD in NVS). With bonding=OFF the link is still encrypted
     *  (if sc/mitm are on) but every reconnect re-pairs. */
    ble_uart_sec_t bonding;

    /** Override MITM protection (man-in-the-middle: link
     *  authentication via passkey display / entry / numeric
     *  comparison). With mitm=OFF the link pairs via Just Works,
     *  which is encrypted but unauthenticated; the GATT permission
     *  flags drop their _AUTHEN bit so a Just-Works peer can
     *  read/write the UART characteristics. */
    ble_uart_sec_t mitm;

    /** SM IO capability — controls which pairing model is chosen
     *  alongside `mitm`. AUTO picks DisplayOnly when the resolved
     *  MITM bit is ON, NoInputNoOutput when it is OFF. */
    ble_uart_io_cap_t io_cap;
} ble_uart_security_t;

/* ----- Advertising payload limits ------------------------------------ */

/** Maximum bytes the application may put in `adv_data`.
 *
 *  BLE 4.x legacy primary advertising packets are capped at 31 bytes
 *  total. Of those, the 3-byte Flags AD element (length+type+value)
 *  is always added by ble_uart, leaving 31 − 3 = 28 bytes for the
 *  application. */
#define BLE_UART_ADV_DATA_MAX        28

/** Maximum bytes the application may put in `scan_rsp_data`.
 *
 *  Scan response packets are also capped at 31 bytes, with no
 *  mandatory AD elements — the entire 31 bytes belong to the
 *  application. */
#define BLE_UART_SCAN_RSP_DATA_MAX   31

/** Maximum length (bytes, excluding NUL terminator) of `device_name`.
 *
 *  Sized so that the *default* advertising payload — Flags AD +
 *  Complete Local Name AD — always fits in the 31-byte primary packet:
 *
 *      31 − 3 (Flags AD) − 2 (Name AD header) = 26
 *
 *  Names that exceed this length make `ble_uart_install()` return
 *  `BLE_UART_EINVAL` synchronously, instead of silently failing later
 *  in the host stack when advertising starts.
 *
 *  This applies regardless of whether `adv_data` is set — the GAP
 *  service Device Name characteristic (UUID 0x2A00) reports the same
 *  string. Apps that need a longer GAP-service name with a shorter
 *  advertised name should keep `device_name` ≤ this limit and use
 *  `adv_data` to broadcast a shortened/different name instead. */
#define BLE_UART_DEVICE_NAME_MAX     26

/** Configuration handed to ble_uart_install(). */
typedef struct {
    /** Security preset (a one-line shortcut for the four override
     *  fields under `security` below).
     *
     *  True  = LE Secure Connections + Bonding + MITM, DisplayOnly IO,
     *          encrypted+authenticated RX/TX chars, bond persisted in
     *          NVS (NimBLE: requires CONFIG_BT_NIMBLE_NVS_PERSIST=y;
     *          Bluedroid: default).
     *  False = plaintext (lab debugging only — sniffable).
     *
     *  Every member of `security` defaults to AUTO, meaning "follow
     *  this preset". Override individual bits there; see
     *  ble_uart_security_t for the resolution rules. */
    bool encrypted;

    /** Per-feature security overrides. A zero-initialised value
     *  (every field AUTO) inherits everything from `encrypted`, so
     *  callers that just want the secure-by-default preset can leave
     *  this field unset:
     *
     *      ble_uart_install(&(ble_uart_config_t){
     *          .encrypted = true,        // sc/bonding/mitm/io_cap all AUTO
     *          ...
     *      });
     *
     *  Surgical override:
     *
     *      ble_uart_install(&(ble_uart_config_t){
     *          .encrypted = true,
     *          .security  = { .mitm = BLE_UART_SEC_OFF },  // SC + Bonding, no MITM
     *          ...
     *      });
     *
     *  See ble_uart_security_t for the full per-field docs. */
    ble_uart_security_t security;

    /** GAP device name (peer-readable via the GAP service, UUID 0x2A00).
     *  NULL keeps the host-stack default.
     *
     *  Length must be ≤ BLE_UART_DEVICE_NAME_MAX (26) — over-long
     *  strings make ble_uart_install() return BLE_UART_EINVAL.
     *
     *  This is NOT automatically inserted into the advertising payload
     *  when `adv_data` (below) is non-NULL — if you want the name to
     *  appear in scans without connecting, include a Complete Local
     *  Name AD element (type 0x09) in your `adv_data` bytes yourself. */
    const char *device_name;

    /** Optional raw advertising data — everything that goes after the
     *  3-byte Flags AD element in the primary advertising packet. The
     *  Flags element is built by ble_uart and is NOT part of these
     *  bytes (don't include it).
     *
     *  Format: standard BT Core "AD structure" sequence — repeating
     *  `[length(1)][AD type(1)][value(length-1)]` triplets. See the
     *  Bluetooth Assigned Numbers (Generic Access Profile) document
     *  for the full type list.
     *
     *  Length must be ≤ BLE_UART_ADV_DATA_MAX (28). The buffer is
     *  copied at install time; the pointer does not need to outlive
     *  the call.
     *
     *  Set to NULL (with adv_data_len=0) to keep the built-in default,
     *  which advertises only the Complete Local Name (taken from
     *  device_name). */
    const uint8_t *adv_data;
    size_t         adv_data_len;

    /** Optional raw scan response data — entire 31-byte payload is at
     *  the application's disposal; ble_uart adds nothing.
     *
     *  Same `[len][type][value]` format and copy semantics as
     *  adv_data. Length must be ≤ BLE_UART_SCAN_RSP_DATA_MAX (31).
     *
     *  Set to NULL (with scan_rsp_data_len=0) to keep the built-in
     *  default, which advertises the 128-bit BLE UART service UUID. */
    const uint8_t *scan_rsp_data;
    size_t         scan_rsp_data_len;

    /** Byte handler for RX writes. NULL discards incoming data. */
    ble_uart_rx_cb_t ble_uart_on_rx;

    /** Lifecycle / link-state event sink. NULL drops every event.
     *  See ble_uart_evt_id_t for the supported events; runs on the
     *  BLE host task with the same caveats as ble_uart_on_rx. */
    ble_uart_evt_cb_t on_event;
} ble_uart_config_t;

/* ----- Lifecycle ------------------------------------------------------ */

/** Bring up host stack + Security Manager + SIG services + BLE UART GATT
 *  service. Caller must have already called nvs_flash_init().
 *
 *  cfg->device_name, cfg->adv_data and cfg->scan_rsp_data are all
 *  copied internally; the caller's buffers don't need to outlive the
 *  call. Returns BLE_UART_EINVAL if any of these checks fail:
 *      strlen(cfg->device_name) > BLE_UART_DEVICE_NAME_MAX (26)
 *      cfg->adv_data_len        > BLE_UART_ADV_DATA_MAX     (28)
 *      cfg->scan_rsp_data_len   > BLE_UART_SCAN_RSP_DATA_MAX (31)
 *      cfg->security.{sc,bonding,mitm} outside BLE_UART_SEC_{AUTO,OFF,ON}
 *      cfg->security.io_cap     outside BLE_UART_IO_CAP_{AUTO,
 *                               NO_INPUT_OUTPUT,DISPLAY_ONLY,
 *                               KEYBOARD_ONLY,DISPLAY_YES_NO,
 *                               KEYBOARD_DISPLAY}
 *      resolved mitm=ON         + io_cap=NO_INPUT_OUTPUT
 *                               (Just Works can never satisfy MITM)
 *      io_cap requires user input (KEYBOARD_ONLY, DISPLAY_YES_NO,
 *                               KEYBOARD_DISPLAY) but cfg->on_event
 *                               is NULL — the application would have
 *                               no way to receive PASSKEY_REQUEST /
 *                               NUMERIC_COMPARE and answer it
 *      (io_cap=AUTO with resolved mitm=ON, or DISPLAY_ONLY, does not
 *       need on_event — equivalent to Passkey Display handled inside
 *       the port; PASSKEY_DISPLAY via on_event is optional)
 *
 *  Single-shot until ble_uart_uninstall(); a second call returns
 *  BLE_UART_EALREADY. */
int  ble_uart_install(const ble_uart_config_t *cfg);

/** Start advertising. NimBLE: spawns the host task and primes the bond
 *  store; advertising begins once the controller signals ready.
 *  Bluedroid: triggers adv-data + scan-response config; advertising
 *  begins once the stack acknowledges both.
 *
 *  Returns immediately; the ESP-BLE-UART then runs autonomously
 *  (connect, pairing, passkey display, RX delivery all via internal
 *  callbacks). Single-shot. */
int  ble_uart_open(void);

/** Counterpart to ble_uart_open(). Stops advertising, gracefully
 *  disconnects (waits up to 500 ms for LL_TERMINATE_IND ack), and
 *  quiesces the host. install state is preserved — call open() again
 *  to resume.
 *
 *  NimBLE: also resets the local GATT server; the next open() re-adds
 *  GAP/GATT/UART. Service UUIDs are unchanged but ATT handles may
 *  differ — centrals must rediscover and re-subscribe (PORTING.md
 *  §5.3.1a). Bluedroid: host and GATT table stay up; open() only
 *  restarts advertising.
 *
 *  Don't call from the BLE host task (i.e. from ble_uart_on_rx or
 *  ble_uart_evt_cb_t) — it would deadlock waiting for the disconnect
 *  event that the host task itself is supposed to deliver. Use
 *  ble_uart_close_async() in those contexts instead. */
int  ble_uart_close(void);

/** Fire-and-forget variant of ble_uart_close(). Returns immediately
 *  after spawning a small worker task that runs the regular close
 *  sequence in the background; safe from ANY task — including the
 *  BLE host task (i.e. from inside ble_uart_on_rx or on_event), where
 *  the synchronous variant deadlocks.
 *
 *  Completion is reported on the on_event callback as
 *  BLE_UART_EVT_CLOSED with .closed.status set to the worker's
 *  ble_uart_close() result. When status is BLE_UART_OK the host stack
 *  is fully torn down — then uninstall on an app task after
 *  BLE_UART_EVT_CLOSED (PORTING.md §5.3.2 Path B).
 *  (BLE_UART_EVT_DISCONNECTED is also delivered, ahead of CLOSED, if
 *  there was a peer.)
 *
 *  Idempotent in the harmless sense: calling it before
 *  ble_uart_open() has succeeded, or while a previous async close
 *  is still draining, returns BLE_UART_EALREADY without spawning a
 *  second worker. Returns BLE_UART_ENOMEM if FreeRTOS can't
 *  allocate the worker task. */
int  ble_uart_close_async(void);

/** Counterpart to ble_uart_install(). Force-closes if still open,
 *  then tears down the host stack + controller. After this returns,
 *  install() can run from scratch.
 *
 *  Don't call from the BLE host task (NimBLE host / Bluedroid BTC).
 *  If a ble_uart_close_async() worker is still running, this call
 *  polls for up to ~5 s and then proceeds with teardown anyway if the
 *  worker has not exited — do not call uninstall from another task
 *  while a close_async is in flight unless you follow PORTING.md §5.3:
 *  Path A — ble_uart_close() then uninstall from an app task; or
 *  Path B — close_async, then uninstall on an app task after
 *  BLE_UART_EVT_CLOSED with .closed.status == BLE_UART_OK (never
 *  call uninstall from inside the CLOSED handler). */
int  ble_uart_uninstall(void);

/* ----- Pairing replies ----------------------------------------------- */

/** Answer an in-flight BLE_UART_EVT_PASSKEY_REQUEST.
 *
 *  `passkey` is the 6-digit value the user read off the central's
 *  display and entered on this device — must be in 0..999999.
 *
 *  Safe from any task. Returns:
 *      BLE_UART_OK         reply was injected into the SM
 *      BLE_UART_EINVAL     passkey > 999999
 *      BLE_UART_ENOTCONN   no PASSKEY_REQUEST is currently pending
 *                          (link dropped, pairing already timed out,
 *                          or the SM asked for something else)
 *      BLE_UART_EFAIL      backend rejected the inject
 *
 *  Each PASSKEY_REQUEST event accepts exactly one reply; subsequent
 *  calls return BLE_UART_ENOTCONN until the next request. */
int  ble_uart_passkey_reply(uint32_t passkey);

/** Answer an in-flight BLE_UART_EVT_NUMERIC_COMPARE.
 *
 *  `match` is the user's verdict: true if the 6-digit values shown
 *  on this device and on the central are identical, false otherwise.
 *  A `false` reply makes pairing fail with a numeric-comparison
 *  mismatch, surfaced as BLE_UART_EVT_PAIRING_FAILED.
 *
 *  Same threading semantics and return codes as
 *  ble_uart_passkey_reply(). */
int  ble_uart_compare_reply(bool match);

/* ----- TX ------------------------------------------------------------- */

/** Send raw bytes to the connected central as one or more TX
 *  notifications, fragmented to fit the live ATT MTU. Safe from any
 *  FreeRTOS task; not safe from ISR.
 *
 *  Returns BLE_UART_ENOTCONN when no peer is connected (this is
 *  normal — typically just ignore). */
int  ble_uart_tx(const uint8_t *data, size_t len);

/* ----- Status (best-effort, optional) -------------------------------- */

/** True when a central is connected (link may not yet be encrypted).
 *  Best-effort snapshot; production callers should rely on the return
 *  code of ble_uart_tx() instead. */
bool ble_uart_is_connected(void);

/** True when the central has subscribed to TX notifications.
 *  ble_uart_tx() does NOT gate on this (bonded reconnects often skip
 *  the CCCD write); exposed for diagnostics only. */
bool ble_uart_is_subscribed(void);

/* ----- Bond management ----------------------------------------------- */

/** Number of bonded peers in the persistent store.
 *
 *  Requires ble_uart_install() to have run; works whether or not
 *  ble_uart_open() has been called. *out_count is left untouched on
 *  failure. Safe from any task. */
int ble_uart_get_bond_count(size_t *out_count);

/** List the bonded peers' addresses.
 *
 *  Up to `cap` entries are written to `out`; on success *out_count
 *  receives the **total** number of bonds (which may exceed `cap`).
 *  When *out_count > cap the caller may allocate a larger buffer
 *  and re-call to read the rest.
 *
 *  `out` may be NULL if `cap` is 0 — useful as a preflight to size
 *  an exactly-fitting buffer (although ble_uart_get_bond_count()
 *  does the same with one less argument).
 *
 *  Safe from any task. Requires ble_uart_install() to have run. */
int ble_uart_get_bonded_peers(ble_uart_addr_t *out,
                              size_t cap,
                              size_t *out_count);

/** Drop the bond (LTK / IRK / persisted CCCD) for one peer.
 *
 *  Does not actively disconnect the current link (encrypted or not).
 *  Call ble_uart_close() first if you need an immediate disconnect
 *  and re-pair.
 *
 *  `peer` is matched against the identity address in the bond store.
 *  Backend matching:
 *  - NimBLE: `(type, bytes)`. `BLE_UART_EVT_CONNECTED` and
 *    `get_bonded_peers()` both yield identity addresses suitable
 *    for this call (first connect: same as over-the-air; bonded RPA
 *    reconnect: resolved identity, not the random on the wire).
 *  - Bluedroid: `bytes` only — `type` is ignored. The bond store
 *    and CONNECT both use the address seen when bonding was
 *    established (address_A); later over-the-air changes are not
 *    reflected in either API.
 *
 *  Idempotent: returns BLE_UART_OK whether or not the peer was bonded
 *  (both backends treat "already absent" as success — NimBLE's
 *  ble_store_util_delete_peer maps BLE_HS_ENOENT to 0). Use
 *  ble_uart_get_bonded_peers() first if you need to distinguish
 *  "removed" from "was never bonded".
 *
 *  Returns BLE_UART_EINVAL if peer is NULL or ble_uart_install() has
 *  not run. Safe from any task. */
int ble_uart_remove_peer(const ble_uart_addr_t *peer);

/** Drop ALL bonded peers — equivalent to a factory reset of the bond
 *  store, but does not touch any other NVS namespace.
 *
 *  Does not actively disconnect the current link (encrypted or not).
 *  Call ble_uart_close() first if you need an immediate disconnect
 *  and re-pair.
 *
 *  Returns BLE_UART_OK if the store was cleared. Safe from any task. */
int ble_uart_clear_bonds(void);

/* ----- Service UUID -------------------------------------------------- */

/** The BLE UART service UUID, exposed for custom advertising payloads.
 *  The two characteristic UUIDs are private to the backend. */
extern const ble_uart_uuid128_t ble_uart_service_uuid;

#ifdef __cplusplus
}
#endif
