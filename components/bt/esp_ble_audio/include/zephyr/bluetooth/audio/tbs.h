/**
 * @file
 * @brief Public APIs for Bluetooth Telephone Bearer Service.
 *
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_TBS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_TBS_H_

/**
 * @brief Telephone Bearer Service (TBS)
 *
 * @defgroup bt_tbs Telephone Bearer Service (TBS)
 *
 * @since 3.0
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Telephone Bearer Service (TBS) provide procedures to discover telephone bearers and control
 * calls.
 */

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/slist.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A characteristic value has changed while a Read Long Value Characteristic sub-procedure is in
 * progress
 */
#define BT_TBS_ERR_VAL_CHANGED 0x80

/**
 * @name Call States
 * @{
 */
/** A remote party is calling (incoming call). */
#define BT_TBS_CALL_STATE_INCOMING                      0x00
/**
 * The process to call the remote party has started on the server, but the remote party is not
 * being alerted (outgoing call).
 */
#define BT_TBS_CALL_STATE_DIALING                       0x01
/** A remote party is being alerted (outgoing call). */
#define BT_TBS_CALL_STATE_ALERTING                      0x02
/** The call is in an active conversation. */
#define BT_TBS_CALL_STATE_ACTIVE                        0x03
/**
 * The call is connected but held locally. Locally Held implies that either the server or the
 * client can affect the state.
 */
#define BT_TBS_CALL_STATE_LOCALLY_HELD                  0x04
/**
 *The call is connected but held remotely. Remotely Held means that the state is controlled by the
 * remote party of a call.
 */
#define BT_TBS_CALL_STATE_REMOTELY_HELD                 0x05
/** The call is connected but held both locally and remotely. */
#define BT_TBS_CALL_STATE_LOCALLY_AND_REMOTELY_HELD     0x06
/** @} */

/**
 * @name Terminate Reason
 * @{
 */
/** The URI value used to originate a call was formed improperly. */
#define BT_TBS_REASON_BAD_REMOTE_URI                    0x00
/** The call failed. */
#define BT_TBS_REASON_CALL_FAILED                       0x01
/** The remote party ended the call. */
#define BT_TBS_REASON_REMOTE_ENDED_CALL                 0x02
/** The call ended from the server. */
#define BT_TBS_REASON_SERVER_ENDED_CALL                 0x03
/** The line was busy. */
#define BT_TBS_REASON_LINE_BUSY                         0x04
/** Network congestion. */
#define BT_TBS_REASON_NETWORK_CONGESTED                 0x05
/** The client terminated the call. */
#define BT_TBS_REASON_CLIENT_TERMINATED                 0x06
/** No service. */
#define BT_TBS_REASON_NO_SERVICE                        0x07
/** No answer. */
#define BT_TBS_REASON_NO_ANSWER                         0x08
/** Unspecified. */
#define BT_TBS_REASON_UNSPECIFIED                       0x09
/** @} */

/**
 * @name Control point error codes
 * @{
 */
/** The opcode write was successful. */
#define BT_TBS_RESULT_CODE_SUCCESS                0x00
/** An invalid opcode was used for the Call Control Point write. */
#define BT_TBS_RESULT_CODE_OPCODE_NOT_SUPPORTED   0x01
/** The requested operation cannot be completed. */
#define BT_TBS_RESULT_CODE_OPERATION_NOT_POSSIBLE 0x02
/** The Call Index used for the Call Control Point write is invalid. */
#define BT_TBS_RESULT_CODE_INVALID_CALL_INDEX     0x03
/**
 * The opcode written to the Call Control Point was received when the current Call State for the
 * Call Index was not in the expected state.
 */
#define BT_TBS_RESULT_CODE_STATE_MISMATCH         0x04
/** Lack of internal resources to complete the requested action. */
#define BT_TBS_RESULT_CODE_OUT_OF_RESOURCES       0x05
/** The Outgoing URI is incorrect or invalid when an Originate opcode is sent. */
#define BT_TBS_RESULT_CODE_INVALID_URI            0x06
/** @} */

