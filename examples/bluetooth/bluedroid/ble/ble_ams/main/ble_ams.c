/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "ble_ams.h"

#define BLE_AMS_TAG  "BLE_AMS"

char *EntityID_to_String(uint8_t EntityID)
{
    char *str = NULL;
    switch (EntityID)
    {
        case EntityIDPlayer:
            str = "Player";
            break;
        case EntityIDQueue:
            str = "Queue";
            break;
        case EntityIDTrack:
            str = "Track";
            break;
        default:
            str = "Unknown EntityID";
            break;
    }
    return str;
}

char *PlayerAttributeID_to_String(uint8_t PlayerAttributeID)
{
    char *str = NULL;
    switch (PlayerAttributeID)
    {
        case PlayerAttributeIDName:
            str = "Name";
            break;
        case PlayerAttributeIDPlaybackInfo:
            str = "PlaybackInfo";
            break;
        case PlayerAttributeIDVolume:
            str = "Volume";
            break;
        default:
            str = "Unknown PlayerAttributeID";
            break;
    }
    return str;
}

char *QueueAttributeID_to_String(uint8_t QueueAttributeID)
{
    char *str = NULL;
    switch (QueueAttributeID)
    {
        case QueueAttributeIDIndex:
            str = "Index";
            break;
        case QueueAttributeIDCount:
            str = "Count";
            break;
        case QueueAttributeIDShuffleMode:
            str = "ShuffleMode";
            break;
        case QueueAttributeIDRepeatMode:
            str = "RepeatMode";
            break;
        default:
            str = "Unknown QueueAttributeID";
            break;
    }
    return str;
}

char *TrackAttributeID_to_String(uint8_t TrackAttributeID)
{
    char *str = NULL;
    switch (TrackAttributeID)
    {
        case TrackAttributeIDArtist:
            str = "Artist";
            break;
        case TrackAttributeIDAlbum:
            str = "Album";
            break;
        case TrackAttributeIDTitle:
            str = "Title";
            break;
        case TrackAttributeIDDuration:
            str = "Duration";
            break;
        default:
            str = "Unknown TrackAttributeID";
            break;
    }
    return str;
}

char *RemoteCommandID_to_String(uint8_t RemoteCommandID)
{
    char *str = NULL;
    switch (RemoteCommandID)
    {
        case RemoteCommandIDPlay:
            str = "Play";
            break;
        case RemoteCommandIDPause:
            str = "Pause";
            break;
        case RemoteCommandIDTogglePlayPause:
            str = "TogglePlayPause";
            break;
        case RemoteCommandIDNextTrack:
            str = "NextTrack";
            break;
        case RemoteCommandIDPreviousTrack:
            str = "PreviousTrack";
            break;
        case RemoteCommandIDVolumeUp:
            str = "VolumeUp";
            break;
        case RemoteCommandIDVolumeDown:
            str = "VolumeDown";
            break;
        case RemoteCommandIDAdvanceRepeatMode:
            str = "AdvanceRepeatMode";
            break;
        case RemoteCommandIDAdvanceShuffleMode:
            str = "AdvanceShuffleMode";
            break;
        case RemoteCommandIDSkipForward:
            str = "SkipForward";
            break;
        case RemoteCommandIDSkipBackward:
            str = "SkipBackward";
            break;
        case RemoteCommandIDLikeTrack:
            str = "LikeTrack";
            break;
        case RemoteCommandIDDislikeTrack:
            str = "DislikeTrack";
            break;
        case RemoteCommandIDBookmarkTrack:
            str = "BookmarkTrack";
            break;
        default:
            str = "Unknown RemoteCommandID";
            break;
    }
    return str;
}

char *PlaybackState_to_String(uint8_t PlaybackState)
{
    char *str = NULL;
    switch (PlaybackState)
    {
        case PlaybackStatePaused:
            str = "Paused";
            break;
        case PlaybackStatePlaying:
            str = "Playing";
            break;
        case PlaybackStateRewinding:
            str = "Rewinding";
            break;
        case PlaybackStateFastForwarding:
            str = "FastForwarding";
            break;
        default:
            str = "Unknown PlaybackState";
            break;
    }
    return str;
}

char *ShuffleMode_to_String(uint8_t ShuffleMode)
{
    char *str = NULL;
    switch (ShuffleMode)
    {
        case ShuffleModeOff:
            str = "Off";
            break;
        case ShuffleModeOne:
            str = "One";
            break;
        case ShuffleModeAll:
            str = "All";
            break;
        default:
            str = "Unknown ShuffleMode";
            break;
    }
    return str;
}

char *RepeatMode_to_String(uint8_t RepeatMode)
{
    char *str = NULL;
    switch (RepeatMode)
    {
        case RepeatModeOff:
            str = "Off";
            break;
        case RepeatModeOne:
            str = "One";
            break;
        case RepeatModeAll:
            str = "All";
            break;
        default:
            str = "Unknown RepeatMode";
            break;
    }
    return str;
}

