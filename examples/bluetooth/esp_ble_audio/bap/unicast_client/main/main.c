/*
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "nvs_flash.h"

#include "central.h"
#include "stream_tx.h"

#define TARGET_DEVICE_NAME      "BAP Unicast Server"
#define TARGET_DEVICE_NAME_LEN  (sizeof(TARGET_DEVICE_NAME) - 1)

#define CONN_HANDLE_INIT        0xFFFF

#define ASCS_RSP_NONE           (0b00)
#define ASCS_RSP_SUCCESS        (0b01)
#define ASCS_RSP_FAILURE        (0b10)
#define ASCS_RSP_CONNECT        (0b11)

static uint16_t default_conn_handle = CONN_HANDLE_INIT;
static bool disc_completed;
static bool mtu_exchanged;

ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_16_2_1_DEFINE(unicast_preset,
                                                   ESP_BLE_AUDIO_LOCATION_FRONT_LEFT,
                                                   ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED);

static struct audio_sink {
    uint8_t configured : 2;
    uint8_t qos_set : 2;
    uint8_t enabled : 2;
    uint8_t connected : 2;
    esp_ble_audio_bap_stream_t stream;
    esp_ble_audio_bap_ep_t *ep;
} sinks[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT];

static struct audio_source {
    uint8_t configured : 2;
    uint8_t qos_set : 2;
    uint8_t enabled : 2;
    uint8_t connected : 2;
    esp_ble_audio_bap_stream_t stream;
    esp_ble_audio_bap_ep_t *ep;
} sources[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT];

static esp_ble_audio_bap_stream_t *streams[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT +
                                           CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT];

static esp_ble_audio_bap_unicast_group_t *unicast_group;

static size_t configured_sink_stream_count;
static size_t configured_source_stream_count;
#define configured_stream_count (configured_sink_stream_count + configured_source_stream_count)

static struct stream_pair_state {
    esp_ble_audio_bap_stream_t *sink_stream;
    esp_ble_audio_bap_stream_t *source_stream;
} stream_pairs[MAX(ARRAY_SIZE(sinks), ARRAY_SIZE(sources))];
static size_t stream_pair_count;

static void reset_stream_pair_state(void)
{
    stream_pair_count = 0;

    for (size_t i = 0; i < ARRAY_SIZE(stream_pairs); i++) {
        stream_pairs[i].sink_stream = NULL;
        stream_pairs[i].source_stream = NULL;
    }
}

static void update_stream_pair_state(void)
{
    stream_pair_count = MAX(configured_sink_stream_count, configured_source_stream_count);

    for (size_t i = 0; i < ARRAY_SIZE(stream_pairs); i++) {
        stream_pairs[i].sink_stream = NULL;
        stream_pairs[i].source_stream = NULL;
    }

    for (size_t i = 0; i < stream_pair_count; i++) {
        if (i < configured_sink_stream_count) {
            stream_pairs[i].sink_stream = streams[i];
        } else {
            stream_pairs[i].sink_stream = NULL;
        }

        if (i < configured_source_stream_count) {
            stream_pairs[i].source_stream = streams[i + configured_sink_stream_count];
        } else {
            stream_pairs[i].source_stream = NULL;
        }
    }
}

static int get_stream_pair_index(const esp_ble_audio_bap_stream_t *stream)
{
    if (stream == NULL) {
        return -1;
    }

    for (size_t i = 0; i < stream_pair_count; i++) {
        if (stream_pairs[i].sink_stream == stream ||
                stream_pairs[i].source_stream == stream) {
            return (int)i;
        }
    }

    return -1;
}

static esp_ble_audio_bap_stream_t *get_paired_stream(const esp_ble_audio_bap_stream_t *stream)
{
    int pair_index = get_stream_pair_index(stream);

    if (pair_index < 0) {
        return NULL;
    }

    if (stream_pairs[pair_index].sink_stream == stream) {
        return stream_pairs[pair_index].source_stream;
    }

    if (stream_pairs[pair_index].source_stream == stream) {
        return stream_pairs[pair_index].sink_stream;
    }

    return NULL;
}

static bool streams_are_same_pair(const esp_ble_audio_bap_stream_t *stream_a,
                                  const esp_ble_audio_bap_stream_t *stream_b)
{
    int pair_a = get_stream_pair_index(stream_a);
    int pair_b = get_stream_pair_index(stream_b);

    return pair_a >= 0 && pair_b >= 0 && pair_a == pair_b;
}

static void reset_stream_state(void)
{
    configured_sink_stream_count = 0;
    configured_source_stream_count = 0;

    reset_stream_pair_state();

    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        esp_ble_audio_bap_stream_ops_t *ops = sinks[i].stream.ops;

        memset(&sinks[i].stream, 0, sizeof(sinks[i].stream));
        sinks[i].stream.ops = ops;

        sinks[i].configured = ASCS_RSP_NONE;
        sinks[i].qos_set = ASCS_RSP_NONE;
        sinks[i].enabled = ASCS_RSP_NONE;
        sinks[i].connected = ASCS_RSP_NONE;
        sinks[i].ep = NULL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        esp_ble_audio_bap_stream_ops_t *ops = sources[i].stream.ops;

        memset(&sources[i].stream, 0, sizeof(sources[i].stream));
        sources[i].stream.ops = ops;

        sources[i].configured = ASCS_RSP_NONE;
        sources[i].qos_set = ASCS_RSP_NONE;
        sources[i].enabled = ASCS_RSP_NONE;
        sources[i].connected = ASCS_RSP_NONE;
        sources[i].ep = NULL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        streams[i] = NULL;
    }
}

static bool stream_is_sink(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (&sinks[i].stream == stream) {
            return true;
        }
    }

    return false;
}

static const char *dir_str(esp_ble_audio_dir_t dir)
{
    return dir == ESP_BLE_AUDIO_DIR_SINK ? "SNK" : "SRC";
}

static const char *stream_dir_str(const esp_ble_audio_bap_stream_t *stream)
{
    return stream_is_sink(stream) ? "SNK" : "SRC";
}

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (&sinks[i].stream == stream) {
            return (int)i;
        }
    }
    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (&sources[i].stream == stream) {
            return (int)i;
        }
    }
    return -1;
}

static int discover_sinks(void)
{
    int err;

    err = esp_ble_audio_bap_unicast_client_discover(default_conn_handle, ESP_BLE_AUDIO_DIR_SINK);
    if (err) {
        ESP_LOGE(TAG, "[SNK] Discover failed, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "[SNK] Discovering...");

    return 0;
}

static int discover_sources(void)
{
    int err;

    err = esp_ble_audio_bap_unicast_client_discover(default_conn_handle, ESP_BLE_AUDIO_DIR_SOURCE);
    if (err) {
        ESP_LOGE(TAG, "[SRC] Discover failed, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "[SRC] Discovering...");

    return 0;
}

static bool configure_stream(void)
{
    int err;

    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (sinks[i].ep == NULL || sinks[i].configured != ASCS_RSP_NONE) {
            continue;
        }

        err = esp_ble_audio_bap_stream_config(default_conn_handle,
                                              &sinks[i].stream, sinks[i].ep,
                                              &unicast_preset.codec_cfg);
        if (err) {
            ESP_LOGE(TAG, "[SNK #%zu] Config request failed, err %d", i, err);

            sinks[i].configured = ASCS_RSP_FAILURE;
            /* Try to configure the next sink stream */
            continue;
        }

        return false;
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].ep == NULL || sources[i].configured != ASCS_RSP_NONE) {
            continue;
        }

        err = esp_ble_audio_bap_stream_config(default_conn_handle,
                                              &sources[i].stream, sources[i].ep,
                                              &unicast_preset.codec_cfg);
        if (err) {
            ESP_LOGE(TAG, "[SRC #%zu] Config request failed, err %d", i, err);

            sources[i].configured = ASCS_RSP_FAILURE;
            /* Try to configure the next source stream */
            continue;
        }

        return false;
    }

    return true;
}

