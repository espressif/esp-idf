/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_cs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/ras/ble_svc_ras.h"
#include "ble_chan_initiator.h"
#include "esp_nimble_cfg.h"
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
#include "esp_nimble_mem.h"
#endif

static const char *tag = "NimBLE_CS_INITIATOR";
static int ble_chan_sound_init_gap_event(struct ble_gap_event *event, void *arg);

void ble_store_config_init(void);

/* Ranging counter tracking (logged on RD Ready to correlate with the peer's counter) */
static int32_t most_recent_local_ranging_counter = -1;

/* Run MTU exchange and service discovery only once per connection */
static bool disc_initiated;

/* ---- Ranging data transfer mode (change these two to switch test scenarios) ----
 * CS_INIT_PREFER_REALTIME : 1 = real-time push,  0 = on-demand GET_RD
 * CS_INIT_USE_INDICATE    : 1 = data RD indicate, 0 = notify  (RT and OD)
 * ----------------------------------------------------------------------------- */
#define CS_INIT_PREFER_REALTIME 1
#define CS_INIT_USE_INDICATE    1

/* Real-time vs On-demand mode (decided after reading server features) */
static bool s_use_realtime;

/* Free-running CS: procedures are enabled once (max_procedure_count = 0 in
 * ble_cs.c) and repeat until the link drops; this only counts completions. */
static uint32_t cs_procedures_done;

/* Local CS step data storage.  Holds one full procedure's raw HCI step
 * data (per-step layout: mode(1) + channel(1) + data_len(1) + data[]) so
 * an application-supplied distance-estimation module can consume it via
 * local_steps / local_steps_len together with the peer's ranging data.
 * STEP_DATA_BUF_LEN tracks the BLE_RAS_MAX_* sizing in ble_svc_ras.h. */
#define CS_STEP_HDR_LEN  3  /* sizeof packed {mode, channel, data_len} */
#define STEP_DATA_BUF_LEN \
    (BLE_RAS_MAX_STEPS_PER_PROCEDURE * \
     (CS_STEP_HDR_LEN + BLE_RAS_MAX_STEP_DATA_LEN))

/* Local CS step data buffer.  Mirrors the NimBLE BLE_STATIC_TO_DYNAMIC
 * idiom: legacy static .bss array when the flag is off, heap-allocated via
 * nimble_platform_mem_calloc() on the first subevent of a procedure when it
 * is on. */
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
static uint8_t  *local_steps = NULL;
#else
static uint8_t   local_steps[STEP_DATA_BUF_LEN];
#endif
static uint16_t  local_steps_len;
static uint16_t  local_steps_num;
static uint8_t   local_steps_nap;
static int32_t   local_steps_proc_counter = -1;

/**
 * Walk packed variable-length step entries and return total byte length.
 * HCI format: mode(1) + channel(1) + data_len(1) + data[data_len].
 */
static uint16_t
cs_step_data_total_len(const struct cs_steps_data *steps, uint8_t num_steps)
{
    const uint8_t *ptr = (const uint8_t *)steps;
    uint16_t off = 0;

    for (int i = 0; i < num_steps; i++) {
        const struct cs_steps_data *s =
            (const struct cs_steps_data *)(ptr + off);
        off += sizeof(struct cs_steps_data) + s->data_len;
    }
    return off;
}

/**
 * Allocate the local_steps buffer on first use; no-op when
 * BLE_STATIC_TO_DYNAMIC is disabled (static array).  Released on
 * disconnect by local_steps_buf_deinit().
 */
static int
local_steps_buf_init(void)
{
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    if (local_steps == NULL) {
        local_steps = nimble_platform_mem_calloc(1, STEP_DATA_BUF_LEN);
        if (local_steps == NULL) {
            MODLOG_DFLT(ERROR, "[CS] local_steps alloc failed (%d B)\n",
                        STEP_DATA_BUF_LEN);
            return BLE_HS_ENOMEM;
        }
    }
#endif
    return 0;
}

/**
 * Release the local_steps buffer on disconnect; the next procedure
 * re-allocates it via local_steps_buf_init().
 */
static void
local_steps_buf_deinit(void)
{
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    if (local_steps != NULL) {
        nimble_platform_mem_free(local_steps);
        local_steps = NULL;
    }
#endif
    local_steps_len = 0;
    local_steps_num = 0;
    local_steps_nap = 0;
    local_steps_proc_counter = -1;
}

