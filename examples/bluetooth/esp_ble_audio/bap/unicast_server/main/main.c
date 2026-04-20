/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#include "esp_ble_audio_lc3_defs.h"
#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_pacs_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "BAP_USR"

#define LOCAL_DEVICE_NAME       "BAP Unicast Server"
#define LOCAL_DEVICE_NAME_LEN   (sizeof(LOCAL_DEVICE_NAME) - 1)

#define ADV_HANDLE              0x00
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_ADDRESS             BLE_OWN_ADDR_PUBLIC
#define ADV_PRIMARY_PHY         BLE_HCI_LE_PHY_1M
#define ADV_SECONDARY_PHY       BLE_HCI_LE_PHY_2M
#define ADV_INTERVAL            BLE_GAP_ADV_ITVL_MS(200)

#define SINK_LOCATION           (ESP_BLE_AUDIO_LOCATION_FRONT_LEFT | \
                                 ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT)

#define SOURCE_LOCATION         (ESP_BLE_AUDIO_LOCATION_FRONT_LEFT | \
                                 ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT)

#define SINK_CONTEXT            (ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                                 ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                                 ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA)

#define SOURCE_CONTEXT          (ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                                 ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                                 ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA)

static uint8_t codec_data[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_DATA(
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_ANY,               /* Sampling frequency Any */
        ESP_BLE_AUDIO_CODEC_CAP_DURATION_10,            /* Frame duration 10ms */
        ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(2),  /* Supported channels 2 */
        40,                                             /* Minimum 40 octets per frame */
        120,                                            /* Maximum 120 octets per frame */
        2);                                             /* Maximum 2 codec frames per SDU */

static uint8_t codec_meta[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_META(SINK_CONTEXT | SOURCE_CONTEXT);

static const esp_ble_audio_codec_cap_t codec_cap =
    ESP_BLE_AUDIO_CODEC_CAP_LC3(codec_data, codec_meta);

static esp_ble_audio_pacs_cap_t cap_sink = {
    .codec_cap = &codec_cap,
};

static esp_ble_audio_pacs_cap_t cap_source = {
    .codec_cap = &codec_cap,
};

static esp_ble_audio_bap_unicast_server_register_param_t param = {
    CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT,
    CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT
};

static struct unicast_server_sink_stream {
    esp_ble_audio_bap_stream_t stream;

    /* RX */
    example_audio_rx_metrics_t rx_metrics;
} sink_streams[CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT];

static struct unicast_server_stream {
    esp_ble_audio_bap_stream_t stream;

    /* TX */
    uint16_t seq_num;
    uint16_t max_sdu;
    uint32_t send_count;
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

static uint8_t ext_adv_data[3 + 4 + 10 + 2 + LOCAL_DEVICE_NAME_LEN];

static esp_ble_audio_dir_t stream_dir(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
        if (stream == &source_streams[i].stream) {
            return ESP_BLE_AUDIO_DIR_SOURCE;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
        if (stream == &sink_streams[i].stream) {
            return ESP_BLE_AUDIO_DIR_SINK;
        }
    }

    assert(0 && "Invalid stream");
}

static const char *dir_str(esp_ble_audio_dir_t dir)
{
    return dir == ESP_BLE_AUDIO_DIR_SINK ? "SNK" : "SRC";
}

static const char *stream_dir_str(const esp_ble_audio_bap_stream_t *stream)
{
    return dir_str(stream_dir(stream));
}

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
        if (stream == &sink_streams[i].stream) {
            return (int)i;
        }
    }
    for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
        if (stream == &source_streams[i].stream) {
            return (int)i;
        }
    }
    return -1;
}

static esp_ble_audio_bap_stream_t *stream_alloc(esp_ble_audio_dir_t dir)
{
    if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
            esp_ble_audio_bap_stream_t *stream = &source_streams[i].stream;

            if (stream->conn == NULL) {
                return stream;
            }
        }
    } else {
        for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
            esp_ble_audio_bap_stream_t *stream = &sink_streams[i].stream;

            if (stream->conn == NULL) {
                return stream;
            }
        }
    }

    return NULL;
}