/**
 * @name Optional feature bits
 *
 * Optional features that can be supported. See bt_tbs_client_read_optional_opcodes() on how to
 * read these from a remote device
 * @{
 */
/** Local Hold and Local Retrieve Call Control Point Opcodes supported */
#define BT_TBS_FEATURE_HOLD BIT(0)
/** Join Call Control Point Opcode supported */
#define BT_TBS_FEATURE_JOIN BIT(1)
/** All Control Point Opcodes supported */
#define BT_TBS_FEATURE_ALL  (BT_TBS_FEATURE_HOLD | BT_TBS_FEATURE_JOIN)
/** @} */

/**
 * @name Signal strength value limits
 * @{
 */
/** No service */
#define BT_TBS_SIGNAL_STRENGTH_NO_SERVICE               0
/** Maximum signal strength */
#define BT_TBS_SIGNAL_STRENGTH_MAX                      100
/** Signal strength is unknown  */
#define BT_TBS_SIGNAL_STRENGTH_UNKNOWN                  255
/** @} */

/**
 * @name Bearer Technology
 * @{
 */
/** 3G */
#define BT_TBS_TECHNOLOGY_3G                       0x01
/** 4G */
#define BT_TBS_TECHNOLOGY_4G                       0x02
/** Long-term evolution (LTE) */
#define BT_TBS_TECHNOLOGY_LTE                      0x03
/** Wifi */
#define BT_TBS_TECHNOLOGY_WIFI                     0x04
/** 5G */
#define BT_TBS_TECHNOLOGY_5G                       0x05
/** Global System for Mobile Communications (GSM) */
#define BT_TBS_TECHNOLOGY_GSM                      0x06
/** Code-Division Multiple Access (CDMA) */
#define BT_TBS_TECHNOLOGY_CDMA                     0x07
/** 2G */
#define BT_TBS_TECHNOLOGY_2G                       0x08
/** Wideband Code-Division Multiple Access (WCDMA) */
#define BT_TBS_TECHNOLOGY_WCDMA                    0x09
/** @} */

/**
 * @name Call status flags bitfield
 * @{
 */
/** Inband ringtone enabled */
#define BT_TBS_STATUS_FLAG_INBAND_RINGTONE BIT(0)
/** Server is in silent mod */
#define BT_TBS_STATUS_FLAG_SILENT_MOD      BIT(1)
/** @} */

/**
 * @name Call flags bitfield
 * @{
 */
/** If set, call is outgoing else incoming */
#define BT_TBS_CALL_FLAG_OUTGOING            BIT(0)
/** If set call is withheld, else not withheld */
#define BT_TBS_CALL_FLAG_WITHHELD            BIT(1)
/** If set call is withheld by network, else provided by network */
#define BT_TBS_CALL_FLAG_WITHHELD_BY_NETWORK BIT(2)
/** @} */

/**
 * @brief The GTBS index denotes whenever a callback is from a
 * Generic Telephone Bearer Service (GTBS) instance, or
 * whenever the client should perform on action on the GTBS instance of the
 * server, rather than any of the specific Telephone Bearer Service instances.
 */
#define BT_TBS_GTBS_INDEX                               0xFF

/** @brief Opaque Telephone Bearer Service instance. */
struct bt_tbs_instance;

/**
 * @brief Callback function for client originating a call.
 *
 * @param conn          The connection used.
 * @param call_index    The call index.
 * @param uri           The URI. The value may change, so should be
 *                      copied if persistence is wanted.
 *
 * @return true if the call request was accepted and remote party is alerted.
 */
typedef bool (*bt_tbs_originate_call_cb)(struct bt_conn *conn,
                                         uint8_t call_index,
                                         const char *uri);

/**
 * @brief Callback function for client terminating a call.
 *
 * The call may be either terminated by the client or the server.
 *
 * @param conn          The connection used.
 * @param call_index    The call index.
 * @param reason        The termination BT_TBS_REASON_* reason.
 */
typedef void (*bt_tbs_terminate_call_cb)(struct bt_conn *conn,
                                         uint8_t call_index,
                                         uint8_t reason);

