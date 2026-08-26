/**
 * @file
 * @brief Bluetooth subsystem core APIs.
 */

/*
 * SPDX-FileCopyrightText: 2017 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_BLUETOOTH_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_BLUETOOTH_H_

/**
 * @brief Bluetooth APIs
 *
 * @details The Bluetooth Subsystem Core APIs provide essential functionalities
 *          to use and manage Bluetooth based communication. These APIs include
 *          APIs for Bluetooth stack initialization, device discovery,
 *          connection management, data transmission, profiles and services.
 *          These APIs support both classic Bluetooth and Bluetooth Low Energy
 *          (LE) operations.
 *
 * @defgroup bluetooth Bluetooth APIs
 * @ingroup connectivity
 * @{
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/sys/util.h>
#include <zephyr/net_buf.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/crypto.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generic Access Profile (GAP)
 *
 * @details The Generic Access Profile (GAP) defines fundamental Bluetooth
 *          operations, including device discovery, pairing, and connection
 *          management. Zephyr's GAP implementation supports both classic
 *          Bluetooth and Bluetooth Low Energy (LE) functionalities, enabling
 *          roles such as Broadcaster, Observer, Peripheral, and Central. These
 *          roles define the device's behavior in advertising, scanning, and
 *          establishing connections within Bluetooth networks.
 *
 * @defgroup bt_gap Generic Access Profile (GAP)
 * @since 1.0
 * @version 1.0.0
 * @ingroup bluetooth
 * @{
 */

/** Opaque type representing an advertiser. */
struct bt_le_ext_adv;

/** Opaque type representing an periodic advertising sync. */
struct bt_le_per_adv_sync;

/* Don't require everyone to include conn.h */
struct bt_conn;

/* Don't require everyone to include iso.h */
struct bt_iso_biginfo;

/**
 * @brief Bluetooth data.
 *
 * @details Description of different AD Types that can be encoded into advertising data. Used to
 * form arrays that are passed to the @ref bt_le_adv_start function. The @ref BT_DATA define can
 * be used as a helpter to declare the elements of an @ref bt_data array.
 */
struct bt_data {
    /** Type of scan response data or advertisement data. */
    uint8_t type;
    /** Length of scan response data or advertisement data. */
    uint8_t data_len;
    /** Pointer to Scan response or advertisement data. */
    const uint8_t *data;
};

/**
 * @brief Helper to declare elements of bt_data arrays
 *
 * This macro is mainly for creating an array of struct bt_data
 * elements which is then passed to e.g. @ref bt_le_adv_start function.
 *
 * @param _type Type of advertising data field
 * @param _data Pointer to the data field payload
 * @param _data_len Number of octets behind the _data pointer
 */
#define BT_DATA(_type, _data, _data_len) \
    { \
        .type = (_type), \
        .data_len = (_data_len), \
        .data = (const uint8_t *)(_data), \
    }

/**
 * @brief Parameters for starting an extended advertising session.
 *
 * @details This struct provides the parameters to control the behavior of an extended advertising
 * session, including the timeout and the number of advertising events to send. The timeout is
 * specified in units of 10 ms, and the number of events determines how many times the advertising
 * will be sent before stopping. If either the timeout or number of events is reached, the
 * advertising session will be stopped, and the application will be notified via the advertiser sent
 * callback. If both parameters are provided, the advertising session will stop when either limit is
 * reached.
 *
 * @note Used in @ref bt_le_ext_adv_start function.
 */
struct bt_le_ext_adv_start_param {
    /**
     * @brief Maximum advertising set duration (N * 10 ms)
     *
     * The advertising set can be automatically disabled after a
     * certain amount of time has passed since it first appeared on
     * air.
     *
     * Set to zero for no limit. Set in units of 10 ms.
     *
     * When the advertising set is automatically disabled because of
     * this limit, @ref bt_le_ext_adv_cb.sent will be called.
     *
     * When using high duty cycle directed connectable advertising
     * then this parameters must be set to a non-zero value less
     * than or equal to the maximum of
     * @ref BT_GAP_ADV_HIGH_DUTY_CYCLE_MAX_TIMEOUT.
     *
     * If privacy @kconfig{CONFIG_BT_PRIVACY} is enabled then the
     * timeout must be less than @kconfig{CONFIG_BT_RPA_TIMEOUT}.
     *
     * For background information, see parameter "Duration" in
     * Bluetooth Core Specification Version 6.0 Vol. 4 Part E,
     * Section 7.8.56.
     */
    uint16_t timeout;