static void stream_configured(esp_ble_audio_bap_stream_t *stream, bool success)
{
    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (sinks[i].configured == ASCS_RSP_NONE &&
                &sinks[i].stream == stream) {
            if (success) {
                ESP_LOGI(TAG, "[SNK #%zu] Configured", i);
                sinks[i].configured = ASCS_RSP_SUCCESS;
                configured_sink_stream_count++;
            } else {
                ESP_LOGE(TAG, "[SNK #%zu] Configure failed", i);
                sinks[i].configured = ASCS_RSP_FAILURE;
            }
            return;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].configured == ASCS_RSP_NONE &&
                &sources[i].stream == stream) {
            if (success) {
                ESP_LOGI(TAG, "[SRC #%zu] Configured", i);
                sources[i].configured = ASCS_RSP_SUCCESS;
                configured_source_stream_count++;
            } else {
                ESP_LOGE(TAG, "[SRC #%zu] Configure failed", i);
                sources[i].configured = ASCS_RSP_FAILURE;
            }
            return;
        }
    }

    ESP_LOGW(TAG, "No matching stream for configured callback");
}

static int create_group(void)
{
    const size_t params_count = MAX(configured_sink_stream_count, configured_source_stream_count);
    esp_ble_audio_bap_unicast_group_stream_param_t stream_params[configured_stream_count];
    esp_ble_audio_bap_unicast_group_stream_pair_param_t pair_params[params_count];
    esp_ble_audio_bap_unicast_group_param_t group_param = {0};
    int err;

    for (size_t i = 0; i < configured_stream_count; i++) {
        stream_params[i].stream = streams[i];
        stream_params[i].qos = &unicast_preset.qos;
    }

    update_stream_pair_state();

    for (size_t i = 0; i < params_count; i++) {
        if (i < configured_sink_stream_count) {
            pair_params[i].tx_param = &stream_params[i];
        } else {
            pair_params[i].tx_param = NULL;
        }

        if (i < configured_source_stream_count) {
            pair_params[i].rx_param = &stream_params[i + configured_sink_stream_count];
        } else {
            pair_params[i].rx_param = NULL;
        }

        ESP_LOGI(TAG, "Pair[%zu]: SNK=%s SRC=%s", i,
                 stream_pairs[i].sink_stream ? "yes" : "-",
                 stream_pairs[i].source_stream ? "yes" : "-");
    }

    group_param.params = pair_params;
    group_param.params_count = params_count;
    group_param.packing = ESP_BLE_ISO_PACKING_SEQUENTIAL;

    err = esp_ble_audio_bap_unicast_group_create(&group_param, &unicast_group);
    if (err) {
        ESP_LOGE(TAG, "Failed to create unicast group, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Unicast group created (%zu pair(s))", params_count);

    return 0;
}

static int set_stream_qos(void)
{
    int err;

    err = esp_ble_audio_bap_stream_qos(default_conn_handle, unicast_group);
    if (err) {
        ESP_LOGE(TAG, "Failed to setup QoS, err %d", err);
        return err;
    }

    return 0;
}

static void stream_qos_set(esp_ble_audio_bap_stream_t *stream, bool success)
{
    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (sinks[i].configured == ASCS_RSP_SUCCESS &&
                sinks[i].qos_set == ASCS_RSP_NONE &&
                &sinks[i].stream == stream) {
            if (success) {
                ESP_LOGI(TAG, "[SNK #%zu] QoS set", i);
                sinks[i].qos_set = ASCS_RSP_SUCCESS;
            } else {
                ESP_LOGE(TAG, "[SNK #%zu] QoS set failed", i);
                sinks[i].qos_set = ASCS_RSP_FAILURE;
            }
            return;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].configured == ASCS_RSP_SUCCESS &&
                sources[i].qos_set == ASCS_RSP_NONE &&
                &sources[i].stream == stream) {
            if (success) {
                ESP_LOGI(TAG, "[SRC #%zu] QoS set", i);
                sources[i].qos_set = ASCS_RSP_SUCCESS;
            } else {
                ESP_LOGE(TAG, "[SRC #%zu] QoS set failed", i);
                sources[i].qos_set = ASCS_RSP_FAILURE;
            }
            return;
        }
    }

    ESP_LOGW(TAG, "No matching stream for QoS callback");
}