/**
 * @brief Callback function for client joining calls.
 *
 * @param conn                  The connection used.
 * @param call_index_count      The number of call indexes to join.
 * @param call_indexes          The call indexes.
 */
typedef void (*bt_tbs_join_calls_cb)(struct bt_conn *conn,
                                     uint8_t call_index_count,
                                     const uint8_t *call_indexes);

/**
 * @brief Callback function for client request call state change
 *
 * @param conn          The connection used.
 * @param call_index    The call index.
 */
typedef void (*bt_tbs_call_change_cb)(struct bt_conn *conn,
                                      uint8_t call_index);

/**
 * @brief Callback function for authorizing a client.
 *
 * @param conn         The connection used.
 *
 * @return true if authorized, false otherwise
 */
typedef bool (*bt_tbs_authorize_cb)(struct bt_conn *conn);

/**
 * @brief Struct to hold the Telephone Bearer Service callbacks
 *
 * These can be registered for usage with bt_tbs_register_cb().
 */
struct bt_tbs_cb {
    /** Client originating call */
    bt_tbs_originate_call_cb      originate_call;
    /** Client terminating call */
    bt_tbs_terminate_call_cb      terminate_call;
    /** Client holding call */
    bt_tbs_call_change_cb         hold_call;
    /** Client accepting call */
    bt_tbs_call_change_cb         accept_call;
    /** Client retrieving call */
    bt_tbs_call_change_cb         retrieve_call;
    /** Client joining calls */
    bt_tbs_join_calls_cb          join_calls;
    /** Callback to authorize a client */
    bt_tbs_authorize_cb           authorize;
};

/**
 * @brief Accept an alerting call.
 *
 * @param call_index    The index of the call that will be accepted.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_accept_safe(uint8_t call_index);

/**
 * @brief Hold a call.
 *
 * @param call_index    The index of the call that will be held.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_hold_safe(uint8_t call_index);

/**
 * @brief Retrieve a call.
 *
 * @param call_index    The index of the call that will be retrieved.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_retrieve_safe(uint8_t call_index);

/**
 * @brief Terminate a call.
 *
 * @param call_index    The index of the call that will be terminated.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_terminate_safe(uint8_t call_index);

/**
 * @brief Originate a call
 *
 * @param[in]  bearer_index  The index of the Telephone Bearer.
 * @param[in]  uri           The remote URI.
 * @param[out] call_index    Pointer to a value where the new call_index will be
 *                           stored.
 *
 * @return int          A call index on success (positive value),
 *                      errno value on fail.
 */
int bt_tbs_originate_safe(uint8_t bearer_index, char *uri, uint8_t *call_index);

/**
 * @brief Join calls
 *
 * @param call_index_cnt   The number of call indexes to join
 * @param call_indexes     Array of call indexes to join.
 *
 * @return int             BT_TBS_RESULT_CODE_* if positive or 0,
 *                         errno value if negative.
 */
int bt_tbs_join_safe(uint8_t call_index_cnt, uint8_t *call_indexes);

/**
 * @brief Notify the server that the remote party answered the call.
 *
 * @param call_index    The index of the call that was answered.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_remote_answer_safe(uint8_t call_index);

/**
 * @brief Notify the server that the remote party held the call.
 *
 * @param call_index    The index of the call that was held.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_remote_hold_safe(uint8_t call_index);

/**
 * @brief Notify the server that the remote party retrieved the call.
 *
 * @param call_index    The index of the call that was retrieved.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_remote_retrieve_safe(uint8_t call_index);

/**
 * @brief Notify the server that the remote party terminated the call.
 *
 * @param call_index    The index of the call that was terminated.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_remote_terminate_safe(uint8_t call_index);

/**
 * @brief Notify the server of an incoming call.
 *
 * @param bearer_index    The index of the Telephone Bearer.
 * @param to              The URI that is receiving the call.
 * @param from            The URI of the remote caller.
 * @param friendly_name   The  friendly name of the remote caller.
 *
 * @return int            New call index if positive or 0,
 *                        errno value if negative.
 */
int bt_tbs_remote_incoming_safe(uint8_t bearer_index, const char *to,
                                const char *from, const char *friendly_name);