/** Append raw step bytes to local_steps[]; shared by RESULT and CONTINUE. */
static void
local_steps_append(const struct cs_steps_data *steps, uint8_t num_steps)
{
    uint16_t len = cs_step_data_total_len(steps, num_steps);

#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    /* Lazy alloc on first subevent of a procedure so idle/scanning
     * periods do not hold onto the step buffer. */
    if (local_steps == NULL && local_steps_buf_init() != 0) {
        return;
    }
#endif

    if (local_steps_len + len > STEP_DATA_BUF_LEN) {
        MODLOG_DFLT(ERROR, "[CS] Step buffer overflow (%d + %d > %d)\n",
                    local_steps_len, len, STEP_DATA_BUF_LEN);
        return;
    }

    memcpy(local_steps + local_steps_len, steps, len);
    local_steps_len += len;
    local_steps_num += num_steps;
}

/*
 * Local CS event callback.  Accumulates every subevent of the current
 * procedure into local_steps[]:
 *   RESULT (new proc)  -> reset buffer, then append
 *   RESULT (same proc) -> append (next subevent)
 *   CONTINUE           -> append (more steps for same subevent)
 *   PROC_COMPLETE      -> local_steps[] holds the complete procedure
 */
static void
ble_chan_sound_init_on_local_cs_event(struct ble_cs_event *event)
{
    switch (event->type) {
    case BLE_CS_EVENT_SUBEVENT_RESULT: {
        int32_t pc =
            BLE_SVC_RAS_RANGING_COUNTER(event->subev_result.procedure_counter);

        if (pc != local_steps_proc_counter) {
            local_steps_len = 0;
            local_steps_num = 0;
            local_steps_proc_counter = pc;
        }

        most_recent_local_ranging_counter = pc;
        local_steps_nap = event->subev_result.num_antenna_paths;

        local_steps_append(event->subev_result.steps,
                           event->subev_result.num_steps_reported);

        MODLOG_DFLT(DEBUG, "[CS] Subevent result: proc=%d +%d steps "
                    "(total=%d, %d bytes) nap=%d done=%d\n",
                    (int)pc, event->subev_result.num_steps_reported,
                    local_steps_num, local_steps_len,
                    local_steps_nap,
                    event->subev_result.procedure_done_status);
        break;
    }

    case BLE_CS_EVENT_SUBEVENT_RESULT_CONTINUE: {
        local_steps_append(event->subev_result_continue.steps,
                           event->subev_result_continue.num_steps_reported);

        MODLOG_DFLT(DEBUG, "[CS] Subevent continue: +%d steps "
                    "(total=%d, %d bytes) done=%d\n",
                    event->subev_result_continue.num_steps_reported,
                    local_steps_num, local_steps_len,
                    event->subev_result_continue.procedure_done_status);
        break;
    }

    case BLE_CS_EVENT_CS_PROCEDURE_COMPLETE:
        MODLOG_DFLT(INFO, "[CS] Procedure %d complete status=%d "
                    "(local: %d bytes, %d steps, nap=%d)\n",
                    (int)local_steps_proc_counter,
                    event->procedure_complete.status,
                    local_steps_len, local_steps_num, local_steps_nap);
        break;

    default:
        break;
    }
}

/*
 * RREQ library callbacks (peer ranging data).
 *
 * CS procedures are free-running: Procedure Enable is sent once with
 * max_procedure_count = 0 and the controllers repeat procedures until
 * the link drops.
 *
 * On-demand mode runs RD Ready -> GET_RD -> RD segments -> ACK_RD ->
 * RSP_CODE per procedure, then data_complete fires.  Real-time mode
 * receives segments unsolicited on the Real-time RD CCCD.  Errors on a
 * single procedure are logged and its data dropped; the stream delivers
 * the next procedure regardless.
 */

