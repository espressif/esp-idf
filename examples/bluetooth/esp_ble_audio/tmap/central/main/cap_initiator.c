/*
 * SPDX-FileCopyrightText: 2022-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "esp_ble_audio_bap_lc3_preset_defs.h"

#include "tmap_central.h"

static esp_ble_audio_cap_stream_t unicast_sink_streams[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT];
static esp_ble_audio_cap_stream_t unicast_source_streams[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT];
static esp_ble_audio_bap_ep_t *unicast_sink_eps[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT];
static esp_ble_audio_bap_ep_t *unicast_source_eps[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT];

static esp_ble_audio_cap_unicast_group_t *unicast_group;

static example_audio_tx_scheduler_t tx_scheduler;
static uint16_t tx_seq_num;
static uint8_t *iso_data;

static void unicast_audio_tx(void);

ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_2_1_DEFINE(unicast_preset_48_2_1,
                                                   ESP_BLE_AUDIO_LOCATION_FRONT_LEFT,
                                                   ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);

static const char *dir_str(esp_ble_audio_dir_t dir)
{
    return dir == ESP_BLE_AUDIO_DIR_SINK ? "SNK" : "SRC";
}

static bool stream_is_sink(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_streams); i++) {
        if (&unicast_sink_streams[i].bap_stream == stream) {
            return true;
        }
    }
    return false;
}

static const char *stream_dir_str(const esp_ble_audio_bap_stream_t *stream)
{
    return stream_is_sink(stream) ? "SNK" : "SRC";
}

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_streams); i++) {
        if (&unicast_sink_streams[i].bap_stream == stream) {
            return (int)i;
        }
    }
    for (size_t i = 0; i < ARRAY_SIZE(unicast_source_streams); i++) {
        if (&unicast_source_streams[i].bap_stream == stream) {
            return (int)i;
        }
    }
    return -1;
}

static void unicast_stream_configured_cb(esp_ble_audio_bap_stream_t *stream,
                                         const esp_ble_audio_bap_qos_cfg_pref_t *pref)
{
    ESP_LOGI(TAG, "[%s #%d] Stream configured, QoS preference:",
             stream_dir_str(stream), stream_index(stream));
    example_print_qos_pref(TAG, pref);
}

static void unicast_stream_qos_set_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] QoS set",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_enabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream enabled",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Stream started",
             stream_dir_str(stream), stream_index(stream));

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to get ep info, err %d", err);
        return;
    }

    /* Note:
     * For a BAP Unicast Client, this is considered outgoing audio (TX).
     */
    if (ep_info.dir == ESP_BLE_AUDIO_DIR_SINK) {
        if (stream->qos == NULL || stream->qos->sdu == 0) {
            ESP_LOGE(TAG, "Invalid stream qos");
            return;
        }

        if (iso_data == NULL) {
            iso_data = calloc(1, stream->qos->sdu);
            if (iso_data == NULL) {
                ESP_LOGE(TAG, "Failed to alloc TX buffer, SDU %u", stream->qos->sdu);
                return;
            }
        }

        assert(stream->qos->interval);

        ESP_LOGI(TAG, "Streaming, interval %u, length %u", stream->qos->interval, stream->qos->sdu);

        tx_seq_num = 0;
        example_audio_tx_scheduler_reset(&tx_scheduler);

        /* Note: esp timer is not accurate enough */
        err = example_audio_tx_scheduler_start(&tx_scheduler, stream->qos->interval);
        if (err) {
            ESP_LOGE(TAG, "Failed to start tx scheduler, err %d", err);
            return;
        }

        unicast_audio_tx();
    }
}

static void unicast_stream_metadata_updated_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Metadata updated",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_disabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream disabled",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Stream stopped, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to get ep info, err %d", err);
        return;
    }

    if (ep_info.dir == ESP_BLE_AUDIO_DIR_SINK) {
        err = example_audio_tx_scheduler_stop(&tx_scheduler);
        if (err) {
            ESP_LOGE(TAG, "Failed to stop tx scheduler, err %d", err);
        }

        if (iso_data != NULL) {
            free(iso_data);
            iso_data = NULL;
        }
    }
}

