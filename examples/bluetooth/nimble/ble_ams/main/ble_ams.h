/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>

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
} ble_remote_command_id_t;

typedef enum {
    EntityIDPlayer = 0,
    EntityIDQueue = 1,
    EntityIDTrack = 2,
} ble_entity_id_t;

typedef enum {
    EntityUpdateFlagTruncated = (1 << 0),
} ble_entity_update_flag_t;

typedef enum {
    PlayerAttributeIDName = 0,
    PlayerAttributeIDPlaybackInfo = 1,
    PlayerAttributeIDVolume = 2,
} ble_player_attribute_id_t;

typedef enum {
    QueueAttributeIDIndex = 0,
    QueueAttributeIDCount = 1,
    QueueAttributeIDShuffleMode = 2,
    QueueAttributeIDRepeatMode = 3,
} ble_queue_attribute_id_t;

typedef enum {
    TrackAttributeIDArtist = 0,
    TrackAttributeIDAlbum = 1,
    TrackAttributeIDTitle = 2,
    TrackAttributeIDDuration = 3,
} ble_track_attribute_id_t;

typedef enum {
    PlaybackStatePaused = 0,
    PlaybackStatePlaying = 1,
    PlaybackStateRewinding = 2,
    PlaybackStateFastForwarding = 3,
} ble_playback_state_t;

const char *RemoteCommandID_to_String(uint8_t command_id);
const char *EntityID_to_String(uint8_t entity_id);
const char *AttributeID_to_String(uint8_t entity_id, uint8_t attribute_id);
const char *PlaybackState_to_String(uint8_t playback_state);

void ble_receive_remote_command_notification(uint8_t *message, uint16_t message_len);
void ble_receive_entity_update(uint8_t *message, uint16_t message_len);
void ble_receive_entity_attribute_value(uint8_t entity_id, uint8_t attribute_id,
                                        uint8_t *message, uint16_t message_len);

void ble_send_remote_command(uint16_t conn_handle, uint8_t command_id);
