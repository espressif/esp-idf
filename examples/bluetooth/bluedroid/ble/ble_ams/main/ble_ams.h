/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Entity ID values for AMS
typedef enum {
    EntityIDPlayer         = 0,
    EntityIDQueue          = 1, 
    EntityIDTrack          = 2,
    //Reserved EntityID values = 3–255
} esp_EntityID;

// Player Attribute ID values
typedef enum {
    PlayerAttributeIDName           = 0,
    PlayerAttributeIDPlaybackInfo   = 1,
    PlayerAttributeIDVolume         = 2,
    //Reserved PlayerAttributeID values = 3–255
} esp_PlayerAttributeID;

// Queue Attribute ID values
typedef enum {
    QueueAttributeIDIndex          = 0,
    QueueAttributeIDCount          = 1,
    QueueAttributeIDShuffleMode    = 2,
    QueueAttributeIDRepeatMode     = 3,
    //Reserved QueueAttributeID values = 4–255
} esp_QueueAttributeID;

// Track Attribute ID values
typedef enum {
    TrackAttributeIDArtist         = 0,
    TrackAttributeIDAlbum          = 1,
    TrackAttributeIDTitle          = 2,
    TrackAttributeIDDuration       = 3,
    //Reserved TrackAttributeID values = 4–255
} esp_TrackAttributeID;

// Remote Command ID values
typedef enum {
    RemoteCommandIDPlay            = 0,
    RemoteCommandIDPause           = 1,
    RemoteCommandIDTogglePlayPause = 2,
    RemoteCommandIDNextTrack       = 3,
    RemoteCommandIDPreviousTrack   = 4,
    RemoteCommandIDVolumeUp        = 5,
    RemoteCommandIDVolumeDown      = 6,
    RemoteCommandIDAdvanceRepeatMode      = 7,
    RemoteCommandIDAdvanceShuffleMode     = 8,
    RemoteCommandIDSkipForward     = 9,
    RemoteCommandIDSkipBackward    = 10,
    RemoteCommandIDLikeTrack       = 11,
    RemoteCommandIDDislikeTrack    = 12,
    RemoteCommandIDBookmarkTrack   = 13,
    //Reserved RemoteCommandID values = 14–255
} esp_RemoteCommandID;

// Playback State values
typedef enum {
    PlaybackStatePaused   = 0,
    PlaybackStatePlaying  = 1,
    PlaybackStateRewinding = 2,
    PlaybackStateFastForwarding = 3,
    //Reserved PlaybackState values = 4–255
} esp_PlaybackState;

// Shuffle Mode values
typedef enum {
    ShuffleModeOff = 0,
    ShuffleModeOne = 1,
    ShuffleModeAll = 2,
    //Reserved ShuffleMode values = 3–255
} esp_ShuffleMode;

// Repeat Mode values
typedef enum {
    RepeatModeOff = 0,
    RepeatModeOne = 1,
    RepeatModeAll = 2,
    //Reserved RepeatMode values = 3–255
} esp_RepeatMode;

// Entity Update Event Flag values
typedef enum {
    EntityUpdateFlagTruncated = (1 << 0),
    //Reserved EntityUpdateEventFlag values = (1 << 1)–(1 << 7)
} esp_EntityUpdateEventFlag;

typedef struct {
    uint8_t  entity_id;
    uint8_t  attribute_id;
    uint8_t  entity_update_flags;
} esp_entity_attr_t;

typedef struct {
    uint8_t  entity_id;
    uint8_t  attribute_id;
} esp_entity_update_t;

typedef enum {
    attr_player_name_index = 0,
    attr_player_playback_info_index,
    attr_player_volume_index,
    attr_queue_index_index,
    attr_queue_count_index,
    attr_queue_shuffle_mode_index,
    attr_queue_repeat_mode_index,
    attr_track_artist_index,
    attr_track_album_index,
    attr_track_title_index,
    attr_track_duration_index,
} esp_ams_attr_index;

// Function declarations
char *EntityID_to_String(uint8_t EntityID);
char *PlayerAttributeID_to_String(uint8_t PlayerAttributeID);
char *QueueAttributeID_to_String(uint8_t QueueAttributeID);
char *TrackAttributeID_to_String(uint8_t TrackAttributeID);
char *RemoteCommandID_to_String(uint8_t RemoteCommandID);
char *PlaybackState_to_String(uint8_t PlaybackState);
char *ShuffleMode_to_String(uint8_t ShuffleMode);
char *RepeatMode_to_String(uint8_t RepeatMode);
void esp_receive_apple_entity_update(uint8_t *message, uint16_t message_len);
void esp_receive_apple_entity_attribute(uint8_t *message, uint16_t message_len);
