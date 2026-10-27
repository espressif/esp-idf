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

/* Lead: how long before its SDU reference time the controller handles an SDU.
 * The best lead of a window follows the difference between the TX clock and
 * the controller clock, while a stall lowers only some SDUs and the ticks then
 * catch up. The minimum is below the lead of a first SDU without timestamp.
 */
#define TX_LEAD_WINDOW_SDUS     50
#define TX_LEAD_MIN_US          200

#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
#define TX_SYNC_TAG             "AUDIO_TX_SYNC"
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

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
    int32_t count;

    assert(scheduler);
    assert(scheduler->cb);

    count = 1 + scheduler->drift;
    scheduler->drift = 0;

    for (int32_t i = 0; i < count; i++) {
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
    scheduler->lead_cnt = 0;
    scheduler->lead_max = INT32_MIN;
}

int example_audio_tx_scheduler_start(example_audio_tx_scheduler_t *scheduler,
                                     uint64_t period_us)
{
    assert(scheduler);
    assert(period_us > 0);

    scheduler->period_us = period_us;

    return k_work_schedule_periodic_us(&scheduler->timer, period_us);
}

int example_audio_tx_scheduler_stop(example_audio_tx_scheduler_t *scheduler)
{
    assert(scheduler);

    return k_work_cancel_delayable(&scheduler->timer);
}

/* One frame more when even the best lead runs out, one less when it grows by
 * two SDU intervals. A clock difference moves the lead slowly and is caught at
 * the minimum, so a best lead below minus one SDU interval is a stall or a
 * backlog: one more frame would only add to it.
 */
static void tx_lead_check(example_audio_tx_scheduler_t *scheduler, int32_t lead)
{
    /* SDUs sent before a change are still reported: skip a window. */
    if (scheduler->lead_cnt < 0) {
        scheduler->lead_cnt++;
        return;
    }

    if (lead > scheduler->lead_max) {
        scheduler->lead_max = lead;
    }

    if (++scheduler->lead_cnt < TX_LEAD_WINDOW_SDUS) {
        return;
    }

    scheduler->lead_cnt = 0;
    if (scheduler->lead_max < TX_LEAD_MIN_US &&
            scheduler->lead_max > -(int32_t)scheduler->period_us) {
        scheduler->drift++;
        scheduler->lead_cnt = -TX_LEAD_WINDOW_SDUS;
    } else if (scheduler->lead_max > TX_LEAD_MIN_US + 2 * (int32_t)scheduler->period_us) {
        scheduler->drift--;
        scheduler->lead_cnt = -TX_LEAD_WINDOW_SDUS;
    }
    scheduler->lead_max = INT32_MIN;
}

void example_audio_tx_scheduler_on_sent(example_audio_tx_scheduler_t *scheduler,
                                        const esp_ble_iso_tx_cb_info_t *info,
                                        example_audio_tx_count_t *count,
                                        const char *tag,
                                        const char *obj_name)
{
    const char *name = obj_name ? obj_name : "ISO";
    uint32_t prev;
    bool unframed;
    bool late;

    assert(scheduler);
    assert(info);
    assert(count);

    prev = count->sdus;
    unframed = info->framing == ESP_BLE_ISO_FRAMING_UNFRAMED;

    for (size_t i = 0; i < info->sdu_cnt; i++) {
        /* An SDU reported early (more set) comes again; take the final report. */
        if (unframed && info->sdu[i].unf.more) {
            continue;
        }

        late = unframed && info->sdu[i].unf.late;

        if (late) {
            count->late++;
        }
        if (unframed) {
            tx_lead_check(scheduler, (int32_t)(info->sdu[i].sdu_ref_time -
                                               info->sdu[i].sdu_hdl_time));
        }
        count->sdus++;
    }

    count->sdus += info->sdu_dropped;

    /* The count can step by more than one. */
    if (prev / LOG_INTERVAL_PACKETS != count->sdus / LOG_INTERVAL_PACKETS) {
        if (count->late) {
            ESP_LOGI(tag, "[%s] TX: %u packets, %u late", name, count->sdus, count->late);
            count->late = 0;
        } else {
            ESP_LOGI(tag, "[%s] TX: %u packets", name, count->sdus);
        }
    }
}

