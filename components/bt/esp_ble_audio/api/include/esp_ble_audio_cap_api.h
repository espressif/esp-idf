/*
 * SPDX-FileCopyrightText: 2022-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_CAP_API_H_
#define ESP_BLE_AUDIO_CAP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/cap.h>

#include "common/init.h"
#include "common/host.h"

#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_csip_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< The set is an ad-hoc set */
#define ESP_BLE_AUDIO_CAP_SET_TYPE_AD_HOC   BT_CAP_SET_TYPE_AD_HOC
/*!< The set is a CSIP Coordinated Set */
#define ESP_BLE_AUDIO_CAP_SET_TYPE_CSIP     BT_CAP_SET_TYPE_CSIP
/** Type of CAP set */
typedef enum bt_cap_set_type                esp_ble_audio_cap_set_type_t;

/** Common Audio Profile stream structure */
typedef struct bt_cap_stream            esp_ble_audio_cap_stream_t;

/** Common Audio Broadcast Source structure */
typedef struct bt_cap_broadcast_source  esp_ble_audio_cap_broadcast_source_t;

/** Common Audio Unicast Group structure */
typedef struct bt_cap_unicast_group     esp_ble_audio_cap_unicast_group_t;

/** Callback structure for CAP procedures */
typedef struct bt_cap_initiator_cb      esp_ble_audio_cap_initiator_cb_t;

/** Union represents a Common Audio Set member that are either in a Coordinated or ad-hoc set */
typedef union bt_cap_set_member         esp_ble_audio_cap_set_member_t;

/** Parameter structure for each stream in the unicast group */
typedef struct bt_cap_unicast_group_stream_param \
    esp_ble_audio_cap_unicast_group_stream_param_t;

/* Parameter structure for the unicast group functions */
typedef struct bt_cap_unicast_group_stream_pair_param \
    esp_ble_audio_cap_unicast_group_stream_pair_param_t;

/** Parameters for the creating unicast groups */
typedef struct bt_cap_unicast_group_param \
    esp_ble_audio_cap_unicast_group_param_t;

/** CAP unicast audio stream specific parameters for starting audio */
typedef struct bt_cap_unicast_audio_start_stream_param \
    esp_ble_audio_cap_unicast_audio_start_stream_param_t;

/** CAP unicast audio specific parameters for starting audio */
typedef struct bt_cap_unicast_audio_start_param \
    esp_ble_audio_cap_unicast_audio_start_param_t;

/** CAP unicast audio stream specific parameters for updating audio */
typedef struct bt_cap_unicast_audio_update_stream_param \
    esp_ble_audio_cap_unicast_audio_update_stream_param_t;

/** CAP unicast audio specific parameters for updating audio */
typedef struct bt_cap_unicast_audio_update_param \
    esp_ble_audio_cap_unicast_audio_update_param_t;

/** CAP unicast audio specific parameters for stopping audio */
typedef struct bt_cap_unicast_audio_stop_param \
    esp_ble_audio_cap_unicast_audio_stop_param_t;

/** CAP Initiator broadcast audio stream specific parameters */
typedef struct bt_cap_initiator_broadcast_stream_param \
    esp_ble_audio_cap_initiator_broadcast_stream_param_t;

/** CAP Initiator broadcast audio subgroup specific parameters */
typedef struct bt_cap_initiator_broadcast_subgroup_param \
    esp_ble_audio_cap_initiator_broadcast_subgroup_param_t;

/** CAP Initiator broadcast audio specific parameters for creating audio */
typedef struct bt_cap_initiator_broadcast_create_param \
    esp_ble_audio_cap_initiator_broadcast_create_param_t;

/** Callback structure for CAP Handover procedures */
typedef struct bt_cap_handover_cb   esp_ble_audio_cap_handover_cb_t;

/** CAP Handover unicast audio to broadcast audio specific parameters */
typedef struct bt_cap_handover_unicast_to_broadcast_param \
    esp_ble_audio_cap_handover_unicast_to_broadcast_param_t;

/** CAP Handover broadcast audio to unicast audio specific parameters */
typedef struct bt_cap_handover_broadcast_to_unicast_param \
    esp_ble_audio_cap_handover_broadcast_to_unicast_param_t;

/** Callback structure for CAP Commander procedures */
typedef struct bt_cap_commander_cb  esp_ble_audio_cap_commander_cb_t;

