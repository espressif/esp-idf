/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
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

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_random.h"
#include "esp_timer.h"

#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#include "esp_ble_audio_lc3_defs.h"
#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_pacs_api.h"
#include "esp_ble_audio_bap_lc3_preset_defs.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "BAP_BSRC"

ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_16_2_1_DEFINE(preset_active,
                                                     ESP_BLE_AUDIO_LOCATION_FRONT_LEFT |
                                                     ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT,
                                                     ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED);

#define LOCAL_DEVICE_NAME      "BAP Broadcast Source"
#define LOCAL_DEVICE_NAME_LEN  (sizeof(LOCAL_DEVICE_NAME) - 1)

#define LOCAL_BROADCAST_CODE   "1234"   /* Maximum length is 16 */
#define LOCAL_BROADCAST_ID     0x123456

#define ADV_HANDLE              0x00
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_ADDRESS             BLE_OWN_ADDR_PUBLIC
#define ADV_PRIMARY_PHY         BLE_HCI_LE_PHY_1M
#define ADV_SECONDARY_PHY       BLE_HCI_LE_PHY_2M
#define ADV_INTERVAL            BLE_GAP_ADV_ITVL_MS(200)

#define PER_ADV_INTERVAL        BLE_GAP_ADV_ITVL_MS(100)

#define STREAM_COUNT            CONFIG_BT_BAP_BROADCAST_SRC_STREAM_COUNT
#define SUBGROUP_COUNT          CONFIG_BT_BAP_BROADCAST_SRC_SUBGROUP_COUNT

static struct broadcast_source_stream {
    esp_ble_audio_bap_stream_t stream;
    uint16_t seq_num;
    uint8_t *data;
    example_audio_tx_scheduler_t scheduler;
} streams[STREAM_COUNT];

static esp_ble_audio_bap_broadcast_source_t *broadcast_source;

static void broadcast_source_tx(struct broadcast_source_stream *source_stream);

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        if (&streams[i].stream == stream) {
            return (int)i;
        }
    }
    return -1;
}

static void stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    struct broadcast_source_stream *source_stream = CONTAINER_OF(stream,
                                                                 struct broadcast_source_stream,
                                                                 stream);
    int idx = stream_index(stream);
    esp_err_t err;

    ESP_LOGI(TAG, "[SRC #%d] Stream started", idx);

    if (source_stream->stream.qos == NULL || source_stream->stream.qos->sdu == 0) {
        ESP_LOGE(TAG, "[SRC #%d] Invalid QoS", idx);
        return;
    }

    if (source_stream->data == NULL) {
        source_stream->data = calloc(1, source_stream->stream.qos->sdu);
        if (source_stream->data == NULL) {
            ESP_LOGE(TAG, "[SRC #%d] Failed to alloc TX buffer (sdu %u)",
                     idx, source_stream->stream.qos->sdu);
            return;
        }
    }

    source_stream->seq_num = 0;
    example_audio_tx_scheduler_reset(&source_stream->scheduler);

    /* Note: esp timer is not accurate enough */
    err = example_audio_tx_scheduler_start(&source_stream->scheduler, preset_active.qos.interval);
    if (err) {
        ESP_LOGE(TAG, "[SRC #%d] Scheduler start failed, err %d", idx, err);
        return;
    }

    broadcast_source_tx(source_stream);
}

static void stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    struct broadcast_source_stream *source_stream = CONTAINER_OF(stream,
                                                                 struct broadcast_source_stream,
                                                                 stream);
    int idx = stream_index(stream);
    esp_err_t err;

    ESP_LOGI(TAG, "[SRC #%d] Stream stopped, reason 0x%02x", idx, reason);

    err = example_audio_tx_scheduler_stop(&source_stream->scheduler);
    if (err) {
        ESP_LOGE(TAG, "[SRC #%d] Scheduler stop failed, err %d", idx, err);
    }
}

