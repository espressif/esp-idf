/**
 * @file
 * @brief Bluetooth Volume Control Profile (VCP) APIs.
 */

/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_VCP_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_VCP_H_

/**
 * @brief Volume Control Profile (VCP)
 *
 * @defgroup bt_vcp Volume Control Profile (VCP)
 *
 * @since 2.7
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Volume Control Profile (VCP) provides procedures to control the volume level and mute state
 * on audio devices.
 */

#include <stdint.h>

#include <zephyr/bluetooth/audio/aics.h>
#include <zephyr/bluetooth/audio/vocs.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/slist.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Volume Control Service Error codes
 * @{
 */
/**
 * The Change_Counter operand value does not match the Change_Counter field value of the Volume
 * State characteristic.
 */
#define BT_VCP_ERR_INVALID_COUNTER             0x80
/** An invalid opcode has been used in a control point procedure. */
#define BT_VCP_ERR_OP_NOT_SUPPORTED            0x81
/** @} */

/**
 * @name Volume Control Service Mute Values
 * @{
 */
/** The volume state is unmuted */
#define BT_VCP_STATE_UNMUTED                   0x00
/** The volume state is muted */
#define BT_VCP_STATE_MUTED                     0x01
/** @} */

/** @brief Opaque Volume Control Service instance. */
struct bt_vcp_vol_ctlr;

/** Register structure for Volume Control Service */
struct bt_vcp_vol_rend_register_param {
    /** Initial step size (1-255) */
    uint8_t step;

    /** Initial mute state (0-1) */
    uint8_t mute;

    /** Initial volume level (0-255) */
    uint8_t volume;

    /** Register parameters for Volume Offset Control Services */
    struct bt_vocs_register_param *vocs_param;

    /** Register parameters  for Audio Input Control Services */
    struct bt_aics_register_param *aics_param;

    /** Volume Control Service callback structure. */
    struct bt_vcp_vol_rend_cb *cb;
};

/**
 * @brief Volume Control Service included services
 *
 * Used for to represent the Volume Control Service included service instances,
 * for either a client or a server. The instance pointers either represent
 * local server instances, or remote service instances.
 */
struct bt_vcp_included {
    /** Number of Volume Offset Control Service instances */
    uint8_t vocs_cnt;
    /** Array of pointers to Volume Offset Control Service instances */
    struct bt_vocs **vocs;

    /** Number of Audio Input Control Service instances */
    uint8_t aics_cnt;
    /** Array of pointers to Audio Input Control Service instances */
    struct bt_aics **aics;
};

/**
 * @brief Get Volume Control Service included services.
 *
 * Returns a pointer to a struct that contains information about the
 * Volume Control Service included service instances, such as pointers to the
 * Volume Offset Control Service (Volume Offset Control Service) or
 * Audio Input Control Service (AICS) instances.
 *
 * @param[out] included Pointer to store the result in.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_included_get_safe(struct bt_vcp_included *included);

/**
 * @brief Register the Volume Control Service.
 *
 * This will register and enable the service and make it discoverable by
 * clients.
 *
 * @param      param  Volume Control Service register parameters.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_register_safe(struct bt_vcp_vol_rend_register_param *param);

/**
 * @brief Struct to hold the Volume Renderer callbacks
 *
 * These can be registered for usage with bt_vcp_vol_rend_register().
 */
struct bt_vcp_vol_rend_cb {
    /**
     * @brief Callback function for Volume Control Service volume state.
     *
     * Called when the value is locally read with
     * bt_vcp_vol_rend_get_state(), or if the state is changed by either
     * the Volume Renderer or a remote Volume Controller.
     *
     * @param conn    Pointer to the connection to a remote device if
     *                the change was caused by it, otherwise NULL.
     * @param err     Error value. 0 on success, GATT error on positive value
     *                or errno on negative value.
     * @param volume  The volume of the Volume Control Service server.
     * @param mute    The mute setting of the Volume Control Service server.
     */
    void (*state)(struct bt_conn *conn, int err, uint8_t volume, uint8_t mute);

    /**
     * @brief Callback function for Volume Control Service flags.
     *
     * Called when the value is locally read as the server.
     * Called when the value is remotely read as the client.
     * Called if the value is changed by either the server or client.
     *
     * @param conn    Pointer to the connection to a remote device if
     *                the change was caused by it, otherwise NULL.
     * @param err     Error value. 0 on success, GATT error on positive value
     *                or errno on negative value.
     * @param flags   The flags of the Volume Control Service server.
     */
    void (*flags)(struct bt_conn *conn, int err, uint8_t flags);
};

/**
 * @brief Set the Volume Control Service volume step size.
 *
 * Set the value that the volume changes, when changed relatively with e.g.
 * @ref bt_vcp_vol_rend_vol_down or @ref bt_vcp_vol_rend_vol_up.
 *
 * This can only be done as the server.
 *
 * @param volume_step  The volume step size (1-255).
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_set_step_safe(uint8_t volume_step);

/**
 * @brief Get the Volume Control Service volume state.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_get_state_safe(void);

/**
 * @brief Get the Volume Control Service flags.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_get_flags_safe(void);

/**
 * @brief Turn the volume down by one step on the server.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_vol_down_safe(void);

/**
 * @brief Turn the volume up by one step on the server.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_vol_up_safe(void);

/**
 * @brief Turn the volume down and unmute the server.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_unmute_vol_down_safe(void);

/**
 * @brief Turn the volume up and unmute the server.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_unmute_vol_up_safe(void);

/**
 * @brief Set the volume on the server
 *
 * @param volume The absolute volume to set.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_set_vol_safe(uint8_t volume);

/**
 * @brief Unmute the server.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_unmute_safe(void);

/**
 * @brief Mute the server.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_rend_mute_safe(void);

/**
 * @brief Struct to hold the Volume Controller callbacks
 *
 * These can be registered for usage with bt_vcp_vol_ctlr_cb_register().
 */
struct bt_vcp_vol_ctlr_cb {
    /**
     * @brief Callback function for Volume Control Profile volume state.
     *
     * Called when the value is remotely read as the Volume Controller.
     * Called if the value is changed by either the Volume Renderer or
     * Volume Controller, and notified to the to Volume Controller.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     * @param volume    The volume of the Volume Renderer.
     * @param mute      The mute setting of the Volume Renderer.
     */
    void (*state)(struct bt_vcp_vol_ctlr *vol_ctlr, int err, uint8_t volume,
                  uint8_t mute);

    /**
     * @brief Callback function for Volume Control Profile volume flags.
     *
     * Called when the value is remotely read as the Volume Controller.
     * Called if the value is changed by the Volume Renderer.
     *
     * A non-zero value indicates the volume has been changed on the
     * Volume Renderer since it was booted.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     * @param flags     The flags of the Volume Renderer.
     */

    void (*flags)(struct bt_vcp_vol_ctlr *vol_ctlr, int err, uint8_t flags);

    /**
     * @brief Callback function for bt_vcp_vol_ctlr_discover().
     *
     * This callback is called once the discovery procedure is completed.
     *
     * @param vol_ctlr     Volume Controller instance pointer.
     * @param err          Error value. 0 on success, GATT error on positive
     *                     value or errno on negative value.
     * @param vocs_count   Number of Volume Offset Control Service instances
     *                     on the remote Volume Renderer.
     * @param aics_count   Number of Audio Input Control Service instances
     *                     the remote Volume Renderer.
     */
    void (*discover)(struct bt_vcp_vol_ctlr *vol_ctlr, int err, uint8_t vocs_count,
                     uint8_t aics_count);

    /**
     * @brief Callback function for bt_vcp_vol_ctlr_vol_down().
     *
     * Called when the volume down procedure is completed.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     */
    void (*vol_down)(struct bt_vcp_vol_ctlr *vol_ctlr, int err);

    /**
     * @brief Callback function for bt_vcp_vol_ctlr_vol_up().
     *
     * Called when the volume up procedure is completed.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     */
    void (*vol_up)(struct bt_vcp_vol_ctlr *vol_ctlr, int err);

    /**
     * @brief Callback function for bt_vcp_vol_ctlr_mute().
     *
     * Called when the mute procedure is completed.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     */
    void (*mute)(struct bt_vcp_vol_ctlr *vol_ctlr, int err);

    /**
     * @brief Callback function for bt_vcp_vol_ctlr_unmute().
     *
     * Called when the unmute procedure is completed.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     */
    void (*unmute)(struct bt_vcp_vol_ctlr *vol_ctlr, int err);

    /**
     * @brief Callback function for bt_vcp_vol_ctlr_vol_down_unmute().
     *
     * Called when the volume down and unmute procedure is completed.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     */
    void (*vol_down_unmute)(struct bt_vcp_vol_ctlr *vol_ctlr, int err);

    /**
     * @brief Callback function for bt_vcp_vol_ctlr_vol_up_unmute().
     *
     * Called when the volume up and unmute procedure is completed.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     */
    void (*vol_up_unmute)(struct bt_vcp_vol_ctlr *vol_ctlr, int err);

    /**
     * @brief Callback function for bt_vcp_vol_ctlr_vol_set().
     *
     * Called when the set absolute volume procedure is completed.
     *
     * @param vol_ctlr  Volume Controller instance pointer.
     * @param err       Error value. 0 on success, GATT error on positive
     *                  value or errno on negative value.
     */
    void (*vol_set)(struct bt_vcp_vol_ctlr *vol_ctlr, int err);

    /** Volume Offset Control Service callbacks */
    struct bt_vocs_cb             vocs_cb;

    /** Audio Input Control Service callbacks */
    struct bt_aics_cb             aics_cb;

    /** @cond INTERNAL_HIDDEN */
    /** Internally used field for list handling */
    sys_snode_t _node;
    /** @endcond */
};

/**
 * @brief Registers the callbacks used by the Volume Controller.
 *
 * @param cb   The callback structure.
 *
 * @retval 0 on success
 * @retval -EINVAL if @p cb is NULL
 * @retval -EALREADY if @p cb was already registered
 */
int bt_vcp_vol_ctlr_cb_register_safe(struct bt_vcp_vol_ctlr_cb *cb);

/**
 * @brief Unregisters the callbacks used by the Volume Controller.
 *
 * @param cb   The callback structure.
 *
 * @retval 0 on success
 * @retval -EINVAL if @p cb is NULL
 * @retval -EALREADY if @p cb was not registered
 */
int bt_vcp_vol_ctlr_cb_unregister_safe(struct bt_vcp_vol_ctlr_cb *cb);

/**
 * @brief Discover Volume Control Service and included services.
 *
 * This will start a GATT discovery and setup handles and subscriptions.
 * This shall be called once before any other actions can be
 * executed for the peer device, and the
 * @ref bt_vcp_vol_ctlr_cb.discover callback will notify when it is possible to
 * start remote operations.
 *
 * This shall only be done as the client,
 *
 * @param      conn      The connection to discover Volume Control Service for.
 * @param[out] vol_ctlr  Valid remote instance object on success.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_discover(struct bt_conn *conn,
                             struct bt_vcp_vol_ctlr **vol_ctlr);
int bt_vcp_vol_ctlr_discover_safe(struct bt_conn *conn,
                                  struct bt_vcp_vol_ctlr **vol_ctlr);

/**
 * @brief Get the volume controller from a connection pointer
 *
 * Get the Volume Control Profile Volume Controller pointer from a connection pointer.
 * Only volume controllers that have been initiated via bt_vcp_vol_ctlr_discover() can be
 * retrieved.
 *
 * @param conn     Connection pointer.
 *
 * @retval Pointer to a Volume Control Profile Volume Controller instance
 * @retval NULL if @p conn is NULL or if the connection has not done discovery yet
 */
struct bt_vcp_vol_ctlr *bt_vcp_vol_ctlr_get_by_conn(const struct bt_conn *conn);

/**
 * @brief Get the connection pointer of a client instance
 *
 * Get the Bluetooth connection pointer of a Volume Control Service
 * client instance.
 *
 * @param      vol_ctlr Volume Controller instance pointer.
 * @param[out] conn     Connection pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_conn_get_safe(const struct bt_vcp_vol_ctlr *vol_ctlr,
                                  struct bt_conn **conn);

/**
 * @brief Get Volume Control Service included services.
 *
 * Returns a pointer to a struct that contains information about the
 * Volume Control Service included service instances, such as pointers to the
 * Volume Offset Control Service (Volume Offset Control Service) or
 * Audio Input Control Service (AICS) instances.
 *
 * Requires that @kconfig{CONFIG_BT_VCP_VOL_CTLR_VOCS} or @kconfig{CONFIG_BT_VCP_VOL_CTLR_AICS} is
 * enabled.
 *
 * @param      vol_ctlr Volume Controller instance pointer.
 * @param[out] included Pointer to store the result in.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_included_get_safe(struct bt_vcp_vol_ctlr *vol_ctlr,
                                      struct bt_vcp_included *included);

/**
 * @brief Read the volume state of a remote Volume Renderer.
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_read_state_safe(struct bt_vcp_vol_ctlr *vol_ctlr);

/**
 * @brief Read the volume flags of a remote Volume Renderer.
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_read_flags_safe(struct bt_vcp_vol_ctlr *vol_ctlr);

/**
 * @brief Turn the volume down one step on a remote Volume Renderer
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_vol_down_safe(struct bt_vcp_vol_ctlr *vol_ctlr);

/**
 * @brief Turn the volume up one step on a remote Volume Renderer
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_vol_up_safe(struct bt_vcp_vol_ctlr *vol_ctlr);

/**
 * @brief Turn the volume down one step and unmute on a remote Volume Renderer
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_unmute_vol_down_safe(struct bt_vcp_vol_ctlr *vol_ctlr);

/**
 * @brief Turn the volume up one step and unmute on a remote Volume Renderer
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_unmute_vol_up_safe(struct bt_vcp_vol_ctlr *vol_ctlr);

/**
 * @brief Set the absolute volume on a remote Volume Renderer
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 * @param volume    The absolute volume to set.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_set_vol_safe(struct bt_vcp_vol_ctlr *vol_ctlr, uint8_t volume);

/**
 * @brief Unmute a remote Volume Renderer.
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_unmute_safe(struct bt_vcp_vol_ctlr *vol_ctlr);

/**
 * @brief Mute a remote Volume Renderer.
 *
 * @param vol_ctlr  Volume Controller instance pointer.
 *
 * @return 0 if success, errno on failure.
 */
int bt_vcp_vol_ctlr_mute_safe(struct bt_vcp_vol_ctlr *vol_ctlr);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_VCP_H_ */
