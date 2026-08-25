/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <time.h>
#include "esp_log.h"
#include "os/os_mbuf.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_l2cap.h"
#include "nimble/nimble_port.h"
#include "ble_ots_common.h"
#include "ble_ots_server.h"
#include "ble_ots_server_int.h"

static const char *TAG = "ots_oacp_transfer";

/** @brief Back-off before retrying a read chunk that hit a transient buffer shortage */
#define OTS_TRANSFER_RETRY_DELAY_MS     20

/** @brief Consecutive back-off retries tolerated before a read transfer is failed */
#define OTS_TRANSFER_RETRY_MAX          50

/*****************************************************************************
 * Forward Declarations
 *****************************************************************************/
static void transfer_complete_read(ble_ots_server_conn_state_t *cs,
                                   ble_ots_server_transfer_status_t status);
static void transfer_complete_write(ble_ots_server_conn_state_t *cs,
                                    ble_ots_server_transfer_status_t status);
static int send_oacp_response(uint16_t conn_id, uint8_t req_opcode,
                              uint8_t result_code);
static int send_object_data_chunks(ble_ots_server_conn_state_t *cs);
static void ots_transfer_retry_cb(struct ble_npl_event *ev);

/*****************************************************************************
 * Helper: Build and send OACP response indication
 *****************************************************************************/
static int send_oacp_response(uint16_t conn_id, uint8_t req_opcode,
                              uint8_t result_code)
{
    uint8_t buf[3];
    buf[0] = BLE_OTS_OACP_OPCODE_RESPONSE;
    buf[1] = req_opcode;
    buf[2] = result_code;
    return ble_ots_server_indicate_response(conn_id, BLE_OTS_UUID_OACP, buf, sizeof(buf));
}

/*****************************************************************************
 * Helper: Get current timestamp
 *
 * Only meaningful when the application declared a real-time clock at init: the
 * system clock is otherwise unset and would yield an "unknown" (zeroed)
 * Date-Time that must not replace a timestamp the client already wrote.
 *****************************************************************************/
static bool get_current_timestamp(ble_ots_date_time_t *ts)
{
    memset(ts, 0, sizeof(*ts));

    if (!ble_ots_server_env.config.has_realtime_clock) {
        return false;
    }

    time_t now = time(NULL);
    struct tm tm_now;
    if (now <= 0 || gmtime_r(&now, &tm_now) == NULL) {
        return false;
    }

    /* Date-Time only represents years 1582–9999; anything else is "unknown" */
    int year = tm_now.tm_year + 1900;
    if (year < 1582 || year > 9999) {
        return false;
    }

    ts->year = (uint16_t)year;
    ts->month = (uint8_t)(tm_now.tm_mon + 1);
    ts->day = (uint8_t)tm_now.tm_mday;
    ts->hours = (uint8_t)tm_now.tm_hour;
    ts->minutes = (uint8_t)tm_now.tm_min;
    /* Date-Time has no leap-second representation */
    ts->seconds = (uint8_t)(tm_now.tm_sec > 59 ? 59 : tm_now.tm_sec);

    return true;
}

/*****************************************************************************
 * Helper: Read-transfer back-off on transient buffer exhaustion
 *
 * A bulk read fills the host mbuf pool faster than the controller drains it.
 * The resulting allocation failures are flow-control backpressure, not transfer
 * failures, so the transfer pauses and resumes from a short callout instead of
 * tearing the OTC channel down.
 *****************************************************************************/
static void transfer_retry_stop(ble_ots_server_conn_state_t *cs)
{
    if (cs->transfer_retry_timer_inited) {
        ble_npl_callout_stop(&cs->transfer_retry_timer);
    }
    cs->transfer_retry_count = 0;
}

/**
 * @brief Pause the read transfer and schedule a retry of the current chunk.
 *
 * @param cs    Connection state owning the transfer
 * @param cause Human-readable reason, logged on retry and on give-up
 * @return 0 when a retry is pending, BLE_HS_ENOMEM when the budget is exhausted
 */
