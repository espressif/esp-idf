/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "sdkconfig.h"

#if CONFIG_BLE_LOG_ENABLED
#include "ble_log.h"
#endif

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/util/util.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define TAG "EXAMPLE_INIT"

static SemaphoreHandle_t example_audio_sem;

void ble_store_config_init(void);

/* Names follow BT SIG Assigned Numbers:
 *   https://bitbucket.org/bluetooth-SIG/public/src/main/assigned_numbers/uuids/
 */
static const char *example_audio_uuid16_name(const ble_uuid_t *uuid)
{
    if (uuid->type != BLE_UUID_TYPE_16) {
        return NULL;
    }

    switch (ble_uuid_u16(uuid)) {
    /* GAP / GATT */
    case 0x1800: return "GAP";
    case 0x1801: return "GATT";
    case 0x2a00: return "Device Name";
    case 0x2a01: return "Appearance";
    case 0x2a05: return "Service Changed";
    case 0x2b29: return "Client Sup Features";
    case 0x2b3a: return "Server Sup Features";

    /* LE Audio services */
    case 0x1843: return "AICS";
    case 0x1844: return "VCS";
    case 0x1845: return "VOCS";
    case 0x1846: return "CSIS";
    case 0x1848: return "MCS";
    case 0x1849: return "GMCS";
    case 0x184b: return "TBS";
    case 0x184c: return "GTBS";
    case 0x184d: return "MICS";
    case 0x184e: return "ASCS";
    case 0x184f: return "BASS";
    case 0x1850: return "PACS";
    case 0x1851: return "Basic Audio Ann";
    case 0x1852: return "Broadcast Audio Ann";
    case 0x1853: return "CAS";
    case 0x1854: return "HAS";
    case 0x1855: return "TMAS";
    case 0x1856: return "Public Broadcast Ann";
    case 0x1858: return "GMAS";

    /* Common */
    case 0x2b51: return "TMAP Role";
    case 0x2bba: return "CCID";

    /* AICS chars */
    case 0x2b77: return "AICS State";
    case 0x2b78: return "AICS Gain Settings";
    case 0x2b79: return "AICS Input Type";
    case 0x2b7a: return "AICS Input Status";
    case 0x2b7b: return "AICS Control";
    case 0x2b7c: return "AICS Description";

    /* VCS chars */
    case 0x2b7d: return "VCS State";
    case 0x2b7e: return "VCS Control";
    case 0x2b7f: return "VCS Flags";

    /* VOCS chars */
    case 0x2b80: return "VOCS State";
    case 0x2b81: return "VOCS Location";
    case 0x2b82: return "VOCS Control";
    case 0x2b83: return "VOCS Description";

    /* CSIS chars */
    case 0x2b84: return "CSIS SIRK";
    case 0x2b85: return "CSIS Set Size";
    case 0x2b86: return "CSIS Set Lock";
    case 0x2b87: return "CSIS Rank";

    /* MCS chars */
    case 0x2b93: return "MCS Player Name";
    case 0x2b94: return "MCS Icon Obj ID";
    case 0x2b95: return "MCS Icon URL";
    case 0x2b96: return "MCS Track Changed";
    case 0x2b97: return "MCS Track Title";
    case 0x2b98: return "MCS Track Duration";
    case 0x2b99: return "MCS Track Position";
    case 0x2b9a: return "MCS Playback Speed";
    case 0x2b9b: return "MCS Seeking Speed";
    case 0x2b9c: return "MCS Track Seg Obj ID";
    case 0x2b9d: return "MCS Cur Track Obj ID";
    case 0x2b9e: return "MCS Next Track Obj ID";
    case 0x2b9f: return "MCS Parent Grp Obj ID";
    case 0x2ba0: return "MCS Cur Grp Obj ID";
    case 0x2ba1: return "MCS Playing Order";
    case 0x2ba2: return "MCS Playing Orders";
    case 0x2ba3: return "MCS Media State";
    case 0x2ba4: return "MCS Media CP";
    case 0x2ba5: return "MCS Media CP Opcodes";
    case 0x2ba6: return "MCS Search Res Obj ID";
    case 0x2ba7: return "MCS Search CP";

    /* TBS chars */
    case 0x2bb3: return "TBS Provider Name";
    case 0x2bb4: return "TBS UCI";
    case 0x2bb5: return "TBS Technology";
    case 0x2bb6: return "TBS URI List";
    case 0x2bb7: return "TBS Signal Strength";
    case 0x2bb8: return "TBS Signal Interval";
    case 0x2bb9: return "TBS List Cur Calls";
    case 0x2bbb: return "TBS Status Flags";
    case 0x2bbc: return "TBS Incoming URI";
    case 0x2bbd: return "TBS Call State";
    case 0x2bbe: return "TBS Call CP";
    case 0x2bbf: return "TBS Optional Opcodes";
    case 0x2bc0: return "TBS Term Reason";
    case 0x2bc1: return "TBS Incoming Call";
    case 0x2bc2: return "TBS Friendly Name";

    /* MICS / ASCS / BASS chars */
    case 0x2bc3: return "MICS Mute";
    case 0x2bc4: return "ASCS ASE Snk";
    case 0x2bc5: return "ASCS ASE Src";
    case 0x2bc6: return "ASCS ASE CP";
    case 0x2bc7: return "BASS CP";
    case 0x2bc8: return "BASS Recv State";

    /* PACS chars */
    case 0x2bc9: return "PACS Snk";
    case 0x2bca: return "PACS Snk Loc";
    case 0x2bcb: return "PACS Src";
    case 0x2bcc: return "PACS Src Loc";
    case 0x2bcd: return "PACS Avail Ctx";
    case 0x2bce: return "PACS Sup Ctx";

    default: return NULL;
    }
}

