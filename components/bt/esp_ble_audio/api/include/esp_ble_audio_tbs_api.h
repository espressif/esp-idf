/*
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2021 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_TBS_API_H_
#define ESP_BLE_AUDIO_TBS_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/tbs.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The GTBS index denotes whenever a callback is from a Generic Telephone
 * Bearer Service (GTBS) instance, or whenever the client should perform
 * on action on the GTBS instance of the server, rather than any of the
 * specific Telephone Bearer Service instances.
 */
#define ESP_BLE_AUDIO_TBS_GTBS_INDEX                            BT_TBS_GTBS_INDEX

/*!< A remote party is calling (incoming call). */
#define ESP_BLE_AUDIO_TBS_CALL_STATE_INCOMING                   BT_TBS_CALL_STATE_INCOMING
/**
 * The process to call the remote party has started on the server, but the
 * remote party is not being alerted (outgoing call).
 */
#define ESP_BLE_AUDIO_TBS_CALL_STATE_DIALING                    BT_TBS_CALL_STATE_DIALING
/*!< A remote party is being alerted (outgoing call). */
#define ESP_BLE_AUDIO_TBS_CALL_STATE_ALERTING                   BT_TBS_CALL_STATE_ALERTING
/*!< The call is in an active conversation. */
#define ESP_BLE_AUDIO_TBS_CALL_STATE_ACTIVE                     BT_TBS_CALL_STATE_ACTIVE
/**
 * The call is connected but held locally. Locally Held implies that either
 * the server or the client can affect the state.
 */
#define ESP_BLE_AUDIO_TBS_CALL_STATE_LOCALLY_HELD               BT_TBS_CALL_STATE_LOCALLY_HELD
/**
 * The call is connected but held remotely. Remotely Held means that the state
 * is controlled by the remote party of a call.
 */
#define ESP_BLE_AUDIO_TBS_CALL_STATE_REMOTELY_HELD              BT_TBS_CALL_STATE_REMOTELY_HELD
/*!< The call is connected but held both locally and remotely. */
#define ESP_BLE_AUDIO_TBS_CALL_STATE_LOCALLY_AND_REMOTELY_HELD  BT_TBS_CALL_STATE_LOCALLY_AND_REMOTELY_HELD

/*!< The URI value used to originate a call was formed improperly. */
#define ESP_BLE_AUDIO_TBS_REASON_BAD_REMOTE_URI                 BT_TBS_REASON_BAD_REMOTE_URI
/*!< The call failed. */
#define ESP_BLE_AUDIO_TBS_REASON_CALL_FAILED                    BT_TBS_REASON_CALL_FAILED
/*!< The remote party ended the call. */
#define ESP_BLE_AUDIO_TBS_REASON_REMOTE_ENDED_CALL              BT_TBS_REASON_REMOTE_ENDED_CALL
/*!< The call ended from the server. */
#define ESP_BLE_AUDIO_TBS_REASON_SERVER_ENDED_CALL              BT_TBS_REASON_SERVER_ENDED_CALL
/*!< The line was busy. */
#define ESP_BLE_AUDIO_TBS_REASON_LINE_BUSY                      BT_TBS_REASON_LINE_BUSY
/*!< Network congestion. */
#define ESP_BLE_AUDIO_TBS_REASON_NETWORK_CONGESTED              BT_TBS_REASON_NETWORK_CONGESTED
/*!< The client terminated the call. */
#define ESP_BLE_AUDIO_TBS_REASON_CLIENT_TERMINATED              BT_TBS_REASON_CLIENT_TERMINATED
/*!< No service. */
#define ESP_BLE_AUDIO_TBS_REASON_NO_SERVICE                     BT_TBS_REASON_NO_SERVICE
/*!< No answer. */
#define ESP_BLE_AUDIO_TBS_REASON_NO_ANSWER                      BT_TBS_REASON_NO_ANSWER
/*!< Unspecified. */
#define ESP_BLE_AUDIO_TBS_REASON_UNSPECIFIED                    BT_TBS_REASON_UNSPECIFIED

/*!< The opcode write was successful. */
#define ESP_BLE_AUDIO_TBS_RESULT_CODE_SUCCESS                   BT_TBS_RESULT_CODE_SUCCESS
/*!< An invalid opcode was used for the Call Control Point write. */
#define ESP_BLE_AUDIO_TBS_RESULT_CODE_OPCODE_NOT_SUPPORTED      BT_TBS_RESULT_CODE_OPCODE_NOT_SUPPORTED
/*!< The requested operation cannot be completed. */
#define ESP_BLE_AUDIO_TBS_RESULT_CODE_OPERATION_NOT_POSSIBLE    BT_TBS_RESULT_CODE_OPERATION_NOT_POSSIBLE
/*!< The Call Index used for the Call Control Point write is invalid. */
#define ESP_BLE_AUDIO_TBS_RESULT_CODE_INVALID_CALL_INDEX        BT_TBS_RESULT_CODE_INVALID_CALL_INDEX
/**
 * The opcode written to the Call Control Point was received when the current
 * Call State for the Call Index was not in the expected state.
 */