static bool is_all_stream_qos_set(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (sinks[i].configured == ASCS_RSP_SUCCESS &&
                sinks[i].qos_set == ASCS_RSP_NONE) {
            return false;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].configured == ASCS_RSP_SUCCESS &&
                sources[i].qos_set == ASCS_RSP_NONE) {
            return false;
        }
    }

    return true;
}

static bool enable_stream(void)
{
    int err;

    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (sinks[i].configured == ASCS_RSP_SUCCESS &&
                sinks[i].qos_set == ASCS_RSP_SUCCESS &&
                sinks[i].enabled == ASCS_RSP_NONE) {
            err = esp_ble_audio_bap_stream_enable(&sinks[i].stream,
                                                  unicast_preset.codec_cfg.meta,
                                                  unicast_preset.codec_cfg.meta_len);
            if (err) {
                ESP_LOGE(TAG, "[SNK #%zu] Enable request failed, err %d", i, err);

                sinks[i].enabled = ASCS_RSP_FAILURE;
                /* Try to enable the next sink stream */
                continue;
            }

            return false;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].configured == ASCS_RSP_SUCCESS &&
                sources[i].qos_set == ASCS_RSP_SUCCESS &&
                sources[i].enabled == ASCS_RSP_NONE) {
            err = esp_ble_audio_bap_stream_enable(&sources[i].stream,
                                                  unicast_preset.codec_cfg.meta,
                                                  unicast_preset.codec_cfg.meta_len);
            if (err) {
                ESP_LOGE(TAG, "[SRC #%zu] Enable request failed, err %d", i, err);

                sources[i].enabled = ASCS_RSP_FAILURE;
                /* Try to enable the next source stream */
                continue;
            }

            return false;
        }
    }

    return true;
}

