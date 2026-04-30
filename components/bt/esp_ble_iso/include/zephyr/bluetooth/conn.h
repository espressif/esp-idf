/** @file
 *  @brief Bluetooth connection handling
 */

/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_CONN_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_CONN_H_

/**
 * @brief Connection management
 * @defgroup bt_conn Connection management
 * @ingroup bluetooth
 * @{
 */

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/sys/iterable_sections.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Opaque type representing a connection to a remote device */
struct bt_conn;

/** Connection Type */
enum __packed bt_conn_type {
    /** No Connection Type (free slot marker) */
    BT_CONN_TYPE_NONE = 0,
    /** LE Connection Type */
    BT_CONN_TYPE_LE = BIT(0),
    /** ISO Connection Type */
    BT_CONN_TYPE_ISO = BIT(3),
    /** All Connection Type */
    BT_CONN_TYPE_ALL = BT_CONN_TYPE_LE | BT_CONN_TYPE_ISO,
};

/** @brief Increment a connection's reference count.
 *
 *  Increment the reference count of a connection object.
 *
 *  @note Will return NULL if the reference count is zero.
 *
 *  @param conn Connection object.
 *
 *  @return Connection object with incremented reference count, or NULL if the
 *          reference count is zero.
 */
struct bt_conn *bt_conn_ref(struct bt_conn *conn);

/** @brief Decrement a connection's reference count.
 *
 *  Decrement the reference count of a connection object.
 *
 *  @param conn Connection object.
 */
void bt_conn_unref(struct bt_conn *conn);

/** @brief Iterate through all bt_conn objects.
 *
 * Iterates through all bt_conn objects that are alive in the Host allocator.
 *
 * To find established connections, combine this with @ref bt_conn_get_info.
 * Check that @ref bt_conn_info.state is @ref BT_CONN_STATE_CONNECTED.
 *
 * Thread safety: This API is thread safe, but it does not guarantee a
 * sequentially-consistent view for objects allocated during the current
 * invocation of this API. E.g. If preempted while allocations A then B then C
 * happen then results may include A and C but miss B.
 *
 * @param type  Connection Type
 * @param func  Function to call for each connection.
 * @param data  Data to pass to the callback function.
 */
void bt_conn_foreach(enum bt_conn_type type,
                     void (*func)(struct bt_conn *conn, void *data),
                     void *data);

/** @brief Get destination (peer) address of a connection.
 *
 *  @param conn Connection object.
 *
 *  @return Destination address if @p conn is a valid @ref BT_CONN_TYPE_LE connection
 */
const bt_addr_le_t *bt_conn_get_dst(const struct bt_conn *conn);

/** @brief Get array index of a connection
 *
 *  This function is used to map bt_conn to index of an array of
 *  connections. The array has CONFIG_BT_MAX_CONN elements.
 *
 *  @param conn Connection object.
 *
 *  @return Index of the connection object.
 *          The range of the returned value is 0..CONFIG_BT_MAX_CONN-1
 */
uint8_t bt_conn_index(const struct bt_conn *conn);

/** LE Connection Info Structure */
struct bt_conn_le_info {
    /** Destination (Remote) Identity Address or remote Resolvable Private
     *  Address (RPA) before identity has been resolved.
     */
    const bt_addr_le_t *dst;
    uint16_t interval; /**< Connection interval */
};

/** @brief Convert connection interval to milliseconds
 *
 *  Multiply by 1.25 to get milliseconds.
 *
 *  Note that this may be inaccurate, as something like 7.5 ms cannot be
 *  accurately presented with integers.
 */
#define BT_CONN_INTERVAL_TO_MS(interval) ((interval) * 5U / 4U)

/** @brief Convert connection interval to microseconds
 *
 *  Multiply by 1250 to get microseconds.
 */
#define BT_CONN_INTERVAL_TO_US(interval) ((interval) * 1250U)

enum {
    BT_CONN_ROLE_CENTRAL = 0,
    BT_CONN_ROLE_PERIPHERAL = 1,
};

