/**
 * @file
 * @brief Bluetooth Common Audio Profile (CAP) APIs.
 */

/*
 * SPDX-FileCopyrightText: 2022-2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_CAP_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_CAP_H_

/**
 * @brief Common Audio Profile (CAP)
 *
 * @defgroup bt_cap Common Audio Profile (CAP)
 *
 * @since 3.2
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * Common Audio Profile (CAP) provides procedures to start, update, and stop unicast and broadcast
 * Audio Streams on individual or groups of devices using procedures in the Basic Audio Profile
 * (BAP). This profile also provides procedures to control volume and device input on groups of
 * devices using procedures in the Volume Control Profile (VCP) and the Microphone Control Profile
 * (MICP). This profile specification also refers to the Common Audio Service (CAS).
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/bap.h>
#include <zephyr/bluetooth/audio/csip.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/iso.h>
#include <zephyr/net_buf.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Abstract Audio Broadcast Source structure. */
struct bt_cap_broadcast_source;

/** @brief Abstract CAP Unicast Group structure. */
struct bt_cap_unicast_group;

/**
 * @brief Register the Common Audio Service.
 *
 * This will register and enable the service and make it discoverable by
 * clients. This will also register a Coordinated Set Identification
 * Service instance.
 *
 * This shall only be done as a server, and requires
 * @kconfig{BT_CAP_ACCEPTOR_SET_MEMBER}. If @kconfig{BT_CAP_ACCEPTOR_SET_MEMBER}
 * is not enabled, the Common Audio Service will by statically registered.
 *
 * @param[in]  param     Coordinated Set Identification Service register
 *                       parameters.
 * @param[out] svc_inst  Pointer to the registered Coordinated Set
 *                       Identification Service.
 *
 * @return 0 if success, errno on failure.
 */
int bt_cap_acceptor_register_safe(const struct bt_csip_set_member_register_param *param,
                                  struct bt_csip_set_member_svc_inst **svc_inst);

/** Callback structure for CAP procedures */
struct bt_cap_initiator_cb {
    /**
     * @brief Callback for bt_cap_initiator_unicast_discover().
     *
     * @param conn      The connection pointer supplied to
     *                  bt_cap_initiator_unicast_discover().
     * @param err       0 if Common Audio Service was found else -ENODATA.
     * @param member    Pointer to the set member. NULL if err != 0.
     * @param csis_inst The Coordinated Set Identification Service if
     *                  Common Audio Service was found and includes a
     *                  Coordinated Set Identification Service.
     *                  NULL on error or if remote device does not include
     *                  Coordinated Set Identification Service. NULL if err != 0.
     */
    void (*unicast_discovery_complete)(
        struct bt_conn *conn, int err,
        const struct bt_csip_set_coordinator_set_member *member,
        const struct bt_csip_set_coordinator_csis_inst *csis_inst);

    /**
     * @brief Callback for bt_cap_initiator_unicast_audio_start().
     *
     * @param err            0 if success, BT_GATT_ERR() with a
     *                       specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *                       by bt_cap_initiator_unicast_audio_cancel().
     * @param conn           Pointer to the connection where the error
     *                       occurred. NULL if @p err is 0 or if cancelled by
     *                       bt_cap_initiator_unicast_audio_cancel()
     */
    void (*unicast_start_complete)(int err, struct bt_conn *conn);

    /**
     * @brief Callback for bt_cap_initiator_unicast_audio_update().
     *
     * @param err            0 if success, BT_GATT_ERR() with a
     *                       specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *                       by bt_cap_initiator_unicast_audio_cancel().
     * @param conn           Pointer to the connection where the error
     *                       occurred. NULL if @p err is 0 or if cancelled by
     *                       bt_cap_initiator_unicast_audio_cancel()
     */
    void (*unicast_update_complete)(int err, struct bt_conn *conn);

    /**
     * @brief Callback for bt_cap_initiator_unicast_audio_stop().
     *
     * @param err            0 if success, BT_GATT_ERR() with a
     *                       specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *                       by bt_cap_initiator_unicast_audio_cancel().
     * @param conn           Pointer to the connection where the error
     *                       occurred. NULL if @p err is 0 or if cancelled by
     *                       bt_cap_initiator_unicast_audio_cancel()
     */
    void (*unicast_stop_complete)(int err, struct bt_conn *conn);

    /**
     * @brief The Broadcast Source has started and all of the streams are ready for audio data
     *
     * @param source The started Broadcast Source
     */
    void (*broadcast_started)(struct bt_cap_broadcast_source *source);

    /**
     * @brief The Broadcast Source has stopped and none of the streams are ready for audio data
     *
     * @param source The stopped Broadcast Source
     * @param reason The reason why the Broadcast Source stopped (see the BT_HCI_ERR_* values)
     */
    void (*broadcast_stopped)(struct bt_cap_broadcast_source *source, uint8_t reason);
};

/**
 * @brief Discovers audio support on a remote device.
 *
 * This will discover the Common Audio Service (CAS) on the remote device, to
 * verify if the remote device supports the Common Audio Profile.
 *
 * @param conn Connection to a remote server.
 *
 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 */
int bt_cap_initiator_unicast_discover(struct bt_conn *conn);
int bt_cap_initiator_unicast_discover_safe(struct bt_conn *conn);

/** Type of CAP set */
enum bt_cap_set_type {
    /** The set is an ad-hoc set */
    BT_CAP_SET_TYPE_AD_HOC,
    /** The set is a CSIP Coordinated Set */
    BT_CAP_SET_TYPE_CSIP,
};

/** Represents a Common Audio Set member that are either in a Coordinated or ad-hoc set */
union bt_cap_set_member {
    /** Connection pointer if the type is BT_CAP_SET_TYPE_AD_HOC. */
    struct bt_conn *member;

    /** CSIP Coordinated Set struct used if type is BT_CAP_SET_TYPE_CSIP. */
    struct bt_csip_set_coordinator_csis_inst *csip;
};

