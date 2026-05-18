/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "bluedroid/hci.h"

#if USE_DIRECT_HCI

/* hci/hci_layer.h pulls in osi/osi.h which defines a 2-arg CONCAT(a, b)
 * macro that conflicts with the variadic CONCAT(...) from Zephyr's
 * <zephyr/sys/util.h>. hci.c does not use CONCAT itself, so drop the
 * Zephyr definition before pulling in osi's. */
#undef CONCAT
#include "hci/hci_layer.h"

#include "stack/hcimsgs.h"
#include "stack/btm_ble_api.h"
#include "bluedroid/btm_error.h"

#include "common/host.h"

LOG_MODULE_REGISTER(ISO_BHCI, CONFIG_BT_ISO_LOG_LEVEL);

/* Adapter-private sync cmd path. Decouples sync HCI cmds from BTU's global
 * ble_sync_info entirely: each cmd carries its own command_complete_cb (this
 * file's direct_hci_complete_cb) and the caller waits on direct_hci_sem,
 * not ble_sync_info.sync_sem. Two-task race on sync_info->opcode disappears.
 *
 * Caller sets direct_hci_rsp.opcode before transmit; cb verifies the
 * response opcode matches before giving the sem (guards against unexpected
 * stray Command_Complete dispatches).
 *
 * direct_hci_complete_cb runs on the hci_layer task — keep it tiny and
 * non-blocking, no host_lock, no further dispatch.
 *
 * Concurrent safety: send_sync is serialized by callers via bt_le_host_lock,
 * so the static rsp_buf pointer / opcode latch are single-slot. */
static struct k_sem direct_hci_sem;

/* Set by deinit before deleting the sem. Late-arriving cb's must check
 * this before touching the sem. Residual race exists (cb past the check
 * but pre-give vs. deinit completing the delete) — accepted in practice
 * because the BTU/HCI layer offers no way to cancel an in-flight cmd. */
static volatile bool direct_hci_shutting_down;

static struct {
    uint16_t opcode;   /* expected — set by caller, verified by cb */
    uint8_t  status;   /* HCI status from Command_Complete */
} direct_hci_rsp;

/* Internal sink for the cmd-specific return payload (bytes after the status
 * byte). The cb copies HERE, never into the caller's stack buffer — a
 * timed-out caller may already have freed its stack frame by the time a late
 * cb runs on the hci_layer task. The caller copies this out only after a
 * successful take. Static storage: a late cb always has a valid target, so no
 * teardown race. Sized for the largest payload any direct-HCI caller reads
 * back (SET_CIG: 2 + 2*cis_count, ISO_READ_TX_SYNC: 11). */
#define DIRECT_HCI_RSP_MAX  MAX(2 + 2 * CONFIG_BT_ISO_MAX_CHAN, 11)
static uint8_t direct_hci_rsp_data[DIRECT_HCI_RSP_MAX];
static uint8_t direct_hci_rsp_data_len;

static void direct_hci_complete_cb(BT_HDR *response, void *context)
{
    /* Command_Complete event layout after BT_HDR data/offset:
     *   [0] event code (0x0E)   [1] param len   [2] num_cmd_packets
     *   [3..4] opcode (LE)      [5] status      [6..] cmd-specific params
     */
    uint8_t *stream = response->data + response->offset + 3;
    uint16_t opcode;
    uint8_t status;
    uint8_t event_param_len;

    ARG_UNUSED(context);

    event_param_len = response->data[response->offset + 1];

    STREAM_TO_UINT16(opcode, stream);
    STREAM_TO_UINT8(status, stream);

    if (opcode != direct_hci_rsp.opcode) {
        LOG_ERR("[B]DirectHciOpcodeMismatch[exp=0x%04x][got=0x%04x]",
                direct_hci_rsp.opcode, opcode);
        /* Drop — don't give sem, caller times out. */
        osi_free(response);
        return;
    }

    direct_hci_rsp.status = status;

    /* Copy cmd-specific return params (everything after the status byte)
     * into the caller's buffer. event_param_len counts:
     *   num_cmd_packets(1) + opcode(2) + status(1) + cmd-specific(N)
     * so the payload available to caller is event_param_len - 4. */
    if (event_param_len > 4) {
        direct_hci_rsp_data_len = MIN(event_param_len - 4, sizeof(direct_hci_rsp_data));
        memcpy(direct_hci_rsp_data, stream, direct_hci_rsp_data_len);
    }

    osi_free(response);

    /* deinit may have set the shutdown flag and be about to delete the
     * sem. Skip the give to avoid asserting on a NULL handle. */
    if (direct_hci_shutting_down) {
        return;
    }

    k_sem_give(&direct_hci_sem);
}