static void
ble_chan_sound_init_on_data_complete(uint16_t conn_handle, uint16_t ranging_counter,
                     const uint8_t *data, uint16_t data_len, int status)
{
    if (status == 0) {
        MODLOG_DFLT(INFO, "[RREQ] Ranging data complete conn=%d counter=%d "
                    "peer_data_len=%d\n",
                    conn_handle, ranging_counter, data_len);

        if (data_len >= BLE_RAS_RANGING_HEADER_LEN) {
            struct ranging_header *hdr = (struct ranging_header *)data;
            MODLOG_DFLT(INFO, "[RREQ] Peer ranging: counter=%d config_id=%d "
                        "tx_power=%d\n",
                        hdr->ranging_counter, hdr->config_id,
                        hdr->selected_tx_power);
        }

        MODLOG_DFLT(INFO, "============================================\n");
        MODLOG_DFLT(INFO, " CHANNEL SOUNDING DATA TRANSFER SUMMARY\n");
        MODLOG_DFLT(INFO, " Procedure counter : %d\n", ranging_counter);
        MODLOG_DFLT(INFO, " Peer (server) data: %d bytes received OK\n",
                    data_len);
        MODLOG_DFLT(INFO, " Local (initiator) : %d bytes, %d steps (nap=%d)\n",
                    local_steps_len, local_steps_num, local_steps_nap);
        MODLOG_DFLT(INFO, "============================================\n");

        /* Both local HCI step data (local_steps / local_steps_len) and
         * the peer's RAS ranging data (`data` / `data_len`) are now
         * available for this procedure.  Distance estimation is
         * intentionally out of scope for this example and is expected
         * to be provided by an application-supplied module; plug it in
         * here when integrating. */

        cs_procedures_done++;
        MODLOG_DFLT(INFO, "[CS] Procedures completed: %lu\n",
                    (unsigned long)cs_procedures_done);
    } else {
        /* Free-running stream: this procedure's data is lost, but the
         * next procedure arrives on its own - just log and move on. */
        MODLOG_DFLT(ERROR, "[RREQ] Ranging data failed conn=%d counter=%d "
                    "status=%d - dropped\n",
                    conn_handle, ranging_counter, status);
    }

    if (s_use_realtime) {
        /* RT mode: re-arm the RAP 4.4.1.1 first-segment timer for the
         * next free-running procedure, so a stalled stream is surfaced
         * as a timeout instead of hanging silently. */
        int rc = ble_svc_ras_rreq_rt_cs_started(conn_handle);
        if (rc != 0) {
            MODLOG_DFLT(ERROR, "[CS] rt_cs_started failed rc=%d conn=%d - "
                        "no hang-detection timer\n", rc, conn_handle);
        }
    }
}

/* Step 1-2: On RD Ready, send GET_RD for that counter. */
static void
ble_chan_sound_init_on_rd_ready(uint16_t conn_handle, uint16_t ranging_counter)
{
    MODLOG_DFLT(INFO, "[RREQ] RD Ready counter=%d local_counter=%" PRId32 "\n",
                ranging_counter, most_recent_local_ranging_counter);

    /* In real-time mode the RAS spec (2.7.1) says the server shall NOT send
     * RD Ready indications.  If one arrives anyway (misbehaving peer), silently
     * ignore it - calling get_ranging_data in RT mode would fail with
     * BLE_HS_EINVAL anyway. */
    if (s_use_realtime) {
        MODLOG_DFLT(WARN, "[RREQ] RD Ready ignored in RT mode conn=%d\n",
                    conn_handle);
        return;
    }

    int rc = ble_svc_ras_rreq_get_ranging_data(conn_handle, ranging_counter,
                                                NULL);
    if (rc == BLE_HS_EBUSY) {
        /* Another transfer is still in flight.  With free-running
         * procedures a new RD Ready can arrive before the previous
         * transfer's ACK completes; skip this counter - the next
         * procedure's RD Ready follows shortly. */
        MODLOG_DFLT(WARN, "[RREQ] RD Ready counter=%d skipped: transfer "
                    "already in progress (conn=%d)\n",
                    ranging_counter, conn_handle);
        return;
    }
    if (rc != 0) {
        /* This counter's data is lost, but the free-running stream keeps
         * producing procedures - the next RD Ready arrives on its own. */
        MODLOG_DFLT(ERROR, "[RREQ] get_ranging_data failed rc=%d conn=%d - "
                    "counter=%d dropped\n", rc, conn_handle, ranging_counter);
    }
}

static void
ble_chan_sound_init_on_rd_overwritten(uint16_t conn_handle, uint16_t ranging_counter)
{
    MODLOG_DFLT(INFO, "[RREQ] RD Overwritten counter=%d conn=%d\n",
                ranging_counter, conn_handle);
}

/* Subscribe & Features Callbacks */