static void stream_enabled(esp_ble_audio_bap_stream_t *stream, bool success)
{
    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (sinks[i].configured == ASCS_RSP_SUCCESS &&
                sinks[i].qos_set == ASCS_RSP_SUCCESS &&
                sinks[i].enabled == ASCS_RSP_NONE &&
                &sinks[i].stream == stream) {
            if (success) {
                ESP_LOGI(TAG, "[SNK #%zu] Enabled", i);
                sinks[i].enabled = ASCS_RSP_SUCCESS;
            } else {
                ESP_LOGE(TAG, "[SNK #%zu] Enable failed", i);
                sinks[i].enabled = ASCS_RSP_FAILURE;
            }
            return;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].configured == ASCS_RSP_SUCCESS &&
                sources[i].qos_set == ASCS_RSP_SUCCESS &&
                sources[i].enabled == ASCS_RSP_NONE &&
                &sources[i].stream == stream) {
            if (success) {
                ESP_LOGI(TAG, "[SRC #%zu] Enabled", i);
                sources[i].enabled = ASCS_RSP_SUCCESS;
            } else {
                ESP_LOGE(TAG, "[SRC #%zu] Enable failed", i);
                sources[i].enabled = ASCS_RSP_FAILURE;
            }
            return;
        }
    }

    ESP_LOGW(TAG, "No matching stream for enable callback");
}

static int connect_stream(void)
{
    int err;

    for (size_t i = 0; i < stream_pair_count; i++) {
        esp_ble_audio_bap_stream_t *sink_stream = stream_pairs[i].sink_stream;
        esp_ble_audio_bap_stream_t *source_stream = stream_pairs[i].source_stream;
        esp_ble_audio_bap_stream_t *stream = NULL;
        bool sink_ready = false;
        bool source_ready = false;
        int sink_index = -1;
        int source_index = -1;

        for (size_t j = 0; j < ARRAY_SIZE(sinks); j++) {
            if (&sinks[j].stream == sink_stream) {
                sink_index = (int)j;
                sink_ready = (sinks[j].configured == ASCS_RSP_SUCCESS &&
                              sinks[j].qos_set == ASCS_RSP_SUCCESS &&
                              sinks[j].enabled == ASCS_RSP_SUCCESS &&
                              sinks[j].connected == ASCS_RSP_NONE);
                break;
            }
        }

        for (size_t j = 0; j < ARRAY_SIZE(sources); j++) {
            if (&sources[j].stream == source_stream) {
                source_index = (int)j;
                source_ready = (sources[j].configured == ASCS_RSP_SUCCESS &&
                                sources[j].qos_set == ASCS_RSP_SUCCESS &&
                                sources[j].enabled == ASCS_RSP_SUCCESS &&
                                sources[j].connected == ASCS_RSP_NONE);
                break;
            }
        }

        if (!sink_ready && !source_ready) {
            continue;
        }

        stream = sink_ready ? sink_stream : source_stream;

        err = esp_ble_audio_bap_stream_connect(stream);
        if (err) {
            ESP_LOGE(TAG, "Pair[%zu] connect failed, err %d", i, err);

            if (sink_ready) {
                sinks[sink_index].connected = ASCS_RSP_FAILURE;
            }
            if (source_ready) {
                sources[source_index].connected = ASCS_RSP_FAILURE;
            }
            continue;
        }

        if (sink_ready) {
            sinks[sink_index].connected = ASCS_RSP_CONNECT;
        }
        if (source_ready) {
            sources[source_index].connected = ASCS_RSP_CONNECT;
        }

        ESP_LOGI(TAG, "Pair[%zu] connecting (%s)", i,
                 sink_ready ? "SNK first" : "SRC first");
        return false;
    }

    return true;
}