static int transfer_backoff(ble_ots_server_conn_state_t *cs, const char *cause)
{
    if (cs->transfer_retry_count >= OTS_TRANSFER_RETRY_MAX) {
        ESP_LOGE(TAG, "Read transfer aborted: %s persisted over %d retries",
                 cause, OTS_TRANSFER_RETRY_MAX);
        return BLE_HS_ENOMEM;
    }

    if (!cs->transfer_retry_timer_inited) {
        int rc = ble_npl_callout_init(&cs->transfer_retry_timer,
                                      nimble_port_get_dflt_eventq(),
                                      ots_transfer_retry_cb, cs);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to init transfer retry callout, rc=%d", rc);
            return BLE_HS_ENOMEM;
        }
        cs->transfer_retry_timer_inited = true;
    }

    cs->transfer_retry_count++;
    ESP_LOGD(TAG, "Read transfer backpressure (%s), retry %u in %d ms",
             cause, cs->transfer_retry_count, OTS_TRANSFER_RETRY_DELAY_MS);

    ble_npl_callout_reset(&cs->transfer_retry_timer,
                          ble_npl_time_ms_to_ticks32(OTS_TRANSFER_RETRY_DELAY_MS));
    return 0;
}

/*****************************************************************************
 * Helper: Send object data over L2CAP OTC in chunks
 *****************************************************************************/
static int send_object_data_chunks(ble_ots_server_conn_state_t *cs)
{
    if (!cs || !cs->transfer_active || !cs->transfer_is_read) {
        return -1;
    }

    struct ble_l2cap_chan *chan = cs->otc_chan;
    if (!chan) {
        return -1;
    }

    /* Get channel info to determine MTU for chunking */
    struct ble_l2cap_chan_info chan_info;
    int rc = ble_l2cap_get_chan_info(chan, &chan_info);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to get L2CAP channel info, rc=%d", rc);
        return rc;
    }

    uint16_t mtu = chan_info.peer_coc_mtu;
    if (mtu == 0) {
        mtu = 256; /* fallback */
    }

    while (cs->transfer_bytes_done < cs->transfer_length) {
        uint32_t remaining = cs->transfer_length - cs->transfer_bytes_done;
        uint32_t chunk_size = (remaining > mtu) ? mtu : remaining;

        /* Allocate mbuf for this chunk. A failure here means the fragments
         * already queued for the controller still hold the pool, so wait for
         * them to drain rather than failing the transfer. */
        struct os_mbuf *sdu = os_msys_get_pkthdr(chunk_size, 0);
        if (!sdu) {
            return transfer_backoff(cs, "mbuf pool exhausted");
        }

        /* Read object data into a temporary buffer */
        uint8_t *tmp_buf = malloc(chunk_size);
        if (!tmp_buf) {
            os_mbuf_free_chain(sdu);
            return transfer_backoff(cs, "temp buffer allocation failed");
        }

        rc = ble_ots_server_obj_data_read(cs->transfer_object_id,
                                           cs->transfer_offset + cs->transfer_bytes_done,
                                           chunk_size, tmp_buf);
        if (rc != 0) {
            free(tmp_buf);
            os_mbuf_free_chain(sdu);
            ESP_LOGE(TAG, "Failed to read object data, rc=%d", rc);
            return rc;
        }

        rc = os_mbuf_append(sdu, tmp_buf, (uint16_t)chunk_size);
        free(tmp_buf);
        if (rc != 0) {
            os_mbuf_free_chain(sdu);
            ESP_LOGE(TAG, "Failed to append data to mbuf, rc=%d", rc);
            return rc;
        }

        rc = ble_l2cap_send(chan, sdu);
        if (rc == BLE_HS_ESTALLED) {
            /* L2CAP credits exhausted. The stack keeps the SDU queued and
             * flushes it once the peer grants more credits, so count the chunk
             * as sent and resume when TX_UNSTALLED fires. */
            cs->transfer_bytes_done += chunk_size;
            cs->transfer_retry_count = 0;
            ble_ots_server_transfer_timer_reset(cs->conn_id);
            ESP_LOGD(TAG, "L2CAP send stalled, bytes_done=%lu", (unsigned long)cs->transfer_bytes_done);
            return 0;
        }

        if (rc != 0) {
            /* ble_l2cap_send() consumes the SDU only on success, and on
             * ESTALLED where the stack retains it. On every other return the
             * mbuf is still ours — including the internal failure paths, which
             * detach it from the channel without freeing it. */
            os_mbuf_free_chain(sdu);

            /* Host or controller buffers ran dry for a moment; this is
             * backpressure, not a transfer failure. */
            if (rc == BLE_HS_ENOMEM || rc == BLE_HS_EBUSY) {
                return transfer_backoff(cs, "L2CAP TX buffers exhausted");
            }

            ESP_LOGE(TAG, "ble_l2cap_send failed, rc=%d", rc);
            return rc;
        }

        /* Chunk accepted: the back-off budget applies per chunk, not per transfer */
        cs->transfer_bytes_done += chunk_size;
        cs->transfer_retry_count = 0;
        ble_ots_server_transfer_timer_reset(cs->conn_id);
    }

    /* All data sent successfully */
    if (cs->transfer_bytes_done >= cs->transfer_length) {
        transfer_complete_read(cs, BLE_OTS_TRANSFER_SUCCESS);
    }

    return 0;
}

