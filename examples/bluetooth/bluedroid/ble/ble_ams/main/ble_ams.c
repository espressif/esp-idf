/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "ble_ams.h"

#define BLE_AMS_TAG  "BLE_AMS"

const char *RemoteCommandID_to_String(uint8_t command_id)
{
    switch (command_id) {
    case RemoteCommandIDPlay:
        return "Play";
    case RemoteCommandIDPause:
        return "Pause";
    case RemoteCommandIDTogglePlayPause:
        return "TogglePlayPause";
    case RemoteCommandIDNextTrack:
        return "NextTrack";
    case RemoteCommandIDPreviousTrack:
        return "PreviousTrack";
    case RemoteCommandIDVolumeUp:
        return "VolumeUp";
    case RemoteCommandIDVolumeDown:
        return "VolumeDown";
    case RemoteCommandIDAdvanceRepeatMode:
        return "AdvanceRepeatMode";
    case RemoteCommandIDAdvanceShuffleMode:
        return "AdvanceShuffleMode";
    case RemoteCommandIDSkipForward:
        return "SkipForward";
    case RemoteCommandIDSkipBackward:
        return "SkipBackward";
    case RemoteCommandIDLikeTrack:
        return "LikeTrack";
    case RemoteCommandIDDislikeTrack:
        return "DislikeTrack";
    case RemoteCommandIDBookmarkTrack:
        return "BookmarkTrack";
    default:
        return "Unknown RemoteCommandID";
    }
}

const char *EntityID_to_String(uint8_t entity_id)
{
    switch (entity_id) {
    case EntityIDPlayer:
        return "Player";
    case EntityIDQueue:
        return "Queue";
    case EntityIDTrack:
        return "Track";
    default:
        return "Unknown EntityID";
    }
}

const char *AttributeID_to_String(uint8_t entity_id, uint8_t attribute_id)
{
    switch (entity_id) {
    case EntityIDPlayer:
        switch (attribute_id) {
        case PlayerAttributeIDName:
            return "PlayerName";
        case PlayerAttributeIDPlaybackInfo:
            return "PlaybackInfo";
        case PlayerAttributeIDVolume:
            return "Volume";
        default:
            return "Unknown PlayerAttributeID";
        }
    case EntityIDQueue:
        switch (attribute_id) {
        case QueueAttributeIDIndex:
            return "QueueIndex";
        case QueueAttributeIDCount:
            return "QueueCount";
        case QueueAttributeIDShuffleMode:
            return "ShuffleMode";
        case QueueAttributeIDRepeatMode:
            return "RepeatMode";
        default:
            return "Unknown QueueAttributeID";
        }
    case EntityIDTrack:
        switch (attribute_id) {
        case TrackAttributeIDArtist:
            return "Artist";
        case TrackAttributeIDAlbum:
            return "Album";
        case TrackAttributeIDTitle:
            return "Title";
        case TrackAttributeIDDuration:
            return "Duration";
        default:
            return "Unknown TrackAttributeID";
        }
    default:
        return "Unknown AttributeID";
    }
}

const char *PlaybackState_to_String(uint8_t playback_state)
{
    switch (playback_state) {
    case PlaybackStatePaused:
        return "Paused";
    case PlaybackStatePlaying:
        return "Playing";
    case PlaybackStateRewinding:
        return "Rewinding";
    case PlaybackStateFastForwarding:
        return "FastForwarding";
    default:
        return "Unknown PlaybackState";
    }
}

static void log_playback_info(const uint8_t *value, uint16_t value_len)
{
    if (!value || value_len == 0) {
        return;
    }

    char buffer[256];
    uint16_t copy_len = value_len < (sizeof(buffer) - 1) ? value_len : (sizeof(buffer) - 1);
    memcpy(buffer, value, copy_len);
    buffer[copy_len] = '\0';

    char *state = buffer;
    char *rate = NULL;
    char *elapsed = NULL;
    char *separator = strchr(state, ',');
    if (separator) {
        *separator = '\0';
        rate = separator + 1;
        separator = strchr(rate, ',');
        if (separator) {
            *separator = '\0';
            elapsed = separator + 1;
        }
    }

    if (state[0] != '\0') {
        int playback_state = atoi(state);
        ESP_LOGI(BLE_AMS_TAG, "  PlaybackState: %s", PlaybackState_to_String((uint8_t)playback_state));
    }
    if (rate && rate[0] != '\0') {
        ESP_LOGI(BLE_AMS_TAG, "  PlaybackRate: %s", rate);
    }
    if (elapsed && elapsed[0] != '\0') {
        ESP_LOGI(BLE_AMS_TAG, "  ElapsedTime: %s s", elapsed);
    }
}

void esp_receive_remote_command_notification(uint8_t *message, uint16_t message_len)
{
    if (!message || message_len == 0) {
        return;
    }

    ESP_LOGI(BLE_AMS_TAG, "Supported remote commands (%u):", message_len);
    for (uint16_t i = 0; i < message_len; i++) {
        ESP_LOGI(BLE_AMS_TAG, "  [%u] %s", i, RemoteCommandID_to_String(message[i]));
    }
}

void esp_receive_entity_update(uint8_t *message, uint16_t message_len)
{
    if (!message || message_len < 3) {
        return;
    }

    uint8_t entity_id = message[0];
    uint8_t attribute_id = message[1];
    uint8_t flags = message[2];
    uint8_t *value = &message[3];
    uint16_t value_len = message_len - 3;

    ESP_LOGI(BLE_AMS_TAG, "Entity update: %s / %s, flags=0x%02x, len=%u",
             EntityID_to_String(entity_id),
             AttributeID_to_String(entity_id, attribute_id),
             flags,
             value_len);

    if (entity_id == EntityIDPlayer && attribute_id == PlayerAttributeIDPlaybackInfo) {
        log_playback_info(value, value_len);
        return;
    }

    if (value_len > 0) {
        ESP_LOG_BUFFER_CHAR("Value", value, value_len);
    }

    if (flags & EntityUpdateFlagTruncated) {
        ESP_LOGI(BLE_AMS_TAG, "Value truncated; read full value from Entity Attribute characteristic");
    }
}

void esp_receive_entity_attribute_value(uint8_t entity_id, uint8_t attribute_id,
                                        uint8_t *message, uint16_t message_len)
{
    ESP_LOGI(BLE_AMS_TAG, "Full entity attribute: %s / %s, len=%u",
             EntityID_to_String(entity_id),
             AttributeID_to_String(entity_id, attribute_id),
             message_len);

    if (entity_id == EntityIDPlayer && attribute_id == PlayerAttributeIDPlaybackInfo) {
        log_playback_info(message, message_len);
        return;
    }

    if (message && message_len > 0) {
        ESP_LOG_BUFFER_CHAR("Value", message, message_len);
    }
}

const char *Errcode_to_String(uint16_t status)
{
    switch (status) {
    case Invalid_state:
        return "Invalid_state";
    case Invalid_command:
        return "Invalid_command";
    case Absent_attribute:
        return "Absent_attribute";
    default:
        return "unknown_failed";
    }
}