static bool stream_is_connecting(const esp_ble_audio_bap_stream_t *stream)
{
    if (stream == NULL) {
        return false;
    }

    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (&sinks[i].stream == stream) {
            return sinks[i].connected == ASCS_RSP_CONNECT;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (&sources[i].stream == stream) {
            return sources[i].connected == ASCS_RSP_CONNECT;
        }
    }

    return false;
}

static void stream_connected(esp_ble_audio_bap_stream_t *stream, bool success)
{
    uint8_t state = success ? ASCS_RSP_SUCCESS : ASCS_RSP_FAILURE;

    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (sinks[i].configured == ASCS_RSP_SUCCESS &&
                sinks[i].qos_set == ASCS_RSP_SUCCESS &&
                sinks[i].enabled == ASCS_RSP_SUCCESS &&
                sinks[i].connected == ASCS_RSP_CONNECT &&
                &sinks[i].stream == stream) {
            if (success) {
                ESP_LOGI(TAG, "[SNK #%zu] Connected", i);
            } else {
                ESP_LOGE(TAG, "[SNK #%zu] Connect failed", i);
            }
            sinks[i].connected = state;
            return;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].configured == ASCS_RSP_SUCCESS &&
                sources[i].qos_set == ASCS_RSP_SUCCESS &&
                sources[i].enabled == ASCS_RSP_SUCCESS &&
                sources[i].connected == ASCS_RSP_CONNECT &&
                &sources[i].stream == stream) {
            if (success) {
                ESP_LOGI(TAG, "[SRC #%zu] Connected", i);
            } else {
                ESP_LOGE(TAG, "[SRC #%zu] Connect failed", i);
            }
            sources[i].connected = state;
            return;
        }
    }

    ESP_LOGW(TAG, "No matching stream for connected callback");
}

static int start_stream(void)
{
    esp_err_t err;

    /* Note: sink streams are started by the unicast server */

    ESP_LOGI(TAG, "[SRC] Starting all source streams");

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].configured == ASCS_RSP_SUCCESS &&
                sources[i].qos_set == ASCS_RSP_SUCCESS &&
                sources[i].enabled == ASCS_RSP_SUCCESS &&
                sources[i].connected == ASCS_RSP_SUCCESS) {
            err = esp_ble_audio_bap_stream_start(&sources[i].stream);
            if (err) {
                ESP_LOGE(TAG, "[SRC #%zu] Start failed, err %d", i, err);
            }
        }
    }

    return 0;
}

static void stream_configured_cb(esp_ble_audio_bap_stream_t *stream,
                                 const esp_ble_audio_bap_qos_cfg_pref_t *pref)
{
    ESP_LOGI(TAG, "[%s #%d] Stream configured, QoS preference:",
             stream_dir_str(stream), stream_index(stream));
    example_print_qos_pref(TAG, pref);
}

static void stream_qos_set_cb(esp_ble_audio_bap_stream_t *stream)
{
    /* QoS set is also reported by qos_cb; skip the duplicate log here. */
    (void)stream;
}

static void stream_enabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    /* Enabled is also reported by enable_cb; skip the duplicate log here. */
    (void)stream;
}

static void stream_connected_cb(esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_stream_t *paired_stream;
    int pair_index;
    bool ret;

    pair_index = get_stream_pair_index(stream);
    paired_stream = get_paired_stream(stream);

    ESP_LOGI(TAG, "[%s #%d] ISO connected (pair[%d])",
             stream_dir_str(stream), stream_index(stream), pair_index);

    stream_connected(stream, true);

    if (streams_are_same_pair(stream, paired_stream) &&
            stream_is_connecting(paired_stream)) {
        /* Wait for the paired stream before moving on */
        return;
    }

    ret = connect_stream();
    if (ret == false) {
        return;
    }

    start_stream();
}

static void stream_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] ISO disconnected, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    /* Reset the connected state to allow reconnection */
    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (&sinks[i].stream == stream) {
            (void)stream_tx_unregister(stream);
            sinks[i].connected = ASCS_RSP_NONE;
            return;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (&sources[i].stream == stream) {
            sources[i].connected = ASCS_RSP_NONE;
            return;
        }
    }

    ESP_LOGW(TAG, "No matching stream for disconnected callback");
}