static void
ble_chan_sound_init_on_subscribe_complete(uint16_t conn_handle, int status)
{
    if (status != 0) {
        MODLOG_DFLT(ERROR, "[RREQ] Subscribe failed status=%d conn=%d\n",
                    status, conn_handle);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    MODLOG_DFLT(INFO, "[RREQ] All RAS CCCDs subscribed (%s mode), enabling CS\n",
                s_use_realtime ? "RT" : "OD");

    cs_procedures_done = 0;

    struct ble_cs_initiator_procedure_start_params param;
    memset(&param, 0, sizeof(param));
    param.conn_handle = conn_handle;
    param.cb = ble_svc_ras_rreq_cs_event_handler;
    param.cb_arg = NULL;
    int rc = ble_cs_initiator_procedure_start(&param);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "[CS] initiator_procedure_start failed rc=%d - terminating conn=%d\n",
                    rc, conn_handle);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    /* RAP 4.4.1.1: in real-time mode, arm 5s "no data" timer for the first procedure */
    if (s_use_realtime) {
        int rt_rc = ble_svc_ras_rreq_rt_cs_started(conn_handle);
        if (rt_rc != 0) {
            MODLOG_DFLT(ERROR, "[CS] rt_cs_started failed rc=%d conn=%d - "
                        "no hang-detection timer\n", rt_rc, conn_handle);
        }
    }

    MODLOG_DFLT(INFO, "[CS] Free-running procedures enabled (single Procedure "
                "Enable, max_procedure_count=0)\n");
}

static void
ble_chan_sound_init_on_features_read(uint16_t conn_handle, uint32_t features, int status)
{
    if (status != 0) {
        MODLOG_DFLT(ERROR, "[RREQ] Features read failed status=%d conn=%d\n",
                    status, conn_handle);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    /* Real-time ranging is pushed as the data is produced and needs no GET_RD
     * round trip, but it is optional and has to be advertised in the features.
     * On-demand is supported by every RAS Server, so it is the fallback.
     */
    s_use_realtime = CS_INIT_PREFER_REALTIME &&
                     !!(features & BLE_SVC_RAS_FEAT_REALTIME_RD_BIT);
    int rc = ble_svc_ras_rreq_set_ranging_mode(conn_handle, s_use_realtime);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "[RREQ] set_ranging_mode failed rc=%d conn=%d\n",
                    rc, conn_handle);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    ble_svc_ras_rreq_set_indicate(conn_handle, CS_INIT_USE_INDICATE);

    rc = ble_svc_ras_rreq_subscribe(conn_handle, ble_chan_sound_init_on_subscribe_complete);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "[RREQ] subscribe start failed rc=%d\n", rc);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }
}

