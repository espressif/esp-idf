/*
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_BAP_API_H_
#define ESP_BLE_AUDIO_BAP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/iso.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/bap.h>

#include "common/init.h"
#include "common/host.h"

#include "esp_ble_iso_common_api.h"

#include "esp_ble_audio_codec_api.h"
#include "esp_ble_audio_common_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Check if a BAP BASS BIS_Sync bitfield is valid.
 *
 * Valid options are either a bitmask of valid BIS indices, including
 * none (0x00000000) or no ref (0xFFFFFFFF).
 *
 * @param   _bis_bitfield   BIS_Sync bitfield (uint32).
 */
#define ESP_BLE_AUDIO_BAP_BASS_VALID_BIT_BITFIELD(_bis_bitfield) \
    BT_BAP_BASS_VALID_BIT_BITFIELD(_bis_bitfield)

/**
 * @brief   Helper to declare elements of BAP QoS Configuration.
 *
 * @param   _interval   SDU interval (usec).
 * @param   _framing    Framing.
 * @param   _phy        Target PHY.
 * @param   _sdu        Maximum SDU Size.
 * @param   _rtn        Retransmission number.
 * @param   _latency    Maximum Transport Latency (msec).
 * @param   _pd         Presentation Delay (usec).
 */
#define ESP_BLE_AUDIO_BAP_QOS_CFG(_interval, _framing, _phy, _sdu, _rtn, _latency, _pd) \
    BT_BAP_QOS_CFG(_interval, _framing, _phy, _sdu, _rtn, _latency, _pd)

/**
 * @brief   Helper to declare Input Unframed BAP QoS Configuration.
 *
 * @param   _interval   SDU interval (usec).
 * @param   _sdu        Maximum SDU Size.
 * @param   _rtn        Retransmission number.
 * @param   _latency    Maximum Transport Latency (msec).
 * @param   _pd         Presentation Delay (usec).
 */
#define ESP_BLE_AUDIO_BAP_QOS_CFG_UNFRAMED(_interval, _sdu, _rtn, _latency, _pd) \
    BT_BAP_QOS_CFG_UNFRAMED(_interval, _sdu, _rtn, _latency, _pd)

/**
 * @brief   Helper to declare Input Framed BAP QoS Configuration.
 *
 * @param   _interval   SDU interval (usec).
 * @param   _sdu        Maximum SDU Size.
 * @param   _rtn        Retransmission number.
 * @param   _latency    Maximum Transport Latency (msec).
 * @param   _pd         Presentation Delay (usec).
 */
#define ESP_BLE_AUDIO_BAP_QOS_CFG_FRAMED(_interval, _sdu, _rtn, _latency, _pd) \
    BT_BAP_QOS_CFG_FRAMED(_interval, _sdu, _rtn, _latency, _pd)

/**
 * @brief   Helper to declare elements of BAP QoS Configuration Preference.
 *
 * @param   _unframed_supported Unframed PDUs supported.
 * @param   _phy                Preferred Target PHY.
 * @param   _rtn                Preferred Retransmission number.
 * @param   _latency            Preferred Maximum Transport Latency (msec).
 * @param   _pd_min             Minimum Presentation Delay (usec).
 * @param   _pd_max             Maximum Presentation Delay (usec).
 * @param   _pref_pd_min        Preferred Minimum Presentation Delay (usec).
 * @param   _pref_pd_max        Preferred Maximum Presentation Delay (usec).
 */
#define ESP_BLE_AUDIO_BAP_QOS_CFG_PREF(_unframed_supported, _phy, _rtn, \
                                       _latency, _pd_min, _pd_max, \
                                       _pref_pd_min, _pref_pd_max) \
    BT_BAP_QOS_CFG_PREF(_unframed_supported, _phy, _rtn, \
                        _latency, _pd_min, _pd_max, \
                        _pref_pd_min, _pref_pd_max)

/**
 * @brief   Macro used to initialise the object storing values of
 *          ASE Control Point notification.
 *
 * @param   c   Response Code field.
 * @param   r   Reason field.
 */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP(c, r)                BT_BAP_ASCS_RSP(c, r)

/*!< LE 1M PHY */
#define ESP_BLE_AUDIO_BAP_QOS_CFG_1M                    BT_BAP_QOS_CFG_1M
/*!< LE 2M PHY */
#define ESP_BLE_AUDIO_BAP_QOS_CFG_2M                    BT_BAP_QOS_CFG_2M
/*!< LE Coded PHY */
#define ESP_BLE_AUDIO_BAP_QOS_CFG_CODED                 BT_BAP_QOS_CFG_CODED

/*!< An invalid Broadcast ID */
#define ESP_BLE_AUDIO_BAP_INVALID_BROADCAST_ID          BT_BAP_INVALID_BROADCAST_ID

/*!< Value indicating that the periodic advertising interval is unknown */
#define ESP_BLE_AUDIO_BAP_PA_INTERVAL_UNKNOWN           BT_BAP_PA_INTERVAL_UNKNOWN

/*!< Indicate that the Broadcast Assistant has no preference to which BIS the Scan Delegator syncs to */
#define ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF              BT_BAP_BIS_SYNC_NO_PREF
/*!< BIS sync value indicating that the BIG sync has failed for any reason */
#define ESP_BLE_AUDIO_BAP_BIS_SYNC_FAILED               BT_BAP_BIS_SYNC_FAILED

/*!< Packets may be framed or unframed */
#define ESP_BLE_AUDIO_BAP_QOS_CFG_FRAMING_UNFRAMED      BT_BAP_QOS_CFG_FRAMING_UNFRAMED
/*!< Packets are always framed */
#define ESP_BLE_AUDIO_BAP_QOS_CFG_FRAMING_FRAMED        BT_BAP_QOS_CFG_FRAMING_FRAMED
/** QoS Framing */
typedef enum bt_bap_qos_cfg_framing                     esp_ble_audio_bap_qos_cfg_framing_t;