/**
 * @brief Common Audio Profile stream structure.
 *
 * Streams represents a Basic Audio Profile (BAP) stream and operation callbacks.
 * See @ref bt_bap_stream for additional information.
 */
struct bt_cap_stream {
    /** The underlying BAP audio stream */
    struct bt_bap_stream bap_stream;

    /** Audio stream operations */
    struct bt_bap_stream_ops *ops;
};

/**
 * @brief Register Audio operations for a Common Audio Profile stream.
 *
 * Register Audio operations for a stream.
 *
 * @param stream Stream object.
 * @param ops    Stream operations structure.
 */
void bt_cap_stream_ops_register_safe(struct bt_cap_stream *stream, struct bt_bap_stream_ops *ops);

/**
 * @brief Send data to Common Audio Profile stream without timestamp
 *
 * See bt_bap_stream_send() for more information
 *
 * @note Support for sending must be supported, determined by @kconfig{CONFIG_BT_AUDIO_TX}.
 *
 * @param stream   Stream object.
 * @param buf      Buffer containing data to be sent.
 * @param seq_num  Packet Sequence number. This value shall be incremented for each call to this
 *                 function and at least once per SDU interval for a specific channel.
 *
 * @retval -EINVAL if stream object is NULL
 * @retval Any return value from bt_bap_stream_send()
 */
int bt_cap_stream_send_safe(struct bt_cap_stream *stream, struct net_buf *buf, uint16_t seq_num);

/**
 * @brief Send data to Common Audio Profile stream with timestamp
 *
 * See bt_bap_stream_send() for more information
 *
 * @note Support for sending must be supported, determined by @kconfig{CONFIG_BT_AUDIO_TX}.
 *
 * @param stream   Stream object.
 * @param buf      Buffer containing data to be sent.
 * @param seq_num  Packet Sequence number. This value shall be incremented for each call to this
 *                 function and at least once per SDU interval for a specific channel.
 * @param ts       Timestamp of the SDU in microseconds (us). This value can be used to transmit
 *                 multiple SDUs in the same SDU interval in a CIG or BIG.
 *
 * @retval -EINVAL if stream object is NULL
 * @retval Any return value from bt_bap_stream_send()
 */
int bt_cap_stream_send_ts_safe(struct bt_cap_stream *stream, struct net_buf *buf, uint16_t seq_num,
                               uint32_t ts);

/**
 * @brief Get ISO transmission timing info for a Common Audio Profile stream
 *
 * See bt_bap_stream_get_tx_sync() for more information
 *
 * @note Support for sending must be supported, determined by @kconfig{CONFIG_BT_AUDIO_TX}.
 *
 * @param[in]  stream Stream object.
 * @param[out] info   Transmit info object.
 *
 * @retval -EINVAL if stream object is NULL
 * @retval Any return value from bt_bap_stream_get_tx_sync()
 */
int bt_cap_stream_get_tx_sync_safe(struct bt_cap_stream *stream, struct bt_iso_tx_info *info);

/** Parameter struct for each stream in the unicast group */
struct bt_cap_unicast_group_stream_param {
    /** Pointer to a stream object. */
    struct bt_cap_stream *stream;

    /** The QoS settings for the stream object. */
    struct bt_bap_qos_cfg *qos_cfg;
};

/**
 * @brief Parameter struct for the unicast group functions
 *
 * Parameter struct for the bt_cap_unicast_group_create() and
 * bt_cap_unicast_group_add_streams() functions.
 */
struct bt_cap_unicast_group_stream_pair_param {
    /** Pointer to a receiving stream parameters. */
    struct bt_cap_unicast_group_stream_param *rx_param;

    /** Pointer to a transmitting stream parameters. */
    struct bt_cap_unicast_group_stream_param *tx_param;
};

/** Parameters for the creating unicast groups with bt_cap_unicast_group_create() */
struct bt_cap_unicast_group_param {
    /** The number of parameters in @p params */
    size_t params_count;

    /** Array of stream parameters */
    struct bt_cap_unicast_group_stream_pair_param *params;

    /**
     * @brief Unicast Group packing mode.
     *
     * @ref BT_ISO_PACKING_SEQUENTIAL or @ref BT_ISO_PACKING_INTERLEAVED.
     *
     * @note This is a recommendation to the controller, which the controller may ignore.
     */
    uint8_t packing;

    /**
     * @brief Central to Peripheral flush timeout
     *
     * The flush timeout in multiples of ISO_Interval for each payload sent
     * from the Central to Peripheral.
     *
     * Value range from @ref BT_ISO_FT_MIN to @ref BT_ISO_FT_MAX
     */
    uint8_t c_to_p_ft;

    /**
     * @brief Peripheral to Central flush timeout
     *
     * The flush timeout in multiples of ISO_Interval for each payload sent
     * from the Peripheral to Central.
     *
     * Value range from @ref BT_ISO_FT_MIN to @ref BT_ISO_FT_MAX.
     */
    uint8_t p_to_c_ft;

    /**
     * @brief ISO interval
     *
     * Time between consecutive CIS anchor points.
     *
     * Value range from @ref BT_ISO_ISO_INTERVAL_MIN to @ref BT_ISO_ISO_INTERVAL_MAX.
     */
    uint16_t iso_interval;
};

