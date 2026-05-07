/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_DEFS_H_
#define ESP_BLE_AUDIO_DEFS_H_

#include <stdint.h>
#include <stdbool.h>

#include "sdkconfig.h"

#include <zephyr/bluetooth/audio/audio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!< Minimum ATT MTU size required by Basic Audio Profile (BAP) */
#define ESP_BLE_AUDIO_ATT_MTU_MIN   64

/**
 * Audio direction sink.
 *
 * For a BAP Unicast Client or Broadcast Source this is considered outgoing audio (TX).
 * For a BAP Unicast Server or Broadcast Sink this is considered incoming audio (RX).
 */
#define ESP_BLE_AUDIO_DIR_SINK      BT_AUDIO_DIR_SINK
/**
 * Audio direction source.
 *
 * For a BAP Unicast Client or Broadcast Source this is considered incoming audio (RX).
 * For a BAP Unicast Server or Broadcast Sink this is considered outgoing audio (TX).
 */
#define ESP_BLE_AUDIO_DIR_SOURCE    BT_AUDIO_DIR_SOURCE
/** Audio direction from the perspective of the BAP Unicast Server / BAP Broadcast Sink */
typedef enum bt_audio_dir           esp_ble_audio_dir_t;

/*!< Size of the stream language value, e.g. "eng" */
#define ESP_BLE_AUDIO_LANG_SIZE     BT_AUDIO_LANG_SIZE

/*!< Size of the broadcast ID in octets */
#define ESP_BLE_AUDIO_BROADCAST_ID_SIZE         BT_AUDIO_BROADCAST_ID_SIZE
/*!< Maximum broadcast ID value */
#define ESP_BLE_AUDIO_BROADCAST_ID_MAX          BT_AUDIO_BROADCAST_ID_MAX
/*!< Indicates that the server have no preference for the presentation delay */
#define ESP_BLE_AUDIO_PD_PREF_NONE              BT_AUDIO_PD_PREF_NONE
/*!< Maximum presentation delay in microseconds */
#define ESP_BLE_AUDIO_PD_MAX                    BT_AUDIO_PD_MAX
/*!< Indicates that the unicast server does not have a preference for any retransmission number */
#define ESP_BLE_AUDIO_RTN_PREF_NONE             BT_AUDIO_RTN_PREF_NONE
/*!< The minimum size of a Broadcast Name as defined by Bluetooth Assigned Numbers */
#define ESP_BLE_AUDIO_BROADCAST_NAME_LEN_MIN    BT_AUDIO_BROADCAST_NAME_LEN_MIN
/*!< The maximum size of a Broadcast Name as defined by Bluetooth Assigned Numbers */
#define ESP_BLE_AUDIO_BROADCAST_NAME_LEN_MAX    BT_AUDIO_BROADCAST_NAME_LEN_MAX

/*!< Unicast Server is connectable and is requesting a connection. */
#define ESP_BLE_AUDIO_UNICAST_ANNOUNCEMENT_GENERAL      BT_AUDIO_UNICAST_ANNOUNCEMENT_GENERAL
/*!< Unicast Server is connectable but is not requesting a connection. */
#define ESP_BLE_AUDIO_UNICAST_ANNOUNCEMENT_TARGETED     BT_AUDIO_UNICAST_ANNOUNCEMENT_TARGETED

/*!< No context type */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_NONE                 BT_AUDIO_CONTEXT_TYPE_NONE
/**
 * Identifies audio where the use case context does not match any other defined value,
 * or where the context is unknown or cannot be determined.
 */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED          BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED
/**
 * Conversation between humans, for example, in telephony or video calls, including
 * traditional cellular as well as VoIP and Push-to-Talk
 */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL       BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL
/*!< Media, for example, music playback, radio, podcast or movie soundtrack, or tv audio */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA                BT_AUDIO_CONTEXT_TYPE_MEDIA
/**
 * Audio associated with video gaming, for example gaming media; gaming effects; music
 * and in-game voice chat between participants; or a mix of all the above
 */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_GAME                 BT_AUDIO_CONTEXT_TYPE_GAME
/*!< Instructional audio, for example, in navigation, announcements, or user guidance */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL        BT_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL
/*!< Man-machine communication, for example, with voice recognition or virtual assistants */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_VOICE_ASSISTANTS     BT_AUDIO_CONTEXT_TYPE_VOICE_ASSISTANTS
/**
 * Live audio, for example, from a microphone where audio is perceived both through a
 * direct acoustic path and through an LE Audio Stream
 */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_LIVE                 BT_AUDIO_CONTEXT_TYPE_LIVE
/**
 * Sound effects including keyboard and touch feedback; menu and user interface sounds;
 * and other system sounds
 */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_SOUND_EFFECTS        BT_AUDIO_CONTEXT_TYPE_SOUND_EFFECTS
/**
 * Notification and reminder sounds; attention-seeking audio, for example,
 * in beeps signaling the arrival of a message
 */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_NOTIFICATIONS        BT_AUDIO_CONTEXT_TYPE_NOTIFICATIONS
/**
 * Alerts the user to an incoming call, for example, an incoming telephony or video call,
 * including traditional cellular as well as VoIP and Push-to-Talk
 */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_RINGTONE             BT_AUDIO_CONTEXT_TYPE_RINGTONE