/* Discovery is done, so latch the handles and bring up the RREQ client. */
static void
ble_chan_sound_init_on_disc_complete(const struct peer *peer, int status, void *arg)
{
    int rc;
    uint16_t conn_handle = peer->conn_handle;

    if (status != 0) {
        MODLOG_DFLT(ERROR, "Service discovery failed status=%d conn=%d\n",
                    status, conn_handle);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    MODLOG_DFLT(INFO, "Service discovery complete conn=%d\n", conn_handle);

    /* RAP Table 4.2 (M): Find Included Services sub-procedure.
     * Run against the RAS service handle range for spec compliance.
     * Results are intentionally discarded (NULL callback) - only the ATT
     * request itself is required.  Log a warning on failure but continue:
     * the included-services result is unused so a GATT proc-slot exhaustion
     * here is not fatal to functionality. */
    {
        const struct peer_svc *ras_svc = peer_svc_find_uuid(
            peer, BLE_UUID16_DECLARE(BLE_SVC_RAS_RANGING_SERVICE_VAL));
        if (ras_svc) {
            rc = ble_gattc_find_inc_svcs(conn_handle,
                                         ras_svc->svc.start_handle,
                                         ras_svc->svc.end_handle,
                                         NULL, NULL);
            if (rc != 0) {
                MODLOG_DFLT(WARN, "Find Included Services (RAP Table 4.2 M) "
                            "failed rc=%d conn=%d - continuing\n",
                            rc, conn_handle);
            } else {
                MODLOG_DFLT(DEBUG, "Find Included Services for RAS rc=%d\n", rc);
            }
        } else {
            MODLOG_DFLT(ERROR, "RAS service not found in peer conn=%d - "
                        "terminating\n", conn_handle);
            ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
            return;
        }
    }

    /* Build the RAS handle set from discovered peer data */
    struct ble_svc_ras_handles handles;
    memset(&handles, 0, sizeof(handles));

    const struct peer_chr *chr;
    const struct peer_dsc *dsc;

#define FILL_CHR_HANDLE(field, chr_val)                                      \
    do {                                                                     \
        chr = peer_chr_find_uuid(peer,                                       \
            BLE_UUID16_DECLARE(BLE_SVC_RAS_RANGING_SERVICE_VAL),             \
            BLE_UUID16_DECLARE(chr_val));                                     \
        if (chr) {                                                           \
            handles.field = chr->chr.val_handle;                             \
        }                                                                    \
    } while (0)

#define FILL_CCC_HANDLE(field, chr_val)                                      \
    do {                                                                     \
        dsc = peer_dsc_find_uuid(peer,                                       \
            BLE_UUID16_DECLARE(BLE_SVC_RAS_RANGING_SERVICE_VAL),             \
            BLE_UUID16_DECLARE(chr_val),                                      \
            BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16));               \
        if (dsc) {                                                           \
            handles.field = dsc->dsc.handle;                                 \
        }                                                                    \
    } while (0)

    FILL_CHR_HANDLE(features,       BLE_SVC_RAS_CHR_UUID_FEATURES_VAL);
    FILL_CHR_HANDLE(od_rd,          BLE_SVC_RAS_CHR_UUID_ONDEMAND_RD_VAL);
    FILL_CHR_HANDLE(rt_rd,          BLE_SVC_RAS_CHR_UUID_REALTIME_RD_VAL);
    FILL_CHR_HANDLE(cp,             BLE_SVC_RAS_CHR_UUID_CP_VAL);
    FILL_CHR_HANDLE(rd_ready,       BLE_SVC_RAS_CHR_UUID_RD_READY_VAL);
    FILL_CHR_HANDLE(rd_overwritten, BLE_SVC_RAS_CHR_UUID_RD_OVERWRITTEN_VAL);

    FILL_CCC_HANDLE(od_rd_ccc,          BLE_SVC_RAS_CHR_UUID_ONDEMAND_RD_VAL);
    FILL_CCC_HANDLE(rt_rd_ccc,          BLE_SVC_RAS_CHR_UUID_REALTIME_RD_VAL);
    FILL_CCC_HANDLE(cp_ccc,             BLE_SVC_RAS_CHR_UUID_CP_VAL);
    FILL_CCC_HANDLE(rd_ready_ccc,       BLE_SVC_RAS_CHR_UUID_RD_READY_VAL);
    FILL_CCC_HANDLE(rd_overwritten_ccc, BLE_SVC_RAS_CHR_UUID_RD_OVERWRITTEN_VAL);

#undef FILL_CHR_HANDLE
#undef FILL_CCC_HANDLE

    if (!handles.cp || !handles.rd_ready || !handles.features) {
        MODLOG_DFLT(ERROR, "Peer missing mandatory RAS characteristics\n");
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    /* Allocate RREQ state and register callbacks */
    rc = ble_svc_ras_rreq_alloc(conn_handle, &handles);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "RREQ alloc failed rc=%d\n", rc);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    ble_svc_ras_rreq_set_rd_ready_cb(conn_handle, ble_chan_sound_init_on_rd_ready);
    ble_svc_ras_rreq_set_rd_overwritten_cb(conn_handle, ble_chan_sound_init_on_rd_overwritten);
    ble_svc_ras_rreq_set_data_complete_cb(conn_handle, ble_chan_sound_init_on_data_complete);
    ble_svc_ras_rreq_set_local_cs_cb(conn_handle, ble_chan_sound_init_on_local_cs_event);

    /* Read Features first, then subscribe in the callback */
    rc = ble_svc_ras_rreq_read_features(conn_handle, ble_chan_sound_init_on_features_read);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Features read start failed rc=%d\n", rc);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }
}

/* Scanning & Connection */
static void
ble_chan_sound_init_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params = {0};
    int rc;

    /* RAP 6.1 SHALL: use a random device address */
    rc = ble_hs_id_infer_auto(1, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }
    disc_params.filter_duplicates = 1;
    disc_params.passive = 1;
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      ble_chan_sound_init_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error initiating GAP discovery; rc=%d\n", rc);
    }
}