/**
 * @brief Create unicast group.
 *
 * Create a new audio unicast group with one or more audio streams as a unicast client.
 * All streams shall share the same framing.
 * All streams in the same direction shall share the same interval and latency (see
 * @ref bt_bap_qos_cfg).
 *
 * @param[in]  param          The unicast group create parameters.
 * @param[out] unicast_group  Pointer to the unicast group created.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_cap_unicast_group_create_safe(const struct bt_cap_unicast_group_param *param,
                                     struct bt_cap_unicast_group **unicast_group);

/**
 * @brief Reconfigure unicast group.
 *
 * Reconfigure a unicast group with one or more audio streams as a unicast client.
 * All streams shall share the same framing.
 * All streams in the same direction shall share the same interval and latency (see
 * @ref bt_bap_qos_cfg).
 * All streams in @p param shall already belong to @p unicast_group.
 * Use bt_cap_unicast_group_add_streams() to add additional streams.
 *
 * @param unicast_group  Pointer to the unicast group created.
 * @param param          The unicast group reconfigure parameters.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_cap_unicast_group_reconfig_safe(struct bt_cap_unicast_group *unicast_group,
                                       const struct bt_cap_unicast_group_param *param);

/**
 * @brief Add streams to a unicast group as a unicast client
 *
 * This function can be used to add additional streams to a  bt_cap_unicast_group.
 *
 * This can be called at any time before any of the streams in the group has been started
 * (see bt_bap_stream_ops.started()).
 * This can also be called after the streams have been stopped (see bt_bap_stream_ops.stopped()).
 *
 * Once a stream has been added to a unicast group, it cannot be removed. To remove a stream from a
 * group, the group must be deleted with bt_cap_unicast_group_delete(), but this will require all
 * streams in the group to be released first.
 *
 * @param unicast_group  Pointer to the unicast group
 * @param params         Array of stream parameters with streams being added to the group.
 * @param num_param      Number of parameters in @p params.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_cap_unicast_group_add_streams_safe(struct bt_cap_unicast_group *unicast_group,
                                          const struct bt_cap_unicast_group_stream_pair_param params[],
                                          size_t num_param);

/**
 * @brief Delete audio unicast group.
 *
 * Delete a audio unicast group as a client. All streams in the group shall
 * be in the idle or configured state.
 *
 * @param unicast_group  Pointer to the unicast group to delete
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_cap_unicast_group_delete_safe(struct bt_cap_unicast_group *unicast_group);

/** Callback function for bt_bap_unicast_group_foreach_stream()
 *
 * @param stream     The audio stream
 * @param user_data  User data
 *
 * @retval true Stop iterating.
 * @retval false Continue iterating.
 */
typedef bool (*bt_cap_unicast_group_foreach_stream_func_t)(struct bt_cap_stream *stream,
                                                           void *user_data);

/**
 * @brief Iterate through all streams in a unicast group
 *
 * @param unicast_group  The unicast group
 * @param func           The callback function
 * @param user_data      User specified data that is sent to the callback function
 *
 * @retval 0 Success (even if no streams exists in the group).
 * @retval -ECANCELED The @p func returned true.
 * @retval -EINVAL @p unicast_group or @p func were NULL.
 */
int bt_cap_unicast_group_foreach_stream_safe(struct bt_cap_unicast_group *unicast_group,
                                             bt_cap_unicast_group_foreach_stream_func_t func,
                                             void *user_data);

/** Structure holding information of audio stream endpoint */
struct bt_cap_unicast_group_info {
    /** Pointer to the underlying Basic Audio Profile unicast group */
    const struct bt_bap_unicast_group *unicast_group;
};

/**
 * @brief Return structure holding information of unicast group
 *
 * @param unicast_group The unicast group object.
 * @param info          The structure object to be filled with the info.
 *
 * @retval 0 Success
 * @retval -EINVAL  @p unicast_group or @p info are NULL
 */
int bt_cap_unicast_group_get_info_safe(const struct bt_cap_unicast_group *unicast_group,
                                       struct bt_cap_unicast_group_info *info);

/** Stream specific parameters for the bt_cap_initiator_unicast_audio_start() function */
struct bt_cap_unicast_audio_start_stream_param {
    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member member;

    /** @brief Stream for the @p member */
    struct bt_cap_stream *stream;

    /** Endpoint reference for the @p stream */
    struct bt_bap_ep *ep;

    /**
     * @brief Codec configuration.
     *
     * The @p codec_cfg.meta shall include a list of CCIDs
     * (@ref BT_AUDIO_METADATA_TYPE_CCID_LIST) as well as a non-0
     * stream context (@ref BT_AUDIO_METADATA_TYPE_STREAM_CONTEXT) bitfield.
     *
     * This value is assigned to the @p stream, and shall remain valid while the stream is
     * non-idle.
     */
    struct bt_audio_codec_cfg *codec_cfg;
};

/** Parameters for the bt_cap_initiator_unicast_audio_start() function */
struct bt_cap_unicast_audio_start_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The number of parameters in @p stream_params */
    size_t count;

    /** Array of stream parameters */
    struct bt_cap_unicast_audio_start_stream_param *stream_params;
};

/** Stream specific parameters for the bt_cap_initiator_unicast_audio_update() function */
struct bt_cap_unicast_audio_update_stream_param {
    /** Stream to update */
    struct bt_cap_stream *stream;

    /** The length of @p meta. */
    size_t meta_len;

    /**
     * @brief The new metadata.
     *
     * The metadata shall contain a list of CCIDs as well as a non-0 context bitfield.
     */
    uint8_t *meta;
};

/** Parameters for the bt_cap_initiator_unicast_audio_update() function */
struct bt_cap_unicast_audio_update_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The number of parameters in @p stream_params */
    size_t count;

    /** Array of stream parameters */
    struct bt_cap_unicast_audio_update_stream_param *stream_params;
};

/** Parameters for the bt_cap_initiator_unicast_audio_stop() function */
struct bt_cap_unicast_audio_stop_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The number of streams in @p streams */
    size_t count;

    /** Array of streams to stop */
    struct bt_cap_stream **streams;

    /** Whether to release the streams after they have stopped */
    bool release;
};

/**
 * @brief Register Common Audio Profile Initiator callbacks
 *
 * @param cb   The callback structure. Shall remain static.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_initiator_register_cb_safe(const struct bt_cap_initiator_cb *cb);

/**
 * @brief Unregister Common Audio Profile Initiator callbacks
 *
 * @param cb   The callback structure that was previously registered.
 *
 * @retval 0 Success
 * @retval -EINVAL @p cb is NULL or @p cb was not registered
 */
int bt_cap_initiator_unregister_cb_safe(const struct bt_cap_initiator_cb *cb);

