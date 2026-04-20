/*
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2024 Demant A/S
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "esp_log.h"

#include "ble_audio_example_utils.h"

#include "esp_ble_audio_common_api.h"

#define LOG_GREEN   "\033[0;" "32" "m"
#define LOG_RESET   "\033[0m"

/* Packet count between throughput summary logs.
 * At 10 ms SDU interval this is ~60 s; at 7.5 ms ~45 s.
 */
#define LOG_INTERVAL_PACKETS    6000

int example_audio_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    if (event->type == BLE_GAP_EVENT_EXT_DISC ||
            event->type == BLE_GAP_EVENT_PERIODIC_SYNC ||
            event->type == BLE_GAP_EVENT_PERIODIC_REPORT ||
            event->type == BLE_GAP_EVENT_PERIODIC_SYNC_LOST ||
            event->type == BLE_GAP_EVENT_CONNECT ||
            event->type == BLE_GAP_EVENT_DISCONNECT ||
            event->type == BLE_GAP_EVENT_ENC_CHANGE) {
        esp_ble_audio_gap_app_post_event(event->type, event);
    } else if (event->type == BLE_GAP_EVENT_MTU ||
               event->type == BLE_GAP_EVENT_NOTIFY_RX ||
               event->type == BLE_GAP_EVENT_NOTIFY_TX ||
               event->type == BLE_GAP_EVENT_SUBSCRIBE) {
        esp_ble_audio_gatt_app_post_event(event->type, event);
    }

    return 0;
}

static void print_hex(const uint8_t *ptr, size_t len)
{
    while (len--) {
        printf(LOG_GREEN "%02x" LOG_RESET, *ptr++);
    }
}

static void append_audio_contexts(char *buf, size_t buf_size, uint16_t ctx_bitmap)
{
    static const char *const names[] = {
        "Unspecified", "Conversational", "Media", "Game",
        "Instructional", "Voice Assistants", "Live", "Sound Effects",
        "Notifications", "Ringtone", "Alerts", "Emergency Alarm",
    };
    size_t pos = 0;

    buf[0] = '\0';
    if (ctx_bitmap == 0) {
        snprintf(buf, buf_size, "None");
        return;
    }
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        if (ctx_bitmap & (1U << i)) {
            int n = snprintf(buf + pos, buf_size - pos, "%s%s",
                             pos == 0 ? "" : ", ", names[i]);
            if (n < 0 || (size_t)n >= buf_size - pos) {
                break;
            }
            pos += n;
        }
    }
}

static void copy_to_cstr(char *dst, size_t dst_size, const uint8_t *src, uint8_t src_len)
{
    size_t n = (src_len < dst_size - 1) ? src_len : dst_size - 1;

    memcpy(dst, src, n);
    dst[n] = '\0';
}

static void phy_to_str(char *buf, size_t buf_size, uint8_t phy)
{
    size_t pos = 0;

    buf[0] = '\0';
    if (phy & ESP_BLE_ISO_PHY_1M) {
        pos += snprintf(buf + pos, buf_size - pos, "1M");
    }
    if (phy & ESP_BLE_ISO_PHY_2M) {
        pos += snprintf(buf + pos, buf_size - pos, "%s2M", pos ? " " : "");
    }
    if (phy & ESP_BLE_ISO_PHY_CODED) {
        pos += snprintf(buf + pos, buf_size - pos, "%sCoded", pos ? " " : "");
    }
    if (pos == 0) {
        snprintf(buf, buf_size, "0x%02x", phy);
    }
}