/**
 * @brief Set a new bearer provider.
 *
 * @param bearer_index  The index of the Telephone Bearer or BT_TBS_GTBS_INDEX
 *                      for GTBS.
 * @param name          The new bearer provider name.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_set_bearer_provider_name_safe(uint8_t bearer_index, const char *name);

/**
 * @brief Set a new bearer technology.
 *
 * @param bearer_index   The index of the Telephone Bearer or BT_TBS_GTBS_INDEX
 *                       for GTBS.
 * @param new_technology The new bearer technology.
 *
 * @return int           BT_TBS_RESULT_CODE_* if positive or 0,
 *                       errno value if negative.
 */
int bt_tbs_set_bearer_technology_safe(uint8_t bearer_index, uint8_t new_technology);

/**
 * @brief Update the signal strength reported by the server.
 *
 * @param bearer_index        The index of the Telephone Bearer or
 *                            BT_TBS_GTBS_INDEX for GTBS.
 * @param new_signal_strength The new signal strength.
 *
 * @return int                BT_TBS_RESULT_CODE_* if positive or 0,
 *                            errno value if negative.
 */
int bt_tbs_set_signal_strength_safe(uint8_t bearer_index,
                                    uint8_t new_signal_strength);

/**
 * @brief Sets the feature and status value.
 *
 * @param bearer_index  The index of the Telephone Bearer or BT_TBS_GTBS_INDEX
 *                      for GTBS.
 * @param status_flags  The new feature and status value.
 *
 * @return int          BT_TBS_RESULT_CODE_* if positive or 0,
 *                      errno value if negative.
 */
int bt_tbs_set_status_flags_safe(uint8_t bearer_index, uint16_t status_flags);

/**
 * @brief Sets the URI scheme list of a bearer.
 *
 * @param bearer_index  The index of the Telephone Bearer.
 * @param uri_list      List of URI prefixes (e.g. {"skype", "tel"}).
 * @param uri_count     Number of URI prefixes in @p uri_list.
 *
 * @return BT_TBS_RESULT_CODE_* if positive or 0, errno value if negative.
 */
int bt_tbs_set_uri_scheme_list_safe(uint8_t bearer_index, const char **uri_list,
                                    uint8_t uri_count);
/**
 * @brief Register the callbacks for TBS.
 *
 * @param cbs Pointer to the callback structure.
 */
void bt_tbs_register_cb_safe(struct bt_tbs_cb *cbs);

/** Parameters for registering a Telephone Bearer Service */
struct bt_tbs_register_param {
    /** The name of the provider, for example a cellular service provider */
    char *provider_name;

    /**
     * @brief The Uniform Caller Identifier of the bearer
     *
     * See the Uniform Caller Identifiers table in Bluetooth Assigned Numbers
     */
    char *uci;

    /**
     * The Uniform Resource Identifiers schemes supported by this bearer as an UTF-8 string
     *
     * See https://www.iana.org/assignments/uri-schemes/uri-schemes.xhtml for possible values.
     * If multiple values are used, these shall be comma separated, e.g. "tel,skype".
     */
    char *uri_schemes_supported;

    /**
     * @brief Whether this bearer shall be registered as a Generic Telephone Bearer server
     *
     * A GTBS shall be registered before any non-GTBS services. There can only be a single GTBS
     * registered.
     */
    bool gtbs;

    /**
     * @brief Whether the application will need to authorize changes to calls
     *
     * If set to false then the service will automatically accept write requests from clients.
     */
    bool authorization_required;

    /**
     * @brief The technology of the bearer
     *
     * See the BT_TBS_TECHNOLOGY_* values.
     */
    uint8_t technology;

    /**
     * @brief The optional supported features of the bearer
     *
     * See the BT_TBS_FEATURE_* values.
     */
    uint8_t supported_features;
};