static void unicast_stream_released_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream released",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    char name[24];

    snprintf(name, sizeof(name), "%s #%d",
             stream_dir_str(stream), stream_index(stream));
    example_audio_tx_scheduler_on_sent(&tx_scheduler, user_data, TAG, name);
}

static void unicast_stream_connected_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream connected",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] ISO disconnected, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to get ep info, err %d", err);
        return;
    }

    if (ep_info.dir == ESP_BLE_AUDIO_DIR_SINK) {
        err = example_audio_tx_scheduler_stop(&tx_scheduler);
        if (err) {
            ESP_LOGE(TAG, "Failed to stop tx scheduler, err %d", err);
        }

        if (iso_data != NULL) {
            free(iso_data);
            iso_data = NULL;
        }
    }
}

static esp_ble_audio_bap_stream_ops_t unicast_stream_ops = {
    .configured       = unicast_stream_configured_cb,
    .qos_set          = unicast_stream_qos_set_cb,
    .enabled          = unicast_stream_enabled_cb,
    .started          = unicast_stream_started_cb,
    .metadata_updated = unicast_stream_metadata_updated_cb,
    .disabled         = unicast_stream_disabled_cb,
    .stopped          = unicast_stream_stopped_cb,
    .released         = unicast_stream_released_cb,
    .sent             = unicast_stream_sent_cb,
    .connected        = unicast_stream_connected_cb,
    .disconnected     = unicast_stream_disconnected_cb,
};

static void unicast_discovery_complete_cb(esp_ble_conn_t *conn, int err,
                                          const esp_ble_audio_csip_set_coordinator_set_member_t *member,
                                          const esp_ble_audio_csip_set_coordinator_csis_inst_t *csis_inst)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast discovery completed, err %d", err);
        return;
    }

    if (default_conn_handle_get() != conn->handle) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return;
    }

    if (IS_ENABLED(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)) {
        if (csis_inst == NULL) {
            ESP_LOGW(TAG, "Failed to discover CAS CSIS");
            return;
        }

        ESP_LOGI(TAG, "Found CAS with CSIS");
    } else {
        ESP_LOGI(TAG, "Found CAS");
    }

    err = esp_ble_audio_bap_unicast_client_discover(conn->handle, ESP_BLE_AUDIO_DIR_SINK);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover sink, err %d", err);
    }
}

static void unicast_start_complete_cb(int err, esp_ble_conn_t *conn)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast start completed, err %d", err);
    } else {
        ESP_LOGI(TAG, "Unicast start completed");
    }
}

static void unicast_update_complete_cb(int err, esp_ble_conn_t *conn)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast update completed, err %d", err);
    } else {
        ESP_LOGI(TAG, "Unicast update completed");
    }
}

static void unicast_stop_complete_cb(int err, esp_ble_conn_t *conn)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast stop completed, err %d", err);
    } else {
        ESP_LOGI(TAG, "Unicast stop completed");
    }
}

static esp_ble_audio_cap_initiator_cb_t cap_cb = {
    .unicast_discovery_complete = unicast_discovery_complete_cb,
    .unicast_start_complete     = unicast_start_complete_cb,
    .unicast_update_complete    = unicast_update_complete_cb,
    .unicast_stop_complete      = unicast_stop_complete_cb,
};

static int unicast_group_create(void)
{
    esp_ble_audio_cap_unicast_group_stream_param_t group_stream_params = {0};
    esp_ble_audio_cap_unicast_group_stream_pair_param_t pair_params = {0};
    esp_ble_audio_cap_unicast_group_param_t group_param = {0};
    int err;

    if (unicast_group != NULL) {
        return 0;
    }

    group_stream_params.qos_cfg = &unicast_preset_48_2_1.qos;
    group_stream_params.stream = &unicast_sink_streams[0];

    pair_params.tx_param = &group_stream_params;
    pair_params.rx_param = NULL;

    group_param.packing = ESP_BLE_ISO_PACKING_SEQUENTIAL;
    group_param.params_count = 1;
    group_param.params = &pair_params;

    err = esp_ble_audio_cap_unicast_group_create(&group_param, &unicast_group);
    if (err) {
        ESP_LOGE(TAG, "Failed to create unicast group, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Created unicast group");

    return 0;
}

int unicast_group_delete(void)
{
    int err;

    /* The endpoint pointers become stale once the peer disconnects and are
     * rediscovered on every reconnect. Clear them here so the next discovery
     * does not find filled slots and warn "Unhandled endpoint".
     */
    for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_eps); i++) {
        unicast_sink_eps[i] = NULL;
    }
    for (size_t i = 0; i < ARRAY_SIZE(unicast_source_eps); i++) {
        unicast_source_eps[i] = NULL;
    }

    if (unicast_group == NULL) {
        return 0;
    }

    err = esp_ble_audio_cap_unicast_group_delete(unicast_group);
    if (err) {
        ESP_LOGE(TAG, "Failed to delete unicast group, err %d", err);
        return err;
    }

    unicast_group = NULL;

    ESP_LOGI(TAG, "Deleted unicast group");

    return 0;
}

