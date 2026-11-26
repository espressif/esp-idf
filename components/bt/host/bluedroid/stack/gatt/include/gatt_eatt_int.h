/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* EATT (Enhanced ATT) internal definitions. */

#ifndef GATT_EATT_INT_H
#define GATT_EATT_INT_H

#include "common/bt_target.h"

#if (BLE_EATT_INCLUDED == TRUE)

#include "stack/bt_types.h"
#include "gatt_int.h"

#define GATT_EATT_PSM                   0x0027

/* EATT-bearer TX queue slots kept free for PDUs that must not be refused by
 * the L2CAP CoC cap: one in-progress server response (one per bearer), one
 * unconfirmed server indication (serialized by p_tcb->indicate_handle), one
 * local client command (gatt_eatt_get_available_bearer picks by client_op
 * only), and one client indication confirmation (GATT_HANDLE_VALUE_CONF sent
 * back on the bearer the indication arrived on). Server notifications are
 * refused before reaching this reserve so apps can resend on GATT_BUSY. */
#define GATT_EATT_CTRL_RESERVE          4

_Static_assert(L2CAP_LE_COC_MAX_XMIT_SDU > GATT_EATT_CTRL_RESERVE,
               "L2CAP_LE_COC_MAX_XMIT_SDU must exceed GATT_EATT_CTRL_RESERVE");

typedef void (tGATT_EATT_EVT_CBACK)(UINT16 conn_id, UINT8 status, UINT16 cid);

void gatt_eatt_init(void);
void gatt_eatt_deinit(void);
void gatt_eatt_register_evt_cback(tGATT_EATT_EVT_CBACK *p_cback);
void gatt_eatt_set_chan_num(UINT8 num_chan);
void gatt_eatt_on_encrypted(BD_ADDR bd_addr);

BOOLEAN gatt_eatt_is_bearer(UINT16 lcid);
UINT16 gatt_eatt_get_available_bearer(BD_ADDR bd_addr, UINT8 op);
#if (BLE_EATT_SERVER_INCLUDED == TRUE)
/* Pick an EATT bearer for a server-initiated PDU (notification/indication),
 * round-robin across the connection's bearers. Returns L2CAP_ATT_CID when no
 * EATT bearer is available so the caller falls back to the legacy ATT channel. */
UINT16 gatt_eatt_get_server_tx_bearer(BD_ADDR bd_addr);
/* How many more server PDUs the peer's EATT bearers can take right now.
 * Returns FALSE when the peer has no EATT bearer, so the caller keeps using the
 * ATT fixed channel accounting. */
BOOLEAN gatt_eatt_get_sendable_num(BD_ADDR bd_addr, UINT16 *p_num);
/* Re-read bearer headroom and raise or clear the connection congestion latch.
 * L2CAP only callbacks when credits hit zero or the queue drains, which misses
 * the notify-reserve threshold. */
void gatt_eatt_recheck_congestion(BD_ADDR bd_addr);
#endif
BOOLEAN gatt_eatt_set_default_bearer(UINT16 conn_id, UINT16 lcid);
BOOLEAN gatt_eatt_mark_busy(BD_ADDR bd_addr, UINT16 lcid, UINT8 op, UINT16 clcb_idx);
BOOLEAN gatt_eatt_release_bearer(BD_ADDR bd_addr, UINT16 lcid, UINT8 *p_op, UINT16 *p_clcb_idx);
BOOLEAN gatt_eatt_release_bearer_by_clcb(BD_ADDR bd_addr, UINT16 clcb_idx);

void gatt_eatt_data_ind(UINT16 lcid, BT_HDR *p_buf);
void gatt_eatt_on_chan_mtu_changed(BD_ADDR bd_addr, UINT16 lcid);
UINT16 gatt_eatt_mtu_for_client_op(BD_ADDR bd_addr, UINT8 op_code, UINT16 legacy_mtu);

#endif /* BLE_EATT_INCLUDED == TRUE */

#endif /* GATT_EATT_INT_H */