    /**
     * @brief Maximum number of extended advertising events to be
     * sent
     *
     * The advertiser can be automatically disabled once the whole
     * advertisement (i.e. extended advertising event) has been sent
     * a certain number of times. The number of advertising PDUs
     * sent may be higher and is not relevant.
     *
     * Set to zero for no limit.
     *
     * When the advertising set is automatically disabled because of
     * this limit, @ref bt_le_ext_adv_cb.sent will be called.
     *
     * For background information, see parameter
     * "Max_Extended_Advertising_Events" in Bluetooth Core
     * Specification Version 6.0 Vol. 4 Part E, Section 7.8.56.
     */
    uint8_t  num_events;
};

/**
 * @brief Start advertising with the given advertising set
 *
 * If the advertiser is limited by either the @p param.timeout or @p param.num_events,
 * the application will be notified by the @ref bt_le_ext_adv_cb.sent callback once
 * the limit is reached.
 * If the advertiser is limited by both the timeout and the number of
 * advertising events, then the limit that is reached first will stop the
 * advertiser.
 *
 * @note The advertising set @p adv can be created with @ref bt_le_ext_adv_create.
 *
 * @param adv    Advertising set object.
 * @param param  Advertise start parameters.
 */
int bt_le_ext_adv_start(struct bt_le_ext_adv *adv,
                        const struct bt_le_ext_adv_start_param *param);

/** Advertising states. */
enum bt_le_ext_adv_state {
    /** The advertising set has been created but not enabled. */
    BT_LE_EXT_ADV_STATE_DISABLED,

    /** The advertising set is enabled. */
    BT_LE_EXT_ADV_STATE_ENABLED,
};

/** Periodic Advertising states. */
enum bt_le_per_adv_state {
    /** Not configured for periodic advertising. */
    BT_LE_PER_ADV_STATE_NONE,

    /** The advertising set has been configured for periodic advertising, but is not enabled. */
    BT_LE_PER_ADV_STATE_DISABLED,

    /** Periodic advertising is enabled. */
    BT_LE_PER_ADV_STATE_ENABLED,
};

/** @brief Advertising set info structure. */
struct bt_le_ext_adv_info {
    /** Advertising Set ID */
    uint8_t                    sid;

    /** Current local advertising address used. */
    const bt_addr_le_t         *addr;

    /** Extended advertising state. */
    enum bt_le_ext_adv_state ext_adv_state;

    /** Periodic advertising state. */
    enum bt_le_per_adv_state per_adv_state;
};

/**
 * @brief Get advertising set info
 *
 * @param adv Advertising set object
 * @param info Advertising set info object. The values in this object are only valid on success.
 *
 * @retval 0 Success.
 * @retval -EINVAL @p adv is not valid advertising set or @p info is NULL.
 */
int bt_le_ext_adv_get_info(const struct bt_le_ext_adv *adv,
                           struct bt_le_ext_adv_info *info);

struct bt_le_per_adv_sync_synced_info {
    /** Advertiser LE address and type. */
    const bt_addr_le_t *addr;

    /** Advertising Set Identifier, valid range @ref BT_GAP_SID_MIN to @ref BT_GAP_SID_MAX. */
    uint8_t sid;

    /** Periodic advertising interval (N * 1.25 ms) */
    uint16_t interval;

    /** Advertiser PHY (see @ref bt_gap_le_phy). */
    uint8_t phy;

    /**
     * @brief Peer that transferred the periodic advertising sync
     *
     * Will always be NULL when the sync is locally created.
     *
     */
    struct bt_conn *conn;
};