/**
 * @brief Register a Telephone Bearer
 *
 * This will register a Telephone Bearer Service (TBS) (or a Generic Telephone Bearer service
 * (GTBS)) with the provided parameters.
 *
 * As per the TBS specification, the GTBS shall be instantiated for the feature, and as such a GTBS
 * shall always be registered before any TBS can be registered.
 * Similarly, all TBS shall be unregistered before the GTBS can be unregistered with
 * bt_tbs_unregister_bearer().
 *
 * @param param The parameters to initialize the bearer.

 * @retval index The bearer index if return value is >= 0
 * @retval -EINVAL @p param contains invalid data
 * @retval -EALREADY @p param.gtbs is true and GTBS has already been registered
 * @retval -EAGAIN @p param.gtbs is false and GTBS has not been registered
 * @retval -ENOMEM @p param.gtbs is false and no more TBS can be registered (see
 *         @kconfig{CONFIG_BT_TBS_BEARER_COUNT})
 * @retval -ENOEXEC The service failed to be registered
 */
int bt_tbs_register_bearer_safe(const struct bt_tbs_register_param *param);

/**
 * @brief Unregister a Telephone Bearer
 *
 * This will unregister a Telephone Bearer Service (TBS) (or a Generic Telephone Bearer service
 * (GTBS)) with the provided parameters. The bearer shall be registered first by
 * bt_tbs_register_bearer() before it can be unregistered.
 *
 * Similarly, all TBS shall be unregistered before the GTBS can be unregistered with.
 *
 * @param bearer_index The index of the bearer to unregister.
 *
 * @retval 0 Success
 * @retval -EINVAL @p bearer_index is invalid
 * @retval -EALREADY The bearer identified by @p bearer_index is not registered
 * @retval -EAGAIN The bearer identified by @p bearer_index is GTBS and there are TBS instances
 *                 registered.
 * @retval -ENOEXEC The service failed to be unregistered
 */
int bt_tbs_unregister_bearer_safe(uint8_t bearer_index);

/** @brief Prints all calls of all services to the debug log */
void bt_tbs_dbg_print_calls(void);

/** Struct to hold a call state */
struct bt_tbs_client_call_state {
    /** Index of the call */
    uint8_t index;
    /** State of the call (see BT_TBS_CALL_STATE_*) */
    uint8_t state;
    /** Call flags (see BT_TBS_CALL_FLAG_*) */
    uint8_t flags;
} __packed;

/** Struct to hold a call as the Telephone Bearer Service client */
struct bt_tbs_client_call {
    /** Call information */
    struct bt_tbs_client_call_state call_info;
    /** The remove URI */
    char *remote_uri;
};

/**
 * @brief Callback function for ccp_discover.
 *
 * @param conn          The connection that was used to discover CCP for a
 *                      device.
 * @param err           Error value. BT_TBS_CLIENT_RESULT_CODE_*,
 *                      GATT error or errno value.
 * @param tbs_count     Number of TBS instances on peer device.
 * @param gtbs_found    Whether or not the server has a Generic TBS instance.
 */
typedef void (*bt_tbs_client_discover_cb)(struct bt_conn *conn, int err,
                                          uint8_t tbs_count, bool gtbs_found);

/**
 * @brief Callback function for writing values to peer device.
 *
 * @param conn          The connection used in the function.
 * @param err           Error value. BT_TBS_CLIENT_RESULT_CODE_*,
 *                      GATT error or errno value.
 * @param inst_index    The index of the TBS instance that was updated.
 */
typedef void (*bt_tbs_client_write_value_cb)(struct bt_conn *conn, int err,
                                             uint8_t inst_index);

/**
 * @brief Callback function for the CCP call control functions.
 *
 * @param conn          The connection used in the function.
 * @param err           Error value. BT_TBS_CLIENT_RESULT_CODE_*,
 *                      GATT error or errno value.
 * @param inst_index    The index of the TBS instance that was updated.
 * @param call_index    The call index. For #bt_tbs_client_originate_call this will
 *                      always be 0, and does not reflect the actual call index.
 */
typedef void (*bt_tbs_client_cp_cb)(struct bt_conn *conn, int err,
                                    uint8_t inst_index, uint8_t call_index);