/**
 * Alarms and timers; immediate alerts, for example, in a critical battery alarm,
 * timer expiry or alarm clock, toaster, cooker, kettle, microwave, etc.
 */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_ALERTS               BT_AUDIO_CONTEXT_TYPE_ALERTS
/*!< Emergency alarm Emergency sounds, for example, fire alarms or other urgent alerts */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_EMERGENCY_ALARM      BT_AUDIO_CONTEXT_TYPE_EMERGENCY_ALARM
/*!< Any known context */
#define ESP_BLE_AUDIO_CONTEXT_TYPE_ANY                  BT_AUDIO_CONTEXT_TYPE_ANY
/** Audio Context Type for Generic Audio */
typedef enum bt_audio_context                           esp_ble_audio_context_t;

/*!< No rating */
#define ESP_BLE_AUDIO_PARENTAL_RATING_NO_RATING         BT_AUDIO_PARENTAL_RATING_NO_RATING
/*!< For all ages */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_ANY           BT_AUDIO_PARENTAL_RATING_AGE_ANY
/*!< Recommended for listeners of age 5 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_5_OR_ABOVE    BT_AUDIO_PARENTAL_RATING_AGE_5_OR_ABOVE
/*!< Recommended for listeners of age 6 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_6_OR_ABOVE    BT_AUDIO_PARENTAL_RATING_AGE_6_OR_ABOVE
/*!< Recommended for listeners of age 7 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_7_OR_ABOVE    BT_AUDIO_PARENTAL_RATING_AGE_7_OR_ABOVE
/*!< Recommended for listeners of age 8 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_8_OR_ABOVE    BT_AUDIO_PARENTAL_RATING_AGE_8_OR_ABOVE
/*!< Recommended for listeners of age 9 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_9_OR_ABOVE    BT_AUDIO_PARENTAL_RATING_AGE_9_OR_ABOVE
/*!< Recommended for listeners of age 10 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_10_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_10_OR_ABOVE
/*!< Recommended for listeners of age 11 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_11_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_11_OR_ABOVE
/*!< Recommended for listeners of age 12 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_12_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_12_OR_ABOVE
/*!< Recommended for listeners of age 13 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_13_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_13_OR_ABOVE
/*!< Recommended for listeners of age 14 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_14_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_14_OR_ABOVE
/*!< Recommended for listeners of age 15 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_15_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_15_OR_ABOVE
/*!< Recommended for listeners of age 16 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_16_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_16_OR_ABOVE
/*!< Recommended for listeners of age 17 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_17_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_17_OR_ABOVE
/*!< Recommended for listeners of age 18 and above */
#define ESP_BLE_AUDIO_PARENTAL_RATING_AGE_18_OR_ABOVE   BT_AUDIO_PARENTAL_RATING_AGE_18_OR_ABOVE
/** Parental rating defined by the Generic Audio assigned numbers */
typedef enum bt_audio_parental_rating                   esp_ble_audio_parental_rating_t;

/*!< No audio data is being transmitted */
#define ESP_BLE_AUDIO_ACTIVE_STATE_DISABLED             BT_AUDIO_ACTIVE_STATE_DISABLED
/*!< Audio data is being transmitted */
#define ESP_BLE_AUDIO_ACTIVE_STATE_ENABLED              BT_AUDIO_ACTIVE_STATE_ENABLED
/** Audio Active State defined by the Generic Audio assigned numbers */
typedef enum bt_audio_active_state                      esp_ble_audio_active_state_t;

/*!< Preferred audio context */
#define ESP_BLE_AUDIO_METADATA_TYPE_PREF_CONTEXT                BT_AUDIO_METADATA_TYPE_PREF_CONTEXT
/*!< Streaming audio context */
#define ESP_BLE_AUDIO_METADATA_TYPE_STREAM_CONTEXT              BT_AUDIO_METADATA_TYPE_STREAM_CONTEXT
/*!< UTF-8 encoded title or summary of stream content */
#define ESP_BLE_AUDIO_METADATA_TYPE_PROGRAM_INFO                BT_AUDIO_METADATA_TYPE_PROGRAM_INFO
/*!< Language */
#define ESP_BLE_AUDIO_METADATA_TYPE_LANG                        BT_AUDIO_METADATA_TYPE_LANG
/*!< Array of 8-bit CCID values */
#define ESP_BLE_AUDIO_METADATA_TYPE_CCID_LIST                   BT_AUDIO_METADATA_TYPE_CCID_LIST
/*!< Parental rating */
#define ESP_BLE_AUDIO_METADATA_TYPE_PARENTAL_RATING             BT_AUDIO_METADATA_TYPE_PARENTAL_RATING
/*!< UTF-8 encoded URI for additional Program information */
#define ESP_BLE_AUDIO_METADATA_TYPE_PROGRAM_INFO_URI            BT_AUDIO_METADATA_TYPE_PROGRAM_INFO_URI
/*!< Audio active state */
#define ESP_BLE_AUDIO_METADATA_TYPE_AUDIO_STATE                 BT_AUDIO_METADATA_TYPE_AUDIO_STATE
/*!< Broadcast Audio Immediate Rendering flag  */
#define ESP_BLE_AUDIO_METADATA_TYPE_BROADCAST_IMMEDIATE         BT_AUDIO_METADATA_TYPE_BROADCAST_IMMEDIATE
/*!< Assisted listening stream */
#define ESP_BLE_AUDIO_METADATA_TYPE_ASSISTED_LISTENING_STREAM   BT_AUDIO_METADATA_TYPE_ASSISTED_LISTENING_STREAM
/*!< UTF-8 encoded Broadcast name */
#define ESP_BLE_AUDIO_METADATA_TYPE_BROADCAST_NAME              BT_AUDIO_METADATA_TYPE_BROADCAST_NAME
/*!< Extended metadata */
#define ESP_BLE_AUDIO_METADATA_TYPE_EXTENDED                    BT_AUDIO_METADATA_TYPE_EXTENDED
/*!< Vendor specific metadata */
#define ESP_BLE_AUDIO_METADATA_TYPE_VENDOR                      BT_AUDIO_METADATA_TYPE_VENDOR
/**
 * @brief   Helper to check whether metadata type is known by the stack.
 *
 * @param   _type   Metadata type.
 */