static void stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    int err;

    ESP_LOGI(TAG, "[%s #%d] Stream started",
             stream_dir_str(stream), stream_index(stream));

    if (stream_is_sink(stream)) {
        err = stream_tx_register(stream);
        if (err) {
            ESP_LOGE(TAG, "[SNK #%d] Failed to register TX, err %d",
                     stream_index(stream), err);
        }
    }
}

static void stream_metadata_updated_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Metadata updated",
             stream_dir_str(stream), stream_index(stream));
}

static void stream_disabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream disabled",
             stream_dir_str(stream), stream_index(stream));
}

static void stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Stream stopped, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    if (stream_is_sink(stream)) {
        (void)stream_tx_unregister(stream);
    }
}

static void stream_released_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream released",
             stream_dir_str(stream), stream_index(stream));
}

static void stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    stream_tx_sent(stream, user_data);
}

static esp_ble_audio_bap_stream_ops_t stream_ops = {
    .configured       = stream_configured_cb,
    .qos_set          = stream_qos_set_cb,
    .enabled          = stream_enabled_cb,
    .started          = stream_started_cb,
    .metadata_updated = stream_metadata_updated_cb,
    .disabled         = stream_disabled_cb,
    .stopped          = stream_stopped_cb,
    .released         = stream_released_cb,
    .sent             = stream_sent_cb,
    .connected        = stream_connected_cb,
    .disconnected     = stream_disconnected_cb,
};

static void location_cb(esp_ble_conn_t *conn,
                        esp_ble_audio_dir_t dir,
                        esp_ble_audio_location_t loc)
{
    ESP_LOGI(TAG, "[%s] Location: 0x%08lx", dir_str(dir), loc);
}

static void available_contexts_cb(esp_ble_conn_t *conn,
                                  esp_ble_audio_context_t snk_ctx,
                                  esp_ble_audio_context_t src_ctx)
{
    ESP_LOGI(TAG, "Available contexts: SNK=0x%04x SRC=0x%04x", snk_ctx, src_ctx);
}

static void log_rsp(const char *op, esp_ble_audio_bap_stream_t *stream,
                    esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                    esp_ble_audio_bap_ascs_reason_t reason)
{
    if (rsp_code == ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_SUCCESS) {
        ESP_LOGI(TAG, "[%s #%d] %s ACK'd",
                 stream_dir_str(stream), stream_index(stream), op);
    } else {
        ESP_LOGW(TAG, "[%s #%d] %s rejected (rsp 0x%02x reason 0x%02x)",
                 stream_dir_str(stream), stream_index(stream), op,
                 rsp_code, reason);
    }
}

static void config_cb(esp_ble_audio_bap_stream_t *stream,
                      esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                      esp_ble_audio_bap_ascs_reason_t reason)
{
    uint8_t stream_count;
    bool ret;
    int err;

    log_rsp("Config", stream, rsp_code, reason);

    stream_configured(stream, rsp_code == ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_SUCCESS);

    ret = configure_stream();
    if (ret == false) {
        return;
    }

    if (configured_stream_count == 0) {
        ESP_LOGW(TAG, "No streams were configured");
        return;
    }

    stream_count = 0;

    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        if (sinks[i].configured == ASCS_RSP_SUCCESS) {
            streams[stream_count++] = &sinks[i].stream;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        if (sources[i].configured == ASCS_RSP_SUCCESS) {
            streams[stream_count++] = &sources[i].stream;
        }
    }

    err = create_group();
    if (err) {
        return;
    }

    err = set_stream_qos();
    if (err) {
        return;
    }
}

static void qos_cb(esp_ble_audio_bap_stream_t *stream,
                   esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                   esp_ble_audio_bap_ascs_reason_t reason)
{
    log_rsp("QoS", stream, rsp_code, reason);

    stream_qos_set(stream, rsp_code == ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_SUCCESS);

    if (is_all_stream_qos_set()) {
        enable_stream();
    }
}

static void enable_cb(esp_ble_audio_bap_stream_t *stream,
                      esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                      esp_ble_audio_bap_ascs_reason_t reason)
{
    bool ret;

    log_rsp("Enable", stream, rsp_code, reason);

    stream_enabled(stream, rsp_code == ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_SUCCESS);

    ret = enable_stream();
    if (ret == false) {
        return;
    }

    connect_stream();
}

static void start_cb(esp_ble_audio_bap_stream_t *stream,
                     esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                     esp_ble_audio_bap_ascs_reason_t reason)
{
    log_rsp("Start", stream, rsp_code, reason);
}