/**
 * @brief Callback function for functions that read a string value.
 *
 * @param conn          The connection used in the function.
 * @param err           Error value. BT_TBS_CLIENT_RESULT_CODE_*,
 *                      GATT error or errno value.
 * @param inst_index    The index of the TBS instance that was updated.
 * @param value         The Null-terminated string value. The value is not kept
 *                      by the client, so must be copied to be saved.
 */
typedef void (*bt_tbs_client_read_string_cb)(struct bt_conn *conn, int err,
                                             uint8_t inst_index,
                                             const char *value);

/**
 * @brief Callback function for functions that read an integer value.
 *
 * @param conn          The connection used in the function.
 * @param err           Error value. BT_TBS_CLIENT_RESULT_CODE_*,
 *                      GATT error or errno value.
 * @param inst_index    The index of the TBS instance that was updated.
 * @param value         The integer value.
 */
typedef void (*bt_tbs_client_read_value_cb)(struct bt_conn *conn, int err,
                                            uint8_t inst_index, uint32_t value);

/**
 * @brief Callback function for ccp_read_termination_reason.
 *
 * @param conn          The connection used in the function.
 * @param err           Error value. BT_TBS_CLIENT_RESULT_CODE_*,
 *                      GATT error or errno value.
 * @param inst_index    The index of the TBS instance that was updated.
 * @param call_index    The call index.
 * @param reason        The termination reason.
 */
typedef void (*bt_tbs_client_termination_reason_cb)(struct bt_conn *conn,
                                                    int err, uint8_t inst_index,
                                                    uint8_t call_index,
                                                    uint8_t reason);

/**
 * @brief Callback function for ccp_read_current_calls.
 *
 * @param conn          The connection used in the function.
 * @param err           Error value. BT_TBS_CLIENT_RESULT_CODE_*,
 *                      GATT error or errno value.
 * @param inst_index    The index of the TBS instance that was updated.
 * @param call_count    Number of calls read.
 * @param calls         Array of calls. The array is not kept by
 *                      the client, so must be copied to be saved.
 */
typedef void (*bt_tbs_client_current_calls_cb)(struct bt_conn *conn, int err,
                                               uint8_t inst_index,
                                               uint8_t call_count,
                                               const struct bt_tbs_client_call *calls);

/**
 * @brief Callback function for ccp_read_call_state.
 *
 * @param conn          The connection used in the function.
 * @param err           Error value. BT_TBS_CLIENT_RESULT_CODE_*,
 *                      GATT error or errno value.
 * @param inst_index    The index of the TBS instance that was updated.
 * @param call_count    Number of call states read.
 * @param call_states   Array of call states. The array is not kept by
 *                      the client, so must be copied to be saved.
 */
typedef void (*bt_tbs_client_call_states_cb)(struct bt_conn *conn, int err,
                                             uint8_t inst_index,
                                             uint8_t call_count,
                                             const struct bt_tbs_client_call_state *call_states);

/**
 * @brief Struct to hold the Telephone Bearer Service client callbacks
 *
 * These can be registered for usage with bt_tbs_client_register_cb().
 */
struct bt_tbs_client_cb {
    /** Discovery has completed */
    bt_tbs_client_discover_cb            discover;
    /** Originate call has completed */
    bt_tbs_client_cp_cb                  originate_call;
    /** Terminate call has completed */
    bt_tbs_client_cp_cb                  terminate_call;
    /** Hold call has completed */
    bt_tbs_client_cp_cb                  hold_call;
    /** Accept call has completed */
    bt_tbs_client_cp_cb                  accept_call;
    /** Retrieve call has completed */
    bt_tbs_client_cp_cb                  retrieve_call;
    /** Join calls has completed */
    bt_tbs_client_cp_cb                  join_calls;
    /** Bearer provider name has been read */
    bt_tbs_client_read_string_cb         bearer_provider_name;
    /** Bearer UCI has been read */
    bt_tbs_client_read_string_cb         bearer_uci;
    /** Bearer technology has been read */
    bt_tbs_client_read_value_cb          technology;
    /** Bearer URI list has been read */
    bt_tbs_client_read_string_cb         uri_list;
    /** Bearer signal strength has been read */
    bt_tbs_client_read_value_cb          signal_strength;
    /** Bearer signal interval has been read */
    bt_tbs_client_read_value_cb          signal_interval;
    /** Bearer current calls has been read */
    bt_tbs_client_current_calls_cb       current_calls;
    /** Bearer CCID has been read */
    bt_tbs_client_read_value_cb          ccid;
    /** Bearer call URI has been read */
    bt_tbs_client_read_string_cb         call_uri;
    /** Bearer status flags has been read */
    bt_tbs_client_read_value_cb          status_flags;
    /** Bearer call states has been read */
    bt_tbs_client_call_states_cb         call_state;
    /** Bearer optional opcodes has been read */
    bt_tbs_client_read_value_cb          optional_opcodes;
    /** Bearer terminate reason has been read */
    bt_tbs_client_termination_reason_cb  termination_reason;
    /** Bearer remote URI has been read */
    bt_tbs_client_read_string_cb         remote_uri;
    /** Bearer friendly name has been read */
    bt_tbs_client_read_string_cb         friendly_name;