#define ESP_BLE_AUDIO_METADATA_TYPE_IS_KNOWN(_type) \
    BT_AUDIO_METADATA_TYPE_IS_KNOWN(_type)
/** Metadata types defined by the Generic Audio assigned numbers */
typedef enum bt_audio_metadata_type                             esp_ble_audio_metadata_type_t;

/*!< Mono Audio (no specified Audio Location) */
#define ESP_BLE_AUDIO_LOCATION_MONO_AUDIO               BT_AUDIO_LOCATION_MONO_AUDIO
/*!< Front Left */
#define ESP_BLE_AUDIO_LOCATION_FRONT_LEFT               BT_AUDIO_LOCATION_FRONT_LEFT
/*!< Front Right */
#define ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT              BT_AUDIO_LOCATION_FRONT_RIGHT
/*!< Front Center */
#define ESP_BLE_AUDIO_LOCATION_FRONT_CENTER             BT_AUDIO_LOCATION_FRONT_CENTER
/*!< Low Frequency Effects 1 */
#define ESP_BLE_AUDIO_LOCATION_LOW_FREQ_EFFECTS_1       BT_AUDIO_LOCATION_LOW_FREQ_EFFECTS_1
/*!< Back Left */
#define ESP_BLE_AUDIO_LOCATION_BACK_LEFT                BT_AUDIO_LOCATION_BACK_LEFT
/*!< Back Right */
#define ESP_BLE_AUDIO_LOCATION_BACK_RIGHT               BT_AUDIO_LOCATION_BACK_RIGHT
/*!< Front Left of Center */
#define ESP_BLE_AUDIO_LOCATION_FRONT_LEFT_OF_CENTER     BT_AUDIO_LOCATION_FRONT_LEFT_OF_CENTER
/*!< Front Right of Center */
#define ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT_OF_CENTER    BT_AUDIO_LOCATION_FRONT_RIGHT_OF_CENTER
/*!< Back Center */
#define ESP_BLE_AUDIO_LOCATION_BACK_CENTER              BT_AUDIO_LOCATION_BACK_CENTER
/*!< Low Frequency Effects 2 */
#define ESP_BLE_AUDIO_LOCATION_LOW_FREQ_EFFECTS_2       BT_AUDIO_LOCATION_LOW_FREQ_EFFECTS_2
/*!< Side Left */
#define ESP_BLE_AUDIO_LOCATION_SIDE_LEFT                BT_AUDIO_LOCATION_SIDE_LEFT
/*!< Side Right */
#define ESP_BLE_AUDIO_LOCATION_SIDE_RIGHT               BT_AUDIO_LOCATION_SIDE_RIGHT
/*!< Top Front Left */
#define ESP_BLE_AUDIO_LOCATION_TOP_FRONT_LEFT           BT_AUDIO_LOCATION_TOP_FRONT_LEFT
/*!< Top Front Right */
#define ESP_BLE_AUDIO_LOCATION_TOP_FRONT_RIGHT          BT_AUDIO_LOCATION_TOP_FRONT_RIGHT
/*!< Top Front Center */
#define ESP_BLE_AUDIO_LOCATION_TOP_FRONT_CENTER         BT_AUDIO_LOCATION_TOP_FRONT_CENTER
/*!< Top Center */
#define ESP_BLE_AUDIO_LOCATION_TOP_CENTER               BT_AUDIO_LOCATION_TOP_CENTER
/*!< Top Back Left */
#define ESP_BLE_AUDIO_LOCATION_TOP_BACK_LEFT            BT_AUDIO_LOCATION_TOP_BACK_LEFT
/*!< Top Back Right */
#define ESP_BLE_AUDIO_LOCATION_TOP_BACK_RIGHT           BT_AUDIO_LOCATION_TOP_BACK_RIGHT
/*!< Top Side Left */
#define ESP_BLE_AUDIO_LOCATION_TOP_SIDE_LEFT            BT_AUDIO_LOCATION_TOP_SIDE_LEFT
/*!< Top Side Right */
#define ESP_BLE_AUDIO_LOCATION_TOP_SIDE_RIGHT           BT_AUDIO_LOCATION_TOP_SIDE_RIGHT
/*!< Top Back Center */
#define ESP_BLE_AUDIO_LOCATION_TOP_BACK_CENTER          BT_AUDIO_LOCATION_TOP_BACK_CENTER
/*!< Bottom Front Center */
#define ESP_BLE_AUDIO_LOCATION_BOTTOM_FRONT_CENTER      BT_AUDIO_LOCATION_BOTTOM_FRONT_CENTER
/*!< Bottom Front Left */
#define ESP_BLE_AUDIO_LOCATION_BOTTOM_FRONT_LEFT        BT_AUDIO_LOCATION_BOTTOM_FRONT_LEFT
/*!< Bottom Front Right */
#define ESP_BLE_AUDIO_LOCATION_BOTTOM_FRONT_RIGHT       BT_AUDIO_LOCATION_BOTTOM_FRONT_RIGHT
/*!< Front Left Wide */
#define ESP_BLE_AUDIO_LOCATION_FRONT_LEFT_WIDE          BT_AUDIO_LOCATION_FRONT_LEFT_WIDE
/*!< Front Right Wide */
#define ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT_WIDE         BT_AUDIO_LOCATION_FRONT_RIGHT_WIDE
/*!< Left Surround */
#define ESP_BLE_AUDIO_LOCATION_LEFT_SURROUND            BT_AUDIO_LOCATION_LEFT_SURROUND
/*!< Right Surround */
#define ESP_BLE_AUDIO_LOCATION_RIGHT_SURROUND           BT_AUDIO_LOCATION_RIGHT_SURROUND
/*!< Any known location */
#define ESP_BLE_AUDIO_LOCATION_ANY                      BT_AUDIO_LOCATION_ANY
/** Location values defined by the Generic Audio assigned numbers */
typedef enum bt_audio_location                          esp_ble_audio_location_t;