#define ESP_BLE_AUDIO_TBS_RESULT_CODE_STATE_MISMATCH            BT_TBS_RESULT_CODE_STATE_MISMATCH
/*!< Lack of internal resources to complete the requested action. */
#define ESP_BLE_AUDIO_TBS_RESULT_CODE_OUT_OF_RESOURCES          BT_TBS_RESULT_CODE_OUT_OF_RESOURCES
/*!< The Outgoing URI is incorrect or invalid when an Originate opcode is sent. */
#define ESP_BLE_AUDIO_TBS_RESULT_CODE_INVALID_URI               BT_TBS_RESULT_CODE_INVALID_URI

/*!< Local Hold and Local Retrieve Call Control Point Opcodes supported */
#define ESP_BLE_AUDIO_TBS_FEATURE_HOLD                          BT_TBS_FEATURE_HOLD
/*!< Join Call Control Point Opcode supported */
#define ESP_BLE_AUDIO_TBS_FEATURE_JOIN                          BT_TBS_FEATURE_JOIN
/*!< All Control Point Opcodes supported */
#define ESP_BLE_AUDIO_TBS_FEATURE_ALL                           BT_TBS_FEATURE_ALL

/*!< No service */
#define ESP_BLE_AUDIO_TBS_SIGNAL_STRENGTH_NO_SERVICE            BT_TBS_SIGNAL_STRENGTH_NO_SERVICE
/*!< Maximum signal strength */
#define ESP_BLE_AUDIO_TBS_SIGNAL_STRENGTH_MAX                   BT_TBS_SIGNAL_STRENGTH_MAX
/*!< Signal strength is unknown  */
#define ESP_BLE_AUDIO_TBS_SIGNAL_STRENGTH_UNKNOWN               BT_TBS_SIGNAL_STRENGTH_UNKNOWN

/*!< 3G */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_3G                         BT_TBS_TECHNOLOGY_3G
/*!< 4G */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_4G                         BT_TBS_TECHNOLOGY_4G
/*!< Long-term evolution (LTE) */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_LTE                        BT_TBS_TECHNOLOGY_LTE
/*!< Wifi */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_WIFI                       BT_TBS_TECHNOLOGY_WIFI
/*!< 5G */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_5G                         BT_TBS_TECHNOLOGY_5G
/*!< Global System for Mobile Communications (GSM) */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_GSM                        BT_TBS_TECHNOLOGY_GSM
/*!< Code-Division Multiple Access (CDMA) */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_CDMA                       BT_TBS_TECHNOLOGY_CDMA
/*!< 2G */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_2G                         BT_TBS_TECHNOLOGY_2G
/*!< Wideband Code-Division Multiple Access (WCDMA) */
#define ESP_BLE_AUDIO_TBS_TECHNOLOGY_WCDMA                      BT_TBS_TECHNOLOGY_WCDMA

/*!< Inband ringtone enabled */
#define ESP_BLE_AUDIO_TBS_STATUS_FLAG_INBAND_RINGTONE           BT_TBS_STATUS_FLAG_INBAND_RINGTONE
/*!< Server is in silent mod */
#define ESP_BLE_AUDIO_TBS_STATUS_FLAG_SILENT_MOD                BT_TBS_STATUS_FLAG_SILENT_MOD

/*!< If set, call is outgoing else incoming */
#define ESP_BLE_AUDIO_TBS_CALL_FLAG_OUTGOING                    BT_TBS_CALL_FLAG_OUTGOING
/*!< If set call is withheld, else not withheld */
#define ESP_BLE_AUDIO_TBS_CALL_FLAG_WITHHELD                    BT_TBS_CALL_FLAG_WITHHELD
/*!< If set call is withheld by network, else provided by network */
#define ESP_BLE_AUDIO_TBS_CALL_FLAG_WITHHELD_BY_NETWORK         BT_TBS_CALL_FLAG_WITHHELD_BY_NETWORK

/** Struct to hold the Telephone Bearer Service callbacks */
typedef struct bt_tbs_cb                esp_ble_audio_tbs_cb_t;

/** Telephone Bearer Service instance */
typedef struct bt_tbs_instance          esp_ble_audio_tbs_instance_t;