/*!< The periodic advertising has not been synchronized */
#define ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED           BT_BAP_PA_STATE_NOT_SYNCED
/*!< Waiting for SyncInfo from Broadcast Assistant */
#define ESP_BLE_AUDIO_BAP_PA_STATE_INFO_REQ             BT_BAP_PA_STATE_INFO_REQ
/*!< Synchronized to periodic advertising */
#define ESP_BLE_AUDIO_BAP_PA_STATE_SYNCED               BT_BAP_PA_STATE_SYNCED
/*!< Failed to synchronized to periodic advertising */
#define ESP_BLE_AUDIO_BAP_PA_STATE_FAILED               BT_BAP_PA_STATE_FAILED
/*!< No periodic advertising sync transfer receiver from Broadcast Assistant */
#define ESP_BLE_AUDIO_BAP_PA_STATE_NO_PAST              BT_BAP_PA_STATE_NO_PAST
/** Periodic advertising state reported by the Scan Delegator */
typedef enum bt_bap_pa_state                            esp_ble_audio_bap_pa_state_t;

/*!< The Broadcast Isochronous Group not encrypted */
#define ESP_BLE_AUDIO_BAP_BIG_ENC_STATE_NO_ENC          BT_BAP_BIG_ENC_STATE_NO_ENC
/*!< The Broadcast Isochronous Group broadcast code requested */
#define ESP_BLE_AUDIO_BAP_BIG_ENC_STATE_BCODE_REQ       BT_BAP_BIG_ENC_STATE_BCODE_REQ
/*!< The Broadcast Isochronous Group decrypted */
#define ESP_BLE_AUDIO_BAP_BIG_ENC_STATE_DEC             BT_BAP_BIG_ENC_STATE_DEC
/*!< The Broadcast Isochronous Group bad broadcast code */
#define ESP_BLE_AUDIO_BAP_BIG_ENC_STATE_BAD_CODE        BT_BAP_BIG_ENC_STATE_BAD_CODE
/** Broadcast Isochronous Group encryption state reported by the Scan Delegator */
typedef enum bt_bap_big_enc_state                       esp_ble_audio_bap_big_enc_state_t;

/*!< Audio Stream Endpoint Idle state */
#define ESP_BLE_AUDIO_BAP_EP_STATE_IDLE                 BT_BAP_EP_STATE_IDLE
/*!< Audio Stream Endpoint Codec Configured state */
#define ESP_BLE_AUDIO_BAP_EP_STATE_CODEC_CONFIGURED     BT_BAP_EP_STATE_CODEC_CONFIGURED
/*!< Audio Stream Endpoint QoS Configured state */
#define ESP_BLE_AUDIO_BAP_EP_STATE_QOS_CONFIGURED       BT_BAP_EP_STATE_QOS_CONFIGURED
/*!< Audio Stream Endpoint Enabling state */
#define ESP_BLE_AUDIO_BAP_EP_STATE_ENABLING             BT_BAP_EP_STATE_ENABLING
/*!< Audio Stream Endpoint Streaming state */
#define ESP_BLE_AUDIO_BAP_EP_STATE_STREAMING            BT_BAP_EP_STATE_STREAMING
/*!< Audio Stream Endpoint Disabling state */
#define ESP_BLE_AUDIO_BAP_EP_STATE_DISABLING            BT_BAP_EP_STATE_DISABLING
/*!< Audio Stream Endpoint Releasing state */
#define ESP_BLE_AUDIO_BAP_EP_STATE_RELEASING            BT_BAP_EP_STATE_RELEASING
/** Endpoint states */
typedef enum bt_bap_ep_state                            esp_ble_audio_bap_ep_state_t;

/*!< No reason */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_NONE              BT_BAP_ASCS_REASON_NONE
/*!< Codec ID */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_CODEC             BT_BAP_ASCS_REASON_CODEC
/*!< Codec configuration */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_CODEC_DATA        BT_BAP_ASCS_REASON_CODEC_DATA
/*!< SDU interval */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_INTERVAL          BT_BAP_ASCS_REASON_INTERVAL
/*!< Framing */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_FRAMING           BT_BAP_ASCS_REASON_FRAMING
/*!< PHY */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_PHY               BT_BAP_ASCS_REASON_PHY
/*!< Maximum SDU size*/
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_SDU               BT_BAP_ASCS_REASON_SDU
/*!< RTN */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_RTN               BT_BAP_ASCS_REASON_RTN
/*!< Max transport latency */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_LATENCY           BT_BAP_ASCS_REASON_LATENCY
/*!< Presentation delay */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_PD                BT_BAP_ASCS_REASON_PD
/*!< Invalid CIS mapping */
#define ESP_BLE_AUDIO_BAP_ASCS_REASON_CIS               BT_BAP_ASCS_REASON_CIS
/** Response Reasons */
typedef enum bt_bap_ascs_reason                         esp_ble_audio_bap_ascs_reason_t;

/*!< Server completed operation successfully */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_SUCCESS                 BT_BAP_ASCS_RSP_CODE_SUCCESS
/*!< Server did not support operation by client */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_NOT_SUPPORTED           BT_BAP_ASCS_RSP_CODE_NOT_SUPPORTED
/*!< Server rejected due to invalid operation length */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_INVALID_LENGTH          BT_BAP_ASCS_RSP_CODE_INVALID_LENGTH
/*!< Invalid ASE ID */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_INVALID_ASE             BT_BAP_ASCS_RSP_CODE_INVALID_ASE
/*!< Invalid ASE state */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_INVALID_ASE_STATE       BT_BAP_ASCS_RSP_CODE_INVALID_ASE_STATE
/*!< Invalid operation for direction */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_INVALID_DIR             BT_BAP_ASCS_RSP_CODE_INVALID_DIR
/*!< Capabilities not supported by server */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CAP_UNSUPPORTED         BT_BAP_ASCS_RSP_CODE_CAP_UNSUPPORTED
/*!< Configuration parameters not supported by server */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED        BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED
/*!< Configuration parameters rejected by server */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_REJECTED           BT_BAP_ASCS_RSP_CODE_CONF_REJECTED
/*!< Invalid Configuration parameters */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_CONF_INVALID            BT_BAP_ASCS_RSP_CODE_CONF_INVALID
/*!< Unsupported metadata */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_UNSUPPORTED    BT_BAP_ASCS_RSP_CODE_METADATA_UNSUPPORTED
/*!< Metadata rejected by server */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_REJECTED       BT_BAP_ASCS_RSP_CODE_METADATA_REJECTED
/*!< Invalid metadata */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_METADATA_INVALID        BT_BAP_ASCS_RSP_CODE_METADATA_INVALID
/*!< Server has insufficient resources */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_NO_MEM                  BT_BAP_ASCS_RSP_CODE_NO_MEM
/*!< Unspecified error */
#define ESP_BLE_AUDIO_BAP_ASCS_RSP_CODE_UNSPECIFIED             BT_BAP_ASCS_RSP_CODE_UNSPECIFIED
/** Response Status Code */
typedef enum bt_bap_ascs_rsp_code                               esp_ble_audio_bap_ascs_rsp_code_t;