/**
 * @brief Information about the termination of a periodic advertising sync.
 *
 * @details This struct provides information about the termination of a periodic advertising sync.
 * It includes the advertiser’s address and SID, along with the reason for the sync termination.
 * This information is provided in the callback when the sync is terminated, either due to a
 * local or remote request, or due to missing data (e.g., out of range or lost sync).
 *
 * @note Used in @ref bt_le_per_adv_sync_cb structure.
 */
struct bt_le_per_adv_sync_term_info {
    /** Advertiser LE address and type. */
    const bt_addr_le_t *addr;

    /** Advertising Set Identifier, valid range @ref BT_GAP_SID_MIN to @ref BT_GAP_SID_MAX. */
    uint8_t sid;

    /** Cause of periodic advertising termination (see the BT_HCI_ERR_* values). */
    uint8_t reason;
};

/**
 * @brief Information about a received periodic advertising report.
 *
 * @details This struct holds information about a periodic advertising event that has been received.
 * It contains details such as the advertiser’s address, SID, transmit power, RSSI, CTE type, and
 * additional information depending on the configuration (e.g., event counter and subevent in case
 * of a subevent indication). This information is provided in the callback when periodic advertising
 * data is received.
 *
 * @note Used in @ref bt_le_per_adv_sync_cb structure.
 */
struct bt_le_per_adv_sync_recv_info {
    /** Advertiser LE address and type. */
    const bt_addr_le_t *addr;

    /** Advertising Set Identifier, valid range @ref BT_GAP_SID_MIN to @ref BT_GAP_SID_MAX. */
    uint8_t sid;

    /** The TX power of the advertisement. */
    int8_t tx_power;

    /** The RSSI of the advertisement excluding any CTE. */
    int8_t rssi;
};

/**
 * @brief Callback struct for periodic advertising sync events.
 *
 * @details This struct defines the callback functions that are invoked for various periodic
 * advertising sync events. These include when the sync is successfully established, terminated,
 * when data is received, state changes, BIG info reports, and IQ samples from the periodic
 * advertising.
 *
 * @note Used in @ref bt_le_per_adv_sync_cb_register function.
 */

struct bt_le_per_adv_sync_cb {
    /**
     * @brief The periodic advertising has been successfully synced.
     *
     * This callback notifies the application that the periodic advertising
     * set has been successfully synced, and will now start to
     * receive periodic advertising reports.
     *
     * @param sync The periodic advertising sync object.
     * @param info Information about the sync event.
     */
    void (*synced)(struct bt_le_per_adv_sync *sync,
                   struct bt_le_per_adv_sync_synced_info *info);

    /**
     * @brief The periodic advertising sync has been terminated.
     *
     * This callback notifies the application that the periodic advertising
     * sync has been terminated, either by local request, remote request or
     * because due to missing data, e.g. by being out of range or sync.
     *
     * @param sync  The periodic advertising sync object.
     * @param info  Information about the termination event.
     */
    void (*term)(struct bt_le_per_adv_sync *sync,
                 const struct bt_le_per_adv_sync_term_info *info);

    /**
     * @brief Periodic advertising data received.
     *
     * This callback notifies the application of an periodic advertising
     * report.
     *
     * @param sync  The advertising set object.
     * @param info  Information about the periodic advertising event.
     * @param buf   Buffer containing the periodic advertising data.
     *              NULL if the controller failed to receive a subevent
     *              indication. Only happens if
     *              @kconfig{CONFIG_BT_PER_ADV_SYNC_RSP} is enabled.
     */
    void (*recv)(struct bt_le_per_adv_sync *sync,
                 const struct bt_le_per_adv_sync_recv_info *info,
                 struct net_buf_simple *buf);

    /**
     * @brief BIGInfo advertising report received.
     *
     * This callback notifies the application of a BIGInfo advertising report.
     * This is received if the advertiser is broadcasting isochronous streams in a BIG.
     * See iso.h for more information.
     *
     * @param sync     The advertising set object.
     * @param biginfo  The BIGInfo report.
     */
    void (*biginfo)(struct bt_le_per_adv_sync *sync, const struct bt_iso_biginfo *biginfo);

    sys_snode_t node;
};

/** @brief Periodic advertising set info structure. */
struct bt_le_per_adv_sync_info {
    /** Periodic Advertiser Address */
    bt_addr_le_t addr;