static int config_cb(esp_ble_conn_t *conn,
                     const esp_ble_audio_bap_ep_t *ep,
                     esp_ble_audio_dir_t dir,
                     const esp_ble_audio_codec_cfg_t *codec_cfg,
                     esp_ble_audio_bap_stream_t **stream,
                     esp_ble_audio_bap_qos_cfg_pref_t *const pref,
                     esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    example_print_codec_cfg(TAG, codec_cfg);

    *stream = stream_alloc(dir);
    if (*stream == NULL) {
        ESP_LOGW(TAG, "[%s] No streams available", dir_str(dir));

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_NO_MEM,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);
        return -ENOMEM;
    }

    ESP_LOGI(TAG, "[%s #%d] Config request", dir_str(dir), stream_index(*stream));

    if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        configured_source_stream_count++;
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
    ESP_LOGW(TAG, "[%s #%d] Reconfig rejected (not supported)",
             dir_str(dir), stream_index(stream));

    *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED,
                                      ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE);

    /* We only support one QoS at the moment, reject changes */
    return -ENOEXEC;
}

static int qos_cb(esp_ble_audio_bap_stream_t *stream,
                  const esp_ble_audio_bap_qos_cfg_t *qos,
                  esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    ESP_LOGI(TAG, "[%s #%d] QoS request:",
             stream_dir_str(stream), stream_index(stream));
    example_print_qos(TAG, qos);

    for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
        if (stream == &source_streams[i].stream) {
            source_streams[i].max_sdu = qos->sdu;
            break;
        }
    }

    return 0;
}

static int enable_cb(esp_ble_audio_bap_stream_t *stream,
                     const uint8_t meta[], size_t meta_len,
                     esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    esp_ble_audio_codec_cfg_frame_dur_t frame_dur;
    esp_ble_audio_codec_cfg_freq_t freq;
    uint32_t frame_dur_us;
    uint8_t frame_blocks;
    uint32_t freq_hz;
    esp_err_t err;

    err = esp_ble_audio_codec_cfg_get_freq(stream->codec_cfg, &freq);
    if (err) {
        ESP_LOGE(TAG, "Error: Codec frequency not set");

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_INVALID,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_CODEC_DATA);
        return err;
    }

    err = esp_ble_audio_codec_cfg_freq_to_freq_hz(freq, &freq_hz);
    if (err) {
        ESP_LOGE(TAG, "Failed to get frequency hz");

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_INVALID,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_CODEC_DATA);
        return err;
    }

    err = esp_ble_audio_codec_cfg_get_frame_dur(stream->codec_cfg, &frame_dur);
    if (err) {
        ESP_LOGE(TAG, "Error: Frame duration not set");

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_INVALID,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_CODEC_DATA);
        return err;
    }

    err = esp_ble_audio_codec_cfg_frame_dur_to_frame_dur_us(frame_dur, &frame_dur_us);
    if (err) {
        ESP_LOGE(TAG, "Failed to get frame duration us");

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_INVALID,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_CODEC_DATA);
        return err;
    }

    err = esp_ble_audio_codec_cfg_get_frame_blocks_per_sdu(stream->codec_cfg, &frame_blocks, true);
    if (err) {
        ESP_LOGE(TAG, "Error: Frame blocks per SDU not set");

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_INVALID,
                                          ESP_BLE_AUDIO_BAP_ASCS_REASON_CODEC_DATA);
        return err;
    }

    ESP_LOGI(TAG, "[%s #%d] Enable request: %lu Hz, %lu us, %u block(s)/SDU",
             stream_dir_str(stream), stream_index(stream),
             freq_hz, frame_dur_us, frame_blocks);

    *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_SUCCESS,
                                      ESP_BLE_AUDIO_BAP_ASCS_REASON_CODEC_DATA);
    return 0;
}

static int start_cb(esp_ble_audio_bap_stream_t *stream,
                    esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
        if (stream == &source_streams[i].stream) {
            source_streams[i].seq_num = 0;
            source_streams[i].send_count = 0;
            break;
        }
    }

    return 0;
}

static bool data_cb(uint8_t type, const uint8_t *data,
                    uint8_t data_len, void *user_data)
{
    esp_ble_audio_bap_ascs_rsp_t *rsp = user_data;

    if (ESP_BLE_AUDIO_METADATA_TYPE_IS_KNOWN(type) == false) {
        ESP_LOGW(TAG, "Invalid metadata type 0x%02x len %u", type, data_len);

        *rsp = ESP_BLE_AUDIO_BAP_ASCS_RSP(ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_REJECTED,
                                          type);
        return false;
    }

    return true;
}