/** Broadcast Audio Source Endpoint (BASE) structure */
typedef struct bt_bap_base                              esp_ble_audio_bap_base_t;

/** Broadcast Audio Source Endpoint (BASE) subgroup structure */
typedef struct bt_bap_base_subgroup                     esp_ble_audio_bap_base_subgroup_t;

/** Codec ID structure for a Broadcast Audio Source Endpoint (BASE) */
typedef struct bt_bap_base_codec_id                     esp_ble_audio_bap_base_codec_id_t;

/** BIS structure for each BIS in a Broadcast Audio Source Endpoint (BASE) subgroup */
typedef struct bt_bap_base_subgroup_bis                 esp_ble_audio_bap_base_subgroup_bis_t;

/** QoS configuration structure */
typedef struct bt_bap_qos_cfg                           esp_ble_audio_bap_qos_cfg_t;

/** Audio Stream Quality of Service Preference structure */
typedef struct bt_bap_qos_cfg_pref                      esp_ble_audio_bap_qos_cfg_pref_t;

/** Audio Endpoint structure */
typedef struct bt_bap_ep                                esp_ble_audio_bap_ep_t;

/** Structure holding information of audio stream endpoint */
typedef struct bt_bap_ep_info                           esp_ble_audio_bap_ep_info_t;

/** Basic Audio Profile stream structure */
typedef struct bt_bap_stream                            esp_ble_audio_bap_stream_t;

/** Stream operation */
typedef struct bt_bap_stream_ops                        esp_ble_audio_bap_stream_ops_t;

/** Structure storing values of fields of ASE Control Point notification */
typedef struct bt_bap_ascs_rsp                          esp_ble_audio_bap_ascs_rsp_t;

/** Struct to hold subgroup specific information for the receive state */
typedef struct bt_bap_bass_subgroup                     esp_ble_audio_bap_bass_subgroup_t;

/** Audio Unicast Group structure */
typedef struct bt_bap_unicast_group                     esp_ble_audio_bap_unicast_group_t;

/** Parameter struct for each stream in the unicast group */
typedef struct bt_bap_unicast_group_stream_param        esp_ble_audio_bap_unicast_group_stream_param_t;

/** Parameter struct for the unicast group functions */
typedef struct bt_bap_unicast_group_stream_pair_param   esp_ble_audio_bap_unicast_group_stream_pair_param_t;

/** Parameters for creating unicast groups */
typedef struct bt_bap_unicast_group_param               esp_ble_audio_bap_unicast_group_param_t;

/** Structure for registering Unicast Server */
typedef struct bt_bap_unicast_server_register_param     esp_ble_audio_bap_unicast_server_register_param_t;

/** Unicast Server callback structure */
typedef struct bt_bap_unicast_server_cb                 esp_ble_audio_bap_unicast_server_cb_t;

/** Unicast Client callback structure */
typedef struct bt_bap_unicast_client_cb                 esp_ble_audio_bap_unicast_client_cb_t;

/** Broadcast Source advertising information */
typedef esp_ble_iso_ext_adv_info_t                      esp_ble_audio_bap_broadcast_adv_info_t;

/** Broadcast Source callback structure */
typedef struct bt_bap_broadcast_source_cb               esp_ble_audio_bap_broadcast_source_cb_t;

/** Broadcast Source structure */
typedef struct bt_bap_broadcast_source                  esp_ble_audio_bap_broadcast_source_t;

/** Broadcast Source stream parameters */
typedef struct bt_bap_broadcast_source_stream_param     esp_ble_audio_bap_broadcast_source_stream_param_t;

/** Broadcast Source subgroup parameters */
typedef struct bt_bap_broadcast_source_subgroup_param   esp_ble_audio_bap_broadcast_source_subgroup_param_t;

/** Broadcast Source create parameters */
typedef struct bt_bap_broadcast_source_param            esp_ble_audio_bap_broadcast_source_param_t;

/** Broadcast Sink handle */
typedef struct bt_bap_broadcast_sink                    esp_ble_audio_bap_broadcast_sink_t;

/** Broadcast Audio Sink callback structure */
typedef struct bt_bap_broadcast_sink_cb                 esp_ble_audio_bap_broadcast_sink_cb_t;

/** Struct to hold the Basic Audio Profile Broadcast Assistant callbacks */
typedef struct bt_bap_broadcast_assistant_cb            esp_ble_audio_bap_broadcast_assistant_cb_t;

/** Parameters for adding a source to a Broadcast Audio Scan Service server */
typedef struct bt_bap_broadcast_assistant_add_src_param esp_ble_audio_bap_broadcast_assistant_add_src_param_t;

/** Parameters for modifying a source */
typedef struct bt_bap_broadcast_assistant_mod_src_param esp_ble_audio_bap_broadcast_assistant_mod_src_param_t;

/** Struct to hold the Basic Audio Profile Scan Delegator callbacks */
typedef struct bt_bap_scan_delegator_cb                 esp_ble_audio_bap_scan_delegator_cb_t;

/** Represents the Broadcast Audio Scan Service receive state */
typedef struct bt_bap_scan_delegator_recv_state         esp_ble_audio_bap_scan_delegator_recv_state_t;