static void
ble_chan_sound_init_connect_if_interesting(void *disc)
{
    uint8_t own_addr_type;
    int rc;
    ble_addr_t *addr;
    struct ble_hs_adv_fields fields;
    const uint8_t *adv_data = NULL;
    int adv_data_len = 0;
    int found = 0;

#if CONFIG_EXAMPLE_EXTENDED_ADV
    struct ble_gap_ext_disc_desc *ext = (struct ble_gap_ext_disc_desc *)disc;
    addr = &ext->addr;
    adv_data = ext->data;
    adv_data_len = ext->length_data;
#else
    struct ble_gap_disc_desc *d = (struct ble_gap_disc_desc *)disc;
    addr = &d->addr;
    adv_data = d->data;
    adv_data_len = d->length_data;
#endif

    /* Connect only to a peer advertising the Ranging Service */
    rc = ble_hs_adv_parse_fields(&fields, adv_data, adv_data_len);
    if (rc != 0) {
        return;
    }
    for (int i = 0; i < fields.num_uuids16; i++) {
        if (ble_uuid_u16(&fields.uuids16[i].u) == BLE_SVC_RAS_RANGING_SERVICE_VAL) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return;
    }

#if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
    rc = ble_gap_disc_cancel();
    if (rc != 0) {
        MODLOG_DFLT(DEBUG, "Failed to cancel scan; rc=%d\n", rc);
        return;
    }
#endif

    /* RAP 6.1 SHALL: use a random device address */
    rc = ble_hs_id_infer_auto(1, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        /* The scan was cancelled above, so it has to be restarted here or the
         * device ends up neither scanning nor connected. */
#if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
        ble_chan_sound_init_scan();
#endif
        return;
    }
    rc = ble_gap_connect(own_addr_type, addr, 30000, NULL,
                         ble_chan_sound_init_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Connection failed rc=%d\n", rc);
#if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
        ble_chan_sound_init_scan();
#endif
        return;
    }
}

static int
exchange_func(uint16_t conn_handle, const struct ble_gatt_error *error,
              uint16_t mtu, void *arg)
{
    MODLOG_DFLT(INFO, "MTU exchange complete; status=%d conn_handle=%d mtu=%d\n",
                error->status, conn_handle, mtu);
    return 0;
}

