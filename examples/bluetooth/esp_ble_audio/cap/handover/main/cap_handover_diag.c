/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stddef.h>

#include "esp_ble_iso_common_api.h"

#include "cap_handover.h"

/* flush_timeout holds FT x ISO_Interval, so divide FT back out. sdu_interval
 * only comes with HCI_LE_CIS_Established_V2; a v1 event leaves it UNKNOWN.
 */
static void diag_log_cis_dir(const char *phase, size_t idx, const char *dir,
                             uint16_t iso_interval,
                             const struct bt_iso_unicast_tx_info *info)
{
    ESP_LOGI(TAG, "[%s][%zu] %s {ft %u bn %u latency %lu us sdu_interval %s}",
             phase, idx, dir,
             iso_interval != 0 ? (unsigned)(info->flush_timeout / iso_interval) : 0U,
             info->bn, (unsigned long)info->latency,
             info->sdu_interval == BT_ISO_SDU_INTERVAL_UNKNOWN ? "unknown (v1 event)"
                                                               : "see qos");
}

static void diag_sample_stream(const char *phase, size_t idx,
                               esp_ble_audio_cap_stream_t *stream, bool has_sent)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_ble_iso_tx_info_t tx_info = {0};
    esp_ble_iso_info_t chan_info = {0};
    esp_ble_audio_bap_ep_t *ep = stream->bap_stream.ep;
    esp_err_t err;

    if (ep == NULL) {
        ESP_LOGI(TAG, "[%s][%zu] no endpoint", phase, idx);
        return;
    }

    if (esp_ble_audio_bap_ep_get_info(ep, &ep_info) != ESP_OK) {
        ESP_LOGW(TAG, "[%s][%zu] endpoint info unavailable", phase, idx);
        return;
    }

    /* HCI answers Command Disallowed until the stream has sent an SDU, and the
     * adapter logs that as an HCI error - so do not ask a transport that just
     * came up. Its timestamp shows up in the next pre-handover sample.
     */
    if (!has_sent) {
        ESP_LOGI(TAG, "[%s][%zu] state %d (timestamp not read: transport just started)",
                 phase, idx, ep_info.state);
    } else if ((err = esp_ble_audio_cap_stream_get_tx_sync(stream, &tx_info)) != ESP_OK) {
        ESP_LOGW(TAG, "[%s][%zu] state %d tx_sync failed, err %d",
                 phase, idx, ep_info.state, err);
    } else {
        ESP_LOGI(TAG, "[%s][%zu] state %d ts %lu us offset %lu us seq %u",
                 phase, idx, ep_info.state,
                 (unsigned long)tx_info.ts, (unsigned long)tx_info.offset,
                 tx_info.seq_num);
    }

    if (ep_info.iso_chan == NULL ||
            esp_ble_iso_chan_get_info(ep_info.iso_chan, &chan_info) != ESP_OK) {
        ESP_LOGI(TAG, "[%s][%zu] no ISO channel info", phase, idx);
        return;
    }

    /* iso_interval is in 1.25 ms units; print us like every other duration. The
     * channel kind shows in the field names, so it is not printed separately.
     */
    if (chan_info.type == BT_ISO_CHAN_TYPE_BROADCASTER) {
        ESP_LOGI(TAG, "[%s][%zu] iso_interval %lu us big_sync_delay %lu us",
                 phase, idx,
                 (unsigned long)chan_info.iso_interval * 1250UL,
                 (unsigned long)chan_info.broadcaster.sync_delay);
        ESP_LOGI(TAG, "[%s][%zu] bis {pto %lu bn %u latency %lu us}",
                 phase, idx,
                 (unsigned long)chan_info.broadcaster.pto,
                 chan_info.broadcaster.bn,
                 (unsigned long)chan_info.broadcaster.latency);

        /* No qos_pref: it comes from a peer ASE's Codec Configured, and a
         * broadcast source has no peer. Printing four zeros would mislead.
         */
        return;
    }

    ESP_LOGI(TAG, "[%s][%zu] iso_interval %lu us cig_sync_delay %lu us cis_sync_delay %lu us",
             phase, idx,
             (unsigned long)chan_info.iso_interval * 1250UL,
             (unsigned long)chan_info.unicast.cig_sync_delay,
             (unsigned long)chan_info.unicast.cis_sync_delay);

    diag_log_cis_dir(phase, idx, "c2p", chan_info.iso_interval, &chan_info.unicast.central);
    diag_log_cis_dir(phase, idx, "p2c", chan_info.iso_interval, &chan_info.unicast.peripheral);

    if (ep_info.qos_pref != NULL) {
        ESP_LOGI(TAG, "[%s][%zu] pd_pref [%lu, %lu] us pd_range [%lu, %lu] us",
                 phase, idx,
                 (unsigned long)ep_info.qos_pref->pref_pd_min,
                 (unsigned long)ep_info.qos_pref->pref_pd_max,
                 (unsigned long)ep_info.qos_pref->pd_min,
                 (unsigned long)ep_info.qos_pref->pd_max);
    }
}

void cap_handover_diag_sample(const char *phase, bool has_sent)
{
    for (size_t i = 0; i < peer.sink_ep_count && i < ARRAY_SIZE(peer.sink_streams); i++) {
        diag_sample_stream(phase, i, &peer.sink_streams[i], has_sent);
    }
}