/*!< Unspecified audio enhancement */
#define ESP_BLE_AUDIO_ASSISTED_LISTENING_STREAM_UNSPECIFIED BT_AUDIO_ASSISTED_LISTENING_STREAM_UNSPECIFIED
/** Assisted Listening Stream defined by the Generic Audio assigned numbers */
typedef enum bt_audio_assisted_listening_stream             esp_ble_audio_assisted_listening_stream_t;

/*!< Size in octets of a 16-bit UUID */
#define ESP_BLE_AUDIO_UUID_SIZE_16                          BT_UUID_SIZE_16

/*!< Audio Input Control Service UUID value */
#define ESP_BLE_AUDIO_UUID_AICS_VAL                         BT_UUID_AICS_VAL
/*!< Audio Stream Control Service UUID value */
#define ESP_BLE_AUDIO_UUID_ASCS_VAL                         BT_UUID_ASCS_VAL
/*!< Broadcast Audio Scan Service UUID value */
#define ESP_BLE_AUDIO_UUID_BASS_VAL                         BT_UUID_BASS_VAL
/*!< Basic Audio Announcement Service UUID value */
#define ESP_BLE_AUDIO_UUID_BASIC_AUDIO_VAL                  BT_UUID_BASIC_AUDIO_VAL
/*!< Broadcast Audio Announcement Service UUID value */
#define ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL              BT_UUID_BROADCAST_AUDIO_VAL
/*!< Common Audio Service UUID value */
#define ESP_BLE_AUDIO_UUID_CAS_VAL                          BT_UUID_CAS_VAL
/*!< Coordinated Set Identification Service UUID value */
#define ESP_BLE_AUDIO_UUID_CSIS_VAL                         BT_UUID_CSIS_VAL
/*!< Gaming Service UUID value */
#define ESP_BLE_AUDIO_UUID_GMAS_VAL                         BT_UUID_GMAS_VAL
/*!< Generic Media Control Service UUID value */
#define ESP_BLE_AUDIO_UUID_GMCS_VAL                         BT_UUID_GMCS_VAL
/*!< Generic Telephone Bearer Service UUID value */
#define ESP_BLE_AUDIO_UUID_GTBS_VAL                         BT_UUID_GTBS_VAL
/*!< Hearing Access Service UUID value */
#define ESP_BLE_AUDIO_UUID_HAS_VAL                          BT_UUID_HAS_VAL
/*!< Media Control Service UUID value */
#define ESP_BLE_AUDIO_UUID_MCS_VAL                          BT_UUID_MCS_VAL
/*!< Microphone Control Service UUID value */
#define ESP_BLE_AUDIO_UUID_MICS_VAL                         BT_UUID_MICS_VAL
/*!< Published Audio Capabilities Service UUID value */
#define ESP_BLE_AUDIO_UUID_PACS_VAL                         BT_UUID_PACS_VAL
/*!< Public Broadcast Announcement Service UUID value */
#define ESP_BLE_AUDIO_UUID_PBA_VAL                          BT_UUID_PBA_VAL
/*!< Telephone Bearer Service UUID value */
#define ESP_BLE_AUDIO_UUID_TBS_VAL                          BT_UUID_TBS_VAL
/*!< Telephony and Media Audio Service UUID value */
#define ESP_BLE_AUDIO_UUID_TMAS_VAL                         BT_UUID_TMAS_VAL
/*!< Volume Control Service UUID value */
#define ESP_BLE_AUDIO_UUID_VCS_VAL                          BT_UUID_VCS_VAL
/*!< Volume Offset Control Service UUID value */
#define ESP_BLE_AUDIO_UUID_VOCS_VAL                         BT_UUID_VOCS_VAL
/*!< Object Transfer Service UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_VAL                          BT_UUID_OTS_VAL

/*!< Audio Input Control Service State value */
#define ESP_BLE_AUDIO_UUID_AICS_STATE_VAL                   BT_UUID_AICS_STATE_VAL
/*!< Audio Input Control Service Gain Settings Properties value */
#define ESP_BLE_AUDIO_UUID_AICS_GAIN_SETTINGS_VAL           BT_UUID_AICS_GAIN_SETTINGS_VAL
/*!< Audio Input Control Service Input Type value */
#define ESP_BLE_AUDIO_UUID_AICS_INPUT_TYPE_VAL              BT_UUID_AICS_INPUT_TYPE_VAL
/*!< Audio Input Control Service Input Status value */
#define ESP_BLE_AUDIO_UUID_AICS_INPUT_STATUS_VAL            BT_UUID_AICS_INPUT_STATUS_VAL
/*!< Audio Input Control Service Control Point value */
#define ESP_BLE_AUDIO_UUID_AICS_CONTROL_VAL                 BT_UUID_AICS_CONTROL_VAL
/*!< Audio Input Control Service Input Description value */
#define ESP_BLE_AUDIO_UUID_AICS_DESCRIPTION_VAL             BT_UUID_AICS_DESCRIPTION_VAL