static void stream_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    struct broadcast_source_stream *source_stream = CONTAINER_OF(stream,
                                                                 struct broadcast_source_stream,
                                                                 stream);
    int idx = stream_index(stream);
    esp_err_t err;

    ESP_LOGI(TAG, "[SRC #%d] ISO disconnected, reason 0x%02x", idx, reason);

    err = example_audio_tx_scheduler_stop(&source_stream->scheduler);
    if (err) {
        ESP_LOGE(TAG, "[SRC #%d] Scheduler stop failed, err %d", idx, err);
    }
}

static void stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    struct broadcast_source_stream *source_stream = CONTAINER_OF(stream,
                                                                 struct broadcast_source_stream,
                                                                 stream);
    char name[24];

    snprintf(name, sizeof(name), "SRC #%d", stream_index(stream));
    example_audio_tx_scheduler_on_sent(&source_stream->scheduler, user_data, TAG, name);
}

static esp_ble_audio_bap_stream_ops_t stream_ops = {
    .started      = stream_started_cb,
    .stopped      = stream_stopped_cb,
    .sent         = stream_sent_cb,
    .disconnected = stream_disconnected_cb,
};

static bool stream_is_streaming(const esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};

    if (stream->ep == NULL) {
        return false;
    }

    if (esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info)) {
        return false;
    }

    return (ep_info.state == ESP_BLE_AUDIO_BAP_EP_STATE_STREAMING);
}

static void broadcast_source_tx(struct broadcast_source_stream *source_stream)
{
    esp_err_t err;

    if (!stream_is_streaming(&source_stream->stream)) {
        return;
    }

    int idx = stream_index(&source_stream->stream);

    if (source_stream->stream.qos == NULL || source_stream->stream.qos->sdu == 0) {
        ESP_LOGE(TAG, "[SRC #%d] Invalid QoS", idx);
        return;
    }

    if (source_stream->data == NULL) {
        ESP_LOGE(TAG, "[SRC #%d] TX buffer unavailable (sdu %u)",
                 idx, source_stream->stream.qos->sdu);
        return;
    }

    memset(source_stream->data, (uint8_t)source_stream->seq_num, source_stream->stream.qos->sdu);

    err = esp_ble_audio_bap_stream_send(&source_stream->stream,
                                        source_stream->data,
                                        source_stream->stream.qos->sdu,
                                        source_stream->seq_num);
    if (err) {
        ESP_LOGD(TAG, "[SRC #%d] send failed, err %d", idx, err);
        return;
    }

    source_stream->seq_num++;
}

static void tx_scheduler_cb(void *arg)
{
    struct broadcast_source_stream *source_stream = arg;

    if (source_stream == NULL) {
        return;
    }

    broadcast_source_tx(source_stream);
}

static void source_started_cb(esp_ble_audio_bap_broadcast_source_t *source)
{
    ESP_LOGI(TAG, "Broadcast source started");
}

static void source_stopped_cb(esp_ble_audio_bap_broadcast_source_t *source, uint8_t reason)
{
    ESP_LOGI(TAG, "Broadcast source stopped, reason 0x%02x", reason);

    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        if (streams[i].data != NULL) {
            free(streams[i].data);
            streams[i].data = NULL;
        }
    }
}

