/**
 * @file
 * @brief Header for Bluetooth Gaming Audio Profile (GMAP).
 *
 * SPDX-FileCopyrightText: 2023-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_GMAP_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_GMAP_

/**
 * @brief Bluetooth Gaming Audio Profile (GMAP)
 *
 * @defgroup bt_gmap Bluetooth Gaming Audio Profile
 *
 * @since 3.5
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 */

#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Gaming Role bitfield */
enum bt_gmap_role {
    /**
     * @brief Gaming Role Unicast Game Gateway
     *
     * Requires @kconfig{CONFIG_BT_CAP_INITIATOR}, @kconfig{CONFIG_BT_BAP_UNICAST_CLIENT} and
     * @kconfig{CONFIG_BT_VCP_VOL_CTLR} to be enabled.
     */
    BT_GMAP_ROLE_UGG = BIT(0),
    /**
     * @brief Gaming Role Unicast Game Terminal
     *
     * Requires @kconfig{CONFIG_BT_CAP_ACCEPTOR} and @kconfig{CONFIG_BT_BAP_UNICAST_SERVER} to
     * be enabled.
     */
    BT_GMAP_ROLE_UGT = BIT(1),
    /**
     * @brief Gaming Role Broadcast Game Sender
     *
     * Requires @kconfig{CONFIG_BT_CAP_INITIATOR} and @kconfig{CONFIG_BT_BAP_BROADCAST_SOURCE}
     * to be enabled.
     */
    BT_GMAP_ROLE_BGS = BIT(2),
    /**
     * @brief Gaming Role Broadcast Game Receiver
     *
     * Requires @kconfig{CONFIG_BT_CAP_ACCEPTOR}, @kconfig{CONFIG_BT_BAP_BROADCAST_SINK} and
     * @kconfig{CONFIG_BT_VCP_VOL_REND} to be enabled.
     */
    BT_GMAP_ROLE_BGR = BIT(3),
};

/** Unicast Game Gateway Feature bitfield */
enum bt_gmap_ugg_feat {
    /**
     * @brief Support transmitting multiple LC3 codec frames per block in an SDU
     *
     * Requires @kconfig{CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT} > 0
     */
    BT_GMAP_UGG_FEAT_MULTIPLEX = BIT(0),
    /**
     * @brief 96 kbps source support
     *
     * Requires @kconfig{CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT} > 0
     */
    BT_GMAP_UGG_FEAT_96KBPS_SOURCE = BIT(1),
    /**
     * @brief Support for receiving at least two channels of audio, each in a separate CIS
     *
     * Requires @kconfig{CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT} > 1 and
     * @kconfig{CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT} > 1
     */
    BT_GMAP_UGG_FEAT_MULTISINK = BIT(2),
};

/** Unicast Game Terminal Feature bitfield */
enum bt_gmap_ugt_feat {
    /**
     * @brief Source support
     *
     * Requires @kconfig{CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT} > 0
     */
    BT_GMAP_UGT_FEAT_SOURCE = BIT(0),
    /**
     * @brief 80 kbps source support
     *
     * Requires BT_GMAP_UGT_FEAT_SOURCE to be set as well
     */
    BT_GMAP_UGT_FEAT_80KBPS_SOURCE = BIT(1),
    /**
     * @brief Sink support
     *
     * Requires @kconfig{CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT} > 0
     */
    BT_GMAP_UGT_FEAT_SINK = BIT(2),
    /**
     * @brief 64 kbps sink support
     *
     * Requires BT_GMAP_UGT_FEAT_SINK to be set as well
     */
    BT_GMAP_UGT_FEAT_64KBPS_SINK = BIT(3),
    /**
     * @brief Support for receiving multiple LC3 codec frames per block in an SDU
     *
     * Requires BT_GMAP_UGT_FEAT_SINK to be set as well
     */
    BT_GMAP_UGT_FEAT_MULTIPLEX = BIT(4),
    /**
     * @brief Support for receiving at least two audio channels, each in a separate CIS
     *
     * Requires @kconfig{CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT} > 1 and
     * @kconfig{CONFIG_BT_ASCS_MAX_ACTIVE_ASES} > 1, and BT_GMAP_UGT_FEAT_SINK to be set as well
     */
    BT_GMAP_UGT_FEAT_MULTISINK = BIT(5),
    /**
     * @brief Support for sending at least two audio channels, each in a separate CIS
     *
     * Requires @kconfig{CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT} > 1 and
     * @kconfig{CONFIG_BT_ASCS_MAX_ACTIVE_ASES} > 1, and BT_GMAP_UGT_FEAT_SOURCE to be set
     * as well
     */
    BT_GMAP_UGT_FEAT_MULTISOURCE = BIT(6),
};