enum bt_conn_state {
    /** Channel disconnected */
    BT_CONN_STATE_DISCONNECTED,
    /** Channel in connecting state */
    BT_CONN_STATE_CONNECTING,
    /** Channel connected and ready for upper layer traffic on it */
    BT_CONN_STATE_CONNECTED,
    /** Channel in disconnecting state */
    BT_CONN_STATE_DISCONNECTING,
};

/** Security level. */
typedef enum __packed {
    /** Level 0: Only for BR/EDR special cases, like SDP */
    BT_SECURITY_L0,
    /** Level 1: No encryption and no authentication. */
    BT_SECURITY_L1,
    /** Level 2: Encryption and no authentication (no MITM). */
    BT_SECURITY_L2,
    /** Level 3: Encryption and authentication (MITM). */
    BT_SECURITY_L3,
    /** Level 4: Authenticated Secure Connections and 128-bit key. */
    BT_SECURITY_L4,
    /** Bit to force new pairing procedure, bit-wise OR with requested
     *  security level.
     */
    BT_SECURITY_FORCE_PAIR = BIT(7),
} bt_security_t;

/** Security Info Flags. */
enum bt_security_flag {
    /** Paired with Secure Connections. */
    BT_SECURITY_FLAG_SC = BIT(0),
    /** Paired with Out of Band method. */
    BT_SECURITY_FLAG_OOB = BIT(1),
};

/** Security Info Structure. */
struct bt_security_info {
    /** Security Level. */
    bt_security_t level;
    /** Encryption Key Size. */
    uint8_t enc_key_size;
    /** Flags. */
    enum bt_security_flag flags;
};

/** Connection Info Structure */
struct bt_conn_info {
    /** Connection Type. */
    enum bt_conn_type type;
    /** Connection Role. */
    uint8_t role;
    /** Which local identity the connection was created with */
    uint8_t id;
    /** LE Connection specific Info. */
    struct bt_conn_le_info le;
    /** Connection state. */
    enum bt_conn_state state;
    /** Security specific info. */
    struct bt_security_info security;
};

/** @brief Get connection info
 *
 *  @param conn Connection object.
 *  @param info Connection info object.
 *
 *  @return Zero on success or (negative) error code on failure.
 */
int bt_conn_get_info(const struct bt_conn *conn, struct bt_conn_info *info);

/** @brief Disconnect from a remote device or cancel pending connection.
 *
 *  Disconnect an active connection with the specified reason code or cancel
 *  pending outgoing connection.
 *
 *  The disconnect reason for a normal disconnect should be:
 *  @ref BT_HCI_ERR_REMOTE_USER_TERM_CONN.
 *
 *  The following disconnect reasons are accepted:
 *   - @ref BT_HCI_ERR_AUTH_FAIL
 *   - @ref BT_HCI_ERR_REMOTE_USER_TERM_CONN
 *   - @ref BT_HCI_ERR_REMOTE_LOW_RESOURCES
 *   - @ref BT_HCI_ERR_REMOTE_POWER_OFF
 *   - @ref BT_HCI_ERR_UNSUPP_REMOTE_FEATURE
 *   - @ref BT_HCI_ERR_PAIRING_NOT_SUPPORTED
 *   - @ref BT_HCI_ERR_UNACCEPT_CONN_PARAM
 *
 *  @param conn Connection to disconnect.
 *  @param reason Reason code for the disconnection.
 *
 *  @return Zero on success or (negative) error code on failure.
 */
int bt_conn_disconnect(struct bt_conn *conn, uint8_t reason);

