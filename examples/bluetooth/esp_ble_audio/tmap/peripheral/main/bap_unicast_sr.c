/*
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2022 Codecoup
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "tmap_peripheral.h"

static uint8_t codec_data[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_DATA(
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_16KHZ | \
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_32KHZ | \
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_48KHZ,             /* Sampling frequency 16kHz/32kHz/48kHz */
        ESP_BLE_AUDIO_CODEC_CAP_DURATION_7_5 | \
        ESP_BLE_AUDIO_CODEC_CAP_DURATION_10,            /* Frame duration 7.5ms/10ms */
        ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1),  /* Supported channels 1 */
        30,                                             /* Minimum 30 octets per frame */
        155,                                            /* Maximum 155 octets per frame */
        1);                                             /* Maximum 1 codec frame per SDU */

static uint8_t codec_meta[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_META(SINK_CONTEXT | SOURCE_CONTEXT);

static const esp_ble_audio_codec_cap_t lc3_codec_cap =
    ESP_BLE_AUDIO_CODEC_CAP_LC3(codec_data, codec_meta);

static esp_ble_audio_pacs_cap_t cap = {
    .codec_cap = &lc3_codec_cap,
};

static esp_ble_audio_bap_stream_t streams[CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT + \
                                          CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT];

static struct audio_sink {
    esp_ble_audio_bap_stream_t *stream;
    example_audio_rx_metrics_t rx_metrics;
} sink_streams[CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT];

static size_t configured_sink_stream_count;

static struct audio_source {
    esp_ble_audio_bap_stream_t *stream;
    uint16_t seq_num;
} source_streams[CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT];

static size_t configured_source_stream_count;

static const esp_ble_audio_bap_qos_cfg_pref_t qos_pref =
    ESP_BLE_AUDIO_BAP_QOS_CFG_PREF(true,                /* Unframed PDUs supported */
                                   ESP_BLE_ISO_PHY_2M,  /* Preferred Target PHY */
                                   2,                   /* Preferred Retransmission number */
                                   10,                  /* Preferred Maximum Transport Latency (msec) */
                                   20000,               /* Minimum Presentation Delay (usec) */
                                   40000,               /* Maximum Presentation Delay (usec) */
                                   20000,               /* Preferred Minimum Presentation Delay (usec) */
                                   40000);              /* Preferred Maximum Presentation Delay (usec) */

static esp_ble_audio_bap_stream_t *stream_alloc(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        esp_ble_audio_bap_stream_t *stream = &streams[i];

        if (stream->conn == NULL) {
            return stream;
        }
    }

    return NULL;
}

static const char *dir_str(esp_ble_audio_dir_t dir)
{
    return dir == ESP_BLE_AUDIO_DIR_SINK ? "SNK" : "SRC";
}

static bool stream_is_sink(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
        if (sink_streams[i].stream == stream) {
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
    for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
        if (sink_streams[i].stream == stream) {
            return (int)i;
        }
    }
    for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
        if (source_streams[i].stream == stream) {
            return (int)i;
        }
    }
    return -1;
}

static int config_cb(esp_ble_conn_t *conn,
                     const esp_ble_audio_bap_ep_t *ep,
                     esp_ble_audio_dir_t dir,
                     const esp_ble_audio_codec_cfg_t *codec_cfg,
                     esp_ble_audio_bap_stream_t **stream,
                     esp_ble_audio_bap_qos_cfg_pref_t *const pref,
                     esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    bool stream_exist = false;

    ESP_LOGI(TAG, "[%s] Config request:", dir_str(dir));

    example_print_codec_cfg(TAG, codec_cfg);

    *stream = stream_alloc();
    if (*stream == NULL) {
        ESP_LOGE(TAG, "[%s] No streams available", dir_str(dir));

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_NO_MEM,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
        return -ENOMEM;
    }

    if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
            if (sink_streams[i].stream == *stream) {
                stream_exist = true;
                break;
            }
        }

        if (stream_exist == false) {
            if (configured_sink_stream_count >= ARRAY_SIZE(sink_streams)) {
                ESP_LOGE(TAG, "[SNK] No more sink stream available");

                *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_NO_MEM,
                                                  ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
                return -ENOMEM;
            }

            sink_streams[configured_sink_stream_count++].stream = *stream;
        }
    } else if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
            if (source_streams[i].stream == *stream) {
                stream_exist = true;
                break;
            }
        }

        if (stream_exist == false) {
            if (configured_source_stream_count >= ARRAY_SIZE(source_streams)) {
                ESP_LOGE(TAG, "[SRC] No more source stream available");

                *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_NO_MEM,
                                                  ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
                return -ENOMEM;
            }

            source_streams[configured_source_stream_count++].stream = *stream;
        }
    } else {
        assert(0);
    }

    *pref = qos_pref;

    return 0;
}