/** Broadcast Game Sender Feature bitfield */
enum bt_gmap_bgs_feat {
    /** 96 kbps support */
    BT_GMAP_BGS_FEAT_96KBPS = BIT(0),
};

/** Broadcast Game Receiver Feature bitfield */
enum bt_gmap_bgr_feat {
    /**
     * @brief Support for receiving at least two audio channels, each in a separate BIS
     *
     * Requires @kconfig{CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT} > 1
     */
    BT_GMAP_BGR_FEAT_MULTISINK = BIT(0),
    /** @brief Support for receiving multiple LC3 codec frames per block in an SDU */
    BT_GMAP_BGR_FEAT_MULTIPLEX = BIT(1),
};

/** Broadcast Game Receiver Feature bitfield */
struct bt_gmap_feat {
    /** Unicast Game Gateway features */
    enum bt_gmap_ugg_feat ugg_feat;
    /** Unicast Game Terminal features */
    enum bt_gmap_ugt_feat ugt_feat;
    /** Remote Broadcast Game Sender features */
    enum bt_gmap_bgs_feat bgs_feat;
    /** Remote Broadcast Game Receiver features */
    enum bt_gmap_bgr_feat bgr_feat;
};

/** @brief Hearing Access Service Client callback structure. */
struct bt_gmap_cb {
    /**
     * @brief Callback function for bt_has_discover.
     *
     * This callback is called when discovery procedure is complete.
     *
     * @param conn Bluetooth connection object.
     * @param err 0 on success, ATT error or negative errno otherwise.
     * @param role Role of remote device. 0 on failure.
     * @param features Remote features.
     */
    void (*discover)(struct bt_conn *conn, int err, enum bt_gmap_role role,
                     struct bt_gmap_feat features);
};

/**
 * @brief Registers the callbacks used by the Gaming Audio Profile.
 *
 * @param cb The callback structure.
 *
 * @retval -EINVAL if @p cb is NULL
 * @retval -EALREADY if callbacks have already be registered
 * @retval 0 on success
 */
int bt_gmap_cb_register_safe(const struct bt_gmap_cb *cb);

/**
 * @brief Discover Gaming Service on a remote device.
 *
 * Procedure to find a Gaming Service on a server identified by @p conn.
 * The @ref bt_gmap_cb.discover callback is called when the discovery procedure completes of fails.
 * On discovery success the callback contains information about the remote device.
 *
 * @param conn Bluetooth connection object.
 *
 * @retval -EINVAL if @p conn is NULL
 * @retval -EBUSY if discovery is already in progress for @p conn
 * @retval -ENOEXEC if discovery failed to initiate
 * @retval 0 on success
 */
int bt_gmap_discover(struct bt_conn *conn);
int bt_gmap_discover_safe(struct bt_conn *conn);

/**
 * @brief Adds GMAS instance to database and sets the received Gaming Audio Profile role(s).
 *
 * @param role     Gaming Audio Profile role(s) of the device (one or multiple).
 * @param features Features of the roles. If a role is not in the @p role parameter, then the
 *                 feature value for that role is simply ignored.
 *
 * @retval -EINVAL on invalid arguments
 * @retval -ENOEXEC on service register failure
 * @retval 0 on success
 */
int bt_gmap_register_safe(enum bt_gmap_role role, struct bt_gmap_feat features);

/**
 * @brief Set one or multiple Gaming Audio Profile roles and features dynamically.
 *
 * Previously registered value will be overwritten. If there is a role change, this will trigger
 * a Gaming Audio Service (GMAS) service change. If there is only a feature change, no service
 * change will happen.
 *
 * @param role     Gaming Audio Profile role(s).
 * @param features Features of the roles. If a role is not in the @p role parameter, then the
 *                 feature value for that role is simply ignored.
 *
 * @retval -ENOEXEC if the service has not yet been registered
 * @retval -EINVAL on invalid arguments
 * @retval -EALREADY if the @p role and @p features are the same as existing ones
 * @retval -ENOENT on service unregister failure
 * @retval -ECANCELED on service re-register failure
 * @retval 0 on success
 */
int bt_gmap_set_role_safe(enum bt_gmap_role role, struct bt_gmap_feat features);

#ifdef __cplusplus
}
#endif
/** @} */ /* end of bt_gmap */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_GMAP_ */