/*****************************************************************************
 * Transfer Complete: Read
 *****************************************************************************/
static void transfer_complete_read(ble_ots_server_conn_state_t *cs,
                                   ble_ots_server_transfer_status_t status)
{
    if (!cs) {
        return;
    }

    ble_ots_server_transfer_timer_stop(cs->conn_id);
    transfer_retry_stop(cs);

    ble_ots_server_cb_param_t param;
    memset(&param, 0, sizeof(param));
    param.read_complete.object_id = cs->transfer_object_id;
    param.read_complete.offset = cs->transfer_offset;
    param.read_complete.length = cs->transfer_length;
    param.read_complete.bytes_sent = cs->transfer_bytes_done;
    param.read_complete.status = status;

    cs->transfer_active = false;
    cs->transfer_is_read = false;

    ble_ots_server_dispatch_event(BLE_OTS_SERVER_EVT_READ_COMPLETE, &param);
}

/*****************************************************************************
 * Transfer Complete: Write
 *****************************************************************************/
static void transfer_complete_write(ble_ots_server_conn_state_t *cs,
                                    ble_ots_server_transfer_status_t status)
{
    if (!cs) {
        return;
    }

    ble_ots_server_transfer_timer_stop(cs->conn_id);

    ble_ots_obj_id_t obj_id = cs->transfer_object_id;
    uint32_t offset = cs->transfer_offset;
    uint32_t bytes_received = cs->transfer_bytes_done;

    /* The object changed if data landed in it, or if the procedure already
     * truncated it — a Truncate takes effect when the Write procedure starts,
     * so it stands even when the transfer is later interrupted. */
    bool obj_modified = (bytes_received > 0) || cs->transfer_truncated;

    /* Update current size if the write extended the object */
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(obj_id);
    if (obj) {
        uint32_t new_end = offset + bytes_received;
        if (new_end > obj->current_size) {
            obj->current_size = new_end;
        }

        /* Refresh Last-Modified only when the object actually changed and a
         * real time source is available; otherwise keep the value the client
         * or the application wrote instead of blanking it to "unknown". */
        ble_ots_date_time_t now;
        if (obj_modified && get_current_timestamp(&now)) {
            obj->last_modified = now;
        }
    }

    /* Unlock the object */
    ble_ots_server_obj_unlock(obj_id);

    cs->transfer_active = false;
    cs->transfer_is_read = false;
    cs->transfer_truncated = false;

    /* Dispatch Object Changed indication with content flag */
    if (status == BLE_OTS_TRANSFER_SUCCESS || obj_modified) {
        uint8_t flags = BLE_OTS_OBJ_CHANGED_FLAG_SOURCE | BLE_OTS_OBJ_CHANGED_FLAG_CONTENT;
        ble_ots_server_dispatch_obj_changed(obj_id, flags, cs->conn_id);
    }

    /* Fire write complete event */
    ble_ots_server_cb_param_t param;
    memset(&param, 0, sizeof(param));
    param.write_complete.object_id = obj_id;
    param.write_complete.offset = offset;
    param.write_complete.bytes_received = bytes_received;
    param.write_complete.status = status;

    ble_ots_server_dispatch_event(BLE_OTS_SERVER_EVT_WRITE_COMPLETE, &param);
}