/*!< Audio Stream Endpoint Sink Characteristic value */
#define ESP_BLE_AUDIO_UUID_ASCS_ASE_SNK_VAL                 BT_UUID_ASCS_ASE_SNK_VAL
/*!< Audio Stream Endpoint Source Characteristic value */
#define ESP_BLE_AUDIO_UUID_ASCS_ASE_SRC_VAL                 BT_UUID_ASCS_ASE_SRC_VAL
/*!< Audio Stream Endpoint Control Point Characteristic value */
#define ESP_BLE_AUDIO_UUID_ASCS_ASE_CP_VAL                  BT_UUID_ASCS_ASE_CP_VAL

/*!< Broadcast Audio Scan Service Scan State value */
#define ESP_BLE_AUDIO_UUID_BASS_CONTROL_POINT_VAL           BT_UUID_BASS_CONTROL_POINT_VAL
/*!< Broadcast Audio Scan Service Receive State value */
#define ESP_BLE_AUDIO_UUID_BASS_RECV_STATE_VAL              BT_UUID_BASS_RECV_STATE_VAL

/*!< Set Identity Resolving Key value */
#define ESP_BLE_AUDIO_UUID_CSIS_SIRK_VAL                    BT_UUID_CSIS_SIRK_VAL
/*!< Set size value */
#define ESP_BLE_AUDIO_UUID_CSIS_SET_SIZE_VAL                BT_UUID_CSIS_SET_SIZE_VAL
/*!< Set lock value */
#define ESP_BLE_AUDIO_UUID_CSIS_SET_LOCK_VAL                BT_UUID_CSIS_SET_LOCK_VAL
/*!< Rank value */
#define ESP_BLE_AUDIO_UUID_CSIS_RANK_VAL                    BT_UUID_CSIS_RANK_VAL

/*!< Gaming Audio Profile Role UUID value */
#define ESP_BLE_AUDIO_UUID_GMAP_ROLE_VAL                    BT_UUID_GMAP_ROLE_VAL
/*!< Gaming Audio Profile Unicast Game Gateway Features UUID value */
#define ESP_BLE_AUDIO_UUID_GMAP_UGG_FEAT_VAL                BT_UUID_GMAP_UGG_FEAT_VAL
/*!< Gaming Audio Profile Unicast Game Terminal Features UUID value */
#define ESP_BLE_AUDIO_UUID_GMAP_UGT_FEAT_VAL                BT_UUID_GMAP_UGT_FEAT_VAL
/*!< Gaming Audio Profile Broadcast Game Sender Features UUID value */
#define ESP_BLE_AUDIO_UUID_GMAP_BGS_FEAT_VAL                BT_UUID_GMAP_BGS_FEAT_VAL
/*!< Gaming Audio Profile Broadcast Game Receiver Features UUID value */
#define ESP_BLE_AUDIO_UUID_GMAP_BGR_FEAT_VAL                BT_UUID_GMAP_BGR_FEAT_VAL

/*!< Hearing Aid Features Characteristic value */
#define ESP_BLE_AUDIO_UUID_HAS_HEARING_AID_FEATURES_VAL     BT_UUID_HAS_HEARING_AID_FEATURES_VAL
/*!< Hearing Aid Preset Control Point Characteristic value */
#define ESP_BLE_AUDIO_UUID_HAS_PRESET_CONTROL_POINT_VAL     BT_UUID_HAS_PRESET_CONTROL_POINT_VAL
/*!< Hearing Aid Active Preset Index Characteristic value */
#define ESP_BLE_AUDIO_UUID_HAS_ACTIVE_PRESET_INDEX_VAL      BT_UUID_HAS_ACTIVE_PRESET_INDEX_VAL

