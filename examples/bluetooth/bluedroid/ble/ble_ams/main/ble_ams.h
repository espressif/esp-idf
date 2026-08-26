/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

// RemoteCommandID values (Table A-1)
typedef enum {
    RemoteCommandIDPlay = 0,
    RemoteCommandIDPause = 1,
    RemoteCommandIDTogglePlayPause = 2,
    RemoteCommandIDNextTrack = 3,
    RemoteCommandIDPreviousTrack = 4,
    RemoteCommandIDVolumeUp = 5,
    RemoteCommandIDVolumeDown = 6,
    RemoteCommandIDAdvanceRepeatMode = 7,
    RemoteCommandIDAdvanceShuffleMode = 8,
    RemoteCommandIDSkipForward = 9,
    RemoteCommandIDSkipBackward = 10,
    RemoteCommandIDLikeTrack = 11,
    RemoteCommandIDDislikeTrack = 12,
    RemoteCommandIDBookmarkTrack = 13,
} esp_remote_command_id_t;

// EntityID values (Table A-2)
typedef enum {
    EntityIDPlayer = 0,
    EntityIDQueue = 1,
    EntityIDTrack = 2,
} esp_entity_id_t;

// EntityUpdateFlags (Table A-3)
typedef enum {
    EntityUpdateFlagTruncated = (1 << 0),
} esp_entity_update_flag_t;

// PlayerAttributeID values (Table A-4)
typedef enum {
    PlayerAttributeIDName = 0,
    PlayerAttributeIDPlaybackInfo = 1,
    PlayerAttributeIDVolume = 2,
} esp_player_attribute_id_t;

// QueueAttributeID values (Table A-5)
typedef enum {
    QueueAttributeIDIndex = 0,
    QueueAttributeIDCount = 1,
    QueueAttributeIDShuffleMode = 2,
    QueueAttributeIDRepeatMode = 3,
} esp_queue_attribute_id_t;

// TrackAttributeID values (Table A-8)
typedef enum {
    TrackAttributeIDArtist = 0,
    TrackAttributeIDAlbum = 1,
    TrackAttributeIDTitle = 2,
    TrackAttributeIDDuration = 3,
} esp_track_attribute_id_t;

typedef enum {
    PlaybackStatePaused = 0,
    PlaybackStatePlaying = 1,
    PlaybackStateRewinding = 2,
    PlaybackStateFastForwarding = 3,
} esp_playback_state_t;

typedef enum {
    Invalid_state = 0xA0,
    Invalid_command = 0xA1,
    Absent_attribute = 0xA2,
} esp_ams_error_code_t;

const char *RemoteCommandID_to_String(uint8_t command_id);
const char *EntityID_to_String(uint8_t entity_id);
const char *AttributeID_to_String(uint8_t entity_id, uint8_t attribute_id);
const char *PlaybackState_to_String(uint8_t playback_state);
const char *Errcode_to_String(uint16_t status);

void esp_receive_remote_command_notification(uint8_t *message, uint16_t message_len);
void esp_receive_entity_update(uint8_t *message, uint16_t message_len);
void esp_receive_entity_attribute_value(uint8_t entity_id, uint8_t attribute_id,
                                        uint8_t *message, uint16_t message_len);

esp_err_t esp_send_remote_command(uint8_t command_id);