/**
 * @brief Setup and start unicast audio streams for a set of devices.
 *
 * The result of this operation is that the streams in @p param will be
 * initialized and will be usable for streaming audio data.
 * The @p unicast_group value can be used to update and stop the streams.
 *
 * @note @kconfig{CONFIG_BT_CAP_INITIATOR} and
 * @kconfig{CONFIG_BT_BAP_UNICAST_CLIENT} must be enabled for this function
 * to be enabled.
 *
 * @param param Parameters to start the audio streams.
 *
 * @retval 0 on success
 * @retval -EBUSY if a CAP procedure is already in progress
 * @retval -EINVAL if any parameter is invalid
 * @retval -EALREADY All streams are already in the streaming state
 */
int bt_cap_initiator_unicast_audio_start_safe(const struct bt_cap_unicast_audio_start_param *param);

/**
 * @brief Update unicast audio streams.
 *
 * This will update the metadata of one or more streams.
 *
 * @note @kconfig{CONFIG_BT_CAP_INITIATOR} and
 * @kconfig{CONFIG_BT_BAP_UNICAST_CLIENT} must be enabled for this function
 * to be enabled.
 *
 * @param param Update parameters.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_initiator_unicast_audio_update_safe(const struct bt_cap_unicast_audio_update_param *param);

/**
 * @brief Stop unicast audio streams.
 *
 * This will stop one or more streams.
 *
 * @note @kconfig{CONFIG_BT_CAP_INITIATOR} and
 * @kconfig{CONFIG_BT_BAP_UNICAST_CLIENT} must be enabled for this function
 * to be enabled.
 *
 * @param param Stop parameters.
 *
 * @return 0 on success
 * @retval -EBUSY if a CAP procedure is already in progress
 * @retval -EINVAL if any parameter is invalid
 * @retval -EALREADY if no state changes will occur
 */
int bt_cap_initiator_unicast_audio_stop_safe(const struct bt_cap_unicast_audio_stop_param *param);

/**
 * @brief Cancel any current Common Audio Profile procedure
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
 * The respective callbacks of the procedure will be called as part of this with the connection
 * pointer set to 0 and the err value set to -ECANCELED.
 *
 * @retval 0 on success
 * @retval -EALREADY if no procedure is active
 */
int bt_cap_initiator_unicast_audio_cancel_safe(void);

/**
 * Parameters part of @p bt_cap_initiator_broadcast_subgroup_param for
 * bt_cap_initiator_broadcast_audio_create()
 */
struct bt_cap_initiator_broadcast_stream_param {
    /** Audio stream */
    struct bt_cap_stream *stream;

    /** The length of the %p data array.
     *
     * The BIS specific data may be omitted and this set to 0.
     */
    size_t data_len;

    /** BIS Codec Specific Configuration */
    uint8_t *data;
};

/**
 * Parameters part of @p bt_cap_initiator_broadcast_create_param for
 * bt_cap_initiator_broadcast_audio_create()
 */
struct bt_cap_initiator_broadcast_subgroup_param {
    /** The number of parameters in @p stream_params */
    size_t stream_count;

    /** Array of stream parameters */
    struct bt_cap_initiator_broadcast_stream_param *stream_params;

    /** Subgroup Codec configuration. */
    struct bt_audio_codec_cfg *codec_cfg;
};

/** Parameters for * bt_cap_initiator_broadcast_audio_create() */
struct bt_cap_initiator_broadcast_create_param {
    /** The number of parameters in @p subgroup_params */
    size_t subgroup_count;

    /** Array of stream parameters */
    struct bt_cap_initiator_broadcast_subgroup_param *subgroup_params;

    /** Quality of Service configuration. */
    struct bt_bap_qos_cfg *qos;

    /**
     * @brief Broadcast Source packing mode.
     *
     * @ref BT_ISO_PACKING_SEQUENTIAL or @ref BT_ISO_PACKING_INTERLEAVED.
     *
     * @note This is a recommendation to the controller, which the controller may ignore.
     */
    uint8_t packing;

    /** Whether or not to encrypt the streams. */
    bool encryption;

    /**
     * @brief 16-octet broadcast code.
     *
     * Only valid if @p encrypt is true.
     *
     * If the value is a string or a the value is less than 16 octets,
     * the remaining octets shall be 0.
     *
     * Example:
     *   The string "Broadcast Code" shall be
     *   [42 72 6F 61 64 63 61 73 74 20 43 6F 64 65 00 00]
     */
    uint8_t broadcast_code[BT_ISO_BROADCAST_CODE_SIZE];

    /**
     * @brief Immediate Repetition Count
     *
     * The number of times the scheduled payloads are transmitted in a given event.
     *
     * Value range from @ref BT_ISO_IRC_MIN to @ref BT_ISO_IRC_MAX.
     */
    uint8_t irc;

    /**
     * @brief Pre-transmission offset
     *
     * Offset used for pre-transmissions.
     *
     * Value range from @ref BT_ISO_PTO_MIN to @ref BT_ISO_PTO_MAX.
     */
    uint8_t pto;

    /**
     * @brief ISO interval
     *
     * Time between consecutive BIS anchor points.
     *
     * Value range from @ref BT_ISO_ISO_INTERVAL_MIN to  @ref BT_ISO_ISO_INTERVAL_MAX.
     */
    uint16_t iso_interval;
};

/**
 * @brief Create a Common Audio Profile broadcast source.
 *
 * Create a new audio broadcast source with one or more audio streams.
 *
 * @note @kconfig{CONFIG_BT_CAP_INITIATOR} and
 * @kconfig{CONFIG_BT_BAP_BROADCAST_SOURCE} must be enabled for this function
 * to be enabled.
 *
 * @param[in]  param             Parameters to start the audio streams.
 * @param[out] broadcast_source  Pointer to the broadcast source created.
 *
 * @retval 0 Success
 * @retval -EINVAL @p param is invalid or @p broadcast_source is NULL
 * @retval -ENOMEM Could not allocate more broadcast sources, subgroups or ISO streams, or the
 *         provided codec configuration data is too large when merging the BIS and subgroup
 *         configuration data.
 * @retval -ENOEXEC The broadcast source failed to be created for other reasons
 */