/*****************************************************************************
 * Read-Transfer Back-off Retry Callback
 *
 * Fires in the NimBLE host task after transfer_backoff() paused a read that
 * ran out of TX buffers.
 *****************************************************************************/
static void ots_transfer_retry_cb(struct ble_npl_event *ev)
{
    if (!BLE_OTS_SERVER_ENV_OK()) {
        return;
    }

    ble_ots_server_conn_state_t *cs =
        (ble_ots_server_conn_state_t *)ble_npl_event_get_arg(ev);
    if (!cs || !cs->in_pool || !cs->transfer_active || !cs->transfer_is_read) {
        return;
    }

    int rc = send_object_data_chunks(cs);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to resume read transfer after back-off, rc=%d", rc);
        ble_ots_server_otc_close(cs->conn_id);
        transfer_complete_read(cs, BLE_OTS_TRANSFER_CHANNEL_CLOSED);
    }
}

/*****************************************************************************
 * L2CAP OTC Data Receive Callback (for write transfers)
 *****************************************************************************/
void ble_ots_server_otc_receive_cb(uint16_t conn_handle, struct ble_l2cap_chan *chan,
                                   struct os_mbuf *sdu_rx)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_handle);
    if (!cs || !cs->transfer_active || cs->transfer_is_read) {
        /* Not expecting data; free the mbuf */
        if (sdu_rx) {
            os_mbuf_free_chain(sdu_rx);
        }
        return;
    }

    if (!sdu_rx) {
        return;
    }

    /* Calculate total received data length from mbuf chain */
    uint16_t data_len = OS_MBUF_PKTLEN(sdu_rx);
    if (data_len == 0) {
        os_mbuf_free_chain(sdu_rx);
        return;
    }

    /* Check for excess data */
    uint32_t remaining = cs->transfer_length - cs->transfer_bytes_done;
    if (data_len > remaining) {
        ESP_LOGW(TAG, "Excess data received: got %u, expected at most %lu",
                 data_len, (unsigned long)remaining);
        os_mbuf_free_chain(sdu_rx);

        /* Close OTC channel to prevent further data */
        ble_ots_server_otc_close(conn_handle);
        transfer_complete_write(cs, BLE_OTS_TRANSFER_EXCESS_DATA);
        return;
    }

    /* Extract data from mbuf chain and write to object */
    uint32_t write_offset = cs->transfer_offset + cs->transfer_bytes_done;
    struct os_mbuf *cur = sdu_rx;
    uint32_t written = 0;

    int write_err = 0;
    while (cur != NULL && written < data_len) {
        if (cur->om_len > 0) {
            int rc = ble_ots_server_obj_data_write(cs->transfer_object_id,
                                                    write_offset + written,
                                                    cur->om_data, cur->om_len);
            if (rc != 0) {
                ESP_LOGE(TAG, "Failed to write object data, rc=%d", rc);
                write_err = rc;
                break;
            }
            written += cur->om_len;
        }
        cur = SLIST_NEXT(cur, om_next);
    }

    os_mbuf_free_chain(sdu_rx);

    cs->transfer_bytes_done += written;

    /* If writing to the object database failed, abort the transfer immediately */
    if (write_err != 0) {
        ESP_LOGE(TAG, "Aborting write transfer due to obj_data_write failure, rc=%d", write_err);
        ble_ots_server_otc_close(conn_handle);
        transfer_complete_write(cs, BLE_OTS_TRANSFER_CHANNEL_CLOSED);
        return;
    }

    /* Reset inactivity timer */
    ble_ots_server_transfer_timer_reset(cs->conn_id);

    ESP_LOGD(TAG, "Write transfer: received %lu/%lu bytes",
             (unsigned long)cs->transfer_bytes_done,
             (unsigned long)cs->transfer_length);

    /* Check if transfer is complete */
    if (cs->transfer_bytes_done >= cs->transfer_length) {
        transfer_complete_write(cs, BLE_OTS_TRANSFER_SUCCESS);
    }
}