static esp_err_t broadcast_source_setup(void)
{
    esp_ble_audio_bap_broadcast_source_subgroup_param_t subgroup_param[SUBGROUP_COUNT];
    esp_ble_audio_bap_broadcast_source_stream_param_t stream_params[STREAM_COUNT];
    esp_ble_audio_bap_broadcast_source_param_t create_param = {0};
    size_t streams_per_subgroup;
    uint8_t left[] = {
        ESP_BLE_AUDIO_CODEC_DATA(ESP_BLE_AUDIO_CODEC_CFG_CHAN_ALLOC,
                                 EXAMPLE_BYTES_LIST_LE32(ESP_BLE_AUDIO_LOCATION_FRONT_LEFT))
    };
    uint8_t right[] = {
        ESP_BLE_AUDIO_CODEC_DATA(ESP_BLE_AUDIO_CODEC_CFG_CHAN_ALLOC,
                                 EXAMPLE_BYTES_LIST_LE32(ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT))
    };
    static esp_ble_audio_bap_broadcast_source_cb_t broadcast_source_cb = {
        .started = source_started_cb,
        .stopped = source_stopped_cb,
    };
    esp_err_t err;

    err = esp_ble_audio_bap_broadcast_source_register_cb(&broadcast_source_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register broadcast source callbacks, err %d", err);
        return err;
    }

    streams_per_subgroup = ARRAY_SIZE(stream_params) / ARRAY_SIZE(subgroup_param);

    for (size_t i = 0; i < ARRAY_SIZE(subgroup_param); i++) {
        subgroup_param[i].params_count = streams_per_subgroup;
        subgroup_param[i].params = stream_params + i * streams_per_subgroup;
        subgroup_param[i].codec_cfg = &preset_active.codec_cfg;
    }

    for (size_t i = 0; i < ARRAY_SIZE(stream_params); i++) {
        stream_params[i].stream = &streams[i].stream;
        stream_params[i].data = (i == 0 ? left : right);
        stream_params[i].data_len = (i == 0 ? sizeof(left) : sizeof(right));

        esp_ble_audio_bap_stream_cb_register(stream_params[i].stream, &stream_ops);
    }

    create_param.params_count = ARRAY_SIZE(subgroup_param);
    create_param.params = subgroup_param;
    create_param.qos = &preset_active.qos;
    create_param.encryption = (strlen(LOCAL_BROADCAST_CODE) > 0);
    create_param.packing = ESP_BLE_ISO_PACKING_SEQUENTIAL;

    if (create_param.encryption) {
        memcpy(create_param.broadcast_code, LOCAL_BROADCAST_CODE, strlen(LOCAL_BROADCAST_CODE));
    }

    ESP_LOGI(TAG, "Creating broadcast source: %u subgroup(s), %u stream(s)/subgroup",
             ARRAY_SIZE(subgroup_param), streams_per_subgroup);

    err = esp_ble_audio_bap_broadcast_source_create(&create_param, &broadcast_source);
    if (err) {
        ESP_LOGE(TAG, "Failed to create broadcast source, err %d", err);
        return err;
    }

    return 0;
}