static int metadata_cb(esp_ble_audio_bap_stream_t *stream,
                       const uint8_t meta[], size_t meta_len,
                       esp_ble_audio_bap_ascs_rsp_t *rsp)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Metadata update (%u bytes)",
             stream_dir_str(stream), stream_index(stream), meta_len);
    example_print_meta(TAG, meta, meta_len);

    err = esp_ble_audio_data_parse(meta, meta_len, data_cb, rsp);
    if (err) {
        return -EIO;
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

    if (stream_dir(stream) == ESP_BLE_AUDIO_DIR_SOURCE &&
            configured_source_stream_count > 0) {
        configured_source_stream_count--;
    }

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

static void stream_enabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Stream enabled",
             stream_dir_str(stream), stream_index(stream));

    /* The unicast server is responsible for starting sink ASEs
     * after the client has enabled them.
     */
    if (stream_dir(stream) == ESP_BLE_AUDIO_DIR_SINK) {
        err = esp_ble_audio_bap_stream_start(stream);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "[SNK #%d] Failed to start stream, err %d",
                     stream_index(stream), err);
        }
    }
}

static void stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream started",
             stream_dir_str(stream), stream_index(stream));

    if (stream_dir(stream) == ESP_BLE_AUDIO_DIR_SOURCE) {
        struct unicast_server_stream *source_stream =
            CONTAINER_OF(stream, struct unicast_server_stream, stream);

        /* Reset TX counters */
        source_stream->seq_num = 0;
        source_stream->send_count = 0;
    } else if (stream_dir(stream) == ESP_BLE_AUDIO_DIR_SINK) {
        struct unicast_server_sink_stream *sink_stream =
            CONTAINER_OF(stream, struct unicast_server_sink_stream, stream);

        /* Reset RX counters */
        example_audio_rx_metrics_reset(&sink_stream->rx_metrics);
    }
}

static void stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Stream stopped, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);
}

static void stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                           const esp_ble_iso_recv_info_t *info,
                           const uint8_t *data, uint16_t len)
{
    if (stream_dir(stream) == ESP_BLE_AUDIO_DIR_SINK) {
        struct unicast_server_sink_stream *sink_stream =
            CONTAINER_OF(stream, struct unicast_server_sink_stream, stream);
        char name[24];
        size_t idx = sink_stream - sink_streams;

        snprintf(name, sizeof(name), "SNK #%zu", idx);
        sink_stream->rx_metrics.last_sdu_len = len;
        example_audio_rx_metrics_on_recv(info, &sink_stream->rx_metrics,
                                         TAG, name);
    }
}

static esp_ble_audio_bap_stream_ops_t stream_ops = {
    .enabled = stream_enabled_cb,
    .started = stream_started_cb,
    .stopped = stream_stopped_cb,
    .recv    = stream_recv_cb,
};

static int set_pacs_location(void)
{
    esp_err_t err;

    if (IS_ENABLED(CONFIG_BT_PAC_SNK_LOC)) {
        err = esp_ble_audio_pacs_set_location(ESP_BLE_AUDIO_DIR_SINK, SINK_LOCATION);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set sink location (err %d)", err);
            return -1;
        }
    }

    if (IS_ENABLED(CONFIG_BT_PAC_SRC_LOC)) {
        err = esp_ble_audio_pacs_set_location(ESP_BLE_AUDIO_DIR_SOURCE, SOURCE_LOCATION);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set source location (err %d)", err);
            return -1;
        }
    }

    return 0;
}

static int set_pacs_supported_contexts(void)
{
    esp_err_t err;

    if (IS_ENABLED(CONFIG_BT_PAC_SNK)) {
        err = esp_ble_audio_pacs_set_supported_contexts(ESP_BLE_AUDIO_DIR_SINK, SINK_CONTEXT);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set sink supported contexts (err %d)", err);
            return -1;
        }
    }

    if (IS_ENABLED(CONFIG_BT_PAC_SRC)) {
        err = esp_ble_audio_pacs_set_supported_contexts(ESP_BLE_AUDIO_DIR_SOURCE, SOURCE_CONTEXT);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set source supported contexts (err %d)", err);
            return -1;
        }
    }

    return 0;
}