/** Parameters for adding a source by Scan Delegator */
typedef struct bt_bap_scan_delegator_add_src_param      esp_ble_audio_bap_scan_delegator_add_src_param_t;

/** Parameters for modifying a source by Scan Delegator */
typedef struct bt_bap_scan_delegator_mod_src_param      esp_ble_audio_bap_scan_delegator_mod_src_param_t;

/**
 * @brief   Register the Unicast Server.
 *
 * Register the Unicast Server. Only a single Unicast Server can be registered at any one time.
 * This will register ASCS in the GATT database.
 *
 * @param   param   Registration parameters for ascs.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_server_register(const esp_ble_audio_bap_unicast_server_register_param_t *param);

/**
 * @brief   Unregister the Unicast Server.
 *
 * Unregister the Unicast Server.
 * This will unregister ASCS in the GATT database.
 * Before calling this function, any callbacks registered through
 * esp_ble_audio_bap_unicast_server_register_cb() needs to be
 * unregistered with esp_ble_audio_bap_unicast_server_unregister_cb().
 *
 * Calling this function will issue an release operation on any ASE
 * in a non-idle state.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_server_unregister(void);

/**
 * @brief   Register unicast server callbacks.
 *
 * Only one callback structure can be registered, and attempting to
 * registering more than one will result in an error.
 * Prior to calling this function the Unicast Server needs to be
 * registered with esp_ble_audio_bap_unicast_server_register().
 *
 * @param   cb  Unicast server callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_server_register_cb(const esp_ble_audio_bap_unicast_server_cb_t *cb);

/**
 * @brief   Unregister unicast server callbacks.
 *
 * May only unregister a callback structure that has previously been
 * registered by esp_ble_audio_bap_unicast_server_register_cb().
 *
 * Calling this function will issue an release operation on any ASE
 * in a non-idle state.
 *
 * @param   cb  Unicast server callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_server_unregister_cb(const esp_ble_audio_bap_unicast_server_cb_t *cb);

/**
 * @brief   Initialize and configure a new ASE.
 *
 * @param   conn_handle Connection handle.
 * @param   stream      Configured stream object to be attached to the ASE.
 * @param   codec_cfg   Codec configuration.
 * @param   qos_pref    Audio Stream Quality of Service Preference.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_server_config_ase(uint16_t conn_handle,
                                                      esp_ble_audio_bap_stream_t *stream,
                                                      esp_ble_audio_codec_cfg_t *codec_cfg,
                                                      const esp_ble_audio_bap_qos_cfg_pref_t *qos_pref);

/**
 * @brief   Create unicast group.
 *
 * Create a new audio unicast group with one or more audio streams as a unicast client.
 * All streams shall share the same framing.
 * All streams in the same direction shall share the same interval and latency.
 *
 * @param   param           The unicast group create parameters.
 * @param   unicast_group   Pointer to the unicast group created.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_group_create(esp_ble_audio_bap_unicast_group_param_t *param,
                                                 esp_ble_audio_bap_unicast_group_t **unicast_group);

/**
 * @brief   Reconfigure unicast group.
 *
 * Reconfigure a unicast group with one or more audio streams as a unicast client.
 * All streams shall share the same framing.
 * All streams in the same direction shall share the same interval and latency.
 * All streams in @p param shall already belong to @p unicast_group.
 * Use esp_ble_audio_bap_unicast_group_add_streams() to add additional streams.
 *
 * @param   unicast_group   Pointer to the unicast group created.
 * @param   param           The unicast group reconfigure parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_group_reconfig(esp_ble_audio_bap_unicast_group_t *unicast_group,
                                                   const esp_ble_audio_bap_unicast_group_param_t *param);

/**
 * @brief   Add streams to a unicast group as a unicast client.
 *
 * This function can be used to add additional streams to a BAP unicast group.
 *
 * This can be called at any time before any of the streams in the group has been started.
 * This can also be called after the streams have been stopped.
 *
 * Once a stream has been added to a unicast group, it cannot be removed. To remove a stream
 * from a group, the group must be deleted with esp_ble_audio_bap_unicast_group_delete(),
 * but this will require all streams in the group to be released first.
 *
 * @param   unicast_group   Pointer to the unicast group.
 * @param   params          Array of stream parameters with streams being added to the group.
 * @param   num_param       Number of parameters in @p params.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_group_add_streams(esp_ble_audio_bap_unicast_group_t *unicast_group,
                                                      esp_ble_audio_bap_unicast_group_stream_pair_param_t params[],
                                                      size_t num_param);

/**
 * @brief   Delete audio unicast group.
 *
 * Delete an audio unicast group as a client. All streams in the group shall
 * be in the idle or configured state.
 *
 * @param   unicast_group   Pointer to the unicast group to delete.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_group_delete(esp_ble_audio_bap_unicast_group_t *unicast_group);

/**
 * @brief   Register unicast client callbacks.
 *
 * Only one callback structure can be registered, and attempting to
 * registering more than one will result in an error.
 *
 * @param   cb  Unicast client callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_client_register_cb(esp_ble_audio_bap_unicast_client_cb_t *cb);

/**
 * @brief   Unregister unicast client callbacks.
 *
 * @param   cb  Unicast client callback structure to unregister.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_client_unregister_cb(esp_ble_audio_bap_unicast_client_cb_t *cb);

/**
 * @brief   Discover remote capabilities and endpoints.
 *
 * This procedure is used by a client to discover remote capabilities and
 * endpoints and notifies via params callback.
 *
 * @param   conn_handle Connection handle.
 * @param   dir         The type of remote endpoints and capabilities to discover.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_unicast_client_discover(uint16_t conn_handle, esp_ble_audio_dir_t dir);

/**
 * @brief   Configure Audio Stream.
 *
 * This procedure is used by a client to configure a new stream using the
 * remote endpoint, local capability and codec configuration.
 *
 * @param   conn_handle Connection handle.
 * @param   stream      Stream object being configured.
 * @param   ep          Remote Audio Endpoint being configured.
 * @param   codec_cfg   Codec configuration.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_config(uint16_t conn_handle,
                                          esp_ble_audio_bap_stream_t *stream,
                                          esp_ble_audio_bap_ep_t *ep,
                                          esp_ble_audio_codec_cfg_t *codec_cfg);

/**
 * @brief Configure Audio Stream QoS
 *
 * This procedure is used by a client to configure the Quality of Service of streams in a unicast
 * group. All streams in the group for the specified @p conn_handle will have the Quality of Service
 * configured. This shall only be used to configure unicast streams.
 *
 * @param   conn_handle Connection handle.
 * @param   group       Unicast group object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_qos(uint16_t conn_handle,
                                       esp_ble_audio_bap_unicast_group_t *group);

/**
 * @brief   Enable Audio Stream.
 *
 * This procedure is used by a client to enable a stream.
 *
 * This shall only be called for unicast streams, as broadcast streams
 * will always be enabled once created.
 *
 * @param   stream      Stream object.
 * @param   meta        Metadata.
 * @param   meta_len    Metadata length.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_enable(esp_ble_audio_bap_stream_t *stream,
                                          const uint8_t meta[], size_t meta_len);

/**
 * @brief   Connect unicast audio stream.
 *
 * This procedure is used by a unicast client to connect the connected isochronous stream (CIS)
 * associated with the audio stream. If two audio streams share a CIS, then this only needs to be
 * done once for those streams. This can only be done for streams in the QoS configured or enabled
 * states.
 *
 * @param   stream  Stream object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_connect(esp_ble_audio_bap_stream_t *stream);

/**
 * @brief   Stop Audio Stream.
 *
 * This procedure is used by a client to make a stream stop streaming.
 *
 * This shall only be called for unicast streams.
 * Broadcast sinks cannot be stopped.
 *
 * @param   stream  Stream object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_stop(esp_ble_audio_bap_stream_t *stream);

/**
 * @brief   Reconfigure Audio Stream.
 *
 * This procedure is used by a unicast client or unicast server to reconfigure
 * a stream to use a different local codec configuration.
 *
 * This can only be done for unicast streams.
 *
 * @param   stream      Stream object being reconfigured.
 * @param   codec_cfg   Codec configuration.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_reconfig(esp_ble_audio_bap_stream_t *stream,
                                            esp_ble_audio_codec_cfg_t *codec_cfg);

/**
 * @brief   Change Audio Stream Metadata.
 *
 * This procedure is used by a unicast client or unicast server to change the metadata of a stream.
 *
 * @param   stream      Stream object.
 * @param   meta        Metadata.
 * @param   meta_len    Metadata length.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_metadata(esp_ble_audio_bap_stream_t *stream,
                                            const uint8_t meta[], size_t meta_len);

/**
 * @brief   Disable Audio Stream.
 *
 * This procedure is used by a unicast client or unicast server to disable a stream.
 *
 * This shall only be called for unicast streams, as broadcast streams will
 * always be enabled once created.
 *
 * @param   stream  Stream object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_disable(esp_ble_audio_bap_stream_t *stream);

/**
 * @brief   Start Audio Stream.
 *
 * This procedure is used by a unicast client or unicast server to make a stream start streaming.
 *
 * For the unicast client, this will send the receiver start ready command to the unicast server
 * for ESP_BLE_AUDIO_DIR_SOURCE ASEs. The CIS is required to be connected first by
 * esp_ble_audio_bap_stream_connect() before the command can be sent.
 *
 * For the unicast server, this will execute the receiver start ready command on the unicast server
 * for ESP_BLE_AUDIO_DIR_SINK ASEs. If the CIS is not connected yet, the stream will go into
 * the streaming state as soon as the CIS is connected.
 *
 * This shall only be called for unicast streams.
 *
 * Broadcast sinks will always be started once synchronized, and broadcast
 * source streams shall be started with esp_ble_audio_bap_broadcast_source_start().
 *
 * @param   stream  Stream object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_start(esp_ble_audio_bap_stream_t *stream);

/**
 * @brief   Release Audio Stream.
 *
 * This procedure is used by a unicast client or unicast server to release a unicast stream.
 *
 * Broadcast sink streams cannot be released, but can be deleted by
 * esp_ble_audio_bap_broadcast_sink_delete().
 * Broadcast source streams cannot be released, but can be deleted by
 * esp_ble_audio_bap_broadcast_source_delete().
 *
 * @param   stream  Stream object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_release(esp_ble_audio_bap_stream_t *stream);

/**
 * @brief   Registers callbacks for Broadcast Sources.
 *
 * @param   cb  Pointer to the callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_register_cb(esp_ble_audio_bap_broadcast_source_cb_t *cb);

/**
 * @brief   Unregisters callbacks for Broadcast Sources.
 *
 * @param   cb  Pointer to the callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_unregister_cb(esp_ble_audio_bap_broadcast_source_cb_t *cb);

/**
 * @brief   Create audio broadcast source.
 *
 * Create a new audio broadcast source with one or more audio streams.
 *
 * The broadcast source will be visible for scanners once this has been called,
 * and the device will advertise audio announcements.
 *
 * No audio data can be sent until esp_ble_audio_bap_broadcast_source_start() has been called and
 * no audio information (BIGInfo) will be visible to scanners.
 *
 * @param   param   Pointer to parameters used to create the broadcast source.
 * @param   source  Pointer to the broadcast source created.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_create(esp_ble_audio_bap_broadcast_source_param_t *param,
                                                    esp_ble_audio_bap_broadcast_source_t **source);

/**
 * @brief   Reconfigure audio broadcast source.
 *
 * Reconfigure an audio broadcast source with a new codec and codec quality of
 * service parameters. This can only be done when the source is stopped.
 *
 * Since this may modify the Broadcast Audio Source Endpoint (BASE),
 * esp_ble_audio_bap_broadcast_source_get_base() should be called after this to get the new BASE
 * information.
 *
 * If the @p param.params_count is smaller than the number of subgroups that have been created in
 * the Broadcast Source, only the first @p param.params_count subgroups are updated. If a stream
 * exist in a subgroup not part of @p param, then that stream is left as is (i.e. it is not removed;
 * the only way to remove a stream from a Broadcast Source is to recreate the Broadcast Source).
 *
 * @param   source  Pointer to the broadcast source.
 * @param   param   Pointer to parameters used to reconfigure the broadcast source.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_reconfig(esp_ble_audio_bap_broadcast_source_t *source,
                                                      esp_ble_audio_bap_broadcast_source_param_t *param);

/**
 * @brief   Modify the metadata of an audio broadcast source.
 *
 * Modify the metadata an audio broadcast source. This can only be done when the source is started.
 * To update the metadata in the stopped state, use esp_ble_audio_bap_broadcast_source_reconfig().
 *
 * @param   source      Pointer to the broadcast source.
 * @param   meta        Metadata.
 * @param   meta_len    Length of metadata.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_update_metadata(esp_ble_audio_bap_broadcast_source_t *source,
                                                             const uint8_t meta[], size_t meta_len);

/**
 * @brief   Add extended advertising for an audio broadcast source.
 *
 * @param   info    Extended advertising information.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_adv_add(esp_ble_audio_bap_broadcast_adv_info_t *info);

/**
 * @brief   Delete extended advertising for an audio broadcast source.
 *
 * @param   info    Extended advertising information.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_adv_delete(esp_ble_audio_bap_broadcast_adv_info_t *info);

/**
 * @brief   Start audio broadcast source.
 *
 * Start an audio broadcast source with one or more audio streams.
 * The broadcast source will start advertising BIGInfo, and audio data can be streamed.
 *
 * @param   source      Pointer to the broadcast source.
 * @param   adv_handle  Handle of an extended advertising set with periodic advertising configured.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_start(esp_ble_audio_bap_broadcast_source_t *source,
                                                   uint8_t adv_handle);

/**
 * @brief   Stop audio broadcast source.
 *
 * Stop an audio broadcast source.
 * The broadcast source will stop advertising BIGInfo, and audio data can no longer be streamed.
 *
 * @param   source  Pointer to the broadcast source.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_stop(esp_ble_audio_bap_broadcast_source_t *source);

/**
 * @brief   Delete audio broadcast source.
 *
 * Delete an audio broadcast source.
 * The broadcast source will stop advertising entirely, and the source can no longer be used.
 *
 * @param   source  Pointer to the broadcast source.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_delete(esp_ble_audio_bap_broadcast_source_t *source);

/**
 * @brief   Get the Broadcast Audio Stream Endpoint of a broadcast source.
 *
 * This will encode the BASE of a broadcast source into a buffer, that can be used for
 * advertisement. The encoded BASE will thus be encoded as little-endian. The BASE shall be
 * put into the periodic advertising data.
 *
 * See table 3.15 in the Basic Audio Profile v1.0.1 for the structure.
 *
 * @param   source      Pointer to the broadcast source.
 * @param   base_buf    Pointer to a buffer where the BASE will be inserted.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_source_get_base(esp_ble_audio_bap_broadcast_source_t *source,
                                                      struct net_buf_simple *base_buf);

/**
 * @brief   Register Broadcast sink callbacks.
 *
 * It is possible to register multiple struct of callbacks, but a single struct can only be
 * registered once.
 * Registering the same callback multiple times is undefined behavior and may break the stack.
 *
 * @param   cb  Broadcast sink callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_sink_register_cb(esp_ble_audio_bap_broadcast_sink_cb_t *cb);

/**
 * @brief   Create a Broadcast Sink from a periodic advertising sync.
 *
 * This should only be done after verifying that the periodic advertising sync
 * is from a Broadcast Source.
 *
 * The created Broadcast Sink will need to be supplied to
 * esp_ble_audio_bap_broadcast_sink_sync() in order to synchronize to the broadcast audio.
 *
 * @param   sync_handle     Sync handle.
 * @param   broadcast_id    24-bit broadcast ID.
 * @param   sink            Pointer to the Broadcast Sink created.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_sink_create(uint16_t sync_handle,
                                                  uint32_t broadcast_id,
                                                  esp_ble_audio_bap_broadcast_sink_t **sink);

/**
 * @brief   Sync to a broadcaster's audio.
 *
 * @param   sink                Pointer to the sink object from the base_recv callback.
 * @param   indexes_bitfield    Bitfield of the BIS index to sync to. To sync to e.g. BIS index 1 and
 *                              2, this should have the value of BIT(1) | BIT(2).
 * @param   streams             Stream object pointers to be used for the receiver. If multiple BIS
 *                              indexes shall be synchronized, multiple streams shall be provided.
 * @param   broadcast_code      The 16-octet broadcast code. Shall be supplied if the broadcast is
 *                              encrypted.
 *                              If the value is a string or a the value is less
 *                              than 16 octets, the remaining octets shall be 0.
 *
 *                              Example:
 *                              The string "Broadcast Code" shall be
 *                              [42 72 6F 61 64 63 61 73 74 20 43 6F 64 65 00 00]
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_sink_sync(esp_ble_audio_bap_broadcast_sink_t *sink,
                                                uint32_t indexes_bitfield,
                                                esp_ble_audio_bap_stream_t *streams[],
                                                const uint8_t broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE]);

/**
 * @brief   Stop audio broadcast sink.
 *
 * Stop an audio broadcast sink.
 * The broadcast sink will stop receiving BIGInfo, and audio data can no longer be streamed.
 *
 * @param   sink    Pointer to the broadcast sink.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_sink_stop(esp_ble_audio_bap_broadcast_sink_t *sink);

/**
 * @brief   Release a broadcast sink.
 *
 * Once a broadcast sink has been allocated, it can be deleted using this function.
 * If the sink has synchronized to any broadcast audio streams, these must first be
 * stopped using esp_ble_audio_bap_stream_stop.
 *
 * @param   sink    Pointer to the sink object to delete.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_sink_delete(esp_ble_audio_bap_broadcast_sink_t *sink);

/**
 * @brief   Register the Basic Audio Profile Scan Delegator and BASS.
 *
 * Register the scan delegator and Broadcast Audio Scan Service (BASS)
 * dynamically at runtime.
 *
 * Only one set of callbacks can be registered at any one time, and calling this
 * function multiple times will override any previously registered callbacks.
 *
 * @param   cb  Pointer to the callback struct.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_scan_delegator_register(esp_ble_audio_bap_scan_delegator_cb_t *cb);

/**
 * @brief   Unregister the Basic Audio Profile Scan Delegator and BASS.
 *
 * Unregister the scan delegator and Broadcast Audio Scan Service (BASS)
 * dynamically at runtime.
 *
 * @return  ESP_OK on success, or an error code on failure.
 *
 */
