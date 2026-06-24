/**
 * @file
 * @brief Bluetooth Volume Offset Control Service (VOCS) APIs.
 */

/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_VOCS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_VOCS_H_

/**
 * @brief Volume Offset Control Service (VOCS)
 *
 * @defgroup bt_vocs Volume Offset Control Service (VOCS)
 *
 * @since 2.6
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Volume Offset Control Service is a secondary service, and as such should not be used own its
 * own, but rather in the context of another (primary) service.
 *
 * This API implements both the server and client functionality.
 * Note that the API abstracts away the change counter in the volume offset control state and will
 * automatically handle any changes to that. If out of date, the client implementation will
 * autonomously read the change counter value when executing a write request.
 */

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/bluetooth/conn.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Volume Offset Control Service Error codes
 * @{
 */
/**
 * The Change_Counter operand value does not match the Change_Counter field value of the Volume
 * Offset State characteristic.
 */
#define BT_VOCS_ERR_INVALID_COUNTER                0x80
/** An invalid opcode has been used in a control point procedure. */
#define BT_VOCS_ERR_OP_NOT_SUPPORTED               0x81
/** An operand value used in a control point procedure is outside the permissible range. */
#define BT_VOCS_ERR_OUT_OF_RANGE                   0x82
/** @} */

/**
 * @name Volume Offset Control Service offset limits
 * @{
 */
/** Minimum offset value */
#define BT_VOCS_MIN_OFFSET                         -255
/** Maximum offset value */
#define BT_VOCS_MAX_OFFSET                         255
/** @} */

/** @brief Opaque Volume Offset Control Service instance. */
struct bt_vocs;

/** @brief Structure for registering a Volume Offset Control Service instance. */
struct bt_vocs_register_param {
    /** Audio Location bitmask */
    uint32_t location;

    /** Boolean to set whether the location is writable by clients */
    bool location_writable;

    /** Initial volume offset (-255 to 255) */
    int16_t offset;

    /** Initial audio output description */
    char *output_desc;

    /** Boolean to set whether the description is writable by clients */
    bool desc_writable;

    /** Pointer to the callback structure. */
    struct bt_vocs_cb *cb;
};

/** @brief Structure for discovering a Volume Offset Control Service instance. */
struct bt_vocs_discover_param {
    /**
     * @brief The start handle of the discovering.
     *
     * Typically the @p start_handle of a @ref bt_gatt_include.
     */
    uint16_t start_handle;
    /**
     * @brief The end handle of the discovering.
     *
     * Typically the @p end_handle of a @ref bt_gatt_include.
     */
    uint16_t end_handle;
};

/**
 * @brief Get a free service instance of Volume Offset Control Service from the pool.
 *
 * @return Volume Offset Control Service instance in case of success or NULL in case of error.
 */
struct bt_vocs *bt_vocs_free_instance_get_safe(void);

/**
 * @brief Get the service declaration attribute.
 *
 * The first service attribute returned can be included in any other GATT service.
 *
 * @param vocs Volume Offset Control Service instance.
 *
 * @return Pointer to the attributes of the service.
 */
void *bt_vocs_svc_decl_get_safe(struct bt_vocs *vocs);

/**
 * @brief Get the connection pointer of a client instance
 *
 * Get the Bluetooth connection pointer of a Audio Input Control Service
 * client instance.
 *
 * @param vocs    Audio Input Control Service client instance pointer.
 * @param conn    Connection pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vocs_client_conn_get_safe(const struct bt_vocs *vocs, struct bt_conn **conn);

/**
 * @brief Register the Volume Offset Control Service instance.
 *
 * @param vocs      Volume Offset Control Service instance.
 * @param param     Volume Offset Control Service register parameters.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vocs_register_safe(struct bt_vocs *vocs,
                          const struct bt_vocs_register_param *param);

/**
 * @brief Callback function for the offset state.
 *
 * Called when the value is read, or if the value is changed by either the server or client.
 *
 * @param inst        The instance pointer.
 * @param err         Error value. 0 on success, GATT error on positive value
 *                    or errno on negative value.
 *                    For notifications, this will always be 0.
 * @param offset      The offset value.
 */
typedef void (*bt_vocs_state_cb)(struct bt_vocs *inst, int err, int16_t offset);

/**
 * @brief Callback function for setting offset.
 *
 * @param inst        The instance pointer.
 * @param err         Error value. 0 on success, GATT error on positive value
 *                    or errno on negative value.
 */
typedef void (*bt_vocs_set_offset_cb)(struct bt_vocs *inst, int err);

/**
 * @brief Callback function for the location.
 *
 * Called when the value is read, or if the value is changed by either the server or client.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 * @param location     The location value.
 */
typedef void (*bt_vocs_location_cb)(struct bt_vocs *inst, int err,
                                    uint32_t location);

/**
 * @brief Callback function for the description.
 *
 * Called when the value is read, or if the value is changed by either the server or client.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 * @param description  The description as an UTF-8 encoded string.
 */
typedef void (*bt_vocs_description_cb)(struct bt_vocs *inst, int err,
                                       char *description);

/**
 * @brief Callback function for bt_vocs_discover.
 *
 * This callback should be overwritten by the primary service that
 * includes the Volume Control Offset Service client, and should thus not be
 * set by the application.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 */
typedef void (*bt_vocs_discover_cb)(struct bt_vocs *inst, int err);

/**
 * @brief Struct to hold the Volume Offset Control Service callbacks
 *
 * These can be registered for usage with bt_vocs_client_cb_register() or bt_vocs_register()
 * depending on the role.
 */
struct bt_vocs_cb {
    /** The offset state has changed */
    bt_vocs_state_cb                state;
    /** The location has changed */
    bt_vocs_location_cb             location;
    /** The Description has changed */
    bt_vocs_description_cb          description;

    /**
     * The discovery procedure has completed
     *
     * Only settable for the client and requires enabling@kconfig{CONFIG_BT_VOCS_CLIENT}.
     */
    bt_vocs_discover_cb             discover;
    /**
     * The set offset procedure has completed
     *
     * Only settable for the client and requires enabling@kconfig{CONFIG_BT_VOCS_CLIENT}.
     */
    bt_vocs_set_offset_cb           set_offset;
};

/**
 * @brief Read the Volume Offset Control Service offset state.
 *
 * The value is returned in the bt_vocs_cb.state callback.
 *
 * @param inst          Pointer to the Volume Offset Control Service instance.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_vocs_state_get_safe(struct bt_vocs *inst);

/**
 * @brief Set the Volume Offset Control Service offset state.
 *
 * @param inst          Pointer to the Volume Offset Control Service instance.
 * @param offset        The offset to set (-255 to 255).
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_vocs_state_set_safe(struct bt_vocs *inst, int16_t offset);

/**
 * @brief Read the Volume Offset Control Service location.
 *
 * The value is returned in the bt_vocs_cb.location callback.
 *
 * @param inst          Pointer to the Volume Offset Control Service instance.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_vocs_location_get_safe(struct bt_vocs *inst);

/**
 * @brief Set the Volume Offset Control Service location.
 *
 * @param inst          Pointer to the Volume Offset Control Service instance.
 * @param location      The location to set.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_vocs_location_set_safe(struct bt_vocs *inst, uint32_t location);

/**
 * @brief Read the Volume Offset Control Service output description.
 *
 * The value is returned in the bt_vocs_cb.description callback.
 *
 * @param inst          Pointer to the Volume Offset Control Service instance.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_vocs_description_get_safe(struct bt_vocs *inst);

/**
 * @brief Set the Volume Offset Control Service description.
 *
 * @param inst          Pointer to the Volume Offset Control Service instance.
 * @param description   The UTF-8 encoded string description to set.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_vocs_description_set_safe(struct bt_vocs *inst, const char *description);

/**
 * @brief Registers the callbacks for the Volume Offset Control Service client.
 *
 * @param inst  Pointer to the Volume Offset Control Service client instance.
 * @param cb    Pointer to the callback structure.
 */
void bt_vocs_client_cb_register_safe(struct bt_vocs *inst, struct bt_vocs_cb *cb);

/**
 * @brief Returns a pointer to a Volume Offset Control Service client instance.
 *
 * @return Pointer to the instance, or NULL if no free instances are left.
 */
struct bt_vocs *bt_vocs_client_free_instance_get_safe(void);

/**
 * @brief Discover a Volume Offset Control Service.
 *
 * Attempts to discover a Volume Offset Control Service on a server given the @p param.
 *
 * @param conn  Connection to the peer with the Volume Offset Control Service.
 * @param inst  Pointer to the Volume Offset Control Service client instance.
 * @param param Pointer to the parameters.
 *
 * @return 0 on success, errno on fail.
 */
int bt_vocs_discover(struct bt_conn *conn, struct bt_vocs *inst,
                     const struct bt_vocs_discover_param *param);
int bt_vocs_discover_safe(struct bt_conn *conn, struct bt_vocs *inst,
                          const struct bt_vocs_discover_param *param);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_VOCS_H_ */