static int reconfig_cb(esp_ble_audio_bap_stream_t *stream,
                       esp_ble_audio_dir_t dir,
                       const esp_ble_audio_codec_cfg_t *codec_cfg,
                       esp_ble_audio_bap_qos_cfg_pref_t *const pref,
                       esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] Reconfig request:",
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
    bool rejected;
};

static bool data_func_cb(uint8_t type, const uint8_t *data,
                         uint8_t data_len, void *user_data)
{
    struct data_func_param *func_param = (struct data_func_param *)user_data;

    if (!ESP_BLE_AUDIO_METADATA_TYPE_IS_KNOWN(type)) {
        ESP_LOGE(TAG, "Invalid metadata type %u or length %u", type, data_len);

        *func_param->rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_REJECTED,
                                                      type);
        func_param->rejected = true;
        return false;
    }

    if (type == ESP_BLE_AUDIO_METADATA_TYPE_STREAM_CONTEXT) {
        func_param->stream_context_present = true;
    }

    if (type == ESP_BLE_AUDIO_METADATA_TYPE_CCID_LIST) {
        for (uint8_t i = 0; i < data_len; i++) {
            const uint8_t ccid = data[i];

#if CONFIG_BT_TBS_CLIENT_CCID
            uint16_t conn_handle = default_conn_handle_get();

            if (conn_handle == CONN_HANDLE_INIT) {
                ESP_LOGE(TAG, "%s, not connected", __func__);

                *func_param->rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_REJECTED,
                                                              ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
                func_param->rejected = true;
                return false;
            }

            if (esp_ble_audio_tbs_client_get_by_ccid(conn_handle, ccid) == NULL) {
                ESP_LOGW(TAG, "CCID %u is unknown", ccid);

                *func_param->rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_REJECTED,
                                                              ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
                func_param->rejected = true;
                return false;
            }
#else /* CONFIG_BT_TBS_CLIENT_CCID */
            ESP_LOGW(TAG, "CCID %u is unknown", ccid);

            *func_param->rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_REJECTED,
                                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
            func_param->rejected = true;
            return false;
#endif /* CONFIG_BT_TBS_CLIENT_CCID */
        }
    }

    func_param->rejected = false;
    return true;
}

static int metadata_cb(esp_ble_audio_bap_stream_t *stream,
                       const uint8_t meta[], size_t meta_len,
                       esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    struct data_func_param func_param = {
        .rsp = rsp,
        .stream_context_present = false,
        .rejected = false,
    };
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Metadata request (meta_len %u)",
             stream_dir_str(stream), stream_index(stream), meta_len);

    err = esp_ble_audio_data_parse(meta, meta_len, data_func_cb, &func_param);
    if (err) {
        return -EIO;
    }

    if (func_param.rejected) {
        /* The rsp is already set by data_func_cb */
        return -EINVAL;
    }

    if (func_param.stream_context_present == false) {
        ESP_LOGE(TAG, "Stream audio context not present on peer!");

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

    for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
        if (sink_streams[i].stream == stream) {
            memset(&sink_streams[i], 0, sizeof(sink_streams[i]));
            if (configured_sink_stream_count > 0) {
                configured_sink_stream_count--;
            }
            return 0;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
        if (source_streams[i].stream == stream) {
            memset(&source_streams[i], 0, sizeof(source_streams[i]));
            if (configured_source_stream_count > 0) {
                configured_source_stream_count--;
            }
            return 0;
        }
    }

    ESP_LOGW(TAG, "Stream not found");
    return -ENODEV;
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

static void stream_enabled(esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    int err;

    ESP_LOGI(TAG, "[%s #%d] Stream enabled",
             stream_dir_str(stream), stream_index(stream));

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to get ep info, err %d", err);
        return;
    }

    /* The unicast server is responsible for starting the sink streams */
    if (ep_info.dir == ESP_BLE_AUDIO_DIR_SINK) {
        /* Automatically do the receiver start ready operation */
        err = esp_ble_audio_bap_stream_start(stream);
        if (err) {
            ESP_LOGE(TAG, "[SNK #%d] Failed to start stream, err %d",
                     stream_index(stream), err);
            return;
        }
    }
}