/** Struct to hold the parameters to initialize the bearer */
typedef struct bt_tbs_register_param    esp_ble_audio_tbs_register_param_t;

/** Struct to hold the Telephone Bearer Service client callbacks */
typedef struct bt_tbs_client_cb         esp_ble_audio_tbs_client_cb_t;

/** Struct to hold a call as the Telephone Bearer Service client */
typedef struct bt_tbs_client_call       esp_ble_audio_tbs_client_call_t;

/** Struct to hold a call state */
typedef struct bt_tbs_client_call_state esp_ble_audio_tbs_client_call_state_t;

/**
 * @brief   Accept an alerting call.
 *
 * @param   call_index  The index of the call that will be accepted.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_accept(uint8_t call_index);

/**
 * @brief   Hold a call.
 *
 * @param   call_index  The index of the call that will be held.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_hold(uint8_t call_index);

/**
 * @brief   Retrieve a call.
 *
 * @param   call_index  The index of the call that will be retrieved.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_retrieve(uint8_t call_index);

/**
 * @brief   Terminate a call.
 *
 * @param   call_index  The index of the call that will be terminated.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_terminate(uint8_t call_index);

/**
 * @brief   Originate a call.
 *
 * @param   bearer_index    The index of the Telephone Bearer.
 * @param   uri             The remote URI.
 * @param   call_index      Pointer to a value where the new call_index will be stored.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_originate(uint8_t bearer_index,
                                      char *uri,
                                      uint8_t *call_index);

/**
 * @brief   Join calls.
 *
 * @param   call_index_cnt  The number of call indexes to join.
 * @param   call_indexes    Array of call indexes to join.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_join(uint8_t call_index_cnt, uint8_t *call_indexes);

/**
 * @brief   Notify the server that the remote party answered the call.
 *
 * @param   call_index  The index of the call that was answered.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_remote_answer(uint8_t call_index);

/**
 * @brief   Notify the server that the remote party held the call.
 *
 * @param   call_index  The index of the call that was held.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_remote_hold(uint8_t call_index);

/**
 * @brief   Notify the server that the remote party retrieved the call.
 *
 * @param   call_index  The index of the call that was retrieved.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_remote_retrieve(uint8_t call_index);

/**
 * @brief   Notify the server that the remote party terminated the call.
 *
 * @param   call_index  The index of the call that was terminated.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_remote_terminate(uint8_t call_index);

/**
 * @brief   Notify the server of an incoming call.
 *
 * @param   bearer_index    The index of the Telephone Bearer.
 * @param   to              The URI that is receiving the call.
 * @param   from            The URI of the remote caller.
 * @param   friendly_name   The friendly name of the remote caller.
 * @param   call_index      The call index on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_remote_incoming(uint8_t bearer_index,
                                            const char *to,
                                            const char *from,
                                            const char *friendly_name,
                                            uint8_t *call_index);

/**
 * @brief   Set a new bearer provider.
 *
 * @param   bearer_index    The index of the Telephone Bearer or
 *                          ESP_BLE_AUDIO_TBS_GTBS_INDEX for GTBS.
 * @param   name            The new bearer provider name.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_set_bearer_provider_name(uint8_t bearer_index, const char *name);

/**
 * @brief   Set a new bearer technology.
 *
 * @param   bearer_index    The index of the Telephone Bearer or
 *                          ESP_BLE_AUDIO_TBS_GTBS_INDEX for GTBS.
 * @param   new_technology  The new bearer technology.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_set_bearer_technology(uint8_t bearer_index, uint8_t new_technology);

/**
 * @brief   Update the signal strength reported by the server.
 *
 * @param   bearer_index        The index of the Telephone Bearer or
 *                              ESP_BLE_AUDIO_TBS_GTBS_INDEX for GTBS.
 * @param   new_signal_strength The new signal strength.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_set_signal_strength(uint8_t bearer_index, uint8_t new_signal_strength);

/**
 * @brief   Sets the feature and status value.
 *
 * @param   bearer_index    The index of the Telephone Bearer or
 *                          ESP_BLE_AUDIO_TBS_GTBS_INDEX for GTBS.
 * @param   status_flags    The new feature and status value.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_set_status_flags(uint8_t bearer_index, uint16_t status_flags);

/**
 * @brief   Sets the URI scheme list of a bearer.
 *
 * @param   bearer_index    The index of the Telephone Bearer.
 * @param   uri_list        List of URI prefixes (e.g. {"skype", "tel"}).
 * @param   uri_count       Number of URI prefixes in @p uri_list.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_set_uri_scheme_list(uint8_t bearer_index,
                                                const char **uri_list,
                                                uint8_t uri_count);

/**
 * @brief   Register the callbacks for TBS.
 *
 * @param   cbs Pointer to the callback structure.
 */
