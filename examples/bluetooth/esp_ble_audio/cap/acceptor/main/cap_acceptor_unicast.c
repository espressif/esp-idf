/*
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "cap_acceptor.h"

static const esp_ble_audio_bap_qos_cfg_pref_t qos_pref =
    ESP_BLE_AUDIO_BAP_QOS_CFG_PREF(true,                /* Unframed PDUs supported */
                                   ESP_BLE_ISO_PHY_2M,  /* Preferred Target PHY */
                                   2,                   /* Preferred Retransmission number */
                                   20,                  /* Preferred Maximum Transport Latency (msec) */
                                   20000,               /* Minimum Presentation Delay (usec) */
                                   40000,               /* Maximum Presentation Delay (usec) */
                                   20000,               /* Preferred Minimum Presentation Delay (usec) */
                                   40000);              /* Preferred Maximum Presentation Delay (usec) */

static example_audio_rx_metrics_t rx_metrics;

static example_audio_tx_scheduler_t tx_scheduler;
static uint16_t tx_seq_num;
static uint8_t *iso_data;

static const struct peer_config *s_peer;

static void unicast_server_tx(void);

static const char *dir_str(esp_ble_audio_dir_t dir)
{
    return dir == ESP_BLE_AUDIO_DIR_SINK ? "SNK" : "SRC";
}

static const char *stream_dir_str(const esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};

    if (stream == NULL) {
        return "???";
    }

    /* When the stream is released, stream->ep is already detached by the lib.
     * Fall back to identifying by the stream's address.
     */
    if (stream->ep == NULL) {
        if (s_peer != NULL) {
            if (stream == &s_peer->sink_stream.bap_stream) {
                return "SNK";
            }
            if (stream == &s_peer->source_stream.bap_stream) {
                return "SRC";
            }
        }
        return "???";
    }

    if (esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info) != 0) {
        return "???";
    }

    return dir_str(ep_info.dir);
}

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    /* Only one sink and one source per peer in this example. */
    (void)stream;
    return 0;
}

static void tx_scheduler_cb(void *arg)
{
    (void)arg;
    unicast_server_tx();
}

static int config_cb(esp_ble_conn_t *conn,
                     const esp_ble_audio_bap_ep_t *ep,
                     esp_ble_audio_dir_t dir,
                     const esp_ble_audio_codec_cfg_t *codec_cfg,
                     esp_ble_audio_bap_stream_t **stream,
                     esp_ble_audio_bap_qos_cfg_pref_t *const pref,
                     esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    esp_ble_audio_cap_stream_t *cap_stream;

    ESP_LOGI(TAG, "[%s] Config request, codec cfg:", dir_str(dir));

    example_print_codec_cfg(TAG, codec_cfg);

    cap_stream = stream_alloc(dir);
    if (cap_stream == NULL) {
        ESP_LOGE(TAG, "[%s] No streams available", dir_str(dir));

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_NO_MEM,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
        return -ENOMEM;
    }

    *stream = &cap_stream->bap_stream;

    *pref = qos_pref;

    return 0;
}

static int reconfig_cb(esp_ble_audio_bap_stream_t *stream,
                       esp_ble_audio_dir_t dir,
                       const esp_ble_audio_codec_cfg_t *codec_cfg,
                       esp_ble_audio_bap_qos_cfg_pref_t *const pref,
                       esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] Reconfig request, codec cfg:",
             dir_str(dir), stream_index(stream));

    example_print_codec_cfg(TAG, codec_cfg);

    *pref = qos_pref;

    return 0;
}

static int qos_cb(esp_ble_audio_bap_stream_t *stream,
                  const esp_ble_audio_bap_qos_cfg_t *qos,
                  esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] QoS request:",
             stream_dir_str(stream), stream_index(stream));

    example_print_qos(TAG, qos);

    return 0;
}

static int enable_cb(esp_ble_audio_bap_stream_t *stream,
                     const uint8_t meta[], size_t meta_len,
                     esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] Enable request (meta_len %u)",
             stream_dir_str(stream), stream_index(stream), meta_len);
    return 0;
}

static int start_cb(esp_ble_audio_bap_stream_t *stream,
                    esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] Start request",
             stream_dir_str(stream), stream_index(stream));
    return 0;
}

struct data_func_param {
    esp_ble_audio_bap_ascs_rsp_t *rsp;
    bool stream_context_present;
};

static bool data_func_cb(uint8_t type, const uint8_t *data,
                         uint8_t data_len, void *user_data)
{
    struct data_func_param *func_param = (struct data_func_param *)user_data;

    if (type == ESP_BLE_AUDIO_METADATA_TYPE_STREAM_CONTEXT) {
        func_param->stream_context_present = true;
    }

    return true;
}

static int metadata_cb(esp_ble_audio_bap_stream_t *stream,
                       const uint8_t meta[], size_t meta_len,
                       esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    struct data_func_param func_param = {
        .rsp = rsp,
        .stream_context_present = false,
    };
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Metadata request (meta_len %u)",
             stream_dir_str(stream), stream_index(stream), meta_len);

    err = esp_ble_audio_data_parse(meta, meta_len, data_func_cb, &func_param);
    if (err) {
        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_REJECTED,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
        return -EIO;
    }

    if (func_param.stream_context_present == false) {
        ESP_LOGE(TAG, "Stream audio context not present");

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_REJECTED,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
        return -EINVAL;
    }

    return 0;
}