static bool print_meta_cb(uint8_t type, const uint8_t *data,
                          uint8_t data_len, void *user_data)
{
    const char *tag = (const char *)user_data;
    char buf[128];

    switch (type) {
    case ESP_BLE_AUDIO_METADATA_TYPE_PREF_CONTEXT:
    case ESP_BLE_AUDIO_METADATA_TYPE_STREAM_CONTEXT: {
        uint16_t ctx = (data_len >= 2)
                       ? (uint16_t)data[0] | ((uint16_t)data[1] << 8)
                       : 0;
        append_audio_contexts(buf, sizeof(buf), ctx);
        ESP_LOGI(tag, "| %-19s: %s",
                 type == ESP_BLE_AUDIO_METADATA_TYPE_PREF_CONTEXT
                 ? "Preferred Context" : "Streaming Context",
                 buf);
        break;
    }
    case ESP_BLE_AUDIO_METADATA_TYPE_PROGRAM_INFO:
        copy_to_cstr(buf, sizeof(buf), data, data_len);
        ESP_LOGI(tag, "| %-19s: %s", "Program Info", buf);
        break;
    case ESP_BLE_AUDIO_METADATA_TYPE_LANG:
        copy_to_cstr(buf, sizeof(buf), data, data_len);
        ESP_LOGI(tag, "| %-19s: %s", "Language", buf);
        break;
    case ESP_BLE_AUDIO_METADATA_TYPE_CCID_LIST: {
        size_t pos = 0;
        buf[0] = '\0';
        for (uint8_t i = 0; i < data_len; i++) {
            int n = snprintf(buf + pos, sizeof(buf) - pos, "%s%u",
                             pos == 0 ? "" : " ", data[i]);
            if (n < 0 || (size_t)n >= sizeof(buf) - pos) {
                break;
            }
            pos += n;
        }
        ESP_LOGI(tag, "| %-19s: %s", "CCID List", buf);
        break;
    }
    case ESP_BLE_AUDIO_METADATA_TYPE_PARENTAL_RATING:
        ESP_LOGI(tag, "| %-19s: %u", "Parental Rating",
                 data_len >= 1 ? data[0] : 0);
        break;
    case ESP_BLE_AUDIO_METADATA_TYPE_PROGRAM_INFO_URI:
        copy_to_cstr(buf, sizeof(buf), data, data_len);
        ESP_LOGI(tag, "| %-19s: %s", "Program Info URI", buf);
        break;
    case ESP_BLE_AUDIO_METADATA_TYPE_AUDIO_STATE:
        ESP_LOGI(tag, "| %-19s: 0x%02x", "Audio Active State",
                 data_len >= 1 ? data[0] : 0);
        break;
    case ESP_BLE_AUDIO_METADATA_TYPE_BROADCAST_IMMEDIATE:
        ESP_LOGI(tag, "| %-19s: (set)", "Broadcast Immediate");
        break;
    case ESP_BLE_AUDIO_METADATA_TYPE_ASSISTED_LISTENING_STREAM:
        ESP_LOGI(tag, "| %-19s: 0x%02x", "Assisted Listening",
                 data_len >= 1 ? data[0] : 0);
        break;
    case ESP_BLE_AUDIO_METADATA_TYPE_BROADCAST_NAME:
        copy_to_cstr(buf, sizeof(buf), data, data_len);
        ESP_LOGI(tag, "| %-19s: %s", "Broadcast Name", buf);
        break;
    case ESP_BLE_AUDIO_METADATA_TYPE_EXTENDED:
    case ESP_BLE_AUDIO_METADATA_TYPE_VENDOR:
    default:
        printf(LOG_GREEN "I (%lu) %s: | meta[type 0x%02x len %u]: " LOG_RESET,
               esp_log_timestamp(), tag, type, data_len);
        print_hex(data, data_len);
        printf("\n");
        break;
    }
    return true;
}

void example_print_meta(const char *tag, const uint8_t *meta, size_t meta_len)
{
    esp_err_t err;

    if (meta_len == 0) {
        return;
    }

    ESP_LOGI(tag, "| Metadata:");
    err = esp_ble_audio_data_parse(meta, meta_len, print_meta_cb, (void *)tag);
    if (err) {
        ESP_LOGE(tag, "Failed to parse meta");
    }
}

static void print_raw_data(const char *tag, const uint8_t *data, size_t data_len)
{
    printf(LOG_GREEN "I (%lu) %s: | data: " LOG_RESET, esp_log_timestamp(), tag);
    print_hex(data, data_len);
    printf("\n");
}