static int unicast_audio_start(esp_ble_conn_t *conn)
{
    esp_ble_audio_cap_unicast_audio_start_stream_param_t stream_param = {0};
    esp_ble_audio_cap_unicast_audio_start_param_t param = {0};
    int err;

    if (unicast_sink_eps[0] == NULL) {
        ESP_LOGE(TAG, "No sink endpoint available");
        return -ENODEV;
    }

    stream_param.member.member = conn;
    stream_param.stream = &unicast_sink_streams[0];
    stream_param.ep = unicast_sink_eps[0];
    stream_param.codec_cfg = &unicast_preset_48_2_1.codec_cfg;

    param.type = ESP_BLE_AUDIO_CAP_SET_TYPE_AD_HOC;
    param.count = 1;
    param.stream_params = &stream_param;

    err = esp_ble_audio_cap_initiator_unicast_audio_start(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to start unicast audio, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Started unicast audio");

    return 0;
}

static void location_cb(esp_ble_conn_t *conn,
                        esp_ble_audio_dir_t dir,
                        esp_ble_audio_location_t loc)
{
    ESP_LOGI(TAG, "Location, dir %u loc 0x%08lx", dir, loc);
}

static void available_contexts_cb(esp_ble_conn_t *conn,
                                  esp_ble_audio_context_t snk_ctx,
                                  esp_ble_audio_context_t src_ctx)
{
    ESP_LOGI(TAG, "Available contexts, sink 0x%04x source 0x%04x", snk_ctx, src_ctx);
}

static void config_cb(esp_ble_audio_bap_stream_t *stream,
                      esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                      esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Config response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void qos_cb(esp_ble_audio_bap_stream_t *stream,
                   esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                   esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] QoS response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void enable_cb(esp_ble_audio_bap_stream_t *stream,
                      esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                      esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Enable response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void start_cb(esp_ble_audio_bap_stream_t *stream,
                     esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                     esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Start response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void stop_cb(esp_ble_audio_bap_stream_t *stream,
                    esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                    esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Stop response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void disable_cb(esp_ble_audio_bap_stream_t *stream,
                       esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                       esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Disable response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void metadata_cb(esp_ble_audio_bap_stream_t *stream,
                        esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                        esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Metadata response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void release_cb(esp_ble_audio_bap_stream_t *stream,
                       esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                       esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Release response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void discover_cb(esp_ble_conn_t *conn, int err, esp_ble_audio_dir_t dir)
{
    if (err) {
        ESP_LOGE(TAG, "Discovery failed, err %d", err);
        return;
    }

    if (default_conn_handle_get() != conn->handle) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return;
    }

    if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        ESP_LOGI(TAG, "Sink discover complete");

        err = esp_ble_audio_bap_unicast_client_discover(conn->handle, ESP_BLE_AUDIO_DIR_SOURCE);
        if (err) {
            ESP_LOGE(TAG, "Failed to discover source, err %d", err);
            return;
        }
    } else if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        ESP_LOGI(TAG, "Source discover complete");

        err = unicast_group_create();
        if (err) {
            return;
        }

        err = unicast_audio_start(conn);
        if (err) {
            unicast_group_delete();
            return;
        }
    }
}

static void pac_record_cb(esp_ble_conn_t *conn, esp_ble_audio_dir_t dir,
                          const esp_ble_audio_codec_cap_t *codec_cap)
{
    example_print_codec_cap(TAG, codec_cap);
}

static void endpoint_cb(esp_ble_conn_t *conn,
                        esp_ble_audio_dir_t dir,
                        esp_ble_audio_bap_ep_t *ep)
{
    if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        for (size_t i = 0; i < ARRAY_SIZE(unicast_source_eps); i++) {
            if (unicast_source_eps[i] == NULL) {
                ESP_LOGI(TAG, "[SRC #%zu] Endpoint discovered", i);
                unicast_source_eps[i] = ep;
                return;
            }
        }
    } else if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_eps); i++) {
            if (unicast_sink_eps[i] == NULL) {
                ESP_LOGI(TAG, "[SNK #%zu] Endpoint discovered", i);
                unicast_sink_eps[i] = ep;
                return;
            }
        }
    }

    ESP_LOGW(TAG, "[%s] Unhandled endpoint", dir_str(dir));
}

static esp_ble_audio_bap_unicast_client_cb_t unicast_client_cbs = {
    .location           = location_cb,
    .available_contexts = available_contexts_cb,
    .config             = config_cb,
    .qos                = qos_cb,
    .enable             = enable_cb,
    .start              = start_cb,
    .stop               = stop_cb,
    .disable            = disable_cb,
    .metadata           = metadata_cb,
    .release            = release_cb,
    .pac_record         = pac_record_cb,
    .endpoint           = endpoint_cb,
    .discover           = discover_cb,
};

static void unicast_audio_tx(void)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_ble_audio_cap_stream_t *cap_stream;
    esp_ble_audio_bap_stream_t *bap_stream;
    esp_err_t err;

    cap_stream = &unicast_sink_streams[0];
    bap_stream = &unicast_sink_streams[0].bap_stream;

    if (bap_stream->ep == NULL) {
        return;
    }

    err = esp_ble_audio_bap_ep_get_info(bap_stream->ep, &ep_info);
    if (err) {
        return;
    }

    if (ep_info.state != ESP_BLE_AUDIO_BAP_EP_STATE_STREAMING) {
        return;
    }

    if (bap_stream->qos == NULL || bap_stream->qos->sdu == 0) {
        ESP_LOGE(TAG, "Invalid stream qos");
        return;
    }

    if (iso_data == NULL) {
        ESP_LOGE(TAG, "TX buffer unavailable, SDU %u", bap_stream->qos->sdu);
        return;
    }

    memset(iso_data, (uint8_t)tx_seq_num, bap_stream->qos->sdu);

    err = esp_ble_audio_cap_stream_send(cap_stream, iso_data,
                                        bap_stream->qos->sdu,
                                        tx_seq_num);
    if (err) {
        ESP_LOGD(TAG, "Failed to transmit data on streams, err %d", err);
        return;
    }

    tx_seq_num++;
}