    /** @cond INTERNAL_HIDDEN */
    /** Internally used field for list handling */
    sys_snode_t _node;
    /** @endcond */
};

/**
 * @brief Discover TBS for a connection. This will start a GATT
 * discover and setup handles and subscriptions.
 *
 * @param conn          The connection to discover TBS for.
 *
 * @return int          0 on success, GATT error value on fail.
 */
int bt_tbs_client_discover(struct bt_conn *conn);
int bt_tbs_client_discover_safe(struct bt_conn *conn);

/**
 * @brief Set the outgoing URI for a TBS instance on the peer device.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 * @param uri           The Null-terminated URI string.
 *
 * @return int          0 on success, errno value on fail.
 */
int bt_tbs_client_set_outgoing_uri_safe(struct bt_conn *conn, uint8_t inst_index,
                                        const char *uri);

/**
 * @brief Set the signal strength reporting interval for a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 * @param interval      The interval to write (0-255 seconds).
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_SET_BEARER_SIGNAL_INTERVAL} must be set
 * for this function to be effective.
 */
int bt_tbs_client_set_signal_strength_interval(struct bt_conn *conn,
                                               uint8_t inst_index,
                                               uint8_t interval);
int bt_tbs_client_set_signal_strength_interval_safe(struct bt_conn *conn,
                                                    uint8_t inst_index,
                                                    uint8_t interval);

/**
 * @brief Request to originate a call.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 * @param uri           The URI of the callee.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_ORIGINATE_CALL} must be set
 * for this function to be effective.
 */
int bt_tbs_client_originate_call(struct bt_conn *conn, uint8_t inst_index,
                                 const char *uri);
int bt_tbs_client_originate_call_safe(struct bt_conn *conn, uint8_t inst_index,
                                      const char *uri);

/**
 * @brief Request to terminate a call
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 * @param call_index    The call index to terminate.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_TERMINATE_CALL} must be set
 * for this function to be effective.
 */
int bt_tbs_client_terminate_call(struct bt_conn *conn, uint8_t inst_index,
                                 uint8_t call_index);
int bt_tbs_client_terminate_call_safe(struct bt_conn *conn, uint8_t inst_index,
                                      uint8_t call_index);

/**
 * @brief Request to hold a call
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 * @param call_index    The call index to place on hold.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_HOLD_CALL} must be set
 * for this function to be effective.
 */
int bt_tbs_client_hold_call(struct bt_conn *conn, uint8_t inst_index,
                            uint8_t call_index);
int bt_tbs_client_hold_call_safe(struct bt_conn *conn, uint8_t inst_index,
                                 uint8_t call_index);

/**
 * @brief Accept an incoming call
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 * @param call_index    The call index to accept.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_ACCEPT_CALL} must be set
 * for this function to be effective.
 */
int bt_tbs_client_accept_call(struct bt_conn *conn, uint8_t inst_index,
                              uint8_t call_index);
int bt_tbs_client_accept_call_safe(struct bt_conn *conn, uint8_t inst_index,
                                   uint8_t call_index);

