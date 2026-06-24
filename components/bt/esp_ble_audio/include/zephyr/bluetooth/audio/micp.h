/**
 * @file
 * @brief Bluetooth Microphone Control Profile (MICP) APIs.
 */

/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MICP_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_MICP_H_

/**
 * @brief Microphone Control Profile (MICP)
 *
 * @defgroup bt_micp Microphone Control Profile (MICP)
 *
 * @since 2.7
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 */

#include <stdint.h>

#include <zephyr/bluetooth/audio/aics.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/slist.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Application error codes
 * @{
 */
/** Mute/unmute commands are disabled. */
#define BT_MICP_ERR_MUTE_DISABLED                  0x80
/** @} */

/**
 * @name Microphone Control Profile mute states
 * @{
 */
/** The microphone state is unmuted */
#define BT_MICP_MUTE_UNMUTED                       0x00
/** The microphone state is muted */
#define BT_MICP_MUTE_MUTED                         0x01
/** The microphone state is disabled and cannot be muted or unmuted */
#define BT_MICP_MUTE_DISABLED                      0x02
/** @} */

/** @brief Opaque Microphone Controller instance. */
struct bt_micp_mic_ctlr;

/** @brief Register parameters structure for Microphone Control Service */
struct bt_micp_mic_dev_register_param {
    /** Register parameter structure for Audio Input Control Services */
    struct bt_aics_register_param *aics_param;

    /** Microphone Control Profile callback structure. */
    struct bt_micp_mic_dev_cb *cb;
};

/**
 * @brief Microphone Control Profile included services
 *
 * Used for to represent the Microphone Control Profile included service
 * instances, for either a Microphone Controller or a Microphone Device.
 * The instance pointers either represent local service instances,
 * or remote service instances.
 */
struct bt_micp_included {
    /** Number of Audio Input Control Service instances */
    uint8_t aics_cnt;
    /** Array of pointers to Audio Input Control Service instances */
    struct bt_aics **aics;
};

/**
 * @brief Initialize the Microphone Control Profile Microphone Device
 *
 * This will enable the Microphone Control Service instance and make it
 * discoverable by Microphone Controllers.
 *
 * @param param Pointer to an initialization structure.
 *
 * @return 0 if success, errno on failure.
 */
int bt_micp_mic_dev_register_safe(struct bt_micp_mic_dev_register_param *param);

/**
 * @brief Get Microphone Device included services
 *
 * Returns a pointer to a struct that contains information about the
 * Microphone Device included Audio Input Control Service instances.
 *
 * Requires that @kconfig{CONFIG_BT_MICP_MIC_DEV_AICS} is enabled.
 *
 * @param included Pointer to store the result in.
 *
 * @return 0 if success, errno on failure.
 */
int bt_micp_mic_dev_included_get_safe(struct bt_micp_included *included);

/**
 * @brief Struct to hold the Microphone Device callbacks
 *
 * These can be registered for usage with bt_micp_mic_dev_register().
 */
struct bt_micp_mic_dev_cb {
    /**
     * @brief Callback function for Microphone Device mute.
     *
     * Called when the value is read with bt_micp_mic_dev_mute_get(),
     * or if the value is changed by either the Microphone Device or a
     * Microphone Controller.
     *
     * @param mute     The mute setting of the Microphone Control Service.
     */
    void (*mute)(uint8_t mute);
};

/**
 * @brief Unmute the Microphone Device.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_micp_mic_dev_unmute_safe(void);

/**
 * @brief Mute the Microphone Device.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_micp_mic_dev_mute_safe(void);

/**
 * @brief Disable the mute functionality on the Microphone Device.
 *
 * Can be re-enabled by called @ref bt_micp_mic_dev_mute or @ref bt_micp_mic_dev_unmute.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_micp_mic_dev_mute_disable_safe(void);

/**
 * @brief Read the mute state on the Microphone Device.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_micp_mic_dev_mute_get_safe(void);

/**
 * @brief Struct to hold the Microphone Controller callbacks
 *
 * These can be registered for usage with bt_micp_mic_ctlr_cb_register().
 */
struct bt_micp_mic_ctlr_cb {
    /**
     * @brief Callback function for Microphone Control Profile mute.
     *
     * Called when the value is read,
     * or if the value is changed by either the Microphone Device or a
     * Microphone Controller.
     *
     * @param mic_ctlr Microphone Controller instance pointer.
     * @param err      Error value. 0 on success, GATT error or errno on fail.
     *                 For notifications, this will always be 0.
     * @param mute     The mute setting of the Microphone Control Service.
     */
    void (*mute)(struct bt_micp_mic_ctlr *mic_ctlr, int err, uint8_t mute);

    /**
     * @brief Callback function for bt_micp_mic_ctlr_discover().
     *
     * @param mic_ctlr     Microphone Controller instance pointer.
     * @param err          Error value. 0 on success, GATT error or errno on fail.
     * @param aics_count   Number of Audio Input Control Service instances on
     *                     peer device.
     */
    void (*discover)(struct bt_micp_mic_ctlr *mic_ctlr, int err,
                     uint8_t aics_count);

    /**
     * @brief Callback function for Microphone Control Profile mute/unmute.
     *
     * @param mic_ctlr  Microphone Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error or errno on fail.
     */
    void (*mute_written)(struct bt_micp_mic_ctlr *mic_ctlr, int err);

    /**
     * @brief Callback function for Microphone Control Profile mute/unmute.
     *
     * @param mic_ctlr  Microphone Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error or errno on fail.
     */
    void (*unmute_written)(struct bt_micp_mic_ctlr *mic_ctlr, int err);

    /** Audio Input Control Service client callback */
    struct bt_aics_cb               aics_cb;

    /** @cond INTERNAL_HIDDEN */
    /** Internally used field for list handling */
    sys_snode_t _node;
    /** @endcond */
};

/**
 * @brief Get Microphone Control Profile included services
 *
 * Returns a pointer to a struct that contains information about the
 * Microphone Control Profile included services instances, such as
 * pointers to the Audio Input Control Service instances.
 *
 * Requires that @kconfig{CONFIG_BT_MICP_MIC_CTLR_AICS} is enabled.
 *
 * @param      mic_ctlr Microphone Controller instance pointer.
 * @param[out] included Pointer to store the result in.
 *
 * @return 0 if success, errno on failure.
 */
int bt_micp_mic_ctlr_included_get_safe(struct bt_micp_mic_ctlr *mic_ctlr,
                                       struct bt_micp_included *included);

/**
 * @brief Get the connection pointer of a Microphone Controller instance
 *
 * Get the Bluetooth connection pointer of a Microphone Controller instance.
 *
 * @param mic_ctlr    Microphone Controller instance pointer.
 * @param conn        Connection pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_micp_mic_ctlr_conn_get_safe(const struct bt_micp_mic_ctlr *mic_ctlr,
                                   struct bt_conn **conn);

/**
 * @brief Get the volume controller from a connection pointer
 *
 * Get the Volume Control Profile Volume Controller pointer from a connection pointer.
 * Only volume controllers that have been initiated via bt_micp_mic_ctlr_discover() can be
 * retrieved.
 *
 * @param conn     Connection pointer.
 *
 * @retval Pointer to a Microphone Control Profile Microphone Controller instance
 * @retval NULL if @p conn is NULL or if the connection has not done discovery yet
 */
struct bt_micp_mic_ctlr *bt_micp_mic_ctlr_get_by_conn(const struct bt_conn *conn);

/**
 * @brief Discover Microphone Control Service
 *
 * This will start a GATT discovery and setup handles and subscriptions.
 * This shall be called once before any other actions can be executed for the
 * peer device, and the @ref bt_micp_mic_ctlr_cb.discover callback will notify
 * when it is possible to start remote operations.
 * *
 * @param conn          The connection to initialize the profile for.
 * @param[out] mic_ctlr Valid remote instance object on success.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_micp_mic_ctlr_discover(struct bt_conn *conn,
                              struct bt_micp_mic_ctlr **mic_ctlr);
int bt_micp_mic_ctlr_discover_safe(struct bt_conn *conn,
                                   struct bt_micp_mic_ctlr **mic_ctlr);

/**
 * @brief Unmute a remote Microphone Device.
 *
 * @param mic_ctlr  Microphone Controller instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_micp_mic_ctlr_unmute_safe(struct bt_micp_mic_ctlr *mic_ctlr);

/**
 * @brief Mute a remote Microphone Device.
 *
 * @param mic_ctlr  Microphone Controller instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_micp_mic_ctlr_mute_safe(struct bt_micp_mic_ctlr *mic_ctlr);

/**
 * @brief Read the mute state of a remote Microphone Device.
 *
 * @param mic_ctlr  Microphone Controller instance pointer.
 *
 * @return 0 on success, GATT error value on fail.
 */
int bt_micp_mic_ctlr_mute_get_safe(struct bt_micp_mic_ctlr *mic_ctlr);

/**
 * @brief Registers the callbacks used by Microphone Controller.
 *
 * This can only be done as the client.
 *
 * @param cb    The callback structure.
 *
 * @return 0 if success, errno on failure.
 */
int bt_micp_mic_ctlr_cb_register_safe(struct bt_micp_mic_ctlr_cb *cb);
#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MICP_H_ */