/* GAP Event Handler */
static int
ble_chan_sound_init_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    struct ble_hs_adv_fields fields;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
        rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                     event->disc.length_data);
        if (rc != 0) {
            return 0;
        }
        print_adv_fields(&fields);
        ble_chan_sound_init_connect_if_interesting(&event->disc);
        return 0;

    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Connection established conn=%d\n",
                        event->connect.conn_handle);
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);

            rc = peer_add(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to add peer; rc=%d\n", rc);
                return 0;
            }

            most_recent_local_ranging_counter = -1;
            local_steps_proc_counter = -1;
            disc_initiated = false;
            local_steps_len = 0;
            local_steps_num = 0;
            cs_procedures_done = 0;

            /* Connections from legacy ADV_IND start on the 1M PHY. Request
             * an upgrade to 2M explicitly for higher throughput on the RAS
             * real-time stream.  Non-fatal: if the peer declines we stay on
             * 1M rather than drop the link. */
            rc = ble_gap_set_prefered_le_phy(event->connect.conn_handle,
                                             BLE_GAP_LE_PHY_2M_MASK,
                                             BLE_GAP_LE_PHY_2M_MASK, 0);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "2M PHY request failed rc=%d\n", rc);
            }

            rc = ble_gap_security_initiate(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Security initiation failed rc=%d\n", rc);
                return ble_gap_terminate(event->connect.conn_handle,
                                         BLE_ERR_REM_USER_CONN_TERM);
            }
        } else {
            MODLOG_DFLT(ERROR, "Connection failed; status=%d\n",
                        event->connect.status);
            ble_chan_sound_init_scan();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d conn=%d\n",
                    event->disconnect.reason,
                    event->disconnect.conn.conn_handle);
        ble_svc_ras_rreq_free(event->disconnect.conn.conn_handle);
        peer_delete(event->disconnect.conn.conn_handle);
        /* Release the per-procedure local_steps buffer so idle/scanning
         * periods do not hold CS-specific RAM.  The next procedure's
         * first subevent append re-allocates on demand. */
        local_steps_buf_deinit();
        ble_chan_sound_init_scan();
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        MODLOG_DFLT(INFO, "discovery complete; reason=%d\n",
                    event->disc_complete.reason);
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        MODLOG_DFLT(INFO, "PHY update complete; status=%d conn=%d tx=%d rx=%d\n",
                    event->phy_updated.status,
                    event->phy_updated.conn_handle,
                    event->phy_updated.tx_phy,
                    event->phy_updated.rx_phy);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        MODLOG_DFLT(INFO, "encryption change event; status=%d\n",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);

        /* Channel Sounding requires an encrypted LE Secure Connections
         * link (RAP 6.2).  Without it RAS reads and CS HCI commands
         * are rejected, so drop the connection and let the peer retry
         * rather than continue with a half-configured link. */
        if (event->enc_change.status != 0 || !desc.sec_state.encrypted ||
            !desc.sec_state.sc) {
            MODLOG_DFLT(ERROR, "encryption failed/missing or not LE Secure "
                        "Connections (status=%d enc=%d sc=%d) "
                        "- terminating conn=%d\n",
                        event->enc_change.status,
                        desc.sec_state.encrypted,
                        desc.sec_state.sc,
                        event->enc_change.conn_handle);
            return ble_gap_terminate(event->enc_change.conn_handle,
                                     BLE_ERR_AUTH_FAIL);
        }

        if (disc_initiated) {
            return 0;
        }
        disc_initiated = true;

        ble_gattc_exchange_mtu(event->enc_change.conn_handle,
                               exchange_func, NULL);
        rc = peer_disc_all(event->enc_change.conn_handle,
                           ble_chan_sound_init_on_disc_complete, NULL);
        if (rc != 0) {
            MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
            return 0;
        }
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        MODLOG_DFLT(DEBUG, "received %s; conn_handle=%d attr_handle=%d "
                    "attr_len=%d\n",
                    event->notify_rx.indication ? "indication" : "notification",
                    event->notify_rx.conn_handle,
                    event->notify_rx.attr_handle,
                    event->notify_rx.om ? OS_MBUF_PKTLEN(event->notify_rx.om) : 0);

        ble_svc_ras_rreq_on_notify(event->notify_rx.conn_handle,
                                    event->notify_rx.attr_handle,
                                    event->notify_rx.om);
        return 0;

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(INFO, "mtu update; conn_handle=%d cid=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        assert(rc == 0);
        ble_store_util_delete_peer(&desc.peer_id_addr);
        return BLE_GAP_REPEAT_PAIRING_RETRY;

#if CONFIG_EXAMPLE_EXTENDED_ADV
    case BLE_GAP_EVENT_EXT_DISC:
        ext_print_adv_report(&event->ext_disc);
        ble_chan_sound_init_connect_if_interesting(&event->ext_disc);
        return 0;
#endif

    default:
        return 0;
    }
}

static void
ble_chan_sound_init_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
ble_chan_sound_init_on_sync(void)
{
    int rc;

    /* Feature bit 47 is LE Channel Sounding Host Support.  The controller
     * refuses every CS command until the host has set it, so this has to happen
     * before anything else CS related.
     */
    rc = ble_gap_set_host_feat(47, 0x01);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Failed to enable Channel Sounding host feature; rc=%d\n", rc);
    }

    /* prefer_random=0 is correct here: ble_hs_id_infer_auto(1) falls back
     * to BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT when no random identity exists,
     * which still uses an RPA on the air (satisfies RAP 6.1). */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    ble_chan_sound_init_scan();
}

void ble_chan_sound_init_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void
app_main(void)
{
    int rc;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* local_steps is allocated lazily on the first subevent of a
     * procedure (see local_steps_append()) and freed on disconnect, so
     * no up-front reservation is needed here.  In static mode this is
     * a .bss array and no allocation ever happens. */

    ble_hs_cfg.reset_cb = ble_chan_sound_init_on_reset;
    ble_hs_cfg.sync_cb = ble_chan_sound_init_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* RAP 6.2 SHALL: LE Secure Connections pairing */
    ble_hs_cfg.sm_sc = 1;

    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);

    rc = ble_svc_gap_device_name_set("NIMBLE_CS_INITIATOR");
    assert(rc == 0);

    ble_store_config_init();
    nimble_port_freertos_init(ble_chan_sound_init_host_task);
}