/** @brief Set security level for a connection.
 *
 *  This function enable security (encryption) for a connection. If the device
 *  has bond information for the peer with sufficiently strong key encryption
 *  will be enabled. If the connection is already encrypted with sufficiently
 *  strong key this function does nothing.
 *
 *  If the device has no bond information for the peer and is not already paired
 *  then the pairing procedure will be initiated. Note that @p sec has no effect
 *  on the security level selected for the pairing process. The selection is
 *  instead controlled by the values of the registered @ref bt_conn_auth_cb. If
 *  the device has bond information or is already paired and the keys are too
 *  weak then the pairing procedure will be initiated.
 *
 *  This function may return an error if the required level of security defined using
 *  @p sec is not possible to achieve due to local or remote device limitation
 *  (e.g., input output capabilities), or if the maximum number of paired devices
 *  has been reached.
 *
 *  This function may return an error if the pairing procedure has already been
 *  initiated by the local device or the peer device.
 *
 *  @note When @kconfig{CONFIG_BT_SMP_SC_ONLY} is enabled then the security
 *        level will always be level 4.
 *
 *  @note When @kconfig{CONFIG_BT_SMP_OOB_LEGACY_PAIR_ONLY} is enabled then the
 *        security level will always be level 3.
 *
 *  @note When @ref BT_SECURITY_FORCE_PAIR within @p sec is enabled then the pairing
 *        procedure will always be initiated.
 *
 *  @param conn Connection object.
 *  @param sec Requested minimum security level.
 *
 *  @return 0 on success or negative error
 */
int bt_conn_set_security(struct bt_conn *conn, bt_security_t sec);

enum bt_security_err {
    /** Security procedure successful. */
    BT_SECURITY_ERR_SUCCESS,

    /** Authentication failed. */
    BT_SECURITY_ERR_AUTH_FAIL,

    /** PIN or encryption key is missing. */
    BT_SECURITY_ERR_PIN_OR_KEY_MISSING,

    /** OOB data is not available.  */
    BT_SECURITY_ERR_OOB_NOT_AVAILABLE,

    /** The requested security level could not be reached. */
    BT_SECURITY_ERR_AUTH_REQUIREMENT,

    /** Pairing is not supported */
    BT_SECURITY_ERR_PAIR_NOT_SUPPORTED,

    /** Pairing is not allowed. */
    BT_SECURITY_ERR_PAIR_NOT_ALLOWED,

    /** Invalid parameters. */
    BT_SECURITY_ERR_INVALID_PARAM,

    /** Distributed Key Rejected */
    BT_SECURITY_ERR_KEY_REJECTED,

    /** Pairing failed but the exact reason could not be specified. */
    BT_SECURITY_ERR_UNSPECIFIED,
};

/** @brief Connection callback structure.
 *
 *  This structure is used for tracking the state of a connection.
 *  It is registered with the help of the bt_conn_cb_register() API.
 *  It's permissible to register multiple instances of this @ref bt_conn_cb
 *  type, in case different modules of an application are interested in
 *  tracking the connection state. If a callback is not of interest for
 *  an instance, it may be set to NULL and will as a consequence not be
 *  used for that instance.
 */
struct bt_conn_cb {
    /** @brief A new connection has been established.
     *
     *  This callback notifies the application of a new connection.
     *  In case the err parameter is non-zero it means that the
     *  connection establishment failed.
     *
     *  @note If the connection was established from an advertising set then
     *        the advertising set cannot be restarted directly from this
     *        callback. Instead use the connected callback of the
     *        advertising set.
     *
     *  @param conn New connection object.
     *  @param err HCI error. Zero for success, non-zero otherwise.
     *
     *  @p err can mean either of the following:
     *  - @ref BT_HCI_ERR_UNKNOWN_CONN_ID Creating the connection started by
     *    @ref bt_conn_le_create was canceled either by the user through
     *    @ref bt_conn_disconnect or by the timeout in the host through
     *    @ref bt_conn_le_create_param timeout parameter, which defaults to
     *    @kconfig{CONFIG_BT_CREATE_CONN_TIMEOUT} seconds.
     *  - @p BT_HCI_ERR_ADV_TIMEOUT High duty cycle directed connectable
     *    advertiser started by @ref bt_le_adv_start failed to be connected
     *    within the timeout.
     */
    void (*connected)(struct bt_conn *conn, uint8_t err);

    /** @brief A connection has been disconnected.
     *
     *  This callback notifies the application that a connection
     *  has been disconnected.
     *
     *  When this callback is called the stack still has one reference to
     *  the connection object. If the application in this callback tries to
     *  start either a connectable advertiser or create a new connection
     *  this might fail because there are no free connection objects
     *  available.
     *  To avoid this issue it is recommended to either start connectable
     *  advertise or create a new connection using @ref k_work_submit or
     *  increase @kconfig{CONFIG_BT_MAX_CONN}.
     *
     *  @param conn Connection object.
     *  @param reason BT_HCI_ERR_* reason for the disconnection.
     */
    void (*disconnected)(struct bt_conn *conn, uint8_t reason);