esp_err_t esp_ble_audio_bap_scan_delegator_unregister(void);

/**
 * @brief   Set the periodic advertising sync state to syncing.
 *
 * Set the periodic advertising sync state for a receive state to syncing,
 * notifying Broadcast Assistants.
 *
 * @param   src_id      The source id used to identify the receive state.
 * @param   pa_state    The Periodic Advertising sync state to set.
 *                      ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED and
 *                      ESP_BLE_AUDIO_BAP_PA_STATE_SYNCED is not necessary
 *                      to provide, as they are handled internally.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_scan_delegator_set_pa_state(uint8_t src_id,
                                                        esp_ble_audio_bap_pa_state_t pa_state);

/**
 * @brief   Set the sync state of a receive state in the server.
 *
 * @param   src_id      The source id used to identify the receive state.
 * @param   bis_synced  Array of bitfields to set the BIS sync state for each subgroup.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_scan_delegator_set_bis_sync_state(uint8_t src_id,
                                                              uint32_t bis_synced[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS]);

/**
 * @brief   Add a receive state source locally.
 *
 * This will notify any connected clients about the new source. This allows them
 * to modify and even remove it.
 *
 * @param   param   The parameters for adding the new source.
 * @param   src_id  Source ID on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_scan_delegator_add_src(const esp_ble_audio_bap_scan_delegator_add_src_param_t *param,
                                                   uint8_t *src_id);

/**
 * @brief   Modify a receive state source.
 *
 * This will modify the receive state.
 *
 * @param   param   The parameters for modifying the source.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_scan_delegator_mod_src(const esp_ble_audio_bap_scan_delegator_mod_src_param_t *param);

/**
 * @brief   Remove a receive state source.
 *
 * This will remove the receive state.
 *
 * @param   src_id  The source ID to remove.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_scan_delegator_rem_src(uint8_t src_id);

/**
 * @brief   Discover Broadcast Audio Scan Service on the server.
 *
 * Warning: Only one connection can be active at any time; discovering for
 * a new connection, will delete all previous data.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_discover(uint16_t conn_handle);

/**
 * @brief   Scan start for BISes for a remote server.
 *
 * This will let the Broadcast Audio Scan Service server know that this device
 * is actively scanning for broadcast sources.
 * The function can optionally also start scanning, if the caller does not want
 * to start scanning itself.
 *
 * @param   conn_handle Connection handle.
 * @param   start_scan  Start scanning if true. If false, the application should
 *                      enable scan itself.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_scan_start(uint16_t conn_handle, bool start_scan);

/**
 * @brief   Stop remote scanning for BISes for a server.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_scan_stop(uint16_t conn_handle);

/**
 * @brief   Registers the callbacks used by Broadcast Audio Scan Service client.
 *
 * @param   cb  The callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_register_cb(esp_ble_audio_bap_broadcast_assistant_cb_t *cb);

/**
 * @brief   Unregisters the callbacks used by the Broadcast Audio Scan Service client.
 *
 * @param   cb  The callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_unregister_cb(esp_ble_audio_bap_broadcast_assistant_cb_t *cb);

/**
 * @brief   Add a source on the server.
 *
 * @param   conn_handle Connection handle.
 * @param   param       Parameter struct.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_add_src(uint16_t conn_handle,
                                                        const esp_ble_audio_bap_broadcast_assistant_add_src_param_t *param);

/**
 * @brief   Modify a source on the server.
 *
 * @param   conn_handle Connection handle.
 * @param   param       Parameter struct.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_mod_src(uint16_t conn_handle,
                                                        const esp_ble_audio_bap_broadcast_assistant_mod_src_param_t *param);

/**
 * @brief   Set a broadcast code to the specified receive state.
 *
 * @param   conn_handle     Connection handle.
 * @param   src_id          Source ID of the receive state.
 * @param   broadcast_code  The broadcast code.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_set_broadcast_code(uint16_t conn_handle, uint8_t src_id,
                                                                   const uint8_t broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE]);

/**
 * @brief   Remove a source from the server.
 *
 * @param   conn_handle Connection handle.
 * @param   src_id      Source ID of the receive state.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_rem_src(uint16_t conn_handle, uint8_t src_id);

/**
 * @brief   Read the specified receive state from the server.
 *
 * @param   conn_handle Connection handle.
 * @param   idx         The index of the receive start.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_broadcast_assistant_read_recv_state(uint16_t conn_handle, uint8_t idx);

/**
 * @brief   Return structure holding information of audio stream endpoint.
 *
 * @param   ep      The audio stream endpoint object.
 * @param   info    The structure object to be filled with the info.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_ep_get_info(const esp_ble_audio_bap_ep_t *ep,
                                        esp_ble_audio_bap_ep_info_t *info);

/**
 * @brief   Register Audio callbacks for a stream.
 *
 * Register Audio callbacks for a stream.
 *
 * @param   stream  Stream object.
 * @param   ops     Stream operations structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_cb_register(esp_ble_audio_bap_stream_t *stream,
                                               esp_ble_audio_bap_stream_ops_t *ops);

/**
 * @brief   Send data to Audio stream without timestamp.
 *
 * Send data from buffer to the stream.
 *
 * @param   stream  Stream object.
 * @param   sdu     Buffer containing data to be sent.
 * @param   sdu_len length of data to be sent.
 * @param   seq_num Packet Sequence number. This value shall be incremented for each call to this
 *                  function and at least once per SDU interval for a specific channel.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_send(esp_ble_audio_bap_stream_t *stream,
                                        const uint8_t *sdu, uint16_t sdu_len,
                                        uint16_t seq_num);

/**
 * @brief   Send data to Audio stream with timestamp.
 *
 * @param   stream  Stream object.
 * @param   sdu     Buffer containing data to be sent.
 * @param   sdu_len length of data to be sent.
 * @param   seq_num Packet Sequence number. This value shall be incremented for each call to this
 *                  function and at least once per SDU interval for a specific channel.
 * @param   ts      Timestamp of the SDU in microseconds (us). This value can be used to transmit
 *                  multiple SDUs in the same SDU interval in a CIG or BIG.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_send_ts(esp_ble_audio_bap_stream_t *stream,
                                           const uint8_t *sdu, uint16_t sdu_len,
                                           uint16_t seq_num, uint32_t ts);

/**
 * @brief   Get ISO transmission timing info for a Basic Audio Profile stream.
 *
 * Reads timing information for transmitted ISO packet on an ISO channel.
 * The HCI_LE_Read_ISO_TX_Sync HCI command is used to retrieve this information from the controller.
 *
 * @note An SDU must have already been successfully transmitted on the ISO channel
 * for this function to return successfully.
 *
 * @param   stream  Stream object.
 * @param   info    Transmit info object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_stream_get_tx_sync(esp_ble_audio_bap_stream_t *stream,
                                               esp_ble_iso_tx_info_t *info);

/**
 * @brief   Generate a pointer to a BASE from periodic advertising data
 *
 * @param   data_type   Type of the periodic advertising data.
 * @param   data        Pointer of the periodic advertising data.
 * @param   data_len    Length of the periodic advertising data.
 *
 * @return  NULL if the data does not contain a BASE.
 *          Pointer to an esp_ble_audio_bap_base_t structure.
 */