void esp_ble_audio_tbs_register_cb(esp_ble_audio_tbs_cb_t *cbs);

/**
 * @brief   Register a Telephone Bearer.
 *
 * This will register a Telephone Bearer Service (TBS) (or a Generic Telephone
 * Bearer service (GTBS)) with the provided parameters.
 *
 * As per the TBS specification, the GTBS shall be instantiated for the feature,
 * and as such a GTBS shall always be registered before any TBS can be registered.
 *
 * @param   param           The parameters to initialize the bearer.
 * @param   bearer_index    The registered bearer index on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_register_bearer(const esp_ble_audio_tbs_register_param_t *param,
                                            uint8_t *bearer_index);

/**
 * @brief   Unregister a Telephone Bearer.
 *
 * This will unregister a Telephone Bearer Service (TBS) (or a Generic Telephone
 * Bearer service (GTBS)) with the provided parameters.
 *
 * Similarly, all TBS shall be unregistered before the GTBS can be unregistered with.
 *
 * @param   bearer_index    The index of the bearer to unregister.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_unregister_bearer(uint8_t bearer_index);

/**
 * @brief   Discover TBS for a connection. This will start a GATT
 *          discover and setup handles and subscriptions.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_discover(uint16_t conn_handle);

/**
 * @brief   Set the signal strength reporting interval for a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 * @param   interval    The interval to write (0-255 seconds).
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_set_signal_strength_interval(uint16_t conn_handle,
                                                                uint8_t inst_index,
                                                                uint8_t interval);

/**
 * @brief   Request to originate a call.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 * @param   uri         The URI of the callee.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_originate_call(uint16_t conn_handle,
                                                  uint8_t inst_index,
                                                  const char *uri);

/**
 * @brief   Request to terminate a call.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 * @param   call_index  The call index to terminate.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_terminate_call(uint16_t conn_handle,
                                                  uint8_t inst_index,
                                                  uint8_t call_index);

/**
 * @brief   Request to hold a call.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 * @param   call_index  The call index to place on hold.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_hold_call(uint16_t conn_handle,
                                             uint8_t inst_index,
                                             uint8_t call_index);

/**
 * @brief   Accept an incoming call.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 * @param   call_index  The call index to accept.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_accept_call(uint16_t conn_handle,
                                               uint8_t inst_index,
                                               uint8_t call_index);

/**
 * @brief   Retrieve call from (local) hold.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 * @param   call_index  The call index to retrieve.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_retrieve_call(uint16_t conn_handle,
                                                 uint8_t inst_index,
                                                 uint8_t call_index);

/**
 * @brief   Join multiple calls.
 *
 * @param   conn_handle     Connection handle.
 * @param   inst_index      The index of the TBS instance.
 * @param   call_indexes    Array of call indexes.
 * @param   count           Number of call indexes in the call_indexes array.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_join_calls(uint16_t conn_handle,
                                              uint8_t inst_index,
                                              const uint8_t *call_indexes,
                                              uint8_t count);

/**
 * @brief   Read the bearer provider name of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_bearer_provider_name(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the UCI of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_bearer_uci(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the technology of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_technology(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the URI schemes list of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_uri_list(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the current signal strength of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_signal_strength(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the signal strength reporting interval of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_signal_interval(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the list of current calls of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_current_calls(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the content ID of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_ccid(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the call target URI of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_call_uri(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the feature and status value of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_status_flags(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the states of the current calls of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_call_state(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the remote URI of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_remote_uri(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the friendly name of a call for a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_friendly_name(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Read the supported opcode of a TBS instance.
 *
 * @param   conn_handle Connection handle.
 * @param   inst_index  The index of the TBS instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_read_optional_opcodes(uint16_t conn_handle, uint8_t inst_index);

/**
 * @brief   Register the callbacks for CCP.
 *
 * @param   cbs Pointer to the callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tbs_client_register_cb(esp_ble_audio_tbs_client_cb_t *cbs);

/**
 * @brief   Look up Telephone Bearer Service instance by CCID.
 *
 * @param   conn_handle Connection handle.
 * @param   ccid        The CCID to lookup a service instance for.
 *
 * @return  Pointer to TBS instance if found, NULL otherwise.
 */
esp_ble_audio_tbs_instance_t *esp_ble_audio_tbs_client_get_by_ccid(uint16_t conn_handle, uint8_t ccid);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_TBS_API_H_ */