static int set_pacs_available_contexts(void)
{
    esp_err_t err;

    if (IS_ENABLED(CONFIG_BT_PAC_SNK)) {
        err = esp_ble_audio_pacs_set_available_contexts(ESP_BLE_AUDIO_DIR_SINK, SINK_CONTEXT);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set sink available contexts (err %d)", err);
            return -1;
        }
    }

    if (IS_ENABLED(CONFIG_BT_PAC_SRC)) {
        err = esp_ble_audio_pacs_set_available_contexts(ESP_BLE_AUDIO_DIR_SOURCE, SOURCE_CONTEXT);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set source available contexts (err %d)", err);
            return -1;
        }
    }

    ESP_LOGI(TAG, "PACS: locations and contexts set");

    return 0;
}

static void build_adv_data(void)
{
    size_t idx = 0;

    /* Flags */
    ext_adv_data[idx++] = 0x02;
    ext_adv_data[idx++] = EXAMPLE_AD_TYPE_FLAGS;
    ext_adv_data[idx++] = EXAMPLE_AD_FLAGS_GENERAL | EXAMPLE_AD_FLAGS_NO_BREDR;

    /* Incomplete List of 16-bit Service UUIDs */
    ext_adv_data[idx++] = 0x03;
    ext_adv_data[idx++] = EXAMPLE_AD_TYPE_UUID16_SOME;
    ext_adv_data[idx++] = (ESP_BLE_AUDIO_UUID_ASCS_VAL & 0xFF);
    ext_adv_data[idx++] = ((ESP_BLE_AUDIO_UUID_ASCS_VAL >> 8) & 0xFF);

    /* Service Data - 16-bit UUID */
    ext_adv_data[idx++] = 0x09;
    ext_adv_data[idx++] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    ext_adv_data[idx++] = (ESP_BLE_AUDIO_UUID_ASCS_VAL & 0xFF);
    ext_adv_data[idx++] = ((ESP_BLE_AUDIO_UUID_ASCS_VAL >> 8) & 0xFF);
    ext_adv_data[idx++] = ESP_BLE_AUDIO_UNICAST_ANNOUNCEMENT_TARGETED;
    ext_adv_data[idx++] = (SINK_CONTEXT & 0xFF);
    ext_adv_data[idx++] = ((SINK_CONTEXT >> 8) & 0xFF);
    ext_adv_data[idx++] = (SOURCE_CONTEXT & 0xFF);
    ext_adv_data[idx++] = ((SOURCE_CONTEXT >> 8) & 0xFF);
    ext_adv_data[idx++] = 0x00; /* Metadata length */

    /* Complete Device Name */
    ext_adv_data[idx++] = LOCAL_DEVICE_NAME_LEN + 1;
    ext_adv_data[idx++] = EXAMPLE_AD_TYPE_NAME_COMPLETE;
    memcpy(&ext_adv_data[idx], LOCAL_DEVICE_NAME, LOCAL_DEVICE_NAME_LEN);
}

static void ext_adv_start(void)
{
    struct ble_gap_ext_adv_params ext_params = {0};
    struct os_mbuf *data = NULL;
    int err;

    build_adv_data();

    ext_params.connectable = 1;
    ext_params.scannable = 0;
    ext_params.legacy_pdu = 0;
    ext_params.own_addr_type = ADV_ADDRESS;
    ext_params.primary_phy = ADV_PRIMARY_PHY;
    ext_params.secondary_phy = ADV_SECONDARY_PHY;
    ext_params.tx_power = ADV_TX_POWER;
    ext_params.sid = ADV_SID;
    ext_params.itvl_min = ADV_INTERVAL;
    ext_params.itvl_max = ADV_INTERVAL;

    err = ble_gap_ext_adv_configure(ADV_HANDLE, &ext_params, NULL,
                                    example_audio_gap_event_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to configure ext adv params, err %d", err);
        return;
    }

    data = os_msys_get_pkthdr(sizeof(ext_adv_data), 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get ext adv mbuf");
        return;
    }

    err = os_mbuf_append(data, ext_adv_data, sizeof(ext_adv_data));
    if (err) {
        ESP_LOGE(TAG, "Failed to append ext adv data, err %d", err);
        os_mbuf_free_chain(data);
        return;
    }

    err = ble_gap_ext_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set ext adv data, err %d", err);
        return;
    }

    err = ble_gap_ext_adv_start(ADV_HANDLE, 0, 0);
    if (err) {
        ESP_LOGE(TAG, "Failed to start ext advertising, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Advertising started (handle %u)", ADV_HANDLE);
}

