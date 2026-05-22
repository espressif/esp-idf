/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * BLE UART — turnkey serial-over-BLE peripheral.
 *
 * Implements the de-facto Nordic UART Service (NUS) GATT layout
 * (RX write, TX notify) on top of either NimBLE or Bluedroid; the
 * backend is picked at compile time via CONFIG_BT_NIMBLE_ENABLED /
 * CONFIG_BT_BLUEDROID_ENABLED.
 *
 * Lifecycle:
 *
 *     ble_uart_install(&cfg);   // host + GATT service
 *     ble_uart_open();          // start advertising + auto-encrypt
 *     ...
 *     ble_uart_close();         // stop adv / disconnect / halt host
 *     ble_uart_uninstall();     // free port + reset state
 *
 * Run-forever apps only need install + open. close / uninstall is
 * for apps that need to power BLE off at runtime.
 *
 * GATT layout (UUIDs fixed by the NUS spec):
 *
 *     Service: 6e400001-b5a3-f393-e0a9-e50e24dcca9e
 *     RX     : 6e400002-b5a3-f393-e0a9-e50e24dcca9e   write
 *     TX     : 6e400003-b5a3-f393-e0a9-e50e24dcca9e   notify
 *
 * See PORTING.md for the integration guide.
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

/** Configuration handed to ble_uart_install(). */
typedef struct {
    /** True = LE Secure Connections + Bonding + MITM, DisplayOnly IO,
     *  encrypted RX/TX chars, bond persisted in NVS (NimBLE: requires
     *  CONFIG_BT_NIMBLE_NVS_PERSIST=y; Bluedroid: default).
     *  False = plaintext (lab debugging only — sniffable). */
    bool encrypted;

    /** GAP device name. NULL keeps the host stack default. Mind the
     *  31-byte primary advertising limit (≤ 8 bytes recommended). */
    const char *device_name;

    /** Byte handler for RX writes. NULL discards incoming data. */
    ble_uart_rx_cb_t ble_uart_on_rx;
} ble_uart_config_t;

/* ----- Lifecycle ------------------------------------------------------ */

/** Bring up host stack + Security Manager + SIG services + NUS GATT
 *  service. Caller must have already called nvs_flash_init().
 *  cfg->device_name is copied; doesn't need to outlive the call.
 *  Single-shot until ble_uart_uninstall(); a second call returns
 *  BLE_UART_EALREADY. */
int  ble_uart_install(const ble_uart_config_t *cfg);

/** Start advertising. NimBLE: spawns the host task and primes the bond
 *  store; advertising begins once the controller signals ready.
 *  Bluedroid: triggers adv-data + scan-response config; advertising
 *  begins once the stack acknowledges both.
 *
 *  Returns immediately; the BLE UART then runs autonomously
 *  (connect, pairing, passkey display, RX delivery all via internal
 *  callbacks). Single-shot. */
int  ble_uart_open(void);

/** Counterpart to ble_uart_open(). Stops advertising, gracefully
 *  disconnects (waits up to 500 ms for LL_TERMINATE_IND ack), and
 *  quiesces the host. install state is preserved — call open() again
 *  to resume.
 *
 *  Don't call from the BLE host task (i.e. from ble_uart_on_rx). */
int  ble_uart_close(void);

/** Counterpart to ble_uart_install(). Force-closes if still open,
 *  then tears down the host stack + controller. After this returns,
 *  install() can run from scratch.
 *
 *  Don't call from the BLE host task. */
int  ble_uart_uninstall(void);

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

/* ----- Service UUID -------------------------------------------------- */

/** The NUS service UUID, exposed for custom advertising payloads.
 *  The two characteristic UUIDs are private to the backend. */
extern const ble_uart_uuid128_t ble_uart_service_uuid;

#ifdef __cplusplus
}
#endif