/*!< Media player name value */
#define ESP_BLE_AUDIO_UUID_MCS_PLAYER_NAME_VAL              BT_UUID_MCS_PLAYER_NAME_VAL
/*!< Media Icon Object ID value */
#define ESP_BLE_AUDIO_UUID_MCS_ICON_OBJ_ID_VAL              BT_UUID_MCS_ICON_OBJ_ID_VAL
/*!< Media Icon URL value */
#define ESP_BLE_AUDIO_UUID_MCS_ICON_URL_VAL                 BT_UUID_MCS_ICON_URL_VAL
/*!< Track Changed value */
#define ESP_BLE_AUDIO_UUID_MCS_TRACK_CHANGED_VAL            BT_UUID_MCS_TRACK_CHANGED_VAL
/*!< Track Title value */
#define ESP_BLE_AUDIO_UUID_MCS_TRACK_TITLE_VAL              BT_UUID_MCS_TRACK_TITLE_VAL
/*!< Track Duration value */
#define ESP_BLE_AUDIO_UUID_MCS_TRACK_DURATION_VAL           BT_UUID_MCS_TRACK_DURATION_VAL
/*!< Track Position value */
#define ESP_BLE_AUDIO_UUID_MCS_TRACK_POSITION_VAL           BT_UUID_MCS_TRACK_POSITION_VAL
/*!< Playback Speed value */
#define ESP_BLE_AUDIO_UUID_MCS_PLAYBACK_SPEED_VAL           BT_UUID_MCS_PLAYBACK_SPEED_VAL
/*!< Seeking Speed value */
#define ESP_BLE_AUDIO_UUID_MCS_SEEKING_SPEED_VAL            BT_UUID_MCS_SEEKING_SPEED_VAL
/*!< Track Segments Object ID value */
#define ESP_BLE_AUDIO_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL    BT_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL
/*!< Current Track Object ID value */
#define ESP_BLE_AUDIO_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL     BT_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL
/*!< Next Track Object ID value */
#define ESP_BLE_AUDIO_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL        BT_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL
/*!< Parent Group Object ID value */
#define ESP_BLE_AUDIO_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL      BT_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL
/*!< Group Object ID value */
#define ESP_BLE_AUDIO_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL     BT_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL
/*!< Playing Order value */
#define ESP_BLE_AUDIO_UUID_MCS_PLAYING_ORDER_VAL            BT_UUID_MCS_PLAYING_ORDER_VAL
/*!< Playing Orders supported value */
#define ESP_BLE_AUDIO_UUID_MCS_PLAYING_ORDERS_VAL           BT_UUID_MCS_PLAYING_ORDERS_VAL
/*!< Media State value */
#define ESP_BLE_AUDIO_UUID_MCS_MEDIA_STATE_VAL              BT_UUID_MCS_MEDIA_STATE_VAL
/*!< Media Control Point value */
#define ESP_BLE_AUDIO_UUID_MCS_MEDIA_CONTROL_POINT_VAL      BT_UUID_MCS_MEDIA_CONTROL_POINT_VAL
/*!< Media control opcodes supported value */
#define ESP_BLE_AUDIO_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL    BT_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL
/*!< Search result object ID value */
#define ESP_BLE_AUDIO_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL    BT_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL
/*!< Search control point value */
#define ESP_BLE_AUDIO_UUID_MCS_SEARCH_CONTROL_POINT_VAL     BT_UUID_MCS_SEARCH_CONTROL_POINT_VAL

/*!< Microphone Control Service Mute value */
#define ESP_BLE_AUDIO_UUID_MICS_MUTE_VAL                    BT_UUID_MICS_MUTE_VAL

/*!< Sink PAC Characteristic value */
#define ESP_BLE_AUDIO_UUID_PACS_SNK_VAL                     BT_UUID_PACS_SNK_VAL
/*!< Sink PAC Locations Characteristic value */
#define ESP_BLE_AUDIO_UUID_PACS_SNK_LOC_VAL                 BT_UUID_PACS_SNK_LOC_VAL
/*!< Source PAC Characteristic value */
#define ESP_BLE_AUDIO_UUID_PACS_SRC_VAL                     BT_UUID_PACS_SRC_VAL
/*!< Source PAC Locations Characteristic value */
#define ESP_BLE_AUDIO_UUID_PACS_SRC_LOC_VAL                 BT_UUID_PACS_SRC_LOC_VAL
/*!< Available Audio Contexts Characteristic value */
#define ESP_BLE_AUDIO_UUID_PACS_AVAILABLE_CONTEXT_VAL       BT_UUID_PACS_AVAILABLE_CONTEXT_VAL
/*!< Supported Audio Context Characteristic value */
#define ESP_BLE_AUDIO_UUID_PACS_SUPPORTED_CONTEXT_VAL       BT_UUID_PACS_SUPPORTED_CONTEXT_VAL