static void acl_connect(esp_ble_audio_gap_app_event_t *event)
{
    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             event->acl_connect.dst.val[5], event->acl_connect.dst.val[4],
             event->acl_connect.dst.val[3], event->acl_connect.dst.val[2],
             event->acl_connect.dst.val[1], event->acl_connect.dst.val[0]);
}

static void acl_disconnect(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    ext_adv_start();
}

static void iso_gap_app_cb(esp_ble_audio_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT:
        acl_connect(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_DISCONNECT:
        acl_disconnect(event);
        break;
    default:
        break;
    }
}

static void gatt_mtu_change(esp_ble_audio_gatt_app_event_t *event)
{
    int err;

    ESP_LOGI(TAG, "MTU updated: handle %u mtu %u",
             event->gatt_mtu_change.conn_handle, event->gatt_mtu_change.mtu);

    if (event->gatt_mtu_change.mtu < ESP_BLE_AUDIO_ATT_MTU_MIN) {
        ESP_LOGW(TAG, "MTU %u below minimum %u",
                 event->gatt_mtu_change.mtu, ESP_BLE_AUDIO_ATT_MTU_MIN);
        return;
    }

    /* This function only needs to be invoked when:
     * - The device is a Peripheral (Link Layer role);
     * - The device works as a GATT client.
     */
    err = esp_ble_audio_gattc_disc_start(event->gatt_mtu_change.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to start service discovery, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Service discovery started: handle %u",
             event->gatt_mtu_change.conn_handle);
}

static void gattc_disc_cmpl(esp_ble_audio_gatt_app_event_t *event)
{
    ESP_LOGI(TAG, "Service discovery complete: handle %u status %u",
             event->gattc_disc_cmpl.conn_handle,
             event->gattc_disc_cmpl.status);
}

static void iso_gatt_app_cb(esp_ble_audio_gatt_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GATT_EVENT_GATT_MTU_CHANGE:
        gatt_mtu_change(event);
        break;
    case ESP_BLE_AUDIO_GATT_EVENT_GATTC_DISC_CMPL:
        gattc_disc_cmpl(event);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    const esp_ble_audio_pacs_register_param_t pacs_param = {
        .snk_pac = true,
        .snk_loc = true,
        .src_pac = true,
        .src_loc = true,
    };
    esp_ble_audio_init_info_t info = {
        .gap_cb  = iso_gap_app_cb,
        .gatt_cb = iso_gatt_app_cb,
    };
    esp_err_t err;

    /* Initialize NVS — it is used to store PHY calibration data */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize BLE, err %d", err);
        return;
    }

    err = esp_ble_audio_common_init(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    err = esp_ble_audio_pacs_register(&pacs_param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs, err %d", err);
        return;
    }

    err = esp_ble_audio_bap_unicast_server_register(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register unicast server, err %d", err);
        return;
    }

    err = esp_ble_audio_bap_unicast_server_register_cb(&unicast_server_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register unicast server callbacks, err %d", err);
        return;
    }

    err = esp_ble_audio_pacs_cap_register(ESP_BLE_AUDIO_DIR_SINK, &cap_sink);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs capabilities, err %d", err);
        return;
    }

    err = esp_ble_audio_pacs_cap_register(ESP_BLE_AUDIO_DIR_SOURCE, &cap_source);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs capabilities, err %d", err);
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(sink_streams); i++) {
        esp_ble_audio_bap_stream_cb_register(&sink_streams[i].stream, &stream_ops);
    }

    for (size_t i = 0; i < ARRAY_SIZE(source_streams); i++) {
        esp_ble_audio_bap_stream_cb_register(&source_streams[i].stream, &stream_ops);
    }

    err = set_pacs_location();
    if (err) {
        return;
    }

    err = set_pacs_supported_contexts();
    if (err) {
        return;
    }

    err = set_pacs_available_contexts();
    if (err) {
        return;
    }

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    err = ble_svc_gap_device_name_set(LOCAL_DEVICE_NAME);
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    ext_adv_start();
}