/*****************************************************************************
 * L2CAP OTC TX Un-stalled Callback (for read transfers)
 *****************************************************************************/
void ble_ots_server_otc_tx_unstalled_cb(uint16_t conn_handle,
                                        struct ble_l2cap_chan *chan)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_handle);
    if (!cs || !cs->transfer_active || !cs->transfer_is_read) {
        return;
    }

    /* Resume sending data */
    int rc = send_object_data_chunks(cs);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to resume read transfer, rc=%d", rc);
        ble_ots_server_otc_close(conn_handle);
        transfer_complete_read(cs, BLE_OTS_TRANSFER_CHANNEL_CLOSED);
    }
}

/*****************************************************************************
 * L2CAP OTC Channel Closed Callback
 *****************************************************************************/
void ble_ots_server_otc_disconnected_cb(uint16_t conn_handle,
                                        struct ble_l2cap_chan *chan)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_handle);
    if (!cs) {
        return;
    }

    /* Clear the OTC channel reference */
    cs->otc_chan = NULL;

    if (!cs->transfer_active) {
        return;
    }

    ESP_LOGW(TAG, "OTC channel closed during transfer on conn_id=%d", conn_handle);

    if (cs->transfer_is_read) {
        transfer_complete_read(cs, BLE_OTS_TRANSFER_CHANNEL_CLOSED);
    } else {
        transfer_complete_write(cs, BLE_OTS_TRANSFER_CHANNEL_CLOSED);
    }
}

/*****************************************************************************
 * Deferred read-data send via NimBLE event queue
 *
 * The initial burst of object data must be sent AFTER the OACP response
 * indication. Both are posted to the NimBLE default event queue (FIFO), so
 * queueing the data-send event after the indication event guarantees ordering.
 *****************************************************************************/
static void ots_transfer_data_event_cb(struct ble_npl_event *ev)
{
    /* cs points into the control block — only valid while it is allocated */
    if (!BLE_OTS_SERVER_ENV_OK()) {
        return;
    }

    ble_ots_server_conn_state_t *cs =
        (ble_ots_server_conn_state_t *)ble_npl_event_get_arg(ev);
    if (!cs || !cs->in_pool || !cs->transfer_active || !cs->transfer_is_read) {
        return;
    }

    int rc = send_object_data_chunks(cs);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to send object data (event), rc=%d", rc);
        ble_ots_server_otc_close(cs->conn_id);
        transfer_complete_read(cs, BLE_OTS_TRANSFER_CHANNEL_CLOSED);
    }
}

static void ots_transfer_data_schedule(ble_ots_server_conn_state_t *cs)
{
    if (!cs->transfer_data_ev_inited) {
        ble_npl_event_init(&cs->transfer_data_ev, ots_transfer_data_event_cb, cs);
        cs->transfer_data_ev_inited = true;
    }
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(), &cs->transfer_data_ev);
}

/*****************************************************************************
 * OACP Read Procedure
 *****************************************************************************/