static void stop_cb(esp_ble_audio_bap_stream_t *stream,
                    esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                    esp_ble_audio_bap_ascs_reason_t reason)
{
    log_rsp("Stop", stream, rsp_code, reason);
}

static void disable_cb(esp_ble_audio_bap_stream_t *stream,
                       esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                       esp_ble_audio_bap_ascs_reason_t reason)
{
    log_rsp("Disable", stream, rsp_code, reason);
}

static void metadata_cb(esp_ble_audio_bap_stream_t *stream,
                        esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                        esp_ble_audio_bap_ascs_reason_t reason)
{
    log_rsp("Metadata", stream, rsp_code, reason);
}

static void release_cb(esp_ble_audio_bap_stream_t *stream,
                       esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                       esp_ble_audio_bap_ascs_reason_t reason)
{
    log_rsp("Release", stream, rsp_code, reason);
}

static void pac_record_cb(esp_ble_conn_t *conn,
                          esp_ble_audio_dir_t dir,
                          const esp_ble_audio_codec_cap_t *codec_cap)
{
    if (codec_cap == NULL) {
        return;
    }

    example_print_codec_cap(TAG, codec_cap);
}

static void endpoint_cb(esp_ble_conn_t *conn,
                        esp_ble_audio_dir_t dir,
                        esp_ble_audio_bap_ep_t *ep)
{
    if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
            if (sources[i].ep == NULL) {
                ESP_LOGI(TAG, "[SRC #%zu] Endpoint discovered", i);
                sources[i].ep = ep;
                return;
            }
        }

        ESP_LOGW(TAG, "[SRC] Endpoint dropped (no free slot)");
    } else if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
            if (sinks[i].ep == NULL) {
                ESP_LOGI(TAG, "[SNK #%zu] Endpoint discovered", i);
                sinks[i].ep = ep;
                return;
            }
        }

        ESP_LOGW(TAG, "[SNK] Endpoint dropped (no free slot)");
    }
}

static void discover_cb(esp_ble_conn_t *conn, int err, esp_ble_audio_dir_t dir)
{
    if (conn->handle != default_conn_handle) {
        return;
    }

    if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        if (err) {
            ESP_LOGE(TAG, "[SNK] Discovery failed, err %d", err);
        } else {
            ESP_LOGI(TAG, "[SNK] Discovery complete");

            discover_sources();
        }
    } else if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        if (err) {
            ESP_LOGE(TAG, "[SRC] Discovery failed, err %d", err);
        } else {
            ESP_LOGI(TAG, "[SRC] Discovery complete");

            configure_stream();
        }
    }
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

struct unicast_server_adv_data {
    bool target_matched;
    bool ascs_found;
    uint8_t announcement_type;
    uint32_t audio_contexts;
    uint8_t meta_len;
};

static bool scan_data_cb(uint8_t type, const uint8_t *data,
                         uint8_t data_len, void *user_data)
{
    struct unicast_server_adv_data *adv = user_data;
    uint8_t announcement_type;
    uint32_t audio_contexts;
    uint16_t uuid_val;
    uint8_t meta_len;
    size_t min_size;

    switch (type) {
    case EXAMPLE_AD_TYPE_NAME_COMPLETE:
        adv->target_matched = (data_len == TARGET_DEVICE_NAME_LEN) &&
                              !memcmp(data, TARGET_DEVICE_NAME, TARGET_DEVICE_NAME_LEN);
        if (!adv->target_matched) {
            return false;
        }
        return true;
    case EXAMPLE_AD_TYPE_SERVICE_DATA16:
        break;
    default:
        return true;
    }

    if (data_len < sizeof(uuid_val)) {
        ESP_LOGW(TAG, "AD invalid size %u", data_len);
        return true;
    }

    uuid_val = sys_get_le16(data);

    if (uuid_val != ESP_BLE_AUDIO_UUID_ASCS_VAL) {
        return true;
    }

    min_size = sizeof(uuid_val) + sizeof(announcement_type) +
               sizeof(audio_contexts) + sizeof(meta_len);
    if (data_len < min_size) {
        ESP_LOGW(TAG, "AD invalid size %u", data_len);
        return false;
    }

    announcement_type = data[2];
    audio_contexts = sys_get_le32(data + 3);
    meta_len = data[7];

    adv->announcement_type = announcement_type;
    adv->audio_contexts = audio_contexts;
    adv->meta_len = meta_len;
    adv->ascs_found = true;

    return true;
}