/** CAP Commander broadcast audio member specific parameters for starting audio reception */
typedef struct bt_cap_commander_broadcast_reception_start_member_param \
    esp_ble_audio_cap_commander_broadcast_reception_start_member_param_t;

/** CAP Commander broadcast audio specific parameters for starting audio reception */
typedef struct bt_cap_commander_broadcast_reception_start_param \
    esp_ble_audio_cap_commander_broadcast_reception_start_param_t;

/** CAP Commander broadcast audio member specific parameters for stopping audio reception */
typedef struct bt_cap_commander_broadcast_reception_stop_member_param \
    esp_ble_audio_cap_commander_broadcast_reception_stop_member_param_t;

/** CAP Commander broadcast audio specific parameters for stopping audio reception */
typedef struct bt_cap_commander_broadcast_reception_stop_param \
    esp_ble_audio_cap_commander_broadcast_reception_stop_param_t;

/** CAP Commander member parameters for distributing broadcast code */
typedef struct bt_cap_commander_distribute_broadcast_code_member_param \
    esp_ble_audio_cap_commander_distribute_broadcast_code_member_param_t;

/** Parameters for distributing broadcast code */
typedef struct bt_cap_commander_distribute_broadcast_code_param \
    esp_ble_audio_cap_commander_distribute_broadcast_code_param_t;

/** Parameters for changing absolute volume */
typedef struct bt_cap_commander_change_volume_param \
    esp_ble_audio_cap_commander_change_volume_param_t;

/** Parameters for changing volume offset for a member */
typedef struct bt_cap_commander_change_volume_offset_member_param \
    esp_ble_audio_cap_commander_change_volume_offset_member_param_t;

/** Parameters for changing volume offset */
typedef struct bt_cap_commander_change_volume_offset_param \
    esp_ble_audio_cap_commander_change_volume_offset_param_t;

/** Parameters for changing volume mute state */
typedef struct bt_cap_commander_change_volume_mute_state_param \
    esp_ble_audio_cap_commander_change_volume_mute_state_param_t;

/** Parameters for changing microphone mute state */
typedef struct bt_cap_commander_change_microphone_mute_state_param \
    esp_ble_audio_cap_commander_change_microphone_mute_state_param_t;

/** Parameters for changing microphone gain setting for a member */
typedef struct bt_cap_commander_change_microphone_gain_setting_member_param \
    esp_ble_audio_cap_commander_change_microphone_gain_setting_member_param_t;

/** Parameters for changing microphone gain setting */
typedef struct bt_cap_commander_change_microphone_gain_setting_param \
    esp_ble_audio_cap_commander_change_microphone_gain_setting_param_t;