int bt_cap_initiator_broadcast_audio_create_safe(
    const struct bt_cap_initiator_broadcast_create_param *param,
    struct bt_cap_broadcast_source **broadcast_source);

/**
 * @brief Start Common Audio Profile broadcast source.
 *
 * The broadcast source will be visible for scanners once this has been called,
 * and the device will advertise audio announcements.
 *
 * This will allow the streams in the broadcast source to send audio by calling
 * bt_bap_stream_send().
 *
 * @note @kconfig{CONFIG_BT_CAP_INITIATOR} and
 * @kconfig{CONFIG_BT_BAP_BROADCAST_SOURCE} must be enabled for this function
 * to be enabled.
 *
 * @param broadcast_source  Pointer to the broadcast source.
 * @param adv               Pointer to an extended advertising set with
 *                          periodic advertising configured.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_initiator_broadcast_audio_start(struct bt_cap_broadcast_source *broadcast_source,
                                           struct bt_le_ext_adv *adv);
int bt_cap_initiator_broadcast_audio_start_safe(struct bt_cap_broadcast_source *broadcast_source,
                                                struct bt_le_ext_adv *adv);

/**
 * @brief Update broadcast audio streams for a Common Audio Profile broadcast source.
 *
 * @note @kconfig{CONFIG_BT_CAP_INITIATOR} and
 * @kconfig{CONFIG_BT_BAP_BROADCAST_SOURCE} must be enabled for this function
 * to be enabled.
 *
 * @param broadcast_source The broadcast source to update.
 * @param meta             The new metadata. The metadata shall contain a list
 *                         of CCIDs as well as a non-0 context bitfield.
 * @param meta_len         The length of @p meta.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_initiator_broadcast_audio_update_safe(struct bt_cap_broadcast_source *broadcast_source,
                                                 const uint8_t meta[], size_t meta_len);

/**
 * @brief Stop broadcast audio streams for a Common Audio Profile broadcast source.
 *
 * @note @kconfig{CONFIG_BT_CAP_INITIATOR} and
 * @kconfig{CONFIG_BT_BAP_BROADCAST_SOURCE} must be enabled for this function
 * to be enabled.
 *
 * @param broadcast_source The broadcast source to stop. The audio streams
 *                         in this will be stopped and reset.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_initiator_broadcast_audio_stop_safe(struct bt_cap_broadcast_source *broadcast_source);

/**
 * @brief Delete Common Audio Profile broadcast source
 *
 * This can only be done after the broadcast source has been stopped by calling
 * bt_cap_initiator_broadcast_audio_stop() and after the
 * bt_bap_stream_ops.stopped() callback has been called for all streams in the
 * broadcast source.
 *
 * @note @kconfig{CONFIG_BT_CAP_INITIATOR} and
 * @kconfig{CONFIG_BT_BAP_BROADCAST_SOURCE} must be enabled for this function
 * to be enabled.
 *
 * @param broadcast_source The broadcast source to delete.
 *                         The @p broadcast_source will be invalidated.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_initiator_broadcast_audio_delete_safe(struct bt_cap_broadcast_source *broadcast_source);

/**
 * @brief Get the Broadcast Audio Stream Endpoint of a Common Audio Profile broadcast source
 *
 * This will encode the BASE of a broadcast source into a buffer, that can be
 * used for advertisement. The encoded BASE will thus be encoded as
 * little-endian. The BASE shall be put into the periodic advertising data
 * (see bt_le_per_adv_set_data()).
 *
 * See table 3.15 in the Basic Audio Profile v1.0.1 for the structure.
 *
 * @param broadcast_source  Pointer to the broadcast source.
 * @param base_buf          Pointer to a buffer where the BASE will be inserted.
 *
 * @return int      0 if on success, errno on error.
 */
int bt_cap_initiator_broadcast_get_base_safe(struct bt_cap_broadcast_source *broadcast_source,
                                             struct net_buf_simple *base_buf);

/** Callback function for bt_cap_initiator_broadcast_foreach_stream()
 *
 * @param stream     The audio stream
 * @param user_data  User data
 *
 * @retval true Stop iterating.
 * @retval false Continue iterating.
 */
typedef bool (*bt_cap_initiator_broadcast_foreach_stream_func_t)(struct bt_cap_stream *stream,
                                                                 void *user_data);

/**
 * @brief Iterate through all streams in a broadcast source
 *
 * @param broadcast_source  The broadcast source.
 * @param func              The callback function.
 * @param user_data         User specified data that is sent to the callback function.
 *
 * @retval 0          Success (even if no streams exists in the group).
 * @retval -ECANCELED The @p func returned true.
 * @retval -EINVAL    @p broadcast_source or @p func were NULL.
 */
int bt_cap_initiator_broadcast_foreach_stream_safe(struct bt_cap_broadcast_source *broadcast_source,
                                                   bt_cap_initiator_broadcast_foreach_stream_func_t func,
                                                   void *user_data);

/** Parameters for  bt_cap_handover_unicast_to_broadcast() */
struct bt_cap_handover_unicast_to_broadcast_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The source unicast group with the streams. */
    struct bt_cap_unicast_group *unicast_group;

    /**
     * @brief The advertising set to use for the broadcast source
     *
     * This shall remain valid until the procedure has completed.
     * If the advertising set is not started at the time of calling
     * bt_cap_handover_unicast_to_broadcast(),
     * the procedure will start the advertising set with @ref BT_LE_EXT_ADV_START_DEFAULT.
     */
    struct bt_le_ext_adv *ext_adv;

    /** The periodic advertising interval configured for the advertising set. */
    uint16_t pa_interval;

    /** The broadcast ID the advertising set is, or will be, using. */
    uint32_t broadcast_id;

    /**
     * @brief Broadcast source parameters.
     *
     * These parameters shall remain valid until the procedure has completed.
     */
    struct bt_cap_initiator_broadcast_create_param *broadcast_create_param;
};