static void stream_started(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream started",
             stream_dir_str(stream), stream_index(stream));

    for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
        if (sink_streams[i].stream == stream) {
            example_audio_rx_metrics_reset(&sink_streams[i].rx_metrics);
            break;
        }
    }
}

static void stream_stopped(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Stream stopped, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);
}

static void stream_recv(esp_ble_audio_bap_stream_t *stream,
                        const esp_ble_iso_recv_info_t *info,
                        const uint8_t *data, uint16_t len)
{

    for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
        if (sink_streams[i].stream == stream) {
            struct audio_sink *sink_stream = CONTAINER_OF(&sink_streams[i].stream,
                                                          struct audio_sink,
                                                          stream);
            char name[24];

            snprintf(name, sizeof(name), "SNK #%zu", i);
            sink_stream->rx_metrics.last_sdu_len = len;
            example_audio_rx_metrics_on_recv(info, &sink_stream->rx_metrics,
                                             TAG, name);
            break;
        }
    }
}

static esp_ble_audio_bap_stream_ops_t stream_ops = {
    .enabled = stream_enabled,
    .started = stream_started,
    .stopped = stream_stopped,
    .recv    = stream_recv,
};

static esp_ble_audio_bap_unicast_server_register_param_t param = {
    CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT,
    CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT
};

int bap_unicast_sr_init(void)
{
    const esp_ble_audio_pacs_register_param_t pacs_param = {
        .snk_pac = true,
        .snk_loc = true,
        .src_pac = true,
        .src_loc = true,
    };
    esp_ble_audio_location_t location;
    int err;

    location = 0;

    err = esp_ble_audio_pacs_register(&pacs_param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs, err %d", err);
        return err;
    }

    err = esp_ble_audio_bap_unicast_server_register(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register Unicast Server params, err %d", err);
        return err;
    }

    err = esp_ble_audio_bap_unicast_server_register_cb(&unicast_server_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register Unicast Server callbacks, err %d", err);
        return err;
    }

#if CONFIG_EXAMPLE_TMAP_PER_LEFT
    location |= ESP_BLE_AUDIO_LOCATION_FRONT_LEFT;
#endif /* CONFIG_EXAMPLE_TMAP_PER_LEFT */
#if CONFIG_EXAMPLE_TMAP_PER_RIGHT
    location |= ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT;
#endif /* CONFIG_EXAMPLE_TMAP_PER_RIGHT */

#if CONFIG_BT_PAC_SNK
    /* Register CT required capabilities */
    err = esp_ble_audio_pacs_cap_register(ESP_BLE_AUDIO_DIR_SINK, &cap);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs capabilities, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_location(ESP_BLE_AUDIO_DIR_SINK, location);
    if (err) {
        ESP_LOGE(TAG, "Failed to set pacs location, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_supported_contexts(ESP_BLE_AUDIO_DIR_SINK, SINK_CONTEXT);
    if (err) {
        ESP_LOGE(TAG, "Failed to set pacs supported contexts, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_available_contexts(ESP_BLE_AUDIO_DIR_SINK, SINK_CONTEXT);
    if (err) {
        ESP_LOGE(TAG, "Failed to set pacs available contexts, err %d", err);
        return err;
    }
#endif /* CONFIG_BT_PAC_SNK */

#if CONFIG_BT_PAC_SRC
    /* Register CT required capabilities */
    err = esp_ble_audio_pacs_cap_register(ESP_BLE_AUDIO_DIR_SOURCE, &cap);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs capabilities, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_location(ESP_BLE_AUDIO_DIR_SOURCE, location);
    if (err) {
        ESP_LOGE(TAG, "Failed to set pacs location, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_supported_contexts(ESP_BLE_AUDIO_DIR_SOURCE, SOURCE_CONTEXT);
    if (err) {
        ESP_LOGE(TAG, "Failed to set pacs supported contexts, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_available_contexts(ESP_BLE_AUDIO_DIR_SOURCE, SOURCE_CONTEXT);
    if (err) {
        ESP_LOGE(TAG, "Failed to set pacs available contexts, err %d", err);
        return err;
    }
#endif /* CONFIG_BT_PAC_SRC */

    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        esp_ble_audio_bap_stream_cb_register(&streams[i], &stream_ops);
    }

    ESP_LOGI(TAG, "BAP unicast server initialized");

    return 0;
}