int ble_ots_server_oacp_read_proc(uint16_t conn_id, uint32_t offset, uint32_t length)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return -1;
    }

    /* Priority 1: Check OACP Read feature support (bit 4) */
    if (!(ble_ots_server_env.ots_feature.oacp_features & BLE_OTS_OACP_FEAT_READ)) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED);
        return 0;
    }

    /* Priority 2: Check Current Object is valid */
    if (!cs->current_obj_valid) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT);
        return 0;
    }

    ble_ots_obj_id_t obj_id = cs->current_object_id;
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(obj_id);
    if (!obj) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT);
        return 0;
    }

    /* Priority 3: Check object Read property (bit 2) */
    if (!(obj->properties & BLE_OTS_OBJ_PROP_READ)) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED);
        return 0;
    }

    /* Priority 4: Check OTC channel available */
    struct ble_l2cap_chan *chan = ble_ots_server_otc_get(conn_id);
    if (!chan) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_CHANNEL_UNAVAILABLE);
        return 0;
    }

    /* Priority 5: Offset exceeds Current Size */
    if (offset > obj->current_size) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Offset and Length are raw client input: reject a sum that would wrap
     * before it can slip past the Priority 6 bounds check below. */
    if (length > 0 && offset > UINT32_MAX - length) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Priority 6: Offset + Length exceeds Current Size */
    if ((offset + length) > obj->current_size) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Priority 7: Length exceeds server capacity to read
     * (we assume we can always read from our own DB, so skip unless length is 0) */
    if (length == 0) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Priority 8: Object locked by another client */
    if (obj->locked_by != BLE_OTS_CONN_ID_NONE && obj->locked_by != conn_id) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_OBJECT_LOCKED);
        return 0;
    }

    /* Priority 9: Transfer already in progress on this object */
    if (cs->transfer_active) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                           BLE_OTS_OACP_RESULT_OBJECT_LOCKED);
        return 0;
    }

    /* All checks passed — mark transfer active */
    cs->transfer_active = true;
    cs->transfer_is_read = true;
    cs->transfer_object_id = obj_id;
    cs->transfer_offset = offset;
    cs->transfer_length = length;
    cs->transfer_bytes_done = 0;
    transfer_retry_stop(cs);

    /* Send Success indication */
    int rc = send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_READ,
                                BLE_OTS_OACP_RESULT_SUCCESS);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to send OACP Read success indication, rc=%d", rc);
        cs->transfer_active = false;
        cs->transfer_is_read = false;
        return rc;
    }

    /* Start transfer inactivity timer */
    ble_ots_server_transfer_timer_start(conn_id);

    /* Queue the object data send as a NimBLE event. Because the response
     * indication was also queued (via ble_ots_server_indicate_response) and the
     * default event queue is FIFO, the data is sent AFTER the indication. */
    ots_transfer_data_schedule(cs);

    return 0;
}

/*****************************************************************************
 * OACP Write Procedure
 *****************************************************************************/