/**
 * @brief   Register the Common Audio Service.
 *
 * This will register and enable the service and make it discoverable by
 * clients. This will also register a Coordinated Set Identification
 * Service instance.
 *
 * @param   param       Coordinated Set Identification Service register parameters.
 * @param   svc_inst    Pointer to the registered Coordinated Set Identification Service.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_acceptor_register(const esp_ble_audio_csip_set_member_register_param_t *param,
                                              esp_ble_audio_csip_set_member_svc_inst_t **svc_inst);

/**
 * @brief   Discovers audio support on a remote device.
 *
 * This will discover the Common Audio Service (CAS) on the remote device, to
 * verify if the remote device supports the Common Audio Profile.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_unicast_discover(uint16_t conn_handle);

/**
 * @brief   Register Audio operations for a Common Audio Profile stream.
 *
 * Register Audio operations for a stream.
 *
 * @param   stream  Stream object.
 * @param   ops     Stream operations structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_stream_ops_register(esp_ble_audio_cap_stream_t *stream,
                                                esp_ble_audio_bap_stream_ops_t *ops);

/**
 * @brief   Send data to Common Audio Profile stream without timestamp.
 *
 * Sends data over the specified CAP audio stream with a sequence number.
 *
 * @param   stream  Stream object.
 * @param   sdu     Buffer containing data to be sent.
 * @param   sdu_len Length of data to be sent.
 * @param   seq_num Packet Sequence number. This value shall be incremented
 *                  for each call to this function and at least once per SDU
 *                  interval for a specific channel.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_stream_send(esp_ble_audio_cap_stream_t *stream,
                                        const uint8_t *sdu, uint16_t sdu_len,
                                        uint16_t seq_num);

/**
 * @brief   Send data to Common Audio Profile stream with timestamp.
 *
 * Sends data over the specified CAP audio stream with a sequence number.
 *
 * @param   stream  Stream object.
 * @param   sdu     Buffer containing data to be sent.
 * @param   sdu_len Length of data to be sent.
 * @param   seq_num Packet Sequence number. This value shall be incremented
 *                  for each call to this function and at least once per SDU
 *                  interval for a specific channel.
 * @param   ts      Timestamp of the SDU in microseconds (us). This value can
 *                  be used to transmit multiple SDUs in the same SDU interval
 *                  in a CIG or BIG.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_stream_send_ts(esp_ble_audio_cap_stream_t *stream,
                                           const uint8_t *sdu, uint16_t sdu_len,
                                           uint16_t seq_num, uint32_t ts);

/**
 * @brief   Get ISO transmission timing info for a Common Audio Profile stream.
 *
 * @param   stream  Stream object.
 * @param   info    Transmit info object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_stream_get_tx_sync(esp_ble_audio_cap_stream_t *stream,
                                               esp_ble_iso_tx_info_t *info);

/**
 * @brief   Create unicast group.
 *
 * Create a new audio unicast group with one or more audio streams as a unicast client.
 * All streams shall share the same framing.
 * All streams in the same direction shall share the same interval and latency (see
 * esp_ble_audio_bap_qos_cfg_t).
 *
 * @param   param           The unicast group create parameters.
 * @param   unicast_group   Pointer to the unicast group created.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_unicast_group_create(const esp_ble_audio_cap_unicast_group_param_t *param,
                                                 esp_ble_audio_cap_unicast_group_t **unicast_group);

/**
 * @brief   Reconfigure unicast group.
 *
 * Reconfigure a unicast group with one or more audio streams as a unicast client.
 * All streams shall share the same framing.
 * All streams in the same direction shall share the same interval and latency (see
 * esp_ble_audio_bap_qos_cfg_t).
 * All streams in @p param shall already belong to @p unicast_group.
 * Use esp_ble_audio_cap_unicast_group_add_streams() to add additional streams.
 *
 * @param   unicast_group   Pointer to the unicast group created.
 * @param   param           The unicast group reconfigure parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_unicast_group_reconfig(esp_ble_audio_cap_unicast_group_t *unicast_group,
                                                   const esp_ble_audio_cap_unicast_group_param_t *param);

/**
 * @brief   Add streams to a unicast group as a unicast client.
 *
 * This function can be used to add additional streams to a esp_ble_audio_cap_unicast_group_t.
 *
 * This can be called at any time before any of the streams in the group has been started
 * (see esp_ble_audio_bap_stream_ops_t.started()).
 * This can also be called after the streams have been stopped (see
 * esp_ble_audio_bap_stream_ops_t.stopped()).
 *
 * Once a stream has been added to a unicast group, it cannot be removed. To remove a stream
 * from a group, the group must be deleted with esp_ble_audio_cap_unicast_group_delete(), but
 * this will require all streams in the group to be released first.
 *
 * @param   unicast_group   Pointer to the unicast group
 * @param   params          Array of stream parameters with streams being added to the group.
 * @param   num_param       Number of parameters in @p params.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_unicast_group_add_streams(esp_ble_audio_cap_unicast_group_t *unicast_group,
                                                      const esp_ble_audio_cap_unicast_group_stream_pair_param_t params[],
                                                      size_t num_param);

/**
 * @brief   Delete audio unicast group.
 *
 * Delete a audio unicast group as a client. All streams in the group shall
 * be in the idle or configured state.
 *
 * @param   unicast_group   Pointer to the unicast group to delete.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_unicast_group_delete(esp_ble_audio_cap_unicast_group_t *unicast_group);

/**
 * @brief   Register Common Audio Profile Initiator callbacks.
 *
 * @param   cb  The callback structure. Shall remain static.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_register_cb(const esp_ble_audio_cap_initiator_cb_t *cb);

/**
 * @brief   Unregister Common Audio Profile Initiator callbacks.
 *
 * @param   cb  The callback structure that was previously registered.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_unregister_cb(const esp_ble_audio_cap_initiator_cb_t *cb);

/**
 * @brief   Setup and start unicast audio streams for a set of devices.
 *
 * @param   param   Parameters to start the audio streams.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_unicast_audio_start(const esp_ble_audio_cap_unicast_audio_start_param_t *param);

/**
 * @brief   Update unicast audio streams.
 *
 * This will update the metadata of one or more streams.
 *
 * @param   param   Update parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_unicast_audio_update(const esp_ble_audio_cap_unicast_audio_update_param_t *param);

/**
 * @brief   Stop unicast audio streams.
 *
 * This will stop one or more streams.
 *
 * @param   param   Stop parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_unicast_audio_stop(const esp_ble_audio_cap_unicast_audio_stop_param_t *param);

/**
 * @brief   Cancel any current Common Audio Profile procedure.
 *
 * This will stop the current procedure from continuing and making it possible to run a new
 * Common Audio Profile procedure.
 *
 * It is recommended to do this if any existing procedure takes longer time than expected, which
 * could indicate a missing response from the Common Audio Profile Acceptor.
 *
 * This does not send any requests to any Common Audio Profile Acceptors involved with the current
 * procedure, and thus notifications from the Common Audio Profile Acceptors may arrive after this
 * has been called. It is thus recommended to either only use this if a procedure has stalled, or
 * wait a short while before starting any new Common Audio Profile procedure after this has been
 * called to avoid getting notifications from the cancelled procedure. The wait time depends on
 * the connection interval, the number of devices in the previous procedure and the behavior of the
 * Common Audio Profile Acceptors.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_unicast_audio_cancel(void);

/**
 * @brief   Create a Common Audio Profile broadcast source.
 *
 * Create a new audio broadcast source with one or more audio streams.
 *
 * @param   param               Parameters to start the audio streams.
 * @param   broadcast_source    Pointer to the broadcast source created.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_create(const esp_ble_audio_cap_initiator_broadcast_create_param_t *param,
                                                             esp_ble_audio_cap_broadcast_source_t **broadcast_source);

/**
 * @brief   Start Common Audio Profile broadcast source.
 *
 * The broadcast source will be visible for scanners once this has been called,
 * and the device will advertise audio announcements.
 *
 * @param   broadcast_source    Pointer to the broadcast source.
 * @param   adv_handle          Handle of an extended advertising set with
 *                              periodic advertising configured.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_start(
    esp_ble_audio_cap_broadcast_source_t *broadcast_source,
    uint8_t adv_handle);

/**
 * @brief   Update broadcast audio streams for a Common Audio Profile broadcast source.
 *
 * @param   broadcast_source    The broadcast source to update.
 * @param   meta                The new metadata. The metadata shall contain a list
 *                              of CCIDs as well as a non-0 context bitfield.
 * @param   meta_len            The length of @p meta.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_update(esp_ble_audio_cap_broadcast_source_t *broadcast_source,
                                                             const uint8_t meta[], size_t meta_len);

/**
 * @brief   Stop broadcast audio streams for a Common Audio Profile broadcast source.
 *
 * @param   broadcast_source    The broadcast source to stop. The audio streams
 *                              in this will be stopped and reset.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_stop(esp_ble_audio_cap_broadcast_source_t *broadcast_source);

/**
 * @brief   Delete Common Audio Profile broadcast source.
 *
 * @param   broadcast_source    The broadcast source to delete.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_delete(esp_ble_audio_cap_broadcast_source_t *broadcast_source);

/**
 * @brief   Get the Broadcast Audio Stream Endpoint of a Common Audio Profile
 *          broadcast source.
 *
 * This will encode the BASE of a broadcast source into a buffer, that can be
 * used for advertisement. The encoded BASE will thus be encoded as
 * little-endian. The BASE shall be put into the periodic advertising data.
 *
 * See table 3.15 in the Basic Audio Profile v1.0.1 for the structure.
 *
 * @param   broadcast_source    Pointer to the broadcast source.
 * @param   base_buf            Pointer to a buffer where the BASE will be inserted.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_initiator_broadcast_get_base(esp_ble_audio_cap_broadcast_source_t *broadcast_source,
                                                         struct net_buf_simple *base_buf);

/**
 * @brief   Register Common Audio Profile Handover callbacks.
 *
 * @param   cb  The callback structure. Shall remain static.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_handover_register_cb(const esp_ble_audio_cap_handover_cb_t *cb);

/**
 * @brief   Unregister Common Audio Profile Handover callbacks.
 *
 * @param   cb  The callback structure that was previously registered.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_handover_unregister_cb(const esp_ble_audio_cap_handover_cb_t *cb);

/**
 * @brief   Hands over the sink streams in a unicast group to a broadcast source.
 *
 * All streams in the provided unicast group will be stopped and released. The sink streams
 * will be transferred to a broadcast source, and the broadcast source information will be
 * shared with all accepters that are currently receiving audio. Any stream that is not in
 * the streaming state will only be released.
 *
 * esp_ble_audio_bap_broadcast_assistant_discover() must have been successfully perform for
 * all members in @p param before starting this procedure.
 *
 * @param   param   The parameters for the handover.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_handover_unicast_to_broadcast(
    const esp_ble_audio_cap_handover_unicast_to_broadcast_param_t *param);

/**
 * @brief   Hands over the data streams in a broadcast source to a unicast group.
 *
 * The streams in the broadcast source will be stopped and the broadcast source
 * will be deleted.
 *
 * @param   param   The parameters for the handover.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_handover_broadcast_to_unicast(
    const esp_ble_audio_cap_handover_broadcast_to_unicast_param_t *param);

/**
 * @brief   Register Common Audio Profile Commander callbacks.
 *
 * @param   cb  The callback structure. Shall remain static.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_register_cb(const esp_ble_audio_cap_commander_cb_t *cb);

/**
 * @brief   Unregister Common Audio Profile Commander callbacks.
 *
 * @param   cb  The callback structure that was previously registered.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_unregister_cb(const esp_ble_audio_cap_commander_cb_t *cb);

/**
 * @brief   Discovers audio support on a remote device.
 *
 * This will discover the Common Audio Service (CAS) on the remote device,
 * to verify if the remote device supports the Common Audio Profile.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_discover(uint16_t conn_handle);

/**
 * @brief   Cancel any current Common Audio Profile commander procedure.
 *
 * This will stop the current procedure from continuing and making it possible to run a new
 * Common Audio Profile procedure.
 *
 * It is recommended to do this if any existing procedure takes longer time than expected, which
 * could indicate a missing response from the Common Audio Profile Acceptor.
 *
 * This does not send any requests to any Common Audio Profile Acceptors involved with the
 * current procedure, and thus notifications from the Common Audio Profile Acceptors may
 * arrive after this has been called. It is thus recommended to either only use this if a
 * procedure has stalled, or wait a short while before starting any new Common Audio Profile
 * procedure after this has been called to avoid getting notifications from the cancelled
 * procedure. The wait time depends on the connection interval, the number of devices in the
 * previous procedure and the behavior of the Common Audio Profile Acceptors.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_cancel(void);

/**
 * @brief   Starts the reception of broadcast audio on one or more remote
 *          Common Audio Profile Acceptors.
 *
 * @param   param   The parameters to start the broadcast audio
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_broadcast_reception_start(
    const esp_ble_audio_cap_commander_broadcast_reception_start_param_t *param);

/**
 * @brief   Stops the reception of broadcast audio on one or more remote
 *          Common Audio Profile Acceptors.
 *
 * @param   param The parameters to stop the broadcast audio
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_broadcast_reception_stop(
    const esp_ble_audio_cap_commander_broadcast_reception_stop_param_t *param);

/**
 * @brief   Distributes the broadcast code on one or more remote Common
 *          Audio Profile Acceptors.
 *
 * @param   param   The parameters for distributing the broadcast code.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_distribute_broadcast_code(
    const esp_ble_audio_cap_commander_distribute_broadcast_code_param_t *param);

/**
 * @brief   Change the volume on one or more Common Audio Profile Acceptors.
 *
 * @param   param   The parameters for the volume change.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_change_volume(
    const esp_ble_audio_cap_commander_change_volume_param_t *param);

/**
 * @brief   Change the volume offset on one or more Common Audio Profile Acceptors.
 *
 * @param   param   The parameters for the volume offset change.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_change_volume_offset(
    const esp_ble_audio_cap_commander_change_volume_offset_param_t *param);

/**
 * @brief   Change the volume mute state on one or more Common Audio Profile Acceptors.
 *
 * @param   param   The parameters for the volume mute state change.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_change_volume_mute_state(
    const esp_ble_audio_cap_commander_change_volume_mute_state_param_t *param);

/**
 * @brief   Change the microphone mute state on one or more Common Audio Profile Acceptors.
 *
 * @param   param   The parameters for the microphone mute state change.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_change_microphone_mute_state(
    const esp_ble_audio_cap_commander_change_microphone_mute_state_param_t *param);

/**
 * @brief   Change the microphone gain setting on one or more Common Audio Profile Acceptors.
 *
 * @param   param   The parameters for the microphone gain setting change.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_cap_commander_change_microphone_gain_setting(
    const esp_ble_audio_cap_commander_change_microphone_gain_setting_param_t *param);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_CAP_API_H_ */