/** Callback structure for CAP procedures */
struct bt_cap_handover_cb {
    /**
     * @brief The unicast to broadcast handover procedure has finished
     *
     * @param err 0 if success else a negative errno value.
     * @param conn Pointer to the connection where the error occurred or NULL if local failure.
     * @param unicast_group NULL if the unicast group was deleted during the procedure, else
     *                      pointer to the unicast group provided in the parameters.
     * @param broadcast_source Pointer to newly created broadcast source, or NULL in case of an
     *                         error happening before it was created.
     */
    void (*unicast_to_broadcast_complete)(int err, struct bt_conn *conn,
                                          struct bt_cap_unicast_group *unicast_group,
                                          struct bt_cap_broadcast_source *broadcast_source);

    /**
     * @brief The broadcast to unicast handover procedure has finished
     *
     * @param err 0 if success else a negative errno value.
     * @param conn Pointer to the connection where the error occurred or NULL if local failure.
     * @param broadcast_source NULL if the broadcast sourced was deleted during the procedure,
     *                         else pointer to the broadcast sourced provided in the parameters.
     * @param unicast_group Pointer to newly created unicast group, or NULL in case of an
     *                      error happening before it was created.
     */
    void (*broadcast_to_unicast_complete)(int err, struct bt_conn *conn,
                                          struct bt_cap_broadcast_source *broadcast_source,
                                          struct bt_cap_unicast_group *unicast_group);
};

/**
 * @brief Register Common Audio Profile Handover callbacks
 *
 * @param cb   The callback structure. Shall remain static.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_handover_register_cb_safe(const struct bt_cap_handover_cb *cb);

/**
 * @brief Unregister Common Audio Profile Handover callbacks
 *
 * @param cb   The callback structure that was previously registered.
 *
 * @retval 0 Success
 * @retval -EINVAL @p cb is NULL or @p cb was not registered
 */
int bt_cap_handover_unregister_cb_safe(const struct bt_cap_handover_cb *cb);

/**
 * @brief Hands over the sink streams in a unicast group to a broadcast source.
 *
 * All streams in the provided unicast group will be stopped and released. The sink streams will be
 * transferred to a broadcast source, and the broadcast source information will be shared with
 * all accepters that are currently receiving audio. Any stream that is not in the streaming state
 * will only be released.
 *
 * bt_bap_broadcast_assistant_discover() must have been successfully perform for all members in @p
 * param before starting this procedure.
 *
 * @kconfig_dep{CONFIG_BT_CAP_HANDOVER}
 *
 * @param param         The parameters for the handover.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_handover_unicast_to_broadcast_safe(
    const struct bt_cap_handover_unicast_to_broadcast_param *param);

/** Parameters for  bt_cap_handover_broadcast_to_unicast() */
struct bt_cap_handover_broadcast_to_unicast_param {
    /**
     * @brief Parameters for stopping broadcast audio reception on acceptors
     *
     * This parameter is optional as stopping the broadcast audio should automatically stop
     * broadcast audio reception on the acceptors. Omitting this parameter will rely on the CAP
     * acceptors timing out on the BIG once it is stopped. The timeout on the CAP acceptors will
     * be between @ref BT_ISO_SYNC_TIMEOUT_MIN and @ref BT_ISO_SYNC_TIMEOUT_MAX.
     */
    struct bt_cap_commander_broadcast_reception_stop_param *reception_stop_param;

    /** @brief Broadcast ID of the @p broadcast_source
     *
     * Ignored if @p reception_stop_param is not NULL.
     */
    uint32_t broadcast_id;

    /** @brief Advertising set ID of the @p broadcast_source
     *
     * Ignored if @p reception_stop_param is not NULL.
     */
    uint8_t adv_sid;

    /** @brief Advertising type of the advertising address of @p broadcast_source
     *
     * Ignored if @p reception_stop_param is not NULL.
     */
    uint8_t adv_type;

    /**
     * @brief The source broadcast source with the streams.
     *
     * The broadcast source will be stopped and deleted.
     */
    struct bt_cap_broadcast_source *broadcast_source;

    /* Parameters for the unicast group to be created */
    struct bt_cap_unicast_group_param *unicast_group_param;

    /* Parameters for starting the unicast audio */
    struct bt_cap_unicast_audio_start_param *unicast_start_param;
};

/**
 * @brief Hands over the data streams in a broadcast source to a unicast group.
 *
 * The streams in the broadcast source will be stopped and the broadcast source
 * will be deleted.
 *
 * @kconfig_dep{CONFIG_BT_CAP_HANDOVER}
 *
 * @param[in]  param          The parameters for the handover.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_handover_broadcast_to_unicast_safe(
    const struct bt_cap_handover_broadcast_to_unicast_param *param);

/** Callback structure for CAP procedures */
struct bt_cap_commander_cb {
    /**
     * @brief Callback for bt_cap_initiator_unicast_discover().
     *
     * @param conn      The connection pointer supplied to
     *                  bt_cap_initiator_unicast_discover().
     * @param err       0 if Common Audio Service was found else -ENODATA.
     * @param member    Pointer to the set member. NULL if err != 0.
     * @param csis_inst The Coordinated Set Identification Service if
     *                  Common Audio Service was found and includes a
     *                  Coordinated Set Identification Service.
     *                  NULL on error or if remote device does not include
     *                  Coordinated Set Identification Service. NULL if err != 0.
     */
    void (*discovery_complete)(struct bt_conn *conn, int err,
                               const struct bt_csip_set_coordinator_set_member *member,
                               const struct bt_csip_set_coordinator_csis_inst *csis_inst);

    /**
     * @brief Callback for bt_cap_commander_change_volume().
     *
     * @param conn           Pointer to the connection where the error
     *                       occurred. NULL if @p err is 0 or if cancelled by
     *                       bt_cap_commander_cancel()
     * @param err            0 on success, BT_GATT_ERR() with a
     *                       specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *                       by bt_cap_commander_cancel().
     */
    void (*volume_changed)(struct bt_conn *conn, int err);