static void print_codec_cfg_lc3(const char *tag, const esp_ble_audio_codec_cfg_t *codec_cfg)
{
    esp_ble_audio_codec_cfg_frame_dur_t frame_dur;
    esp_ble_audio_location_t chan_allocation;
    esp_ble_audio_codec_cfg_freq_t freq;
    uint16_t octets_per_frame;
    uint32_t frame_dur_us;
    uint8_t frame_blocks;
    uint32_t freq_hz;
    esp_err_t err;

    err = esp_ble_audio_codec_cfg_get_freq(codec_cfg, &freq);
    if (err) {
        ESP_LOGE(tag, "Failed to get frequency");
        return;
    }
    err = esp_ble_audio_codec_cfg_freq_to_freq_hz(freq, &freq_hz);
    if (err) {
        ESP_LOGE(tag, "Failed to get frequency hz");
        return;
    }
    ESP_LOGI(tag, "| %-19s: %lu Hz", "Frequency", freq_hz);

    err = esp_ble_audio_codec_cfg_get_frame_dur(codec_cfg, &frame_dur);
    if (err) {
        ESP_LOGE(tag, "Failed to get frame duration");
        return;
    }
    err = esp_ble_audio_codec_cfg_frame_dur_to_frame_dur_us(frame_dur, &frame_dur_us);
    if (err) {
        ESP_LOGE(tag, "Failed to get frame duration us");
        return;
    }
    ESP_LOGI(tag, "| %-19s: %lu us", "Frame Duration", frame_dur_us);

    err = esp_ble_audio_codec_cfg_get_chan_allocation(codec_cfg, &chan_allocation, false);
    if (err) {
        ESP_LOGE(tag, "Failed to get channel allocation");
        return;
    }
    ESP_LOGI(tag, "| %-19s: 0x%08lx", "Channel allocation", chan_allocation);

    err = esp_ble_audio_codec_cfg_get_octets_per_frame(codec_cfg, &octets_per_frame);
    if (err) {
        ESP_LOGE(tag, "Failed to get octets per frame");
        return;
    }
    ESP_LOGI(tag, "| %-19s: %u bytes", "Octets per frame", octets_per_frame);

    err = esp_ble_audio_codec_cfg_get_frame_blocks_per_sdu(codec_cfg, &frame_blocks, true);
    if (err) {
        ESP_LOGE(tag, "Failed to get frame blocks per sdu");
        return;
    }
    ESP_LOGI(tag, "| %-19s: %u", "Frames per SDU", frame_blocks);
}

static void print_codec_cap_lc3(const char *tag, const esp_ble_audio_codec_cap_t *codec_cap)
{
    static const uint16_t freq_khz[] = {
        8, 11, 16, 22, 24, 32, 44, 48, 88, 96, 176, 192, 384,
    };
    esp_ble_audio_codec_cap_freq_t freq;
    esp_ble_audio_codec_cap_frame_dur_t frame_dur;
    esp_ble_audio_codec_cap_chan_count_t chan_count;
    esp_ble_audio_codec_octets_per_codec_frame_t octets;
    uint8_t max_frames;
    char buf[64];
    int pos;
    esp_err_t err;

    err = esp_ble_audio_codec_cap_get_freq(codec_cap, &freq);
    if (err) {
        ESP_LOGE(tag, "Failed to get supported frequencies");
        return;
    }
    pos = 0;
    buf[0] = '\0';
    for (size_t i = 0; i < sizeof(freq_khz) / sizeof(freq_khz[0]); i++) {
        if (freq & (1U << i)) {
            int n = snprintf(buf + pos, sizeof(buf) - pos, "%s%u",
                             pos == 0 ? "" : " ", freq_khz[i]);
            if (n < 0 || (size_t)n >= sizeof(buf) - pos) {
                break;
            }
            pos += n;
        }
    }
    ESP_LOGI(tag, "| %-19s: %s kHz", "Frequencies", buf);

    err = esp_ble_audio_codec_cap_get_frame_dur(codec_cap, &frame_dur);
    if (err) {
        ESP_LOGE(tag, "Failed to get supported frame durations");
        return;
    }
    pos = 0;
    buf[0] = '\0';
    if (frame_dur & ESP_BLE_AUDIO_CODEC_CAP_DURATION_7_5) {
        pos += snprintf(buf + pos, sizeof(buf) - pos, "7.5");
    }
    if (frame_dur & ESP_BLE_AUDIO_CODEC_CAP_DURATION_10) {
        pos += snprintf(buf + pos, sizeof(buf) - pos, "%s10",
                        pos == 0 ? "" : " ");
    }
    ESP_LOGI(tag, "| %-19s: %s ms", "Frame Durations", buf);

    err = esp_ble_audio_codec_cap_get_supported_audio_chan_counts(codec_cap, &chan_count, true);
    if (err) {
        ESP_LOGE(tag, "Failed to get supported channel counts");
        return;
    }
    pos = 0;
    buf[0] = '\0';
    for (size_t i = 0; i < 8; i++) {
        if (chan_count & (1U << i)) {
            int n = snprintf(buf + pos, sizeof(buf) - pos, "%s%u",
                             pos == 0 ? "" : " ", (unsigned)(i + 1));
            if (n < 0 || (size_t)n >= sizeof(buf) - pos) {
                break;
            }
            pos += n;
        }
    }
    ESP_LOGI(tag, "| %-19s: %s", "Channel Counts", buf);

    err = esp_ble_audio_codec_cap_get_octets_per_frame(codec_cap, &octets);
    if (err) {
        ESP_LOGE(tag, "Failed to get supported octets per frame");
        return;
    }
    ESP_LOGI(tag, "| %-19s: %u-%u bytes", "Octets per Frame", octets.min, octets.max);

    err = esp_ble_audio_codec_cap_get_max_codec_frames_per_sdu(codec_cap, &max_frames, true);
    if (err) {
        ESP_LOGE(tag, "Failed to get max frames per SDU");
        return;
    }
    ESP_LOGI(tag, "| %-19s: %u", "Max Frames per SDU", max_frames);
}

