/******************************************************************************
 *
 *  Copyright (C) 2026 Espressif Systems (Shanghai) CO LTD
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Peripheral dual local-identity bond isolation: Host-internal pseudo
 *  address derivation and the per-connection identity side table.
 *
 *  A pseudo address is a 6-byte Host-only key computed from
 *  (local_identity, peer). It lets one peer phone that connects through two
 *  distinct local identities (e.g. Public and a fixed Static Random adv set)
 *  appear as two independent peers inside the Host (separate device record,
 *  LTK and NVS bond section). The over-the-air and SMP cryptography keep
 *  using the real peer and the real local identity; the pseudo never leaves
 *  the Host.
 *
 ******************************************************************************/
#ifndef BTM_BLE_PSEUDO_H
#define BTM_BLE_PSEUDO_H

#include "common/bt_target.h"
#include "stack/bt_types.h"

#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE && BLE_PERIPH_PSEUDO_ADDR_BOND == TRUE)

#include "common/bt_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Debug logging for the pseudo-address bond feature. Routed through the Host
 * BTM trace macro so the "[PSEUDO]" lines follow the standard Bluetooth log
 * level (BT_LOG_LEVEL_BTM) like the rest of the stack. */
#define BLE_PSEUDO_DBG(fmt, ...)   BTM_TRACE_DEBUG("[PSEUDO] " fmt, ##__VA_ARGS__)

/* Helper to print a BD_ADDR without a MACSTR dependency. */
#define BLE_PSEUDO_BDA(a)   (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define BLE_PSEUDO_BDA_FMT  "%02x:%02x:%02x:%02x:%02x:%02x"

/* Bump when the pseudo derivation input layout or algorithm changes (e.g. v1
 * was SHA-256 truncated; v2 is AES-CMAC via the configured SMP crypto backend).
 * Persisted in the input/key so a mismatch yields a different pseudo. */
#define BLE_PSEUDO_SCHEME_VER   2

/* Identity that produced one connection: the resolved real peer plus the
 * local identity (Public or fixed Static Random) of the adv set / link. */
typedef struct {
    BD_ADDR         local;          /* local identity (NOT a transient RPA)   */
    BD_ADDR         peer;           /* resolved real peer identity            */
    tBLE_ADDR_TYPE  local_type;
    tBLE_ADDR_TYPE  peer_type;
} tBLE_CONN_IDENTITY;

/* Per-connection side table entry, keyed by HCI handle. */
typedef struct {
    UINT16              handle;
    BD_ADDR             pseudo;
    tBLE_CONN_IDENTITY  id;
    BOOLEAN             local_ready;    /* TRUE once local identity finalized */
    BOOLEAN             in_use;
} tBTM_BLE_CONN_IDENTITY;

/*******************************************************************************
** Function         btm_ble_identity_to_pseudo
**
** Description      Deterministically derive a 6-byte Host pseudo address from
**                  (local identity || peer). Same input always yields the same
**                  output. The result is forced to Static-Random format (top
**                  two bits = 11) so it can never be mistaken for a resolvable
**                  RPA by btm_find_dev()/btm_ble_addr_resolvable().
*******************************************************************************/
void btm_ble_identity_to_pseudo(const tBLE_CONN_IDENTITY *p_id, BD_ADDR pseudo);

/*******************************************************************************
** Function         btm_ble_pseudo_init / btm_ble_pseudo_deinit
*******************************************************************************/
void btm_ble_pseudo_init(void);
void btm_ble_pseudo_deinit(void);

/*******************************************************************************
** Function         btm_ble_conn_identity_register
**
** Description      Record (handle -> pseudo, identity). If an entry for the
**                  handle already exists it is updated. Returns FALSE when the
**                  table is full.
*******************************************************************************/
BOOLEAN btm_ble_conn_identity_register(UINT16 handle,
                                       const tBLE_CONN_IDENTITY *p_id,
                                       const BD_ADDR pseudo,
                                       BOOLEAN local_ready);

/*******************************************************************************
** Function         btm_ble_conn_identity_unregister
*******************************************************************************/
void btm_ble_conn_identity_unregister(UINT16 handle);

/*******************************************************************************
** Function         btm_ble_conn_identity_get_by_handle / get_by_pseudo
**
** Description      Copy-out snapshot of a side-table entry. Safe from any task.
*******************************************************************************/
BOOLEAN btm_ble_conn_identity_get_by_handle(UINT16 handle, tBTM_BLE_CONN_IDENTITY *p_out);
BOOLEAN btm_ble_conn_identity_get_by_pseudo(const BD_ADDR pseudo, tBTM_BLE_CONN_IDENTITY *p_out);

/*******************************************************************************
** Function         btm_ble_conn_identity_exists_by_handle
*******************************************************************************/
BOOLEAN btm_ble_conn_identity_exists_by_handle(UINT16 handle);

/*******************************************************************************
** Function         btm_ble_pseudo_to_real_peer
**
** Description      Reverse map a pseudo back to the real peer. Returns TRUE if
**                  the pseudo is known.
*******************************************************************************/
BOOLEAN btm_ble_pseudo_to_real_peer(const BD_ADDR pseudo, BD_ADDR real_peer);

/*******************************************************************************
** Function         btm_ble_pseudo_finalize_local
**
** Description      Second-phase finalize, called from the LE Advertising Set
**                  Terminated handler. Re-keys a link to its pseudo when the
**                  ext-adv instance was not resolvable at connection complete.
**                  Defined in btm_ble.c.
*******************************************************************************/
void btm_ble_pseudo_finalize_local(UINT16 handle);

/*******************************************************************************
** Function         btm_ble_pseudo_apply_identity
**
** Description      Re-key a link from a transient-RPA-derived pseudo to the
**                  stable f(local, peer Identity) pseudo once the peer's
**                  Identity Address (PID) is learned during pairing. Returns
**                  TRUE and fills new_pseudo when the pseudo changed so the
**                  SMP caller can keep smp_cb.pairing_bda consistent.
*******************************************************************************/
BOOLEAN btm_ble_pseudo_apply_identity(UINT16 handle, const BD_ADDR identity,
                                      UINT8 id_type, BD_ADDR new_pseudo);

#ifdef __cplusplus
}
#endif

#endif /* BLE_INCLUDED && SMP_INCLUDED && BLE_PERIPH_PSEUDO_ADDR_BOND */
#endif /* BTM_BLE_PSEUDO_H */
