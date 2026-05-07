/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_ble_audio_codec_api.h"

#if CONFIG_BT_BAP_STREAM
esp_err_t esp_ble_audio_codec_cfg_freq_to_freq_hz(esp_ble_audio_codec_cfg_freq_t freq,
                                                  uint32_t *freq_hz)
{
    int ret;

    if (freq_hz == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_freq_to_freq_hz(freq);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *freq_hz = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_freq_hz_to_freq(uint32_t freq_hz,
                                                  esp_ble_audio_codec_cfg_freq_t *freq)
{
    int ret;

    if (freq == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_freq_hz_to_freq(freq_hz);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *freq = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_get_freq(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                           esp_ble_audio_codec_cfg_freq_t *freq)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL || freq == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_get_freq(codec_cfg);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *freq = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_set_freq(esp_ble_audio_codec_cfg_t *codec_cfg,
                                           esp_ble_audio_codec_cfg_freq_t freq)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_set_freq(codec_cfg, freq);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_frame_dur_to_frame_dur_us(esp_ble_audio_codec_cfg_frame_dur_t frame_dur,
                                                            uint32_t *frame_dur_us)
{
    int ret;

    if (frame_dur_us == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_frame_dur_to_frame_dur_us(frame_dur);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *frame_dur_us = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_frame_dur_us_to_frame_dur(uint32_t frame_dur_us,
                                                            esp_ble_audio_codec_cfg_frame_dur_t *frame_dur)
{
    int ret;

    if (frame_dur == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_frame_dur_us_to_frame_dur(frame_dur_us);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *frame_dur = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_get_frame_dur(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                esp_ble_audio_codec_cfg_frame_dur_t *frame_dur)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL || frame_dur == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_get_frame_dur(codec_cfg);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *frame_dur = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_set_frame_dur(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                esp_ble_audio_codec_cfg_frame_dur_t frame_dur)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_set_frame_dur(codec_cfg, frame_dur);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_get_chan_allocation(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                      esp_ble_audio_location_t *chan_allocation,
                                                      bool fallback_to_default)
{
    int err;

    if (codec_cfg == NULL || codec_cfg->data == NULL || chan_allocation == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_audio_codec_cfg_get_chan_allocation(codec_cfg, chan_allocation, fallback_to_default);
    if (err < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_set_chan_allocation(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                      esp_ble_audio_location_t chan_allocation)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL ||
            (chan_allocation & ESP_BLE_AUDIO_LOCATION_ANY) != chan_allocation) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_set_chan_allocation(codec_cfg, chan_allocation);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_get_octets_per_frame(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                       uint16_t *octets_per_frame)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL || octets_per_frame == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_get_octets_per_frame(codec_cfg);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *octets_per_frame = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_set_octets_per_frame(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                       uint16_t octets_per_frame)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_set_octets_per_frame(codec_cfg, octets_per_frame);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_get_frame_blocks_per_sdu(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                           uint8_t *frame_blocks,
                                                           bool fallback_to_default)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL || frame_blocks == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_get_frame_blocks_per_sdu(codec_cfg, fallback_to_default);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *frame_blocks = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_set_frame_blocks_per_sdu(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                           uint8_t frame_blocks)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_set_frame_blocks_per_sdu(codec_cfg, frame_blocks);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_get_val(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                          esp_ble_audio_codec_cfg_type_t type,
                                          const uint8_t **data, size_t *data_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL ||
            data == NULL || data_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_get_val(codec_cfg, type, data);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *data_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_set_val(esp_ble_audio_codec_cfg_t *codec_cfg,
                                          esp_ble_audio_codec_cfg_type_t type,
                                          const uint8_t *data, size_t data_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL ||
            data == NULL ||
            data_len == 0 || data_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_set_val(codec_cfg, type, data, data_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_unset_val(esp_ble_audio_codec_cfg_t *codec_cfg,
                                            esp_ble_audio_codec_cfg_type_t type)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_unset_val(codec_cfg, type);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_val(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                               esp_ble_audio_metadata_type_t type,
                                               const uint8_t **data, size_t *data_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            data == NULL || data_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_val(codec_cfg, type, data);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *data_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_val(esp_ble_audio_codec_cfg_t *codec_cfg,
                                               esp_ble_audio_metadata_type_t type,
                                               const uint8_t *data, size_t data_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            data == NULL ||
            data_len == 0 || data_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_val(codec_cfg, type, data, data_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_unset_val(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                 esp_ble_audio_metadata_type_t type)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_unset_val(codec_cfg, type);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_pref_context(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                        esp_ble_audio_context_t *ctx,
                                                        bool fallback_to_default)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL || ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_pref_context(codec_cfg, fallback_to_default);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *ctx = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_pref_context(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                        esp_ble_audio_context_t ctx)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            (ctx & ESP_BLE_AUDIO_CONTEXT_TYPE_ANY) != ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_pref_context(codec_cfg, ctx);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_stream_context(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                          esp_ble_audio_context_t *ctx)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL || ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_stream_context(codec_cfg);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *ctx = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_stream_context(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                          esp_ble_audio_context_t ctx)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            (ctx & ESP_BLE_AUDIO_CONTEXT_TYPE_ANY) != ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_stream_context(codec_cfg, ctx);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_program_info(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                        const uint8_t **program_info,
                                                        size_t *program_info_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            program_info == NULL || program_info_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_program_info(codec_cfg, program_info);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *program_info_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_program_info(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                        const uint8_t *program_info,
                                                        size_t program_info_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            program_info == NULL ||
            program_info_len == 0 || program_info_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_program_info(codec_cfg, program_info, program_info_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_language(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                    const uint8_t **language)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL || language == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_lang(codec_cfg, language);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_language(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                    const uint8_t language[ESP_BLE_AUDIO_LANG_SIZE])
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL || language == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_lang(codec_cfg, language);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_ccid_list(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                     const uint8_t **ccid_list,
                                                     size_t *ccid_list_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            ccid_list == NULL || ccid_list_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_ccid_list(codec_cfg, ccid_list);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *ccid_list_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_ccid_list(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                     const uint8_t *ccid_list,
                                                     size_t ccid_list_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            ccid_list == NULL ||
            ccid_list_len == 0 || ccid_list_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_ccid_list(codec_cfg, ccid_list, ccid_list_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_parental_rating(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                           esp_ble_audio_parental_rating_t *parental_rating)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL || parental_rating == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_parental_rating(codec_cfg);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *parental_rating = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_parental_rating(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                           esp_ble_audio_parental_rating_t parental_rating)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            parental_rating > ESP_BLE_AUDIO_PARENTAL_RATING_AGE_18_OR_ABOVE) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_parental_rating(codec_cfg, parental_rating);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_program_info_uri(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                            const uint8_t **program_info_uri,
                                                            size_t *program_info_uri_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            program_info_uri == NULL || program_info_uri_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_program_info_uri(codec_cfg, program_info_uri);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *program_info_uri_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_program_info_uri(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                            const uint8_t *program_info_uri,
                                                            size_t program_info_uri_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            program_info_uri == NULL ||
            program_info_uri_len == 0 || program_info_uri_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_program_info_uri(codec_cfg, program_info_uri, program_info_uri_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_audio_active_state(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                              esp_ble_audio_active_state_t *state)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL || state == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_audio_active_state(codec_cfg);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *state = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_audio_active_state(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                              esp_ble_audio_active_state_t state)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            (state != ESP_BLE_AUDIO_ACTIVE_STATE_DISABLED &&
             state != ESP_BLE_AUDIO_ACTIVE_STATE_ENABLED)) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_audio_active_state(codec_cfg, state);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_broadcast_audio_immediate_rend_flag(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                                               bool *found)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL || found == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_bcast_audio_immediate_rend_flag(codec_cfg);
    if (ret == 0) {
        *found = true;
    } else if (ret == -ENODATA) {
        *found = false;
    } else {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_broadcast_audio_immediate_rend_flag(esp_ble_audio_codec_cfg_t *codec_cfg)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_bcast_audio_immediate_rend_flag(codec_cfg);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_assisted_listening_stream(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                                     esp_ble_audio_assisted_listening_stream_t *val)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL || val == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_assisted_listening_stream(codec_cfg);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *val = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_assisted_listening_stream(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                                     esp_ble_audio_assisted_listening_stream_t val)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_assisted_listening_stream(codec_cfg, val);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_broadcast_name(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                          const uint8_t **broadcast_name,
                                                          size_t *broadcast_name_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            broadcast_name == NULL || broadcast_name_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_broadcast_name(codec_cfg, broadcast_name);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *broadcast_name_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_broadcast_name(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                          const uint8_t *broadcast_name,
                                                          size_t broadcast_name_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            broadcast_name == NULL ||
            broadcast_name_len == 0 || broadcast_name_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_broadcast_name(codec_cfg, broadcast_name, broadcast_name_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_extended(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                    const uint8_t **extended_meta,
                                                    size_t *extended_meta_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            extended_meta == NULL || extended_meta_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_extended(codec_cfg, extended_meta);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *extended_meta_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_extended(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                    const uint8_t *extended_meta,
                                                    size_t extended_meta_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            extended_meta == NULL ||
            extended_meta_len == 0 || extended_meta_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_extended(codec_cfg, extended_meta, extended_meta_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_get_vendor(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                  const uint8_t **vendor_meta,
                                                  size_t *vendor_meta_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            vendor_meta == NULL || vendor_meta_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_get_vendor(codec_cfg, vendor_meta);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *vendor_meta_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cfg_meta_set_vendor(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                  const uint8_t *vendor_meta,
                                                  size_t vendor_meta_len)
{
    int ret;

    if (codec_cfg == NULL || codec_cfg->meta == NULL ||
            vendor_meta == NULL ||
            vendor_meta_len == 0 || vendor_meta_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cfg_meta_set_vendor(codec_cfg, vendor_meta, vendor_meta_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_get_val(const esp_ble_audio_codec_cap_t *codec_cap,
                                          esp_ble_audio_codec_cap_type_t type,
                                          const uint8_t **data, size_t *data_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL ||
            data == NULL || data_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_get_val(codec_cap, type, data);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *data_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_set_val(esp_ble_audio_codec_cap_t *codec_cap,
                                          esp_ble_audio_codec_cap_type_t type,
                                          const uint8_t *data, size_t data_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL ||
            data == NULL ||
            data_len == 0 || data_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_set_val(codec_cap, type, data, data_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_unset_val(esp_ble_audio_codec_cap_t *codec_cap,
                                            esp_ble_audio_codec_cap_type_t type)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_unset_val(codec_cap, type);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_get_freq(const esp_ble_audio_codec_cap_t *codec_cap,
                                           esp_ble_audio_codec_cap_freq_t *freq)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL || freq == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_get_freq(codec_cap);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *freq = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_set_freq(esp_ble_audio_codec_cap_t *codec_cap,
                                           esp_ble_audio_codec_cap_freq_t freq)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL ||
            (freq & ESP_BLE_AUDIO_CODEC_CAP_FREQ_ANY) != freq) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_set_freq(codec_cap, freq);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_get_frame_dur(const esp_ble_audio_codec_cap_t *codec_cap,
                                                esp_ble_audio_codec_cap_frame_dur_t *frame_dur)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL || frame_dur == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_get_frame_dur(codec_cap);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *frame_dur = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_set_frame_dur(esp_ble_audio_codec_cap_t *codec_cap,
                                                esp_ble_audio_codec_cap_frame_dur_t frame_dur)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL ||
            (frame_dur & ESP_BLE_AUDIO_CODEC_CAP_DURATION_ANY) == 0 ||
            (frame_dur & ~(ESP_BLE_AUDIO_CODEC_CAP_DURATION_ANY |
                           ESP_BLE_AUDIO_CODEC_CAP_DURATION_PREFER_7_5 |
                           ESP_BLE_AUDIO_CODEC_CAP_DURATION_PREFER_10)) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if ((frame_dur & ESP_BLE_AUDIO_CODEC_CAP_DURATION_PREFER_7_5) != 0) {
        if ((frame_dur & ESP_BLE_AUDIO_CODEC_CAP_DURATION_PREFER_10) != 0) {
            /* Cannot prefer both 7.5 and 10ms */
            return ESP_ERR_INVALID_ARG;
        }

        if ((frame_dur & ESP_BLE_AUDIO_CODEC_CAP_DURATION_7_5) == 0) {
            /* Cannot prefer 7.5ms when not supported */
            return ESP_ERR_INVALID_ARG;
        }
    }

    if ((frame_dur & ESP_BLE_AUDIO_CODEC_CAP_DURATION_PREFER_10) != 0 &&
            (frame_dur & ESP_BLE_AUDIO_CODEC_CAP_DURATION_10) == 0) {
        /* Cannot prefer 10ms when not supported */
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_set_frame_dur(codec_cap, frame_dur);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_get_supported_audio_chan_counts(const esp_ble_audio_codec_cap_t *codec_cap,
                                                                  esp_ble_audio_codec_cap_chan_count_t *chan_count,
                                                                  bool fallback_to_default)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL || chan_count == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_get_supported_audio_chan_counts(codec_cap, fallback_to_default);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *chan_count = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_set_supported_audio_chan_counts(esp_ble_audio_codec_cap_t *codec_cap,
                                                                  esp_ble_audio_codec_cap_chan_count_t chan_count)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL ||
            (chan_count & ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_ANY) != chan_count) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_set_supported_audio_chan_counts(codec_cap, chan_count);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_get_octets_per_frame(const esp_ble_audio_codec_cap_t *codec_cap,
                                                       esp_ble_audio_codec_octets_per_codec_frame_t *codec_frame)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL || codec_frame == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_get_octets_per_frame(codec_cap, codec_frame);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_set_octets_per_frame(esp_ble_audio_codec_cap_t *codec_cap,
                                                       const esp_ble_audio_codec_octets_per_codec_frame_t *codec_frame)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL ||
            codec_frame == NULL || codec_frame->min > codec_frame->max) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_set_octets_per_frame(codec_cap, codec_frame);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_get_max_codec_frames_per_sdu(const esp_ble_audio_codec_cap_t *codec_cap,
                                                               uint8_t *codec_frames_per_sdu,
                                                               bool fallback_to_default)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL || codec_frames_per_sdu == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_get_max_codec_frames_per_sdu(codec_cap, fallback_to_default);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *codec_frames_per_sdu = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_set_max_codec_frames_per_sdu(esp_ble_audio_codec_cap_t *codec_cap,
                                                               uint8_t codec_frames_per_sdu)
{
    int ret;

    if (codec_cap == NULL || codec_cap->data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_set_max_codec_frames_per_sdu(codec_cap, codec_frames_per_sdu);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_val(const esp_ble_audio_codec_cap_t *codec_cap,
                                               esp_ble_audio_metadata_type_t type,
                                               const uint8_t **data, size_t *data_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            data == NULL || data_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_val(codec_cap, type, data);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *data_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_val(esp_ble_audio_codec_cap_t *codec_cap,
                                               esp_ble_audio_metadata_type_t type,
                                               const uint8_t *data, size_t data_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            data == NULL ||
            data_len == 0 || data_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_val(codec_cap, type, data, data_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_unset_val(esp_ble_audio_codec_cap_t *codec_cap,
                                                 esp_ble_audio_metadata_type_t type)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_unset_val(codec_cap, type);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_pref_context(const esp_ble_audio_codec_cap_t *codec_cap,
                                                        esp_ble_audio_context_t *ctx)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL || ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_pref_context(codec_cap);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *ctx = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_pref_context(esp_ble_audio_codec_cap_t *codec_cap,
                                                        esp_ble_audio_context_t ctx)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            (ctx & ESP_BLE_AUDIO_CONTEXT_TYPE_ANY) != ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_pref_context(codec_cap, ctx);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_stream_context(const esp_ble_audio_codec_cap_t *codec_cap,
                                                          esp_ble_audio_context_t *ctx)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL || ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_stream_context(codec_cap);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *ctx = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_stream_context(esp_ble_audio_codec_cap_t *codec_cap,
                                                          esp_ble_audio_context_t ctx)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            (ctx & ESP_BLE_AUDIO_CONTEXT_TYPE_ANY) != ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_stream_context(codec_cap, ctx);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_program_info(const esp_ble_audio_codec_cap_t *codec_cap,
                                                        const uint8_t **program_info,
                                                        size_t *program_info_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            program_info == NULL || program_info_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_program_info(codec_cap, program_info);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *program_info_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_program_info(esp_ble_audio_codec_cap_t *codec_cap,
                                                        const uint8_t *program_info,
                                                        size_t program_info_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            program_info == NULL ||
            program_info_len == 0 || program_info_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_program_info(codec_cap, program_info, program_info_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_language(const esp_ble_audio_codec_cap_t *codec_cap,
                                                    const uint8_t **language)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL || language == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_lang(codec_cap, language);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_language(esp_ble_audio_codec_cap_t *codec_cap,
                                                    const uint8_t language[ESP_BLE_AUDIO_LANG_SIZE])
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL || language == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_lang(codec_cap, language);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_ccid_list(const esp_ble_audio_codec_cap_t *codec_cap,
                                                     const uint8_t **ccid_list,
                                                     size_t *ccid_list_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            ccid_list == NULL || ccid_list_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_ccid_list(codec_cap, ccid_list);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *ccid_list_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_ccid_list(esp_ble_audio_codec_cap_t *codec_cap,
                                                     const uint8_t *ccid_list,
                                                     size_t ccid_list_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            ccid_list == NULL ||
            ccid_list_len == 0 || ccid_list_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_ccid_list(codec_cap, ccid_list, ccid_list_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_parental_rating(const esp_ble_audio_codec_cap_t *codec_cap,
                                                           esp_ble_audio_parental_rating_t *parental_rating)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL || parental_rating == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_parental_rating(codec_cap);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *parental_rating = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_parental_rating(esp_ble_audio_codec_cap_t *codec_cap,
                                                           esp_ble_audio_parental_rating_t parental_rating)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            parental_rating > ESP_BLE_AUDIO_PARENTAL_RATING_AGE_18_OR_ABOVE) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_parental_rating(codec_cap, parental_rating);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_program_info_uri(const esp_ble_audio_codec_cap_t *codec_cap,
                                                            const uint8_t **program_info_uri,
                                                            size_t *program_info_uri_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            program_info_uri == NULL || program_info_uri_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_program_info_uri(codec_cap, program_info_uri);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *program_info_uri_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_program_info_uri(esp_ble_audio_codec_cap_t *codec_cap,
                                                            const uint8_t *program_info_uri,
                                                            size_t program_info_uri_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            program_info_uri == NULL ||
            program_info_uri_len == 0 || program_info_uri_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_program_info_uri(codec_cap, program_info_uri, program_info_uri_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_audio_active_state(const esp_ble_audio_codec_cap_t *codec_cap,
                                                              esp_ble_audio_active_state_t *state)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL || state == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_audio_active_state(codec_cap);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *state = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_audio_active_state(esp_ble_audio_codec_cap_t *codec_cap,
                                                              esp_ble_audio_active_state_t state)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            (state != ESP_BLE_AUDIO_ACTIVE_STATE_DISABLED &&
             state != ESP_BLE_AUDIO_ACTIVE_STATE_ENABLED)) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_audio_active_state(codec_cap, state);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_broadcast_audio_immediate_rend_flag(const esp_ble_audio_codec_cap_t *codec_cap,
                                                                               bool *found)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL || found == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_bcast_audio_immediate_rend_flag(codec_cap);
    if (ret == 0) {
        *found = true;
    } else if (ret == -ENODATA) {
        *found = false;
    } else {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_broadcast_audio_immediate_rend_flag(esp_ble_audio_codec_cap_t *codec_cap)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_bcast_audio_immediate_rend_flag(codec_cap);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_assisted_listening_stream(const esp_ble_audio_codec_cap_t *codec_cap,
                                                                     esp_ble_audio_assisted_listening_stream_t *val)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL || val == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_assisted_listening_stream(codec_cap);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *val = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_assisted_listening_stream(esp_ble_audio_codec_cap_t *codec_cap,
                                                                     esp_ble_audio_assisted_listening_stream_t val)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_assisted_listening_stream(codec_cap, val);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_broadcast_name(const esp_ble_audio_codec_cap_t *codec_cap,
                                                          const uint8_t **broadcast_name,
                                                          size_t *broadcast_name_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            broadcast_name == NULL || broadcast_name_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_broadcast_name(codec_cap, broadcast_name);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *broadcast_name_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_broadcast_name(esp_ble_audio_codec_cap_t *codec_cap,
                                                          const uint8_t *broadcast_name,
                                                          size_t broadcast_name_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            broadcast_name == NULL ||
            broadcast_name_len == 0 || broadcast_name_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_broadcast_name(codec_cap, broadcast_name, broadcast_name_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_extended(const esp_ble_audio_codec_cap_t *codec_cap,
                                                    const uint8_t **extended_meta,
                                                    size_t *extended_meta_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            extended_meta == NULL || extended_meta_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_extended(codec_cap, extended_meta);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *extended_meta_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_extended(esp_ble_audio_codec_cap_t *codec_cap,
                                                    const uint8_t *extended_meta,
                                                    size_t extended_meta_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            extended_meta == NULL ||
            extended_meta_len == 0 || extended_meta_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_extended(codec_cap, extended_meta, extended_meta_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_get_vendor(const esp_ble_audio_codec_cap_t *codec_cap,
                                                  const uint8_t **vendor_meta,
                                                  size_t *vendor_meta_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            vendor_meta == NULL || vendor_meta_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_get_vendor(codec_cap, vendor_meta);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *vendor_meta_len = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_codec_cap_meta_set_vendor(esp_ble_audio_codec_cap_t *codec_cap,
                                                  const uint8_t *vendor_meta,
                                                  size_t vendor_meta_len)
{
    int ret;

    if (codec_cap == NULL || codec_cap->meta == NULL ||
            vendor_meta == NULL ||
            vendor_meta_len == 0 || vendor_meta_len > UINT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_codec_cap_meta_set_vendor(codec_cap, vendor_meta, vendor_meta_len);
    if (ret < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_STREAM */