static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    struct unicast_server_adv_data adv = {0};
    int err;

    if (default_conn_handle != CONN_HANDLE_INIT) {
        return;
    }

    /* Check if the advertising is connectable and if ASCS is supported */
    if (event->ext_scan_recv.event_type & EXAMPLE_ADV_PROP_CONNECTABLE) {
        esp_ble_audio_data_parse(event->ext_scan_recv.data,
                                 event->ext_scan_recv.data_len,
                                 scan_data_cb, &adv);
    }

    if (!adv.target_matched || !adv.ascs_found) {
        return;
    }

    ESP_LOGI(TAG, "Unicast server found: type %u contexts 0x%08x",
             adv.announcement_type, adv.audio_contexts);

    /* Stop scanning before connect — NimBLE rejects ble_gap_connect while
     * a discovery procedure is running; Bluedroid prefers the same ordering
     * even though it accepts both. On failure, restart scanning so we don't
     * stall in a no-scan no-conn state. */
    err = ext_scan_stop();
    if (err) {
        ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
        return;
    }

    err = conn_create(event->ext_scan_recv.addr.type,
                      event->ext_scan_recv.addr.val);
    if (err) {
        ESP_LOGE(TAG, "Failed to create conn, err %d", err);
        ext_scan_start();
    }
}

static void acl_connect(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        /* ext_scan_recv stopped scanning before conn_create. acl_disconnect
         * (which restarts scan) only fires on an established connection, so
         * we must resume here to avoid a no-scan / no-conn dead state. */
        ext_scan_start();
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->acl_connect.dst.val));

    default_conn_handle = event->acl_connect.conn_handle;

    err = pairing_start(event->acl_connect.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to initiate security, err %d", err);
        return;
    }
}

static void acl_disconnect(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    default_conn_handle = CONN_HANDLE_INIT;
    disc_completed = false;
    mtu_exchanged = false;

    /* Delete the group before reset_stream_state() memsets the streams:
     * the library walks stream->iso / stream->group to return ISO slots
     * to the pool, and zeroing them first leaks the slot.
     */
    if (unicast_group != NULL) {
        esp_ble_audio_bap_unicast_group_delete(unicast_group);
        unicast_group = NULL;
    }

    reset_stream_state();

    ext_scan_start();
}

static void security_change(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    if (event->security_change.status) {
        security_failed_recover(event->security_change.conn_handle,
                                event->security_change.status);
        return;
    }

    ESP_LOGI(TAG, "Security: handle %u level %u bonded %u",
             event->security_change.conn_handle,
             event->security_change.sec_level,
             event->security_change.bonded);

    err = exchange_mtu(event->security_change.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to exchange MTU, err %d", err);
        return;
    }
}

static void iso_gap_app_cb(esp_ble_audio_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GAP_EVENT_EXT_SCAN_RECV:
        ext_scan_recv(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT:
        acl_connect(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_DISCONNECT:
        acl_disconnect(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_SECURITY_CHANGE:
        security_change(event);
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

    err = esp_ble_audio_gattc_disc_start(event->gatt_mtu_change.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to start service discovery, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Service discovery started: handle %u",
             event->gatt_mtu_change.conn_handle);

    /* Note:
     * MTU exchanged event may arrived after discover completed event.
     */
    mtu_exchanged = true;

    if (disc_completed) {
        (void)discover_sinks();
    }
}

static void gattc_disc_cmpl(esp_ble_audio_gatt_app_event_t *event)
{
    if (event->gattc_disc_cmpl.status) {
        ESP_LOGE(TAG, "Service discovery failed, status %u",
                 event->gattc_disc_cmpl.status);
        return;
    }

    ESP_LOGI(TAG, "Service discovery complete: handle %u",
             event->gattc_disc_cmpl.conn_handle);

    /* Note:
     * Discover completed event may arrived before MTU exchanged event.
     */
    disc_completed = true;

    if (mtu_exchanged) {
        (void)discover_sinks();
    }
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

    err = app_host_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to init host, err %d", err);
        return;
    }

    err = esp_ble_audio_common_init(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(sinks); i++) {
        sinks[i].stream.ops = &stream_ops;
    }

    for (size_t i = 0; i < ARRAY_SIZE(sources); i++) {
        sources[i].stream.ops = &stream_ops;
    }

    reset_stream_state();

    stream_tx_init();

    err = esp_ble_audio_bap_unicast_client_register_cb(&unicast_client_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register client callbacks, err %d", err);
        return;
    }

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    err = set_device_name();
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    ext_scan_start();
}
