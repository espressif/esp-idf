| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# BLE 5.0 Dual Local-Identity Security Server

This example demonstrates **two local identities advertising and being connected/encrypted at the same time** on a BLE 5.0 peripheral, with **isolated bonds per identity**.

It relies on the Host feature `CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND`: the Bluedroid Host derives a per-link **pseudo address** from `f(local_identity, peer)`, so that one phone connecting through two different local identities is treated as two independent peers (separate device record, LTK and NVS bond section).

## What it does

* Advertises **two connectable extended advertising sets** simultaneously:
  * Adv set 0 — **Public** address (identity A, name `ESP_DUAL_PUBLIC_A`)
  * Adv set 1 — **fixed Static Random** address (identity B, name `ESP_DUAL_RANDOM_B`)
* Runs a minimal GATT server with one characteristic that requires an **encrypted** link to read/write.
* On each connection it starts pairing (`SC + MITM + BOND`, static passkey `123456`).
* On `AUTH_CMPL` it prints the link's **real peer** and **local identity** via `esp_ble_gap_get_conn_identity()`, and lists bonded devices.

## Key points for the application

* The `remote_bda` reported in `ESP_GATTS_CONNECT_EVT` is a **Host pseudo address**, not the phone's real MAC. The **same phone shows up as two different addresses**, one per identity.
* **Always use `conn_id` as the link key.** Do not use `remote_bda` to tell links apart.
* Use `esp_ble_gap_get_conn_identity(pseudo, &id)` to recover the real peer MAC and the local identity.
* Use `esp_ble_gap_remove_bond_for_identity(local, local_type, peer, peer_type)` to delete a single identity's bond without affecting the other.
* The Static Random address used for identity B is **hard-coded and fixed** (`s_identity_b_addr`) so the per-identity bond bucket survives reboots. If you randomize it per boot, reconnection cannot match the stored bond.

### Which address to pass to GAP/GATTS APIs

* **Link/bond operations** (`esp_ble_set_encryption`, `esp_ble_gap_disconnect`, `esp_ble_gap_security_rsp`, `esp_ble_confirm_reply`, `esp_ble_passkey_reply`, `esp_ble_gap_update_conn_params`, `esp_ble_gap_read_rssi`, `esp_ble_remove_bond_device`): pass back **exactly the `remote_bda` the event gave you** (the pseudo). Never build the real MAC yourself.
* **Telling links apart / GATTS data** (`esp_ble_gatts_send_indicate`, `send_response`, `close`, …): use **`conn_id`**, not the address.
* **Controller-level operations** (`esp_ble_gap_update_whitelist`, resolving list, directed advertising, `esp_ble_gatts_open`): use the **real peer Identity/RPA**, never the pseudo (the pseudo never goes on air).
* **Need the real MAC**: call `esp_ble_gap_get_real_peer_addr()` / `esp_ble_gap_get_conn_identity()`.

See the Pseudo design guide **§8.2.2** for the full per-API table.

## How to test

1. `idf.py set-target esp32c3` (or s3/c6/h2), then `idf.py flash monitor`.
2. On a phone, scan: you will see **two devices** — `ESP_DUAL_PUBLIC_A` and `ESP_DUAL_RANDOM_B`.
3. Connect and pair to `..._A` (passkey `123456`). Read/write the characteristic — succeeds because the link is encrypted.
4. While still connected to A, connect and pair to `..._B` from the **same phone**.
5. Observe in the log that both links have the **same real peer** but **different pseudo** addresses and **different local identities**, and that `Bonded devices` shows **two** independent entries.
6. Disconnect/reconnect either identity — it re-encrypts from its own stored LTK independently.
7. Remove one bond (e.g. forget `..._B` on the phone, or call `esp_ble_gap_remove_bond_for_identity` for B) — the other identity's bond and encrypted reconnect are unaffected.

## Example log (abridged)

```
I (xxx) DUAL_ID: Pseudo-address dual-identity bond isolation ENABLED
I (xxx) DUAL_ID: Advertising as two identities: Public (A) + Static Random c1:22:33:44:55:66 (B)
[PSEUDO] conn_complete[sync]: keyed handle=0x0 real=<phone> -> pseudo=<C4..A>
I (xxx) DUAL_ID: CONNECT conn_id 0, remote(pseudo) c4:..:a
I (xxx) DUAL_ID: AUTH_CMPL identity: real peer <phone>, local <public>
[PSEUDO] conn_complete[sync]: keyed handle=0x1 real=<phone> -> pseudo=<FB..B>
I (xxx) DUAL_ID: CONNECT conn_id 1, remote(pseudo) fb:..:b
I (xxx) DUAL_ID: AUTH_CMPL identity: real peer <phone>, local c1:22:33:44:55:66
I (xxx) DUAL_ID: Bonded devices: 2 (each entry is a (local,peer) identity = one pseudo)
```

The `[PSEUDO] ...` lines come from the Host feature's debug logging, emitted through the standard BTM trace at debug level. They make it easy to follow the dual-identity flow during bring-up; raise the Bluetooth log level (or lower the BTM trace level) to suppress them.

## Notes

* Requires a BLE 5.0 capable chip (Extended Advertising). ESP32 (original) does not support it.
* If `CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND` is disabled, the example still builds and runs but the two identities of the same phone will **share one bond** and overwrite each other — a startup warning is printed.

## Verified scenarios (ESP32-C3, Android phone using RPA)

* Connect Public_A and Static-Random_B from the same phone, pair/bond each → `Bonded devices: 2` (two isolated pseudo bonds).
* Delete both bonds on the phone, reconnect A and B → fresh re-pairing succeeds for both, still two isolated bonds.
* Disconnect all, reconnect A → **re-encrypts directly from the stored LTK (no re-pairing)**; the two identities' bonds never interfere.

The Host-side feature internals and the fixes behind these scenarios (peer-RPA Identity derivation, SC `active_remote_addr` using the real on-air RPA, disabling same-identity NVS de-dup, duplicate device-record cleanup, BTA `device_list` removal by handle) are documented in the Pseudo design guide, section **§24 (implementation notes)**: `docs/zh_CN/api-guides/ble/bluedroid-periph-pseudo-addr-dev-guide-v0.10.md`.

## Known limitation

The two bonds share the same peer IRK, so adding that IRK to the controller resolving list a second time is rejected (`Add resolving list error 18`). This does **not** affect encrypted reconnection here (RPA resolution is done in the Host), but it matters if you rely on the controller resolving list, white list, or directed advertising.