/*!< Bearer Provider Name value */
#define ESP_BLE_AUDIO_UUID_TBS_PROVIDER_NAME_VAL            BT_UUID_TBS_PROVIDER_NAME_VAL
/*!< Bearer UCI value */
#define ESP_BLE_AUDIO_UUID_TBS_UCI_VAL                      BT_UUID_TBS_UCI_VAL
/*!< Bearer Technology value */
#define ESP_BLE_AUDIO_UUID_TBS_TECHNOLOGY_VAL               BT_UUID_TBS_TECHNOLOGY_VAL
/*!< Bearer URI Prefixes Supported List value */
#define ESP_BLE_AUDIO_UUID_TBS_URI_LIST_VAL                 BT_UUID_TBS_URI_LIST_VAL
/*!< Bearer Signal Strength value */
#define ESP_BLE_AUDIO_UUID_TBS_SIGNAL_STRENGTH_VAL          BT_UUID_TBS_SIGNAL_STRENGTH_VAL
/*!< Bearer Signal Strength Reporting Interval value */
#define ESP_BLE_AUDIO_UUID_TBS_SIGNAL_INTERVAL_VAL          BT_UUID_TBS_SIGNAL_INTERVAL_VAL
/*!< Bearer List Current Calls value */
#define ESP_BLE_AUDIO_UUID_TBS_LIST_CURRENT_CALLS_VAL       BT_UUID_TBS_LIST_CURRENT_CALLS_VAL
/*!< Status flags value */
#define ESP_BLE_AUDIO_UUID_TBS_STATUS_FLAGS_VAL             BT_UUID_TBS_STATUS_FLAGS_VAL
/*!< Incoming Call Target Caller ID value */
#define ESP_BLE_AUDIO_UUID_TBS_INCOMING_URI_VAL             BT_UUID_TBS_INCOMING_URI_VAL
/*!< Call State value */
#define ESP_BLE_AUDIO_UUID_TBS_CALL_STATE_VAL               BT_UUID_TBS_CALL_STATE_VAL
/*!< Call Control Point value */
#define ESP_BLE_AUDIO_UUID_TBS_CALL_CONTROL_POINT_VAL       BT_UUID_TBS_CALL_CONTROL_POINT_VAL
/*!< Optional Opcodes value */
#define ESP_BLE_AUDIO_UUID_TBS_OPTIONAL_OPCODES_VAL         BT_UUID_TBS_OPTIONAL_OPCODES_VAL
/*!< Terminate reason value */
#define ESP_BLE_AUDIO_UUID_TBS_TERMINATE_REASON_VAL         BT_UUID_TBS_TERMINATE_REASON_VAL
/*!< Incoming Call value */
#define ESP_BLE_AUDIO_UUID_TBS_INCOMING_CALL_VAL            BT_UUID_TBS_INCOMING_CALL_VAL
/*!< Incoming Call Friendly name value */
#define ESP_BLE_AUDIO_UUID_TBS_FRIENDLY_NAME_VAL            BT_UUID_TBS_FRIENDLY_NAME_VAL

/*!< Volume Control Setting value */
#define ESP_BLE_AUDIO_UUID_VCS_STATE_VAL                    BT_UUID_VCS_STATE_VAL
/*!< Volume Control Control point value */
#define ESP_BLE_AUDIO_UUID_VCS_CONTROL_VAL                  BT_UUID_VCS_CONTROL_VAL
/*!< Volume Control Flags value */
#define ESP_BLE_AUDIO_UUID_VCS_FLAGS_VAL                    BT_UUID_VCS_FLAGS_VAL

/*!< Volume Offset State value */
#define ESP_BLE_AUDIO_UUID_VOCS_STATE_VAL                   BT_UUID_VOCS_STATE_VAL
/*!< Audio Location value */
#define ESP_BLE_AUDIO_UUID_VOCS_LOCATION_VAL                BT_UUID_VOCS_LOCATION_VAL
/*!< Volume Offset Control Point value */
#define ESP_BLE_AUDIO_UUID_VOCS_CONTROL_VAL                 BT_UUID_VOCS_CONTROL_VAL
/*!< Volume Offset Audio Output Description value */
#define ESP_BLE_AUDIO_UUID_VOCS_DESCRIPTION_VAL             BT_UUID_VOCS_DESCRIPTION_VAL

/*!< Content Control ID value */
#define ESP_BLE_AUDIO_UUID_CCID_VAL                         BT_UUID_CCID_VAL

/*!< OTS Feature Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_FEATURE_VAL                  BT_UUID_OTS_FEATURE_VAL
/*!< OTS Object Name Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_NAME_VAL                     BT_UUID_OTS_NAME_VAL
/*!< OTS Object Type Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_TYPE_VAL                     BT_UUID_OTS_TYPE_VAL
/*!< OTS Object Size Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_SIZE_VAL                     BT_UUID_OTS_SIZE_VAL
/*!< OTS Object First-Created Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_FIRST_CREATED_VAL            BT_UUID_OTS_FIRST_CREATED_VAL
/*!< OTS Object Last-Modified Characteristic UUI value */
#define ESP_BLE_AUDIO_UUID_OTS_LAST_MODIFIED_VAL            BT_UUID_OTS_LAST_MODIFIED_VAL
/*!< OTS Object ID Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_ID_VAL                       BT_UUID_OTS_ID_VAL
/*!< OTS Object Properties Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_PROPERTIES_VAL               BT_UUID_OTS_PROPERTIES_VAL
/*!< OTS Object Action Control Point Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_ACTION_CP_VAL                BT_UUID_OTS_ACTION_CP_VAL
/*!< OTS Object List Control Point Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_LIST_CP_VAL                  BT_UUID_OTS_LIST_CP_VAL
/*!< OTS Object List Filter Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_LIST_FILTER_VAL              BT_UUID_OTS_LIST_FILTER_VAL
/*!< OTS Object Changed Characteristic UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_CHANGED_VAL                  BT_UUID_OTS_CHANGED_VAL
/*!< OTS Unspecified Object Type UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_TYPE_UNSPECIFIED_VAL         BT_UUID_OTS_TYPE_UNSPECIFIED_VAL
/*!< OTS Directory Listing UUID value */
#define ESP_BLE_AUDIO_UUID_OTS_DIRECTORY_LISTING_VAL        BT_UUID_OTS_DIRECTORY_LISTING_VAL
/*!< OTS Media Player Icon Object Type value */
#define ESP_BLE_AUDIO_UUID_OTS_TYPE_MPL_ICON_VAL            BT_UUID_OTS_TYPE_MPL_ICON_VAL
/*!< OTS Track Segments Object Type value */
#define ESP_BLE_AUDIO_UUID_OTS_TYPE_TRACK_SEGMENT_VAL       BT_UUID_OTS_TYPE_TRACK_SEGMENT_VAL
/*!< OTS Track Object Type value */
#define ESP_BLE_AUDIO_UUID_OTS_TYPE_TRACK_VAL               BT_UUID_OTS_TYPE_TRACK_VAL
/*!< OTS Group Object Type value */
#define ESP_BLE_AUDIO_UUID_OTS_TYPE_GROUP_VAL               BT_UUID_OTS_TYPE_GROUP_VAL

