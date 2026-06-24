/**
 * @file
 * @brief Bluetooth Audio Input Control Service APIs.
 */

/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_AICS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_AICS_H_

/**
 * @brief Audio Input Control Service (AICS)
 *
 * @defgroup bt_aics Audio Input Control Service (AICS)
 *
 * @since 2.6
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Audio Input Control Service is a secondary service, and as such should not be used on its
 * own, but rather in the context of another (primary) service.
 *
 * This API implements both the server and client functionality.
 * Note that the API abstracts away the change counter in the audio input control state and will
 * automatically handle any changes to that. If out of date, the client implementation will
 * autonomously read the change counter value when executing a write request.
 *
 */
#include <stdint.h>
#include <stdbool.h>

#include <zephyr/autoconf.h>
#include <zephyr/bluetooth/bluetooth.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Audio Input Control Service mute states
 * @{
 */
/** The mute state is unmuted */
#define BT_AICS_STATE_UNMUTED                      0x00
/** The mute state is muted */
#define BT_AICS_STATE_MUTED                        0x01
/** The mute state is disabled */
#define BT_AICS_STATE_MUTE_DISABLED                0x02
/** @} */

/**
 * @name Audio Input Control Service input modes
 * @{
 */
/**
 * @brief The gain mode is manual only and cannot be changed to automatic.
 *
 * The gain can be controlled by the client.
 */
#define BT_AICS_MODE_MANUAL_ONLY                   0x00
/**
 * @brief The gain mode is automatic only and cannot be changed to manual.
 *
 * The gain cannot be controlled by the client.
 */
#define BT_AICS_MODE_AUTO_ONLY                     0x01
/**
 * @brief The gain mode is manual.
 *
 * The gain can be controlled by the client.
 */
#define BT_AICS_MODE_MANUAL                        0x02
/**
 * @brief The gain mode is automatic.
 *
 * The gain cannot be controlled by the client.
 */
#define BT_AICS_MODE_AUTO                          0x03
/** @} */

/**
 * @name Audio Input Control Service input types
 * @{
 */
/** The input is unspecified */
#define BT_AICS_INPUT_TYPE_UNSPECIFIED             0x00
/** The input is a Bluetooth Audio Stream */
#define BT_AICS_INPUT_TYPE_BLUETOOTH               0x01
/** The input is a microphone */
#define BT_AICS_INPUT_TYPE_MICROPHONE              0x02
/** The input is analog */
#define BT_AICS_INPUT_TYPE_ANALOG                  0x03
/** The input is digital */
#define BT_AICS_INPUT_TYPE_DIGITAL                 0x04
/** The input is a radio (AM/FM/XM/etc.) */
#define BT_AICS_INPUT_TYPE_RADIO                   0x05
/** The input is a Streaming Audio Source */
#define BT_AICS_INPUT_TYPE_STREAMING               0x06
/** The input is transparent / pass-through */
#define BT_AICS_INPUT_TYPE_AMBIENT                 0x07
/** @} */

/**
 * @name Audio Input Control Service Error codes
 * @{
 */
/**
 * The Change_Counter operand value does not match the Change_Counter field value of the
 * Audio Input State characteristic.
 */
#define BT_AICS_ERR_INVALID_COUNTER                0x80
/** An invalid opcode has been used in a control point procedure */
#define BT_AICS_ERR_OP_NOT_SUPPORTED               0x81
/** Mute/unmute commands are disabled.(see @ref BT_AICS_STATE_MUTE_DISABLED) */
#define BT_AICS_ERR_MUTE_DISABLED                  0x82
/** An operand value used in a control point procedure is outside the permissible range */
#define BT_AICS_ERR_OUT_OF_RANGE                   0x83
/** A requested gain mode change is not allowed */
#define BT_AICS_ERR_GAIN_MODE_NOT_ALLOWED          0x84
/** @} */

/** @brief Opaque Audio Input Control Service instance. */
struct bt_aics;

/** @brief Structure for initializing a Audio Input Control Service instance. */
struct bt_aics_register_param {
    /** Initial audio input gain (-128 to 127) */
    int8_t gain;

    /** Initial audio input mute state */
    uint8_t mute;

    /** Initial audio input mode */
    uint8_t gain_mode;

    /** Initial audio input gain units (N * 0.1 dB) */
    uint8_t units;

    /** Initial audio input minimum gain */
    int8_t min_gain;

    /** Initial audio input maximum gain */
    int8_t max_gain;

    /** Initial audio input type */
    uint8_t type;

    /** Initial audio input status (active/inactive) */
    bool status;

    /** Boolean to set whether the description is writable by clients */
    bool desc_writable;

    /** Initial audio input description */
    char *description;

    /** Pointer to the callback structure. */
    struct bt_aics_cb *cb;
};

/** @brief Structure for discovering a Audio Input Control Service instance. */
struct bt_aics_discover_param {
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
 * @brief Get a free instance of Audio Input Control Service from the pool.
 *
 * @return Audio Input Control Service instance in case of success or NULL in case of error.
 */
struct bt_aics *bt_aics_free_instance_get_safe(void);

/**
 * @brief Get the service declaration attribute.
 *
 * The first service attribute returned can be included in any other GATT service.
 *
 * @param aics Audio Input Control Service instance.
 *
 * @return Pointer to the attributes of the service.
 */
void *bt_aics_svc_decl_get_safe(struct bt_aics *aics);

/**
 * @brief Get the connection pointer of a client instance
 *
 * Get the Bluetooth connection pointer of a Audio Input Control Service
 * client instance.
 *
 * @param aics    Audio Input Control Service client instance pointer.
 * @param conn    Connection pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_aics_client_conn_get(const struct bt_aics *aics, struct bt_conn **conn);

/**
 * @brief Initialize the Audio Input Control Service instance.
 *
 * @param aics      Audio Input Control Service instance.
 * @param param     Audio Input Control Service register parameters.
 *
 * @return 0 if success, errno on failure.
 */
int bt_aics_register_safe(struct bt_aics *aics, struct bt_aics_register_param *param);

/**
 * @brief Callback function for writes.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 */
typedef void (*bt_aics_write_cb)(struct bt_aics *inst, int err);

/**
 * @brief Callback function for the input state.
 *
 * Called when the value is read,
 * or if the value is changed by either the server or client.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 * @param gain         The gain setting value.
 * @param mute         The mute value.
 * @param mode         The mode value.
 */
typedef void (*bt_aics_state_cb)(struct bt_aics *inst, int err, int8_t gain,
                                 uint8_t mute, uint8_t mode);

/**
 * @brief Callback function for the gain settings.
 *
 * Called when the value is read,
 * or if the value is changed by either the server or client.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 * @param units        The value that reflect the size of a single increment or decrement of the
 *                     Gain Setting value in 0.1 decibel units.
 * @param minimum      The minimum gain allowed for the gain setting.
 * @param maximum      The maximum gain allowed for the gain setting.
 */
typedef void (*bt_aics_gain_setting_cb)(struct bt_aics *inst, int err,
                                        uint8_t units, int8_t minimum,
                                        int8_t maximum);

/**
 * @brief Callback function for the input type.
 *
 * Called when the value is read, or if the value is changed by either the server or client.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 * @param type   The input type.
 */
typedef void (*bt_aics_type_cb)(struct bt_aics *inst, int err, uint8_t type);

/**
 * @brief Callback function for the input status.
 *
 * Called when the value is read, or if the value is changed by either the server or client.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 * @param active       Whether the instance is active or inactive.
 */
typedef void (*bt_aics_status_cb)(struct bt_aics *inst, int err, bool active);

/**
 * @brief Callback function for the description.
 *
 * Called when the value is read, or if the value is changed by either the server or client.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 * @param description  The description as an UTF-8 encoded string (may have been clipped).
 */
typedef void (*bt_aics_description_cb)(struct bt_aics *inst, int err,
                                       char *description);

/**
 * @brief Callback function for bt_aics_discover.
 *
 * This callback will usually be overwritten by the primary service that
 * includes the Audio Input Control Service client.
 *
 * @param inst         The instance pointer.
 * @param err          Error value. 0 on success, GATT error on positive value
 *                     or errno on negative value.
 *                     For notifications, this will always be 0.
 */
typedef void (*bt_aics_discover_cb)(struct bt_aics *inst, int err);

/**
 * @brief Struct to hold callbacks for the Audio Input Control Service.
 *
 * Used by both clients and servers
 */
struct bt_aics_cb {
    /** The audio input state has changed */
    bt_aics_state_cb                 state;
    /** The gain setting has changed */
    bt_aics_gain_setting_cb          gain_setting;
    /** The audio input type has changed */
    bt_aics_type_cb                  type;
    /** The audio input status has changed */
    bt_aics_status_cb                status;
    /** The audio input description has changed */
    bt_aics_description_cb           description;

    /** The discovery has completed */
    bt_aics_discover_cb              discover;
    /** The set gain operation has completed */
    bt_aics_write_cb                 set_gain;
    /** The unmute operation has completed */
    bt_aics_write_cb                 unmute;
    /** The mut operation has completed */
    bt_aics_write_cb                 mute;
    /** The set manual mode operation has completed */
    bt_aics_write_cb                 set_manual_mode;
    /** The set automatic mode has completed */
    bt_aics_write_cb                 set_auto_mode;
};

/**
 * @brief Discover a Audio Input Control Service.
 *
 * Attempts to discover a Audio Input Control Service on a server given the
 * @p param.
 *
 * @param conn      Connection to the peer with the Audio Input Control Service.
 * @param inst      The instance pointer.
 * @param param     Pointer to the parameters.
 *
 * @return 0 on success, errno on fail.
 */
int bt_aics_discover(struct bt_conn *conn, struct bt_aics *inst,
                     const struct bt_aics_discover_param *param);
int bt_aics_discover_safe(struct bt_conn *conn, struct bt_aics *inst,
                          const struct bt_aics_discover_param *param);

/**
 * @brief Deactivates a Audio Input Control Service instance.
 *
 * Audio Input Control Services are activated by default, but this will allow
 * the server to deactivate an Audio Input Control Service.
 *
 * @param inst         The instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_aics_deactivate_safe(struct bt_aics *inst);

/**
 * @brief Activates a Audio Input Control Service instance.
 *
 * Audio Input Control Services are activated by default, but this will allow
 * the server reactivate a Audio Input Control Service instance after it has
 * been deactivated with @ref bt_aics_deactivate.
 *
 * @param inst         The instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_aics_activate_safe(struct bt_aics *inst);

/**
 * @brief Read the Audio Input Control Service input state.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_state_get_safe(struct bt_aics *inst);

/**
 * @brief Read the Audio Input Control Service gain settings.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_gain_setting_get_safe(struct bt_aics *inst);

/**
 * @brief Read the Audio Input Control Service input type.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_type_get_safe(struct bt_aics *inst);

/**
 * @brief Read the Audio Input Control Service input status.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_status_get_safe(struct bt_aics *inst);

/**
 * @brief Disable mute in the Audio Input Control Service.
 *
 * Calling bt_aics_unmute() or bt_aics_mute() will enable
 * mute again and set the mute state to either unmuted or muted.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, errno value on fail.
 */
int bt_aics_disable_mute_safe(struct bt_aics *inst);

/**
 * @brief Unmute the Audio Input Control Service input.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_unmute_safe(struct bt_aics *inst);

/**
 * @brief Mute the Audio Input Control Service input.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_mute_safe(struct bt_aics *inst);

/**
 * @brief Set manual only gain mode in Audio Input Control Service.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, errno value on fail.
 */
int bt_aics_gain_set_manual_only_safe(struct bt_aics *inst);

/**
 * @brief Set automatic only gain mode in Audio Input Control Service.
 *
 * Using this function and enabling automatic only gain disables
 * setting the gain with bt_aics_gain_set
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, errno value on fail.
 */
int bt_aics_gain_set_auto_only_safe(struct bt_aics *inst);

/**
 * @brief Set input gain to manual.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_manual_gain_set_safe(struct bt_aics *inst);

/**
 * @brief Set the input gain to automatic.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_automatic_gain_set_safe(struct bt_aics *inst);

/**
 * @brief Set the input gain.
 *
 * @param inst          The instance pointer.
 * @param gain          The gain to set (-128 to 127) in gain setting units
 *                      (see @ref bt_aics_gain_setting_cb).
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_gain_set_safe(struct bt_aics *inst, int8_t gain);

/**
 * @brief Read the Audio Input Control Service description.
 *
 * @param inst          The instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_description_get_safe(struct bt_aics *inst);

/**
 * @brief Set the Audio Input Control Service description.
 *
 * @param inst          The instance pointer.
 * @param description   The description as an UTF-8 encoded string.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_aics_description_set_safe(struct bt_aics *inst, const char *description);

/**
 * @brief Get a new Audio Input Control Service client instance.
 *
 * @return Pointer to the instance, or NULL if no free instances are left.
 */
struct bt_aics *bt_aics_client_free_instance_get_safe(void);

/**
 * @brief Registers the callbacks for the Audio Input Control Service client.
 *
 * @param inst      The instance pointer.
 * @param cb        Pointer to the callback structure.
 */
void bt_aics_client_cb_register_safe(struct bt_aics *inst, struct bt_aics_cb *cb);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_AICS_H_ */