static int disable_cb(esp_ble_audio_bap_stream_t *stream,
                      esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] Disable request",
             stream_dir_str(stream), stream_index(stream));
    return 0;
}

static int stop_cb(esp_ble_audio_bap_stream_t *stream,
                   esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] Stop request",
             stream_dir_str(stream), stream_index(stream));
    return 0;
}

static int release_cb(esp_ble_audio_bap_stream_t *stream,
                      esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] Release request",
             stream_dir_str(stream), stream_index(stream));
    return 0;
}

static const esp_ble_audio_bap_unicast_server_cb_t unicast_server_cb = {
    .config   = config_cb,
    .reconfig = reconfig_cb,
    .qos      = qos_cb,
    .enable   = enable_cb,
    .start    = start_cb,
    .metadata = metadata_cb,
    .disable  = disable_cb,
    .stop     = stop_cb,
    .release  = release_cb,
};

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
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Stream enabled",
             stream_dir_str(stream), stream_index(stream));

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to get ep info, err %d", err);
        return;
    }

    if (ep_info.dir == ESP_BLE_AUDIO_DIR_SINK) {
        /* Automatically do the receiver start ready operation */
        err = esp_ble_audio_bap_stream_start(stream);
        if (err) {
            ESP_LOGE(TAG, "Failed to start stream, err %d", err);
            return;
        }
    }
}

static void unicast_stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Stream started",
             stream_dir_str(stream), stream_index(stream));

    example_audio_rx_metrics_reset(&rx_metrics);

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to get ep info, err %d", err);
        return;
    }

    if (ep_info.dir == ESP_BLE_AUDIO_DIR_SOURCE) {
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

        tx_seq_num = 0;
        example_audio_tx_scheduler_reset(&tx_scheduler);

        err = example_audio_tx_scheduler_start(&tx_scheduler, stream->qos->interval);
        if (err) {
            ESP_LOGE(TAG, "Failed to start tx scheduler, err %d", err);
            return;
        }

        unicast_server_tx();
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

    if (ep_info.dir == ESP_BLE_AUDIO_DIR_SOURCE) {
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

    if (ep_info.dir == ESP_BLE_AUDIO_DIR_SOURCE) {
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
    esp_ble_audio_cap_stream_t *cap_stream = CONTAINER_OF(stream,
                                                          esp_ble_audio_cap_stream_t,
                                                          bap_stream);

    ESP_LOGI(TAG, "[%s #%d] Stream released",
             stream_dir_str(stream), stream_index(stream));

    stream_released(cap_stream);
}

static void unicast_stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                                   const esp_ble_iso_recv_info_t *info,
                                   const uint8_t *data, uint16_t len)
{
    char name[24];

    snprintf(name, sizeof(name), "%s #%d",
             stream_dir_str(stream), stream_index(stream));
    rx_metrics.last_sdu_len = len;
    example_audio_rx_metrics_on_recv(info, &rx_metrics, TAG, name);
}

static void unicast_stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    char name[24];

    snprintf(name, sizeof(name), "%s #%d",
             stream_dir_str(stream), stream_index(stream));
    example_audio_tx_scheduler_on_sent(&tx_scheduler, user_data, TAG, name);
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
    .recv             = unicast_stream_recv_cb,
    .sent             = unicast_stream_sent_cb,
    .disconnected     = unicast_stream_disconnected_cb,
};

static void unicast_server_tx(void)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_ble_audio_cap_stream_t *cap_stream;
    esp_ble_audio_bap_stream_t *bap_stream;
    esp_err_t err;

    cap_stream = stream_alloc(ESP_BLE_AUDIO_DIR_SOURCE);
    assert(cap_stream);
    bap_stream = &cap_stream->bap_stream;

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

int cap_acceptor_unicast_init(struct peer_config *peer)
{
    static bool cbs_registered;
    esp_err_t err;

    s_peer = peer;

    if (cbs_registered == false) {
        esp_ble_audio_bap_unicast_server_register_param_t param = {
            CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT,
            CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT
        };

        err = esp_ble_audio_bap_unicast_server_register(&param);
        if (err) {
            ESP_LOGE(TAG, "Failed to register BAP unicast server, err %d", err);
            return -1;
        }

        err = esp_ble_audio_bap_unicast_server_register_cb(&unicast_server_cb);
        if (err) {
            ESP_LOGE(TAG, "Failed to register BAP unicast server callbacks, err %d", err);
            return -1;
        }

        cbs_registered = true;
    }

    err = esp_ble_audio_cap_stream_ops_register(&peer->source_stream, &unicast_stream_ops);
    if (err) {
        ESP_LOGE(TAG, "Failed to register source stream ops, err %d", err);
        return -1;
    }

    err = esp_ble_audio_cap_stream_ops_register(&peer->sink_stream, &unicast_stream_ops);
    if (err) {
        ESP_LOGE(TAG, "Failed to register sink stream ops, err %d", err);
        return -1;
    }

    err = example_audio_tx_scheduler_init(&tx_scheduler,
                                          tx_scheduler_cb,
                                          NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize tx scheduler, err %d", err);
        return -1;
    }

    ESP_LOGI(TAG, "CAP acceptor unicast initialized");

    return 0;
}