    /**
     * @brief Callback for bt_cap_commander_change_volume_mute_state().
     *
     * @param conn           Pointer to the connection where the error
     *                       occurred. NULL if @p err is 0 or if cancelled by
     *                       bt_cap_commander_cancel()
     * @param err            0 on success, BT_GATT_ERR() with a
     *                       specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *                       by bt_cap_commander_cancel().
     */
    void (*volume_mute_changed)(struct bt_conn *conn, int err);

    /**
     * @brief Callback for bt_cap_commander_change_volume_offset().
     *
     * @param conn           Pointer to the connection where the error
     *                       occurred. NULL if @p err is 0 or if cancelled by
     *                       bt_cap_commander_cancel()
     * @param err            0 on success, BT_GATT_ERR() with a
     *                       specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *                       by bt_cap_commander_cancel().
     */
    void (*volume_offset_changed)(struct bt_conn *conn, int err);

    /**
     * @brief Callback for bt_cap_commander_change_microphone_mute_state().
     *
     * @param conn           Pointer to the connection where the error
     *                       occurred. NULL if @p err is 0 or if cancelled by
     *                       bt_cap_commander_cancel()
     * @param err            0 on success, BT_GATT_ERR() with a
     *                       specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *                       by bt_cap_commander_cancel().
     */
    void (*microphone_mute_changed)(struct bt_conn *conn, int err);

    /**
     * @brief Callback for bt_cap_commander_change_microphone_gain_setting().
     *
     * @param conn           Pointer to the connection where the error
     *                       occurred. NULL if @p err is 0 or if cancelled by
     *                       bt_cap_commander_cancel()
     * @param err            0 on success, BT_GATT_ERR() with a
     *                       specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *                       by bt_cap_commander_cancel().
     */
    void (*microphone_gain_changed)(struct bt_conn *conn, int err);

    /**
     * @brief Callback for bt_cap_commander_broadcast_reception_start().
     *
     * @param conn      Pointer to the connection where the error
     *          occurred. NULL if @p err is 0 or if cancelled by
     *          bt_cap_commander_cancel()
     * @param err       0 on success, BT_GATT_ERR() with a
     *          specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *          by bt_cap_commander_cancel().
     */
    void (*broadcast_reception_start)(struct bt_conn *conn, int err);
    /**
     * @brief Callback for bt_cap_commander_broadcast_reception_stop().
     *
     * @param conn      Pointer to the connection where the error
     *          occurred. NULL if @p err is 0 or if cancelled by
     *          bt_cap_commander_cancel()
     * @param err       0 on success, BT_GATT_ERR() with a
     *          specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *          by bt_cap_commander_cancel().
     */
    void (*broadcast_reception_stop)(struct bt_conn *conn, int err);
    /**
     * @brief Callback for bt_cap_commander_distribute_broadcast_code().
     *
     * @param conn      Pointer to the connection where the error
     *          occurred. NULL if @p err is 0 or if cancelled by
     *          bt_cap_commander_cancel()
     * @param err       0 on success, BT_GATT_ERR() with a
     *          specific ATT (BT_ATT_ERR_*) error code or -ECANCELED if cancelled
     *          by bt_cap_commander_cancel().
     */
    void (*distribute_broadcast_code)(struct bt_conn *conn, int err);
};

/**
 * @brief Register Common Audio Profile Commander callbacks
 *
 * @param cb   The callback structure. Shall remain static.
 *
 * @retval 0 Success
 * @retval -EINVAL @p cb is NULL
 * @retval -EALREADY Callbacks are already registered
 */
int bt_cap_commander_register_cb_safe(const struct bt_cap_commander_cb *cb);

/**
 * @brief Unregister Common Audio Profile Commander callbacks
 *
 * @param cb   The callback structure that was previously registered.
 *
 * @retval 0 Success
 * @retval -EINVAL @p cb is NULL or @p cb was not registered
 */
int bt_cap_commander_unregister_cb_safe(const struct bt_cap_commander_cb *cb);

/**
 * @brief Discovers audio support on a remote device.
 *
 * This will discover the Common Audio Service (CAS) on the remote device, to
 * verify if the remote device supports the Common Audio Profile.
 *
 * @note @kconfig{CONFIG_BT_CAP_COMMANDER} must be enabled for this function. If
 * @kconfig{CONFIG_BT_CAP_INITIATOR} is also enabled, it does not matter if
 * bt_cap_commander_discover() or bt_cap_initiator_unicast_discover() is used.
 *
 * @param conn Connection to a remote server.
 *
 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -EBUSY Already doing discovery for @p conn
 */
int bt_cap_commander_discover(struct bt_conn *conn);
int bt_cap_commander_discover_safe(struct bt_conn *conn);

/**
 * @brief Cancel any current Common Audio Profile commander procedure
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
 * The respective callbacks of the procedure will be called as part of this with the connection
 * pointer set to NULL and the err value set to -ECANCELED.
 *
 * @retval 0 on success
 * @retval -EALREADY if no procedure is active
 */
int bt_cap_commander_cancel_safe(void);

/**
 * Parameters part of @ref bt_cap_commander_broadcast_reception_start_param for
 * bt_cap_commander_broadcast_reception_start()
 */
struct bt_cap_commander_broadcast_reception_start_member_param {
    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member member;

    /** Address of the advertiser. */
    bt_addr_le_t addr;

    /** SID of the advertising set. */
    uint8_t adv_sid;

    /**
     * @brief Periodic advertising interval in milliseconds.
     *
     * BT_BAP_PA_INTERVAL_UNKNOWN if unknown.
     */
    uint16_t pa_interval;

    /** 24-bit broadcast ID */
    uint32_t broadcast_id;

    /**
     * @brief Pointer to array of subgroups
     *
     * At least one bit in one of the subgroups bis_sync parameters shall be set.
     */
    struct bt_bap_bass_subgroup *subgroups;

    /** Number of subgroups */
    size_t num_subgroups;
};