    /** @brief Remote Identity Address has been resolved.
     *
     *  This callback notifies the application that a remote
     *  Identity Address has been resolved
     *
     *  @param conn Connection object.
     *  @param rpa Resolvable Private Address.
     *  @param identity Identity Address.
     */
    void (*identity_resolved)(struct bt_conn *conn,
                              const bt_addr_le_t *rpa,
                              const bt_addr_le_t *identity);

    /** @brief The security level of a connection has changed.
     *
     *  This callback notifies the application that the security of a
     *  connection has changed.
     *
     *  The security level of the connection can either have been increased
     *  or remain unchanged. An increased security level means that the
     *  pairing procedure has been performed or the bond information from
     *  a previous connection has been applied. If the security level
     *  remains unchanged this means that the encryption key has been
     *  refreshed for the connection.
     *
     *  @param conn Connection object.
     *  @param level New security level of the connection.
     *  @param err Security error. Zero for success, non-zero otherwise.
     */
    void (*security_changed)(struct bt_conn *conn, bt_security_t level,
                             enum bt_security_err err);

    /** @internal Internally used field for list handling */
    sys_snode_t _node;
};

/** @brief Register connection callbacks.
 *
 *  Register callbacks to monitor the state of connections.
 *
 *  @param cb Callback struct. Must point to memory that remains valid.
 *
 * @retval 0 Success.
 * @retval -EEXIST if @p cb was already registered.
 */
int bt_conn_cb_register(struct bt_conn_cb *cb);
int bt_conn_cb_register_safe(struct bt_conn_cb *cb);

/**
 * @brief Unregister connection callbacks.
 *
 * Unregister the state of connections callbacks.
 *
 * @param cb Callback struct point to memory that remains valid.
 *
 * @retval 0 Success
 * @retval -EINVAL If @p cb is NULL
 * @retval -ENOENT if @p cb was not registered
 */
int bt_conn_cb_unregister(struct bt_conn_cb *cb);

/**
 *  @brief Register a callback structure for connection events.
 *
 *  @param _name Name of callback structure.
 */
#define BT_CONN_CB_DEFINE(_name)                    \
    static STRUCT_SECTION_ITERABLE(bt_conn_cb,        \
                        _CONCAT(bt_conn_cb_,    \
                            _name))

/** Authenticated pairing information callback structure */
struct bt_conn_auth_info_cb {
    /** @brief notify that pairing procedure was complete.
     *
     *  This callback notifies the application that the pairing procedure
     *  has been completed.
     *
     *  @param conn Connection object.
     *  @param bonded Bond information has been distributed during the
     *                pairing procedure.
     */
    void (*pairing_complete)(struct bt_conn *conn, bool bonded);

    /** @brief Notify that bond has been deleted.
     *
     *  This callback notifies the application that the bond information
     *  for the remote peer has been deleted
     *
     *  @param id   Which local identity had the bond.
     *  @param peer Remote address.
     */
    void (*bond_deleted)(uint8_t id, const bt_addr_le_t *peer);

    /** Internally used field for list handling */
    sys_snode_t node;
};

/** @brief Register authentication information callbacks.
 *
 *  Register callbacks to get authenticated pairing information. Multiple
 *  registrations can be done.
 *
 *  @param cb Callback struct.
 *
 *  @return Zero on success or negative error code otherwise
 */
int bt_conn_auth_info_cb_register(struct bt_conn_auth_info_cb *cb);

/** @brief Unregister authentication information callbacks.
 *
 *  Unregister callbacks to stop getting authenticated pairing information.
 *
 *  @param cb Callback struct.
 *
 *  @return Zero on success or negative error code otherwise
 */
int bt_conn_auth_info_cb_unregister(struct bt_conn_auth_info_cb *cb);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_CONN_H_ */