void example_print_codec_cfg(const char *tag, const esp_ble_audio_codec_cfg_t *codec_cfg)
{
    ESP_LOGI(tag, "Codec Configuration:");
    ESP_LOGI(tag, "| id 0x%02x cid 0x%04x vid 0x%04x len %u",
             codec_cfg->id, codec_cfg->cid,
             codec_cfg->vid, codec_cfg->data_len);

    if (codec_cfg->id == ESP_BLE_ISO_CODING_FORMAT_LC3) {
        print_codec_cfg_lc3(tag, codec_cfg);
    } else {
        print_raw_data(tag, codec_cfg->data, codec_cfg->data_len);
    }

    example_print_meta(tag, codec_cfg->meta, codec_cfg->meta_len);
}

void example_print_codec_cap(const char *tag, const esp_ble_audio_codec_cap_t *codec_cap)
{
    ESP_LOGI(tag, "Codec Capability:");
    ESP_LOGI(tag, "| id 0x%02x cid 0x%04x vid 0x%04x len %u",
             codec_cap->id, codec_cap->cid,
             codec_cap->vid, codec_cap->data_len);

    if (codec_cap->id == ESP_BLE_ISO_CODING_FORMAT_LC3) {
        print_codec_cap_lc3(tag, codec_cap);
    } else {
        print_raw_data(tag, codec_cap->data, codec_cap->data_len);
    }

    example_print_meta(tag, codec_cap->meta, codec_cap->meta_len);
}

/* Prints QoS fields only. Caller is expected to print a context header
 * ending with ':' so the '| ' lines below naturally attach to that header.
 */
void example_print_qos(const char *tag, const esp_ble_audio_bap_qos_cfg_t *qos)
{
    char phy_str[16];

    phy_to_str(phy_str, sizeof(phy_str), qos->phy);

    ESP_LOGI(tag, "| %-8s: %u us", "interval", qos->interval);
    ESP_LOGI(tag, "| %-8s: %s", "framing", qos->framing ? "framed" : "unframed");
    ESP_LOGI(tag, "| %-8s: %s", "phy", phy_str);
    ESP_LOGI(tag, "| %-8s: %u bytes", "sdu", qos->sdu);
    ESP_LOGI(tag, "| %-8s: %u retransmission(s)", "rtn", qos->rtn);
    ESP_LOGI(tag, "| %-8s: %u ms", "latency", qos->latency);
    ESP_LOGI(tag, "| %-8s: %u us", "pd", qos->pd);
}

/* Prints QoS Preference fields only. Same expectation as example_print_qos. */
void example_print_qos_pref(const char *tag, const esp_ble_audio_bap_qos_cfg_pref_t *pref)
{
    char phy_str[16];

    phy_to_str(phy_str, sizeof(phy_str), pref->phy);

    ESP_LOGI(tag, "| %-11s: %s", "unframed",
             pref->unframed_supported ? "supported" : "not supported");
    ESP_LOGI(tag, "| %-11s: %s", "phy", phy_str);
    ESP_LOGI(tag, "| %-11s: %u retransmission(s)", "rtn", pref->rtn);
    ESP_LOGI(tag, "| %-11s: %u ms", "latency", pref->latency);
    ESP_LOGI(tag, "| %-11s: %u us", "pd_min", pref->pd_min);
    ESP_LOGI(tag, "| %-11s: %u us", "pd_max", pref->pd_max);
    ESP_LOGI(tag, "| %-11s: %u us", "pref_pd_min", pref->pref_pd_min);
    ESP_LOGI(tag, "| %-11s: %u us", "pref_pd_max", pref->pref_pd_max);
}