const esp_ble_audio_bap_base_t *
esp_ble_audio_bap_base_get_base_from_ad(uint8_t data_type, const uint8_t *data, uint8_t data_len);

/**
 * @brief   Get the size of a BASE.
 *
 * @param   base        The BASE pointer.
 * @param   base_size   The size of the BASE on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_get_size(const esp_ble_audio_bap_base_t *base,
                                          size_t *base_size);

/**
 * @brief   Get the presentation delay value of a BASE.
 *
 * @param   base        The BASE pointer.
 * @param   pres_delay  The 24-bit presentation delay value on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_get_pres_delay(const esp_ble_audio_bap_base_t *base,
                                                uint32_t *pres_delay);

/**
 * @brief   Get the subgroup count of a BASE.
 *
 * @param   base            The BASE pointer.
 * @param   subgroup_count  The 8-bit subgroup count value on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_get_subgroup_count(const esp_ble_audio_bap_base_t *base,
                                                    uint8_t *subgroup_count);

/**
 * @brief   Get all BIS indexes of a BASE.
 *
 * @param   base        The BASE pointer.
 * @param   bis_indexes 32-bit BIS index bitfield that will be populated.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_get_bis_indexes(const esp_ble_audio_bap_base_t *base,
                                                 uint32_t *bis_indexes);

/**
 * @brief   Iterate on all subgroups in the BASE.
 *
 * @param   base        The BASE pointer.
 * @param   func        Callback function. Return true to continue iterating, or false to stop.
 * @param   user_data   User data supplied to @p func.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_foreach_subgroup(const esp_ble_audio_bap_base_t *base,
                                                  bool (*func)(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                               void *user_data),
                                                  void *user_data);

/**
 * @brief   Get the codec ID of a subgroup.
 *
 * @param   subgroup    The subgroup pointer.
 * @param   codec_id    Pointer to the struct where the results are placed.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_get_subgroup_codec_id(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                       esp_ble_audio_bap_base_codec_id_t *codec_id);

/**
 * @brief   Get the codec configuration data of a subgroup.
 *
 * @param   subgroup    The subgroup pointer.
 * @param   data        Pointer that will point to the resulting codec configuration data.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_get_subgroup_codec_data(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                         uint8_t **data);

/**
 * @brief   Get the codec metadata of a subgroup.
 *
 * @param   subgroup    The subgroup pointer.
 * @param   meta        Pointer that will point to the resulting codec metadata.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_get_subgroup_codec_meta(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                         uint8_t **meta);

/**
 * @brief   Store subgroup codec data in a esp_ble_audio_codec_cfg_t.
 *
 * @param   subgroup    The subgroup pointer.
 * @param   codec_cfg   Pointer to the struct where the results are placed.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_subgroup_codec_to_codec_cfg(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                             esp_ble_audio_codec_cfg_t *codec_cfg);

/**
 * @brief   Get the BIS count of a subgroup.
 *
 * @param   subgroup    The subgroup pointer.
 * @param   bis_count   The 8-bit BIS count value on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_get_subgroup_bis_count(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                        uint8_t *bis_count);

/**
 * @brief   Get all BIS indexes of a subgroup.
 *
 * @param   subgroup    The subgroup pointer.
 * @param   bis_indexes 32-bit BIS index bitfield that will be populated.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_subgroup_get_bis_indexes(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                          uint32_t *bis_indexes);

/**
 * @brief   Iterate on all BIS in the subgroup.
 *
 * @param   subgroup    The subgroup pointer.
 * @param   func        Callback function. Return true to continue iterating, or false to stop.
 * @param   user_data   User data supplied to @p func.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_subgroup_foreach_bis(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                      bool (*func)(const esp_ble_audio_bap_base_subgroup_bis_t *bis,
                                                                   void *user_data),
                                                      void *user_data);

/**
 * @brief   Store BIS codec configuration data in a esp_ble_audio_codec_cfg_t.
 *
 * This only sets the esp_ble_audio_codec_cfg_t data and esp_ble_audio_codec_cfg_t
 * data_len, but is useful to use the BIS codec configuration data with the
 * esp_ble_audio_codec_cfg_* functions.
 *
 * @param   bis         The BIS pointer.
 * @param   codec_cfg   Pointer to the struct where the results are placed.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_bap_base_subgroup_bis_codec_to_codec_cfg(const esp_ble_audio_bap_base_subgroup_bis_t *bis,
                                                                 esp_ble_audio_codec_cfg_t *codec_cfg);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_BAP_API_H_ */