/** Parameters for starting broadcast reception  */
struct bt_cap_commander_broadcast_reception_start_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The set of devices for this procedure */
    struct bt_cap_commander_broadcast_reception_start_member_param *param;

    /** The number of parameters in @p param */
    size_t count;
};

/**
 * @brief Starts the reception of broadcast audio on one or more remote Common Audio Profile
 * Acceptors
 *
 * @param param The parameters to start the broadcast audio
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_commander_broadcast_reception_start_safe(
    const struct bt_cap_commander_broadcast_reception_start_param *param);

/** Member parameters for stopping broadcast reception  */
struct bt_cap_commander_broadcast_reception_stop_member_param {
    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member member;

    /** Source ID of the receive state. */
    uint8_t src_id;

    /** Number of subgroups */
    size_t num_subgroups;
};

/** Parameters for stopping broadcast reception */
struct bt_cap_commander_broadcast_reception_stop_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The set of devices for this procedure */
    struct bt_cap_commander_broadcast_reception_stop_member_param *param;

    /** The number of parameters in @p param */
    size_t count;
};

/**
 * @brief Stops the reception of broadcast audio on one or more remote Common Audio Profile
 * Acceptors
 *
 * @param param The parameters to stop the broadcast audio
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_commander_broadcast_reception_stop_safe(
    const struct bt_cap_commander_broadcast_reception_stop_param *param);

/** Member parameters for distributing broadcast code */
struct bt_cap_commander_distribute_broadcast_code_member_param {
    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member member;

    /** Source ID of the receive state. */
    uint8_t src_id;
};

/** Parameters for distributing broadcast code */
struct bt_cap_commander_distribute_broadcast_code_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The set of devices for this procedure */
    struct bt_cap_commander_distribute_broadcast_code_member_param *param;

    /** The number of parameters in @p param */
    size_t count;

    /**
     * @brief 16-octet broadcast code.
     *
     * If the value is a string or a the value is less than 16 octets,
     * the remaining octets shall be 0.
     *
     * Example:
     *   The string "Broadcast Code" shall be
     *   [42 72 6F 61 64 63 61 73 74 20 43 6F 64 65 00 00]
     */
    uint8_t broadcast_code[BT_ISO_BROADCAST_CODE_SIZE];
};

/**
 * @brief Distributes the broadcast code on one or more remote Common Audio Profile
 * Acceptors
 *
 * @param param The parameters for distributing the broadcast code
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_commander_distribute_broadcast_code_safe(
    const struct bt_cap_commander_distribute_broadcast_code_param *param);

/** Parameters for changing absolute volume  */
struct bt_cap_commander_change_volume_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member *members;

    /** The number of members in @p members */
    size_t count;

    /** The absolute volume to set */
    uint8_t volume;
};

/**
 * @brief Change the volume on one or more Common Audio Profile Acceptors
 *
 * @param param The parameters for the volume change
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_commander_change_volume_safe(const struct bt_cap_commander_change_volume_param *param);

/**
 * Parameters part of @ref bt_cap_commander_change_volume_offset_param for
 * bt_cap_commander_change_volume_offset()
 */
struct bt_cap_commander_change_volume_offset_member_param {
    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member member;

    /**
     * @brief  The offset to set
     *
     * Value shall be between @ref BT_VOCS_MIN_OFFSET and @ref BT_VOCS_MAX_OFFSET
     */
    int16_t offset;
};

/** Parameters for changing volume offset */
struct bt_cap_commander_change_volume_offset_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The set of devices for this procedure */
    struct bt_cap_commander_change_volume_offset_member_param *param;

    /** The number of parameters in @p param */
    size_t count;
};

/**
 * @brief Change the volume offset on one or more Common Audio Profile Acceptors
 *
 * @param param The parameters for the volume offset change
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_commander_change_volume_offset_safe(
    const struct bt_cap_commander_change_volume_offset_param *param);

/** Parameters for changing volume mute state */
struct bt_cap_commander_change_volume_mute_state_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member *members;

    /** The number of members in @p members */
    size_t count;

    /**
     * @brief The volume mute state to set
     *
     * true to mute, and false to unmute
     */
    bool mute;
};

/**
 * @brief Change the volume mute state on one or more Common Audio Profile Acceptors
 *
 * @param param The parameters for the volume mute state change
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_commander_change_volume_mute_state_safe(
    const struct bt_cap_commander_change_volume_mute_state_param *param);

/** Parameters for changing microphone mute state */
struct bt_cap_commander_change_microphone_mute_state_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member *members;

    /** The number of members in @p members */
    size_t count;

    /**
     * @brief The microphone mute state to set
     *
     * true to mute, and false to unmute
     */
    bool mute;
};

/**
 * @brief Change the microphone mute state on one or more Common Audio Profile Acceptors
 *
 * @param param The parameters for the microphone mute state change
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_commander_change_microphone_mute_state_safe(
    const struct bt_cap_commander_change_microphone_mute_state_param *param);

/**
 * Parameters part of @ref bt_cap_commander_change_microphone_gain_setting_param for
 * bt_cap_commander_change_microphone_gain_setting()
 */
struct bt_cap_commander_change_microphone_gain_setting_member_param {
    /** Coordinated or ad-hoc set member. */
    union bt_cap_set_member member;

    /** @brief The microphone gain setting to set */
    int8_t gain;
};

/** Parameters for changing microphone mute state */
struct bt_cap_commander_change_microphone_gain_setting_param {
    /** The type of the set. */
    enum bt_cap_set_type type;

    /** The set of devices for this procedure */
    struct bt_cap_commander_change_microphone_gain_setting_member_param *param;

    /** The number of parameters in @p param */
    size_t count;
};

/**
 * @brief Change the microphone gain setting on one or more Common Audio Profile Acceptors
 *
 * @param param The parameters for the microphone gain setting change
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_cap_commander_change_microphone_gain_setting_safe(
    const struct bt_cap_commander_change_microphone_gain_setting_param *param);
#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_CAP_H_ */