    /** Advertising Set Identifier, valid range @ref BT_GAP_SID_MIN to @ref BT_GAP_SID_MAX. */
    uint8_t sid;

    /** Periodic advertising interval (N * 1.25 ms) */
    uint16_t interval;

    /** Advertiser PHY (see @ref bt_gap_le_phy). */
    uint8_t phy;
};

/**
 * @brief Get periodic adv sync information.
 *
 * @param per_adv_sync Periodic advertising sync object.
 * @param info          Periodic advertising sync info object
 *
 * @return Zero on success or (negative) error code on failure.
 */
int bt_le_per_adv_sync_get_info(struct bt_le_per_adv_sync *per_adv_sync,
                                struct bt_le_per_adv_sync_info *info);

/**
 * @brief Look up an existing periodic advertising sync object by advertiser address.
 *
 * @param adv_addr Advertiser address.
 * @param sid      The periodic advertising set ID.
 *
 * @return Periodic advertising sync object or NULL if not found.
 */
struct bt_le_per_adv_sync *bt_le_per_adv_sync_lookup_addr(const bt_addr_le_t *adv_addr,
                                                          uint8_t sid);

/**
 * @brief Register periodic advertising sync callbacks.
 *
 * Adds the callback structure to the list of callback structures for periodic
 * advertising syncs.
 *
 * This callback will be called for all periodic advertising sync activity,
 * such as synced, terminated and when data is received.
 *
 * @param cb Callback struct. Must point to memory that remains valid.
 *
 * @retval 0 Success.
 * @retval -EEXIST if @p cb was already registered.
 */
int bt_le_per_adv_sync_cb_register(struct bt_le_per_adv_sync_cb *cb);

/** LE scan parameters */
struct bt_le_scan_param {
    /** Scan type. @ref BT_LE_SCAN_TYPE_ACTIVE or @ref BT_LE_SCAN_TYPE_PASSIVE. */
    uint8_t  type;

    /** Bit-field of scanning options. */
    uint8_t options;

    /** Scan interval (N * 0.625 ms).
     *
     * @note When @kconfig{CONFIG_BT_SCAN_AND_INITIATE_IN_PARALLEL} is enabled
     *       and the application wants to scan and connect in parallel,
     *       the Bluetooth Controller may require the scan interval used
     *       for scanning and connection establishment to be equal to
     *       obtain the best performance.
     */
    uint16_t interval;

    /** Scan window (N * 0.625 ms)
     *
     * @note When @kconfig{CONFIG_BT_SCAN_AND_INITIATE_IN_PARALLEL} is enabled
     *       and the application wants to scan and connect in parallel,
     *       the Bluetooth Controller may require the scan window used
     *       for scanning and connection establishment to be equal to
     *       obtain the best performance.
     */
    uint16_t window;

    /**
     * @brief Scan timeout (N * 10 ms)
     *
     * Application will be notified by the scan timeout callback.
     * Set zero to disable timeout.
     */
    uint16_t timeout;

    /**
     * @brief Scan interval LE Coded PHY (N * 0.625 MS)
     *
     * Set zero to use same as LE 1M PHY scan interval.
     */
    uint16_t interval_coded;

    /**
     * @brief Scan window LE Coded PHY (N * 0.625 MS)
     *
     * Set zero to use same as LE 1M PHY scan window.
     */
    uint16_t window_coded;
};

/** LE advertisement and scan response packet information */
struct bt_le_scan_recv_info {
    /**
     * @brief Advertiser LE address and type.
     *
     * If advertiser is anonymous then this address will be
     * @ref BT_ADDR_LE_ANY.
     */
    const bt_addr_le_t *addr;

    /** Advertising Set Identifier, valid range @ref BT_GAP_SID_MIN to @ref BT_GAP_SID_MAX. */
    uint8_t sid;

    /** Strength of advertiser signal. */
    int8_t rssi;

    /** Transmit power of the advertiser. */
    int8_t tx_power;