static uint8_t *ext_adv_data_get(uint8_t *data_len)
{
    uint32_t broadcast_id;
    uint8_t *data;

    broadcast_id = LOCAL_BROADCAST_ID;

    /* - Broadcast Audio Announcement Service UUID (2 octets) and
     *   Broadcast ID (3 octets)
     * - Complete Device Name
     */
    *data_len = 7 + 2 + LOCAL_DEVICE_NAME_LEN;

    data = calloc(1, *data_len);
    if (data == NULL) {
        return NULL;
    }

    data[0] = 0x06; /* 1 + 2 + 3 */
    data[1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    data[2] = (ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL & 0xFF);
    data[3] = ((ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL >> 8) & 0xFF);
    data[4] = (broadcast_id & 0xFF);
    data[5] = ((broadcast_id >> 8) & 0xFF);
    data[6] = ((broadcast_id >> 16) & 0xFF);

    data[7] = LOCAL_DEVICE_NAME_LEN + 1;
    data[8] = EXAMPLE_AD_TYPE_NAME_COMPLETE;
    memcpy(data + 9, LOCAL_DEVICE_NAME, LOCAL_DEVICE_NAME_LEN);

    return data;
}

static uint8_t *per_adv_data_get(uint8_t *data_len)
{
    NET_BUF_SIMPLE_DEFINE(base_buf, 128);
    uint8_t *data;
    esp_err_t err;

    /* Broadcast Audio Announcement Service UUID (2 octets) and
     * Broadcast Audio Source Endpoint (BASE)
     */

    err = esp_ble_audio_bap_broadcast_source_get_base(broadcast_source, &base_buf);
    if (err) {
        ESP_LOGE(TAG, "Failed to get encoded BASE, err %d", err);
        return NULL;
    }

    *data_len = 2 + base_buf.len;

    data = calloc(1, *data_len);
    if (data == NULL) {
        return NULL;
    }

    /* base_buf.len has included the UUID length (2 octets) */
    data[0] = 1 + base_buf.len;
    data[1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    memcpy(data + 2, base_buf.data, base_buf.len);

    return data;
}

static int ext_adv_start(void)
{
    struct ble_gap_periodic_adv_params per_params = {0};
    struct ble_gap_ext_adv_params ext_params = {0};
    struct os_mbuf *data = NULL;
    uint8_t *ext_data = NULL;
    uint8_t *per_data = NULL;
    uint8_t data_len = 0;
    int err;

    ext_params.connectable = 0;
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
        goto end;
    }

    ext_data = ext_adv_data_get(&data_len);
    if (ext_data == NULL) {
        err = -ENOMEM;
        goto end;
    }

    data = os_msys_get_pkthdr(data_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get ext adv mbuf");
        err = -ENOMEM;
        goto end;
    }

    err = os_mbuf_append(data, ext_data, data_len);
    if (err) {
        ESP_LOGE(TAG, "Failed to append ext adv data, err %d", err);
        os_mbuf_free_chain(data);
        goto end;
    }

    err = ble_gap_ext_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set ext adv data, err %d", err);
        goto end;
    }

    per_params.include_tx_power = 0;
    per_params.itvl_min = PER_ADV_INTERVAL;
    per_params.itvl_max = PER_ADV_INTERVAL;

    err = ble_gap_periodic_adv_configure(ADV_HANDLE, &per_params);
    if (err) {
        ESP_LOGE(TAG, "Failed to configure per adv params, err %d", err);
        goto end;
    }

    per_data = per_adv_data_get(&data_len);
    if (per_data == NULL) {
        err = -ENOMEM;
        goto end;
    }

    data = os_msys_get_pkthdr(data_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get per adv mbuf");
        err = -ENOMEM;
        goto end;
    }

    err = os_mbuf_append(data, per_data, data_len);
    if (err) {
        ESP_LOGE(TAG, "Failed to append per adv data, err %d", err);
        os_mbuf_free_chain(data);
        goto end;
    }

    err = ble_gap_periodic_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set per adv data, err %d", err);
        goto end;
    }

    err = ble_gap_periodic_adv_start(ADV_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "Failed to start per advertising, err %d", err);
        goto end;
    }

    err = ble_gap_ext_adv_start(ADV_HANDLE, 0, 0);
    if (err) {
        ESP_LOGE(TAG, "Failed to start ext advertising, err %d", err);
        goto end;
    }

    ESP_LOGI(TAG, "Advertising started (handle %u)", ADV_HANDLE);

end:
    if (ext_data) {
        free(ext_data);
    }
    if (per_data) {
        free(per_data);
    }
    return err;
}

static void broadcast_start(void)
{
    esp_ble_audio_bap_broadcast_adv_info_t info = {
        .adv_handle = ADV_HANDLE,
    };
    int err;

    err = esp_ble_audio_bap_broadcast_adv_add(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to add adv for broadcast source, err %d", err);
        return;
    }

    err = esp_ble_audio_bap_broadcast_source_start(broadcast_source, ADV_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "Failed to start broadcast source, err %d", err);
        return;
    }
}

void app_main(void)
{
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

    err = esp_ble_audio_common_init(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    err = broadcast_source_setup();
    if (err) {
        return;
    }

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        err = example_audio_tx_scheduler_init(&streams[i].scheduler,
                                              tx_scheduler_cb,
                                              &streams[i]);
        if (err) {
            ESP_LOGE(TAG, "[SRC #%zu] Scheduler init failed, err %d", i, err);
            return;
        }
    }

    err = ext_adv_start();
    if (err) {
        return;
    }

    broadcast_start();
}