tBTM_STATUS bt_le_bluedroid_hci_send_sync(uint16_t opcode,
                                          const uint8_t *cmd_params,
                                          uint8_t cmd_params_len,
                                          uint8_t *rsp_buf,
                                          uint8_t rsp_buf_len)
{
    BT_HDR *p;
    UINT8 *pp;
    hci_cmd_metadata_t *metadata;

    p = HCI_GET_CMD_BUF(cmd_params_len);
    if (p == NULL) {
        return BTM_NO_RESOURCES;
    }

    pp = p->data;
    UINT16_TO_STREAM(pp, opcode);
    UINT8_TO_STREAM(pp, cmd_params_len);
    if (cmd_params_len > 0 && cmd_params != NULL) {
        memcpy(pp, cmd_params, cmd_params_len);
    }

    metadata = HCI_GET_CMD_METAMSG(p);
    metadata->command_complete_cb = direct_hci_complete_cb;
    metadata->command_status_cb = NULL;
    metadata->opcode = opcode;
    metadata->context = NULL;

    /* Set expected opcode and rsp sink BEFORE transmit — cb may run
     * before transmit returns if response is already queued. */
    direct_hci_rsp.opcode = opcode;
    direct_hci_rsp.status = 0;
    direct_hci_rsp_data_len = 0;

    /* Drop any stale give from a previous timed-out cmd whose cb arrived
     * late. Must run AFTER opcode update (so any cb arriving after this
     * drain hits the mismatch path and doesn't give) and BEFORE transmit
     * (so this cmd's own cb-give isn't accidentally consumed here). */
    k_sem_reset(&direct_hci_sem);

    hci_layer_get_interface()->transmit_command(p, direct_hci_complete_cb,
                                                NULL, NULL);

    if (k_sem_take(&direct_hci_sem, K_SEM_SHORT) != 0) {
        LOG_ERR("[B]DirectHciTimeout[0x%04x]", opcode);
        return BTM_ERR_PROCESSING;
    }

    /* The cb gives the sem only after its memcpy, so the payload is complete;
     * copy it into the caller's (now-confirmed-live) sink. */
    if (rsp_buf != NULL && rsp_buf_len > 0 && direct_hci_rsp_data_len > 0) {
        memcpy(rsp_buf, direct_hci_rsp_data, MIN(direct_hci_rsp_data_len, rsp_buf_len));
    }

    if (direct_hci_rsp.status != HCI_SUCCESS) {
        LOG_ERR("[B]DirectHciStatus[0x%04x][%02x]",
                opcode, direct_hci_rsp.status);
        return BTM_HCI_ERROR | direct_hci_rsp.status;
    }

    return BTM_SUCCESS;
}

int bt_le_bluedroid_hci_init(void)
{
    direct_hci_shutting_down = false;
    k_sem_create(&direct_hci_sem);
    return 0;
}

void bt_le_bluedroid_hci_deinit(void)
{
    /* Flip the gate first so any cb past this point skips the give.
     * Drain any give that was already posted before the flag flip so
     * the sem is clean before deletion. */
    direct_hci_shutting_down = true;
    k_sem_reset(&direct_hci_sem);
    k_sem_delete(&direct_hci_sem);
}

#endif /* USE_DIRECT_HCI */