static void tx_scheduler_cb(void *arg)
{
    (void)arg;
    unicast_audio_tx();
}

int cap_initiator_init(void)
{
    int err;

    err = esp_ble_audio_cap_initiator_register_cb(&cap_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register CAP callbacks (err %d)", err);
        return err;
    }

    err = esp_ble_audio_bap_unicast_client_register_cb(&unicast_client_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register BAP unicast client callbacks (err %d)", err);
        return err;
    }

    for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_streams); i++) {
        esp_ble_audio_cap_stream_ops_register(&unicast_sink_streams[i], &unicast_stream_ops);
    }

    for (size_t i = 0; i < ARRAY_SIZE(unicast_source_streams); i++) {
        esp_ble_audio_cap_stream_ops_register(&unicast_source_streams[i], &unicast_stream_ops);
    }

    err = example_audio_tx_scheduler_init(&tx_scheduler,
                                          tx_scheduler_cb,
                                          NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to init tx scheduler, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "CAP initiator initialized");

    return 0;
}

int cap_initiator_setup(void)
{
    uint16_t conn_handle;
    int err;

    conn_handle = default_conn_handle_get();
    if (conn_handle == CONN_HANDLE_INIT) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    err = esp_ble_audio_cap_initiator_unicast_discover(conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover CAS, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "CAP initiator setup");

    return 0;
}