/*!< Generic Audio Sink */
#define ESP_BLE_AUDIO_APPEARANCE_GENERIC_AUDIO_SINK                 BT_APPEARANCE_GENERIC_AUDIO_SINK
/*!< Standalone Speaker */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SINK_STANDALONE_SPEAKER      BT_APPEARANCE_AUDIO_SINK_STANDALONE_SPEAKER
/*!< Soundbar */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SINK_SOUNDBAR                BT_APPEARANCE_AUDIO_SINK_SOUNDBAR
/*!< Bookshelf Speaker */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SINK_BOOKSHELF_SPEAKER       BT_APPEARANCE_AUDIO_SINK_BOOKSHELF_SPEAKER
/*!< Standmounted Speaker */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SINK_STANDMOUNTED_SPEAKER    BT_APPEARANCE_AUDIO_SINK_STANDMOUNTED_SPEAKER
/*!< Speakerphone */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SINK_SPEAKERPHONE            BT_APPEARANCE_AUDIO_SINK_SPEAKERPHONE

/*!< Generic Audio Source */
#define ESP_BLE_AUDIO_APPEARANCE_GENERIC_AUDIO_SOURCE               BT_APPEARANCE_GENERIC_AUDIO_SOURCE
/*!< Microphone */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_MICROPHONE            BT_APPEARANCE_AUDIO_SOURCE_MICROPHONE
/*!< Alarm */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_ALARM                 BT_APPEARANCE_AUDIO_SOURCE_ALARM
/*!< Bell */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_BELL                  BT_APPEARANCE_AUDIO_SOURCE_BELL
/*!< Horn */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_HORN                  BT_APPEARANCE_AUDIO_SOURCE_HORN
/*!< Broadcasting Device */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_BROADCASTING_DEVICE   BT_APPEARANCE_AUDIO_SOURCE_BROADCASTING_DEVICE
/*!< Service Desk */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_SERVICE_DESK          BT_APPEARANCE_AUDIO_SOURCE_SERVICE_DESK
/*!< Kiosk */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_KIOSK                 BT_APPEARANCE_AUDIO_SOURCE_KIOSK
/*!< Broadcasting Room */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_BROADCASTING_ROOM     BT_APPEARANCE_AUDIO_SOURCE_BROADCASTING_ROOM
/*!< Auditorium */
#define ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_AUDITORIUM            BT_APPEARANCE_AUDIO_SOURCE_AUDITORIUM

/*!< Generic Wearable Audio Device */
#define ESP_BLE_AUDIO_APPEARANCE_GENERIC_WEARABLE_AUDIO_DEVICE      BT_APPEARANCE_GENERIC_WEARABLE_AUDIO_DEVICE
/*!< Earbud */
#define ESP_BLE_AUDIO_APPEARANCE_WEARABLE_AUDIO_DEVICE_EARBUD       BT_APPEARANCE_WEARABLE_AUDIO_DEVICE_EARBUD
/*!< Headset */
#define ESP_BLE_AUDIO_APPEARANCE_WEARABLE_AUDIO_DEVICE_HEADSET      BT_APPEARANCE_WEARABLE_AUDIO_DEVICE_HEADSET
/*!< Headphones */
#define ESP_BLE_AUDIO_APPEARANCE_WEARABLE_AUDIO_DEVICE_HEADPHONES   BT_APPEARANCE_WEARABLE_AUDIO_DEVICE_HEADPHONES
/*!< Neck Band */
#define ESP_BLE_AUDIO_APPEARANCE_WEARABLE_AUDIO_DEVICE_NECK_BAND    BT_APPEARANCE_WEARABLE_AUDIO_DEVICE_NECK_BAND

/*!< Generic Hearing aid */
#define ESP_BLE_AUDIO_APPEARANCE_GENERIC_HEARING_AID                BT_APPEARANCE_GENERIC_HEARING_AID
/*!< In-ear hearing aid */
#define ESP_BLE_AUDIO_APPEARANCE_HEARING_AID_IN_EAR                 BT_APPEARANCE_HEARING_AID_IN_EAR
/*!< Behind-ear hearing aid */
#define ESP_BLE_AUDIO_APPEARANCE_HEARING_AID_BEHIND_EAR             BT_APPEARANCE_HEARING_AID_BEHIND_EAR
/*!< Cochlear Implant */
#define ESP_BLE_AUDIO_APPEARANCE_HEARING_AID_COCHLEAR_IMPLANT       BT_APPEARANCE_HEARING_AID_COCHLEAR_IMPLANT

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_DEFS_H_ */