bool example_is_substring(const char *substr, const char *str)
{
    const size_t sub_str_len = strlen(substr);
    const size_t str_len = strlen(str);

    if (sub_str_len > str_len) {
        return false;
    }

    for (size_t pos = 0; pos < str_len; pos++) {
        if (pos + sub_str_len > str_len) {
            return false;
        }

        if (strncasecmp(substr, &str[pos], sub_str_len) == 0) {
            return true;
        }
    }

    return false;
}

static void example_audio_tx_work_handler(struct k_work *work)
{
    example_audio_tx_scheduler_t *scheduler = work->user_data;
    size_t count;

    assert(scheduler);
    assert(scheduler->cb);

    count = 1 + scheduler->drift;
    scheduler->drift = 0;

    for (size_t i = 0; i < count; i++) {
        scheduler->cb(scheduler->arg);
    }
}

int example_audio_tx_scheduler_init(example_audio_tx_scheduler_t *scheduler,
                                    example_audio_tx_send_cb_t cb,
                                    void *arg)
{
    assert(scheduler);
    assert(cb);

    memset(scheduler, 0, sizeof(*scheduler));
    scheduler->cb = cb;
    scheduler->arg = arg;

    k_work_init_delayable(&scheduler->timer, example_audio_tx_work_handler);
    scheduler->timer.work.user_data = scheduler;

    return 0;
}

void example_audio_tx_scheduler_reset(example_audio_tx_scheduler_t *scheduler)
{
    assert(scheduler);

    scheduler->drift = 0;
    scheduler->count = 0;
}

int example_audio_tx_scheduler_start(example_audio_tx_scheduler_t *scheduler,
                                     uint64_t period_us)
{
    assert(scheduler);
    assert(period_us > 0);

    return k_work_schedule_periodic(&scheduler->timer, (uint32_t)(period_us / 1000));
}

int example_audio_tx_scheduler_stop(example_audio_tx_scheduler_t *scheduler)
{
    assert(scheduler);

    return k_work_cancel_delayable(&scheduler->timer);
}

void example_audio_tx_scheduler_on_sent(example_audio_tx_scheduler_t *scheduler,
                                        const esp_ble_iso_tx_cb_info_t *info,
                                        const char *tag,
                                        const char *obj_name)
{
    size_t drift_count = 0;

    assert(scheduler);
    assert(info);

    for (size_t i = 0; i < info->pkt_cnt; i++) {
        if (info->pkt[i].drift) {
            drift_count++;
        }
    }

    scheduler->drift += drift_count;
    scheduler->count++;

    if (scheduler->count % LOG_INTERVAL_PACKETS == 0) {
        ESP_LOGI(tag, "[%s] TX: %u packets",
                 obj_name ? obj_name : "ISO", scheduler->count);
    }
}

void example_audio_rx_metrics_reset(example_audio_rx_metrics_t *metrics)
{
    assert(metrics);

    metrics->recv_count = 0;
    metrics->valid_count = 0;
    metrics->error_count = 0;
    metrics->lost_count = 0;
    metrics->null_sdu_count = 0;
    metrics->last_sdu_len = 0;
}

void example_audio_rx_metrics_on_recv(const esp_ble_iso_recv_info_t *info,
                                      example_audio_rx_metrics_t *metrics,
                                      const char *tag,
                                      const char *obj_name)
{
    assert(info);
    assert(metrics);

    if (info->flags & ESP_BLE_ISO_FLAGS_ERROR) {
        metrics->error_count++;
    }

    if (info->flags & ESP_BLE_ISO_FLAGS_LOST) {
        metrics->lost_count++;
    }

    if (info->flags & ESP_BLE_ISO_FLAGS_VALID) {
        metrics->valid_count++;
        if (metrics->last_sdu_len == 0) {
            metrics->null_sdu_count++;
        }
    }

    metrics->recv_count++;

    if (metrics->recv_count % LOG_INTERVAL_PACKETS == 0) {
        ESP_LOGI(tag, "[%s] RX: %u packets",
                 obj_name ? obj_name : "ISO", metrics->recv_count);
    }
}
