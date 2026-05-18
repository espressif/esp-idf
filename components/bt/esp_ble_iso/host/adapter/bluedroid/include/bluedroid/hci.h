/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_BLUEDROID_APP_HCI_H_
#define HOST_BLUEDROID_APP_HCI_H_

#include <stdint.h>

#include "stack/btm_ble_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* When set to 1, sync HCI commands issued by the iso adapter bypass BTM
 * and the global ble_sync_info to avoid the sync_info opcode race with
 * concurrent BTC/BTA sync cmds. Experimental. Toggle is shared with
 * iso.c and gap.c via this header so the BTM fallback paths in both
 * files stay in sync. */
#define USE_DIRECT_HCI      1

int bt_le_bluedroid_hci_init(void);

void bt_le_bluedroid_hci_deinit(void);

/* Send a sync HCI command, bypassing BTM and ble_sync_info. `cmd_params`
 * is the cmd parameter payload (without opcode/length preamble).
 *
 * Returns BTM_SUCCESS, BTM_HCI_ERROR | hci_status on controller failure,
 * or BTM_ERR_PROCESSING on host-side timeout.
 *
 * If `rsp_buf` is non-NULL, up to `rsp_buf_len` bytes of the
 * cmd-specific return params (the bytes after the status byte in the
 * HCI Command_Complete event) are copied in. If the actual payload is
 * shorter, the tail of rsp_buf is left untouched; if longer, the excess
 * is silently dropped. Callers parse opcode-specific structure.
 *
 * Serialization: NOT thread-safe — internally uses single-slot static
 * state (sem, opcode latch, rsp buffer pointer). Callers must serialize
 * externally; in practice all current callers (iso.c hci_cmd_* and
 * gap.c scan_start/stop) run on the iso task, so single-task affinity
 * provides serialization without explicit locking. */
tBTM_STATUS bt_le_bluedroid_hci_send_sync(uint16_t opcode,
                                          const uint8_t *cmd_params,
                                          uint8_t cmd_params_len,
                                          uint8_t *rsp_buf,
                                          uint8_t rsp_buf_len);

#ifdef __cplusplus
}
#endif

#endif /* HOST_BLUEDROID_APP_HCI_H_ */