int ble_ots_server_oacp_write_proc(uint16_t conn_id, uint32_t offset,
                                   uint32_t length, uint8_t mode)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return -1;
    }

    bool truncate = (mode & BLE_OTS_OACP_WRITE_MODE_TRUNCATE) != 0;

    /* Priority 1: Check OACP Write feature support (bit 5) */
    if (!(ble_ots_server_env.ots_feature.oacp_features & BLE_OTS_OACP_FEAT_WRITE)) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED);
        return 0;
    }

    /* Priority 2: Check Current Object is valid */
    if (!cs->current_obj_valid) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT);
        return 0;
    }

    ble_ots_obj_id_t obj_id = cs->current_object_id;
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(obj_id);
    if (!obj) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT);
        return 0;
    }

    /* Priority 3: Check object Write property (bit 3) */
    if (!(obj->properties & BLE_OTS_OBJ_PROP_WRITE)) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED);
        return 0;
    }

    /* Check for integer overflow in offset + length (before any arithmetic) */
    if (length > 0 && offset > UINT32_MAX - length) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Priority 4: Patching check — if not truncating and offset + length <= current_size,
     * this is a patch operation. Check OACP Patch feature (bit 8). */
    if (!truncate && (offset + length) <= obj->current_size) {
        if (!(ble_ots_server_env.ots_feature.oacp_features & BLE_OTS_OACP_FEAT_PATCH)) {
            send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                               BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED);
            return 0;
        }

        /* Priority 5: Check object Patch property (bit 6) */
        if (!(obj->properties & BLE_OTS_OBJ_PROP_PATCH)) {
            send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                               BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED);
            return 0;
        }
    }

    /* Priority 6: Truncation check — if truncate bit set, check Truncate property (bit 5) */
    if (truncate) {
        if (!(ble_ots_server_env.ots_feature.oacp_features & BLE_OTS_OACP_FEAT_TRUNCATE)) {
            send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                               BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED);
            return 0;
        }
        if (!(obj->properties & BLE_OTS_OBJ_PROP_TRUNCATE)) {
            send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                               BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED);
            return 0;
        }
    }

    /* Priority 7: Check OTC channel available */
    struct ble_l2cap_chan *chan = ble_ots_server_otc_get(conn_id);
    if (!chan) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_CHANNEL_UNAVAILABLE);
        return 0;
    }

    /* Priority 8: Check RFU bits in mode (bits 0, 2-7 except bit 1) */
    uint8_t rfu_mask = (uint8_t)~BLE_OTS_OACP_WRITE_MODE_TRUNCATE;
    if (mode & rfu_mask) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Priority 9: Offset exceeds Current Size (for non-truncate, offset must be <= current_size) */
    if (!truncate && offset > obj->current_size) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }
    /* For truncate mode, offset can be <= current_size (truncation sets current_size = offset) */
    if (truncate && offset > obj->current_size) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Priority 10: Offset + Length exceeds Allocated Size and no append support */
    bool needs_append = false;
    if ((offset + length) > obj->allocated_size) {
        needs_append = true;
    }

    if (needs_append && !(ble_ots_server_env.ots_feature.oacp_features & BLE_OTS_OACP_FEAT_APPEND)) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Priority 11: Length exceeds server capacity to write.
     * A plain write of zero octets selects nothing and is rejected. Combined
     * with Truncate it is meaningful — Truncate sets the Current Size to Offset
     * plus the octets written, so Length 0 is how a client shrinks an object
     * without supplying new content. */
    if (!truncate && length == 0) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER);
        return 0;
    }

    /* Priority 12: Object locked by another client */
    if (obj->locked_by != BLE_OTS_CONN_ID_NONE && obj->locked_by != conn_id) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_OBJECT_LOCKED);
        return 0;
    }

    /* Priority 13: Transfer already in progress */
    if (cs->transfer_active) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                           BLE_OTS_OACP_RESULT_OBJECT_LOCKED);
        return 0;
    }

    /* Supplementary Append checks (only when needs_append is true) */
    if (needs_append) {
        /* Append Priority 1: Check object Append property (bit 4) */
        if (!(obj->properties & BLE_OTS_OBJ_PROP_APPEND)) {
            send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                               BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED);
            return 0;
        }

        /* Append Priority 2: Check server capacity to increase allocated size.
         * The actual reallocation is deferred to ble_ots_server_obj_data_write
         * when data chunks arrive. Do not update obj->allocated_size here. */
    }

    /* Lock the object under concurrency */
    if (CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY > 1) {
        int lock_rc = ble_ots_server_obj_lock(obj_id, conn_id);
        if (lock_rc != 0) {
            send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                               BLE_OTS_OACP_RESULT_OBJECT_LOCKED);
            return 0;
        }
    }

    /* Mark transfer active */
    cs->transfer_active = true;
    cs->transfer_is_read = false;
    cs->transfer_object_id = obj_id;
    cs->transfer_offset = offset;
    cs->transfer_length = length;
    cs->transfer_bytes_done = 0;
    cs->transfer_truncated = false;

    /* Send Success indication */
    int rc = send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_WRITE,
                                BLE_OTS_OACP_RESULT_SUCCESS);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to send OACP Write success indication, rc=%d", rc);
        cs->transfer_active = false;
        ble_ots_server_obj_unlock(obj_id);
        return rc;
    }

    /* Perform truncation if requested (deferred until after successful response) */
    if (truncate) {
        obj->current_size = offset;
        cs->transfer_truncated = true;
    }

    /* A Truncate-only request carries no object data, so the procedure is
     * already done. Completing it here releases the object lock and emits the
     * Object Changed indication instead of leaving a transfer that could only
     * ever end on the inactivity timeout. */
    if (length == 0) {
        ESP_LOGI(TAG, "Object truncated: obj_id=0x%llx, current_size=%lu",
                 (unsigned long long)obj_id, (unsigned long)offset);
        transfer_complete_write(cs, BLE_OTS_TRANSFER_SUCCESS);
        return 0;
    }

    /* Start transfer inactivity timer */
    ble_ots_server_transfer_timer_start(conn_id);

    ESP_LOGI(TAG, "Write transfer started: obj_id=0x%llx, offset=%lu, length=%lu, mode=0x%02x",
             (unsigned long long)obj_id, (unsigned long)offset,
             (unsigned long)length, mode);

    return 0;
}