    /**
     * @brief Advertising packet type.
     *
     * Uses the @ref bt_gap_adv_type value.
     *
     * May indicate that this is a scan response if the type is
     * @ref BT_GAP_ADV_TYPE_SCAN_RSP.
     */
    uint8_t adv_type;

    /**
     * @brief Advertising packet properties bitfield.
     *
     * Uses the @ref bt_gap_adv_prop values.
     * May indicate that this is a scan response if the value contains the
     * @ref BT_GAP_ADV_PROP_SCAN_RESPONSE bit.
     *
     */
    uint16_t adv_props;

    /**
     * @brief Periodic advertising interval (N * 1.25 ms).
     *
     * If 0 there is no periodic advertising.
     */
    uint16_t interval;

    /** Primary advertising channel PHY. */
    uint8_t primary_phy;

    /** Secondary advertising channel PHY. */
    uint8_t secondary_phy;
};

/** Listener context for (LE) scanning. */
struct bt_le_scan_cb {

    /**
     * @brief Advertisement packet and scan response received callback.
     *
     * @param info Advertiser packet and scan response information.
     * @param buf  Buffer containing advertiser data.
     */
    void (*recv)(const struct bt_le_scan_recv_info *info,
                 struct net_buf_simple *buf);

    sys_snode_t node;
};

/**
 * @brief Start (LE) scanning
 *
 * Start LE scanning with given parameters and provide results through
 * the specified callback.
 *
 * @note The LE scanner by default does not use the Identity Address of the
 *       local device when @kconfig{CONFIG_BT_PRIVACY} is disabled. This is to
 *       prevent the active scanner from disclosing the identity address information
 *       when requesting additional information from advertisers.
 *       In order to enable directed advertiser reports then
 *       @kconfig{CONFIG_BT_SCAN_WITH_IDENTITY} must be enabled.
 *
 * @note Setting the `param.timeout` parameter is not supported when
 *       @kconfig{CONFIG_BT_PRIVACY} is enabled, when the param.type is @ref
 *       BT_LE_SCAN_TYPE_ACTIVE. Supplying a non-zero timeout will result in an
 *       -EINVAL error code.
 *
 * @param param Scan parameters.
 * @param cb Callback to notify scan results. May be NULL if callback
 *           registration through @ref bt_le_scan_cb_register is preferred.
 *
 * @return Zero on success or error code otherwise, positive in case of
 *         protocol error or negative (POSIX) in case of stack internal error.
 * @retval -EBUSY if the scanner is already being started in a different thread.
 */
int bt_le_scan_start(const struct bt_le_scan_param *param, void *cb);

/**
 * @brief Stop (LE) scanning.
 *
 * Stops ongoing LE scanning.
 *
 * @return Zero on success or error code otherwise, positive in case of
 *         protocol error or negative (POSIX) in case of stack internal error.
 */
int bt_le_scan_stop(void);

/**
 * @brief Register scanner packet callbacks.
 *
 * Adds the callback structure to the list of callback structures that monitors
 * scanner activity.
 *
 * This callback will be called for all scanner activity.
 *
 * @param cb Callback struct. Must point to memory that remains valid.
 *
 * @retval 0 Success.
 * @retval -EEXIST if @p cb was already registered.
 */
int bt_le_scan_cb_register(struct bt_le_scan_cb *cb);

/**
 * @brief Unregister scanner packet callbacks.
 *
 * Remove the callback structure from the list of scanner callbacks.
 *
 * @param cb Callback struct. Must point to memory that remains valid.
 */
void bt_le_scan_cb_unregister(struct bt_le_scan_cb *cb);

/** Information about a bond with a remote device. */
struct bt_bond_info {
    /** Address of the remote device. */
    bt_addr_le_t addr;
};

/**
 * @brief Iterate through all existing bonds.
 *
 * @param id         Local identity handle (typically @ref BT_ID_DEFAULT). Corresponds to the
 *                   identity address used in iteration.
 * @param func       Function to call for each bond.
 * @param user_data  Data to pass to the callback function.
 */
void bt_foreach_bond(uint8_t id, void (*func)(const struct bt_bond_info *info,
                                              void *user_data),
                     void *user_data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_BLUETOOTH_H_ */