/*
Entity Update format:
| EntityID(1 Byte) | AttributeID(1 Byte) | EntityUpdateFlags(1 Byte) |

The Entity Update characteristic is used by the Media Source to inform a Media Remote 
that one or more attributes of an entity have been updated.
*/
void esp_receive_apple_entity_update(uint8_t *message, uint16_t message_len)
{
    if (!message || message_len < 3) {
        ESP_LOGE(BLE_AMS_TAG, "Invalid entity update message");
        return;
    }

    uint8_t EntityID = message[0];
    uint8_t AttributeID = message[1];
    uint8_t EntityUpdateFlags = message[2];
    
    char *entity_str = EntityID_to_String(EntityID);
    char *attr_str = NULL;
    
    switch (EntityID) {
        case EntityIDPlayer:
            attr_str = PlayerAttributeID_to_String(AttributeID);
            break;
        case EntityIDQueue:
            attr_str = QueueAttributeID_to_String(AttributeID);
            break;
        case EntityIDTrack:
            attr_str = TrackAttributeID_to_String(AttributeID);
            break;
        default:
            attr_str = "Unknown Attribute";
            break;
    }
    
    ESP_LOGI(BLE_AMS_TAG, "Entity Update - Entity:%s Attribute:%s Flags:0x%02x", 
             entity_str, attr_str, EntityUpdateFlags);
    
    if (EntityUpdateFlags & EntityUpdateFlagTruncated) {
        ESP_LOGI(BLE_AMS_TAG, "  - Attribute value was truncated");
    }
}

/*
Entity Attribute format:
| EntityID(1 Byte) | AttributeID(1 Byte) | AttributeValue(variable length) |

The Entity Attribute characteristic is used by a Media Remote to read attributes of entities 
on the Media Source.
*/
void esp_receive_apple_entity_attribute(uint8_t *message, uint16_t message_len)
{
    if (!message || message_len < 2) {
        ESP_LOGE(BLE_AMS_TAG, "Invalid entity attribute message");
        return;
    }

    uint8_t EntityID = message[0];
    uint8_t AttributeID = message[1];
    uint16_t attribute_len = message_len - 2;
    
    char *entity_str = EntityID_to_String(EntityID);
    char *attr_str = NULL;
    
    switch (EntityID) {
        case EntityIDPlayer:
            attr_str = PlayerAttributeID_to_String(AttributeID);
            break;
        case EntityIDQueue:
            attr_str = QueueAttributeID_to_String(AttributeID);
            break;
        case EntityIDTrack:
            attr_str = TrackAttributeID_to_String(AttributeID);
            break;
        default:
            attr_str = "Unknown Attribute";
            break;
    }
    
    ESP_LOGI(BLE_AMS_TAG, "Entity Attribute - Entity:%s Attribute:%s", entity_str, attr_str);
    
    if (attribute_len > 0) {
        // Handle specific attribute types
        switch (EntityID) {
            case EntityIDPlayer:
                switch (AttributeID) {
                    case PlayerAttributeIDName:
                        ESP_LOG_BUFFER_CHAR("Player Name", &message[2], attribute_len);
                        break;
                    case PlayerAttributeIDPlaybackInfo:
                        if (attribute_len >= 3) {
                            char *playback_state = PlaybackState_to_String(message[2]);
                            uint32_t playback_rate = (message[3] << 24) | (message[4] << 16) | (message[5] << 8) | message[6];
                            uint32_t elapsed_time = (message[7] << 24) | (message[8] << 16) | (message[9] << 8) | message[10];
                            ESP_LOGI(BLE_AMS_TAG, "  PlaybackState:%s PlaybackRate:%lu ElapsedTime:%lu", 
                                     playback_state, (unsigned long)playback_rate, (unsigned long)elapsed_time);
                        }
                        break;
                    case PlayerAttributeIDVolume:
                        if (attribute_len >= 4) {
                            float volume = *((float*)&message[2]);
                            ESP_LOGI(BLE_AMS_TAG, "  Volume:%.2f", volume);
                        }
                        break;
                    default:
                        ESP_LOG_BUFFER_HEX("Player Attribute", &message[2], attribute_len);
                        break;
                }
                break;
                
            case EntityIDQueue:
                switch (AttributeID) {
                    case QueueAttributeIDIndex:
                    case QueueAttributeIDCount:
                        ESP_LOG_BUFFER_CHAR("Queue Value", &message[2], attribute_len);
                        break;
                    case QueueAttributeIDShuffleMode:
                        if (attribute_len >= 1) {
                            char *shuffle_str = ShuffleMode_to_String(message[2]);
                            ESP_LOGI(BLE_AMS_TAG, "  ShuffleMode:%s", shuffle_str);
                        }
                        break;
                    case QueueAttributeIDRepeatMode:
                        if (attribute_len >= 1) {
                            char *repeat_str = RepeatMode_to_String(message[2]);
                            ESP_LOGI(BLE_AMS_TAG, "  RepeatMode:%s", repeat_str);
                        }
                        break;
                    default:
                        ESP_LOG_BUFFER_HEX("Queue Attribute", &message[2], attribute_len);
                        break;
                }
                break;
                
            case EntityIDTrack:
                switch (AttributeID) {
                    case TrackAttributeIDArtist:
                        ESP_LOG_BUFFER_CHAR("Track Artist", &message[2], attribute_len);
                        break;
                    case TrackAttributeIDAlbum:
                        ESP_LOG_BUFFER_CHAR("Track Album", &message[2], attribute_len);
                        break;
                    case TrackAttributeIDTitle:
                        ESP_LOG_BUFFER_CHAR("Track Title", &message[2], attribute_len);
                        break;
                    case TrackAttributeIDDuration:
                        ESP_LOG_BUFFER_CHAR("Track Duration", &message[2], attribute_len);
                        break;
                    default:
                        ESP_LOG_BUFFER_HEX("Track Attribute", &message[2], attribute_len);
                        break;
                }
                break;
                
            default:
                ESP_LOG_BUFFER_HEX("Unknown Entity Attribute", &message[2], attribute_len);
                break;
        }
    }
}