/**
 * @brief Retrieve call from (local) hold.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 * @param call_index    The call index to retrieve.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_RETRIEVE_CALL} must be set
 * for this function to be effective.
 */
int bt_tbs_client_retrieve_call(struct bt_conn *conn, uint8_t inst_index,
                                uint8_t call_index);
int bt_tbs_client_retrieve_call_safe(struct bt_conn *conn, uint8_t inst_index,
                                     uint8_t call_index);

/**
 * @brief Join multiple calls.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 * @param call_indexes  Array of call indexes.
 * @param count         Number of call indexes in the call_indexes array.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_JOIN_CALLS} must be set
 * for this function to be effective.
 */
int bt_tbs_client_join_calls(struct bt_conn *conn, uint8_t inst_index,
                             const uint8_t *call_indexes, uint8_t count);
int bt_tbs_client_join_calls_safe(struct bt_conn *conn, uint8_t inst_index,
                                  const uint8_t *call_indexes, uint8_t count);

/**
 * @brief Read the bearer provider name of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_bearer_provider_name(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_bearer_provider_name_safe(struct bt_conn *conn,
                                                 uint8_t inst_index);

/**
 * @brief Read the UCI of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_BEARER_UCI} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_bearer_uci(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_bearer_uci_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the technology of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_BEARER_TECHNOLOGY} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_technology(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_technology_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the URI schemes list of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return int          0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_BEARER_URI_SCHEMES_SUPPORTED_LIST} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_uri_list(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_uri_list_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the current signal strength of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_BEARER_SIGNAL_STRENGTH} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_signal_strength(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_signal_strength_safe(struct bt_conn *conn,
                                            uint8_t inst_index);

/**
 * @brief Read the signal strength reporting interval of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_READ_BEARER_SIGNAL_INTERVAL} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_signal_interval(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_signal_interval_safe(struct bt_conn *conn,
                                            uint8_t inst_index);

/**
 * @brief Read the list of current calls of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_BEARER_LIST_CURRENT_CALLS} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_current_calls(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_current_calls_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the content ID of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_CCID} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_ccid(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_ccid_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the call target URI of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_INCOMING_URI} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_call_uri(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_call_uri_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the feature and status value of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_STATUS_FLAGS} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_status_flags(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_status_flags_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the states of the current calls of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 */
int bt_tbs_client_read_call_state(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_call_state_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the remote URI of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_INCOMING_CALL} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_remote_uri(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_remote_uri_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the friendly name of a call for a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_CALL_FRIENDLY_NAME} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_friendly_name(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_friendly_name_safe(struct bt_conn *conn, uint8_t inst_index);

/**
 * @brief Read the supported opcode of a TBS instance.
 *
 * @param conn          The connection to the TBS server.
 * @param inst_index    The index of the TBS instance.
 *
 * @return              int 0 on success, errno value on fail.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_OPTIONAL_OPCODES} must be set
 * for this function to be effective.
 */
int bt_tbs_client_read_optional_opcodes(struct bt_conn *conn, uint8_t inst_index);
int bt_tbs_client_read_optional_opcodes_safe(struct bt_conn *conn,
                                             uint8_t inst_index);

/**
 * @brief Register the callbacks for CCP.
 *
 * @param cbs Pointer to the callback structure.
 *
 * @retval 0 Success
 * @retval -EINVAL @p cbs is NULL
 * @retval -EEXIST @p cbs is already registered
 */
int bt_tbs_client_register_cb_safe(struct bt_tbs_client_cb *cbs);

/**
 * @brief Look up Telephone Bearer Service instance by CCID
 *
 * @param conn  The connection to the TBS server.
 * @param ccid  The CCID to lookup a service instance for.
 *
 * @return Pointer to a Telephone Bearer Service instance if found else NULL.
 *
 * @note @kconfig{CONFIG_BT_TBS_CLIENT_CCID} must be set
 * for this function to be effective.
 */
struct bt_tbs_instance *bt_tbs_client_get_by_ccid_safe(const struct bt_conn *conn,
                                                       uint8_t ccid);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_TBS_H_ */