static void example_audio_gatt_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];
    const char *name;

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        name = example_audio_uuid16_name(ctxt->svc.svc_def->uuid);
        ESP_LOGI(TAG, "svc %-20s (%s), handle %u",
                 name ? name : "?",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        name = example_audio_uuid16_name(ctxt->chr.chr_def->uuid);
        ESP_LOGI(TAG, "chr %-20s (%s), handles %u/%u",
                 name ? name : "?",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle, ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        name = example_audio_uuid16_name(ctxt->dsc.dsc_def->uuid);
        ESP_LOGI(TAG, "dsc %-20s (%s), handle %u",
                 name ? name : "?",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

static void example_audio_on_reset(int reason)
{
    ESP_LOGI(TAG, "Resetting state; reason=%d", reason);
}

static void example_audio_on_sync(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    xSemaphoreGive(example_audio_sem);
}

static void example_audio_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");

    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

esp_err_t bluetooth_init(void)
{
    esp_err_t ret;

    /* When controller log is disabled (or running in v1 mode), the
     * automatic ble_log_init() inside esp_bt_controller_init() is skipped.
     * Init manually here so HOST/ISO/AUDIO compressed logs aren't dropped
     * during NimBLE bring-up. The function is idempotent.
     *
     * TODO: Remove this block (and the ble_log_flush() below) once
     *       ble_log_init() is decoupled from the controller log path and
     *       owns its own initialization independently. */
#if CONFIG_BLE_LOG_ENABLED && \
    !(CONFIG_BT_LE_CONTROLLER_LOG_ENABLED && CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2)
    if (!ble_log_init()) {
        ESP_LOGE(TAG, "Failed to init ble_log");
        return ESP_FAIL;
    }
#endif

    example_audio_sem = xSemaphoreCreateBinary();
    if (example_audio_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create audio semaphore");
        ret = ESP_FAIL;
        goto err_log;
    }

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init nimble %d ", ret);
        goto err_sem;
    }

    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.gatts_register_cb = example_audio_gatt_register_cb;
    ble_hs_cfg.reset_cb = example_audio_on_reset;
    ble_hs_cfg.sync_cb = example_audio_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_sc = 1;
    ble_hs_cfg.sm_mitm = 0;
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(example_audio_host_task);

    xSemaphoreTake(example_audio_sem, portMAX_DELAY);

    /* Drain startup-phase log buffers and emit a FLUSH boundary so the
     * parser resets stats here — the example's runtime logs start clean.
     *
     * TODO: Remove together with the manual ble_log_init() above once
     *       ble_log_init() is decoupled from the controller log path. */
#if CONFIG_BLE_LOG_ENABLED
    ble_log_flush();
#endif

    return ESP_OK;

err_sem:
    vSemaphoreDelete(example_audio_sem);
    example_audio_sem = NULL;
err_log:
    /* Mirror the manual ble_log_init() block above — same Kconfig gate so
     * we only deinit what we initialized; the controller-owned path is
     * left alone. */
#if CONFIG_BLE_LOG_ENABLED && \
    !(CONFIG_BT_LE_CONTROLLER_LOG_ENABLED && CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2)
    ble_log_deinit();
#endif
    return ret;
}