/*****************************************************************************
 * OACP Abort Procedure
 *****************************************************************************/
int ble_ots_server_oacp_abort_proc(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return -1;
    }

    /* Priority 1: Check OACP Abort feature support (bit 9) */
    if (!(ble_ots_server_env.ots_feature.oacp_features & BLE_OTS_OACP_FEAT_ABORT)) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_ABORT,
                           BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED);
        return 0;
    }

    /* Priority 2: No Read operation in progress */
    if (!cs->transfer_active || !cs->transfer_is_read) {
        send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_ABORT,
                           BLE_OTS_OACP_RESULT_OPERATION_FAILED);
        return 0;
    }

    /* Priority 3: Abort must come from the same client that initiated the read.
     * Since cs is per-connection and we already matched conn_id, this is inherently satisfied.
     * But we verify the transfer is on this connection's state. */

    /* Cancel the in-progress read transfer */
    ble_ots_server_transfer_timer_stop(conn_id);
    transfer_retry_stop(cs);

    /* Save transfer info before clearing */
    ble_ots_obj_id_t obj_id = cs->transfer_object_id;
    uint32_t transfer_offset = cs->transfer_offset;
    uint32_t transfer_length = cs->transfer_length;
    uint32_t bytes_sent = cs->transfer_bytes_done;

    /* Mark transfer as no longer active */
    cs->transfer_active = false;
    cs->transfer_is_read = false;

    /* Send Success indication for Abort */
    int rc = send_oacp_response(conn_id, BLE_OTS_OACP_OPCODE_ABORT,
                                BLE_OTS_OACP_RESULT_SUCCESS);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to send OACP Abort success indication, rc=%d", rc);
    }

    /* Fire Read Complete event with ABORTED status */
    ble_ots_server_cb_param_t param;
    memset(&param, 0, sizeof(param));
    param.read_complete.object_id = obj_id;
    param.read_complete.offset = transfer_offset;
    param.read_complete.length = transfer_length;
    param.read_complete.bytes_sent = bytes_sent;
    param.read_complete.status = BLE_OTS_TRANSFER_ABORTED;

    ble_ots_server_dispatch_event(BLE_OTS_SERVER_EVT_READ_COMPLETE, &param);

    ESP_LOGI(TAG, "Read transfer aborted: obj_id=0x%llx, bytes_sent=%lu/%lu",
             (unsigned long long)obj_id, (unsigned long)bytes_sent,
             (unsigned long)transfer_length);

    return 0;
}

/*****************************************************************************
 * Non-static wrappers (called from ots_server_init.c via extern)
 *****************************************************************************/

/**
 * @brief Transfer timeout handler called from the init module.
 *
 * Delegates to the internal timeout callback logic.
 *
 * @param conn_id BLE connection identifier
 */
void ble_ots_server_oacp_transfer_timeout(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs || !cs->transfer_active) {
        return;
    }

    ESP_LOGW(TAG, "Transfer timeout on conn_id=%d", conn_id);

    if (cs->transfer_is_read) {
        ble_ots_server_otc_close(conn_id);
        transfer_complete_read(cs, BLE_OTS_TRANSFER_TIMEOUT);
    } else {
        ble_ots_server_otc_close(conn_id);
        transfer_complete_write(cs, BLE_OTS_TRANSFER_TIMEOUT);
    }
}

/**
 * @brief L2CAP COC data receive handler called from the init module.
 *
 * Handles incoming L2CAP COC data during write transfers by delegating
 * to the internal receive callback.
 *
 * @param conn_handle BLE connection handle
 * @param chan        L2CAP channel
 * @param sdu_rx     Received SDU mbuf chain
 * @return 0 on success
 */
int ble_ots_server_oacp_l2cap_recv(uint16_t conn_handle,
                                   struct ble_l2cap_chan *chan,
                                   struct os_mbuf *sdu_rx)
{
    ble_ots_server_otc_receive_cb(conn_handle, chan, sdu_rx);
    return 0;
}