#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
static example_audio_tx_sync_stream_t *tx_sync_stream(example_audio_tx_sync_t *tx,
                                                      const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx->streams); i++) {
        if (tx->streams[i].stream == stream) {
            return &tx->streams[i];
        }
    }

    return NULL;
}

/* The first stream started, or NULL. */
static example_audio_tx_sync_stream_t *tx_sync_stream_first(example_audio_tx_sync_t *tx)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx->streams); i++) {
        if (tx->streams[i].stream) {
            return &tx->streams[i];
        }
    }

    return NULL;
}

int example_audio_tx_sync_start(example_audio_tx_sync_t *tx, esp_ble_audio_bap_stream_t *stream)
{
    example_audio_tx_sync_stream_t *s;
    esp_ble_iso_info_t info;
    int err;

    assert(tx && tx->send && stream);

    if (stream->iso == NULL || stream->qos == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    err = esp_ble_iso_chan_get_info(stream->iso, &info);
    if (err) {
        return err;
    }

    /* Peripheral anchors drift with the central's clock. */
    if (info.type != BT_ISO_CHAN_TYPE_BROADCASTER && info.type != BT_ISO_CHAN_TYPE_CENTRAL) {
        ESP_LOGE(TX_SYNC_TAG, "Only broadcast source and unicast client are supported");
        return ESP_ERR_NOT_SUPPORTED;
    }

    s = tx_sync_stream(tx, NULL);
    if (s == NULL) {
        return ESP_ERR_NO_MEM;
    }

    s->stream = stream;
    s->seq_num = 0;
    tx->sdu_interval_us = stream->qos->interval;
    tx->iso_interval_us = (uint32_t)info.iso_interval * 1250;

    return 0;
}

void example_audio_tx_sync_stop(example_audio_tx_sync_t *tx, esp_ble_audio_bap_stream_t *stream)
{
    example_audio_tx_sync_stream_t *s = tx_sync_stream(tx, stream);

    if (s == NULL) {
        return;
    }

    s->stream = NULL;
    /* Before sync the others have sent nothing, so another one can lead. */
    if (s == tx->lead) {
        tx->lead = NULL;
    }

    /* The anchors may change before a stream starts again. */
    if (tx_sync_stream_first(tx) == NULL) {
        tx->synced = false;
        tx->framed = false;
    }
}

static void tx_sync_send(example_audio_tx_sync_t *tx, example_audio_tx_sync_stream_t *s)
{
    tx->send(s->stream, s->seq_num++, tx->timestamp, tx->synced);
}

/* Untimestamped SDUs fill the USPI slots of each event in seq order, all reporting
 * its tx_time_stamp; the first of USPI SDUs at one tx_time_stamp starts the event.
 */
static void tx_sync_find(example_audio_tx_sync_t *tx)
{
    esp_ble_iso_tx_info_t info;
    uint16_t uspi;

    if (esp_ble_audio_bap_stream_get_tx_sync(tx->lead->stream, &info)) {
        return;
    }

    /* Left unsynced, so the group does not start. */
    if (info.offset != 0) {
        ESP_LOGW(TX_SYNC_TAG, "Framed data is not supported yet");
        tx->framed = true;
        return;
    }

    uspi = tx->iso_interval_us / tx->sdu_interval_us;
    if (tx->anchor_seq == UINT16_MAX || info.ts != tx->timestamp) {
        tx->timestamp = info.ts;
        tx->anchor_seq = info.seq_num;
    }

    if ((uint16_t)(info.seq_num - tx->anchor_seq + 1) >= uspi) {
        tx->timestamp += (uint16_t)(tx->lead->seq_num - tx->anchor_seq) * tx->sdu_interval_us;
        tx->synced = true;
    }
}

void example_audio_tx_sync_send_next(example_audio_tx_sync_t *tx)
{
    example_audio_tx_sync_stream_t *first = tx_sync_stream_first(tx);

    if (first == NULL || tx->framed) {
        return;
    }

    /* TX Sync blocks about 2.5 ms, so it is read after sending, and only until
     * synced: from then on the controller places SDUs by timestamp.
     */
    if (!tx->synced) {
        if (tx->lead == NULL) {
            tx->lead = first;
            tx->anchor_seq = UINT16_MAX;
        }
        tx_sync_send(tx, tx->lead);
        tx_sync_find(tx);
    } else {
        /* A stream started late joins at this frame, as early as the others. */
        for (size_t i = 0; i < ARRAY_SIZE(tx->streams); i++) {
            if (tx->streams[i].stream) {
                tx_sync_send(tx, &tx->streams[i]);
            }
        }
        tx->timestamp += tx->sdu_interval_us;
    }
}
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

#if CONFIG_EXAMPLE_AUDIO_RX_ALIGN_CHECK
/* In a group (BIG or CIG), SDUs of one SDU reference time share the RX timestamp,
 * so the TX timestamps in their first 4 bytes must match. RX timestamps may differ
 * by a few us (each CIS anchor is estimated on its own), while SDU reference times
 * are at least 255 us apart.
 */
#define RX_TS_TOLERANCE_US      100

static struct {
    uint32_t rx_ts;
    uint32_t tx_ts;
} rx_last;
static uint32_t rx_aligned;
static uint32_t rx_misaligned;

static void rx_ts_check(const esp_ble_iso_recv_info_t *info, const uint8_t *sdu,
                        uint16_t len, const char *tag, const char *name)
{
    uint32_t tx_ts;
    int32_t d;

    /* Lost, error and empty SDUs are in the stream's own counts. */
    if (!(info->flags & ESP_BLE_ISO_FLAGS_VALID) ||
            !(info->flags & ESP_BLE_ISO_FLAGS_TS) ||
            sdu == NULL || len < sizeof(tx_ts)) {
        return;
    }

    /* 0: sent without timestamp, at the start. */
    memcpy(&tx_ts, sdu, sizeof(tx_ts));
    if (tx_ts == 0) {
        return;
    }

    /* Beyond the tolerance, the two are of other SDU reference times: not
     * compared. With FT > 1, an SDU received in a later event keeps its RX
     * timestamp but may come after the other stream's next one.
     */
    d = (int32_t)(info->ts - rx_last.rx_ts);
    if (d >= -RX_TS_TOLERANCE_US && d <= RX_TS_TOLERANCE_US) {
        if (tx_ts == rx_last.tx_ts) {
            rx_aligned++;
        } else if (rx_misaligned++ < 5) {
            ESP_LOGW(tag, "[%s] RX: frame %u us with frame %u us", name, tx_ts, rx_last.tx_ts);
        }

        if ((rx_aligned + rx_misaligned) % LOG_INTERVAL_PACKETS == 0) {
            ESP_LOGI(tag, "RX group: total %u, aligned %u, misaligned %u",
                     rx_aligned + rx_misaligned, rx_aligned, rx_misaligned);
        }
    }

    rx_last.rx_ts = info->ts;
    rx_last.tx_ts = tx_ts;
}
#endif /* CONFIG_EXAMPLE_AUDIO_RX_ALIGN_CHECK */

void example_audio_rx_metrics_reset(example_audio_rx_metrics_t *metrics)
{
    assert(metrics);

    metrics->recv_count = 0;
    metrics->valid_count = 0;
    metrics->error_count = 0;
    metrics->lost_count = 0;
    metrics->null_sdu_count = 0;
    metrics->last_sdu = NULL;
    metrics->last_sdu_len = 0;

#if CONFIG_EXAMPLE_AUDIO_RX_ALIGN_CHECK
    /* A stream (re)starts: check the group from scratch. */
    memset(&rx_last, 0, sizeof(rx_last));
    rx_aligned = 0;
    rx_misaligned = 0;
#endif /* CONFIG_EXAMPLE_AUDIO_RX_ALIGN_CHECK */
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
        ESP_LOGI(tag, "[%s] RX: %u packets, valid %u (empty %u), error %u, lost %u",
                 obj_name ? obj_name : "ISO", metrics->recv_count, metrics->valid_count,
                 metrics->null_sdu_count, metrics->error_count, metrics->lost_count);
    }

#if CONFIG_EXAMPLE_AUDIO_RX_ALIGN_CHECK
    rx_ts_check(info, metrics->last_sdu, metrics->last_sdu_len, tag, obj_name ? obj_name : "ISO");
#endif /* CONFIG_EXAMPLE_AUDIO_RX_ALIGN_CHECK */
}
