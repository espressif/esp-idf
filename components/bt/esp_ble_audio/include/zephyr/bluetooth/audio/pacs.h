/**
 * @file
 * @brief Bluetooth Published Audio Capabilities Service (PACS) APIs
 */

/* SPDX-FileCopyrightText: 2021 Intel Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_PACS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_PACS_H_

/**
 * @brief Published Audio Capabilities Service (PACS)
 *
 * @defgroup bt_pacs Published Audio Capabilities Service (PACS)
 *
 * @since 3.0
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Published Audio Capabilities Service (PACS) is used to expose capabilities to remote devices.
 */

#include <stdbool.h>

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/slist.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Published Audio Capability structure. */
struct bt_pacs_cap {
    /** Codec capability reference */
    const struct bt_audio_codec_cap *codec_cap;

    /** @cond INTERNAL_HIDDEN */
    /** Internally used field for list handling */
    sys_snode_t _node;
    /** @endcond */
};

/** Structure for registering PACS */
struct bt_pacs_register_param {
    /**
     * @brief Enables or disables registration of Sink PAC Characteristic.
     */
    bool snk_pac;

    /**
     * @brief Enables or disables registration of Sink Location Characteristic.
     *
     * Registration of Sink Location is dependent on @ref bt_pacs_register_param.snk_pac
     * also being set.
     */
    bool snk_loc;

    /**
     * @brief Enables or disables registration of Source PAC Characteristic.
     */
    bool src_pac;

    /**
     * @brief Enables or disables registration of Source Location Characteristic.
     *
     * Registration of Source Location is dependent on @ref bt_pacs_register_param.src_pac
     * also being set.
     */
    bool src_loc;
};

/**
 * @typedef bt_pacs_cap_foreach_func_t
 * @brief Published Audio Capability iterator callback.
 *
 * @param cap Capability found.
 * @param user_data Data given.
 *
 * @return true to continue to the next capability
 * @return false to stop the iteration
 */
typedef bool (*bt_pacs_cap_foreach_func_t)(const struct bt_pacs_cap *cap,
                                           void *user_data);

/**
 * @brief Published Audio Capability iterator.
 *
 * Iterate capabilities with endpoint direction specified.
 *
 * @param dir Direction of the endpoint to look capability for.
 * @param func Callback function.
 * @param user_data Data to pass to the callback.
 */
void bt_pacs_cap_foreach_safe(enum bt_audio_dir dir,
                              bt_pacs_cap_foreach_func_t func,
                              void *user_data);

/**
 * @brief Register the Published Audio Capability Service instance.
 *
 * @param param PACS register parameters.
 *
 * @retval 0 Success
 * @retval -EINVAL @p param is NULL or bad combination of values in @p param
 * @retval -EALREADY Already registered
 * @retval -ENOEXEC Request was rejected by GATT
 */
int bt_pacs_register_safe(const struct bt_pacs_register_param *param);

/**
 * @brief Unregister the Published Audio Capability Service instance.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_pacs_unregister_safe(void);

/**
 * @brief Register Published Audio Capability.
 *
 * Register Audio Local Capability.
 *
 * @param dir Direction of the endpoint to register capability for.
 * @param cap Capability structure.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_pacs_cap_register_safe(enum bt_audio_dir dir, struct bt_pacs_cap *cap);

/**
 * @brief Unregister Published Audio Capability.
 *
 * Unregister Audio Local Capability.
 *
 * @param dir Direction of the endpoint to unregister capability for.
 * @param cap Capability structure.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_pacs_cap_unregister_safe(enum bt_audio_dir dir, struct bt_pacs_cap *cap);

/**
 * @brief Set the location for an endpoint type
 *
 * @param dir      Direction of the endpoints to change location for.
 * @param location The location to be set.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_pacs_set_location_safe(enum bt_audio_dir dir,
                              enum bt_audio_location location);

/**
 * @brief Set the available contexts for an endpoint type
 *
 * @param dir      Direction of the endpoints to change available contexts for.
 * @param contexts The contexts to be set.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_pacs_set_available_contexts_safe(enum bt_audio_dir dir,
                                        enum bt_audio_context contexts);

/**
 * @brief Get the available contexts for an endpoint type
 *
 * @param dir      Direction of the endpoints to get contexts for.
 *
 * @return Bitmask of available contexts.
 */
enum bt_audio_context bt_pacs_get_available_contexts_safe(enum bt_audio_dir dir);

/**
 * @brief Set the available contexts for a given connection
 *
 * This function sets the available contexts value for a given @p conn connection object.
 * If the @p contexts parameter is NULL the available contexts value is reset to default.
 * The default value of the available contexts is set using @ref bt_pacs_set_available_contexts
 * function.
 * The Available Context Value is reset to default on ACL disconnection.
 *
 * @param conn     Connection object.
 * @param dir      Direction of the endpoints to change available contexts for.
 * @param contexts The contexts to be set or NULL to reset to default.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_pacs_conn_set_available_contexts_for_conn(struct bt_conn *conn, enum bt_audio_dir dir,
                                                 enum bt_audio_context *contexts);
int bt_pacs_conn_set_available_contexts_for_conn_safe(struct bt_conn *conn, enum bt_audio_dir dir,
                                                      enum bt_audio_context *contexts);

/**
 * @brief Get the available contexts for a given connection
 *
 * This server function returns the available contexts value for a given @p conn connection object.
 * The value returned is the one set with @ref bt_pacs_conn_set_available_contexts_for_conn function
 * or the default value set with @ref bt_pacs_set_available_contexts function.
 *
 * @param conn     Connection object.
 * @param dir      Direction of the endpoints to get contexts for.
 *
 * @return Bitmask of available contexts.
 * @retval BT_AUDIO_CONTEXT_TYPE_NONE if @p conn or @p dir are invalid
 */
enum bt_audio_context bt_pacs_get_available_contexts_for_conn(struct bt_conn *conn,
                                                              enum bt_audio_dir dir);
enum bt_audio_context bt_pacs_get_available_contexts_for_conn_safe(struct bt_conn *conn,
                                                                   enum bt_audio_dir dir);

/**
 * @brief Set the supported contexts for an endpoint type
 *
 * @param dir      Direction of the endpoints to change available contexts for.
 * @param contexts The contexts to be set.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_pacs_set_supported_contexts_safe(enum bt_audio_dir dir,
                                        enum bt_audio_context contexts);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_PACS_H_ */
