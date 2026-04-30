/*
 * SPDX-FileCopyrightText: 2023-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_gmap_api.h"

#if CONFIG_BT_GMAP
esp_err_t esp_ble_audio_gmap_cb_register(const esp_ble_audio_gmap_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_gmap_cb_register_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_gmap_discover(uint16_t conn_handle)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_gmap_discover(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

#define GMAP_ROLE_MASK  (ESP_BLE_AUDIO_GMAP_ROLE_UGG | \
                         ESP_BLE_AUDIO_GMAP_ROLE_UGT | \
                         ESP_BLE_AUDIO_GMAP_ROLE_BGS | \
                         ESP_BLE_AUDIO_GMAP_ROLE_BGR)

static bool valid_gmap_role(esp_ble_audio_gmap_role_t role)
{
    if (role == 0 || (role & GMAP_ROLE_MASK) != role) {
        /* Invalid role */
        return false;
    }

    if ((role & ESP_BLE_AUDIO_GMAP_ROLE_UGG) != 0 &&
            !IS_ENABLED(CONFIG_BT_GMAP_UGG_SUPPORTED)) {
        /* Device does not support the UGG role */
        return false;
    }

    if ((role & ESP_BLE_AUDIO_GMAP_ROLE_UGT) != 0 &&
            !IS_ENABLED(CONFIG_BT_GMAP_UGT_SUPPORTED)) {
        /* Device does not support the UGT role */
        return false;
    }

    if ((role & ESP_BLE_AUDIO_GMAP_ROLE_BGS) != 0 &&
            !IS_ENABLED(CONFIG_BT_GMAP_BGS_SUPPORTED)) {
        /* Device does not support the BGS role */
        return false;
    }

    if ((role & ESP_BLE_AUDIO_GMAP_ROLE_BGR) != 0 &&
            !IS_ENABLED(CONFIG_BT_GMAP_BGR_SUPPORTED)) {
        /* Device does not support the BGR role */
        return false;
    }

    return true;
}

static bool valid_gmap_features(esp_ble_audio_gmap_role_t role,
                                esp_ble_audio_gmap_feat_t features)
{
#if CONFIG_BT_GMAP_UGG_SUPPORTED
    if ((role & ESP_BLE_AUDIO_GMAP_ROLE_UGG) != 0) {
        esp_ble_audio_gmap_ugg_feat_t ugg_feat = features.ugg_feat;

        if ((ugg_feat & ESP_BLE_AUDIO_GMAP_UGG_FEAT_MULTIPLEX) != 0 &&
                CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT == 0) {
            /* Cannot support ESP_BLE_AUDIO_GMAP_UGG_FEAT_MULTIPLEX with
             * CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT == 0.
             */
            return false;
        }

        if ((ugg_feat & ESP_BLE_AUDIO_GMAP_UGG_FEAT_96KBPS_SOURCE) != 0 &&
                CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT == 0) {
            /* Cannot support ESP_BLE_AUDIO_GMAP_UGG_FEAT_96KBPS_SOURCE with
             * CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT == 0.
             */
            return false;
        }

        if ((ugg_feat & ESP_BLE_AUDIO_GMAP_UGG_FEAT_MULTISINK) != 0 &&
                (CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT < 2 ||
                 CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT < 2)) {
            /* Cannot support ESP_BLE_AUDIO_GMAP_UGG_FEAT_MULTISINK with
             * CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT or
             * CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT < 2.
             */
            return false;
        }
    }
#endif /* CONFIG_BT_GMAP_UGG_SUPPORTED */

#if CONFIG_BT_GMAP_UGT_SUPPORTED
    if ((role & ESP_BLE_AUDIO_GMAP_ROLE_UGT) != 0) {
        esp_ble_audio_gmap_ugt_feat_t ugt_feat = features.ugt_feat;
        esp_ble_audio_gmap_bgr_feat_t bgr_feat = features.bgr_feat;

        if ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_SOURCE) == 0 &&
                (ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_SINK) == 0) {
            /* Device shall support either ESP_BLE_AUDIO_GMAP_UGT_FEAT_SOURCE or
             * ESP_BLE_AUDIO_GMAP_UGT_FEAT_SINK.
             */
            return false;
        }

        if ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_SOURCE) == 0 &&
                ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_80KBPS_SOURCE) != 0 ||
                 (ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISOURCE) != 0)) {
            /* Device shall support ESP_BLE_AUDIO_GMAP_UGT_FEAT_SOURCE if
             * ESP_BLE_AUDIO_GMAP_UGT_FEAT_80KBPS_SOURCE or
             * ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISOURCE is supported.
             */
            return false;
        }

        if ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_SOURCE) != 0 &&
                CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT == 0) {
            /* Cannot support ESP_BLE_AUDIO_GMAP_UGT_FEAT_SOURCE with
             * CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT == 0.
             */
            return false;
        }

        if ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISOURCE) != 0 &&
                (CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT < 2 ||
                 CONFIG_BT_ASCS_MAX_ACTIVE_ASES < 2)) {
            /* Cannot support ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISOURCE with
             * CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT or
             * CONFIG_BT_ASCS_MAX_ACTIVE_ASES < 2.
             */
            return false;
        }

        if ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_SINK) != 0 &&
                CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT == 0) {
            /* Cannot support ESP_BLE_AUDIO_GMAP_UGT_FEAT_SINK with
             * CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT == 0.
             */
            return false;
        }

        if ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_SINK) == 0 &&
                ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_64KBPS_SINK) != 0 ||
                 (ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISINK) != 0 ||
                 (ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTIPLEX) != 0)) {
            /* Device shall support ESP_BLE_AUDIO_GMAP_UGT_FEAT_SINK if
             * ESP_BLE_AUDIO_GMAP_UGT_FEAT_64KBPS_SINK,
             * ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISINK or
             * ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTIPLEX is supported.
             */
            return false;
        }

        if ((ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISINK) != 0 &&
                (CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT < 2 ||
                 CONFIG_BT_ASCS_MAX_ACTIVE_ASES < 2)) {
            /* Cannot support ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISINK with
             * CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT or
             * CONFIG_BT_ASCS_MAX_ACTIVE_ASES < 2.
             */
            return false;
        }

        /* If the device supports both the UGT and BGT roles, then it needs
         * have the same support for multiplexing for both roles.
         */
        if ((role & ESP_BLE_AUDIO_GMAP_ROLE_BGR) != 0 &&
                IS_ENABLED(CONFIG_BT_GMAP_BGR_SUPPORTED) &&
                !!(ugt_feat & ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTIPLEX) !=
                !!(bgr_feat & ESP_BLE_AUDIO_GMAP_BGR_FEAT_MULTIPLEX)) {
            /* Device shall support ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTIPLEX if
             * ESP_BLE_AUDIO_GMAP_BGR_FEAT_MULTIPLEX is supported, and vice versa.
             */
            return false;
        }
    }
#endif /* CONFIG_BT_GMAP_UGT_SUPPORTED */

#if CONFIG_BT_GMAP_BGR_SUPPORTED
    if ((role & ESP_BLE_AUDIO_GMAP_ROLE_BGR) != 0) {
        esp_ble_audio_gmap_bgr_feat_t bgr_feat = features.bgr_feat;

        if ((bgr_feat & ESP_BLE_AUDIO_GMAP_BGR_FEAT_MULTISINK) != 0 &&
                CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT < 2) {
            /* Cannot support ESP_BLE_AUDIO_GMAP_BGR_FEAT_MULTISINK with
             * CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT < 2.
             */
            return false;
        }
    }
#endif /* CONFIG_BT_GMAP_BGR_SUPPORTED */

    /* If the roles are not supported, then the feature characteristics
     * are not instantiated and the feature values do not need to be
     * checked, as they will never be read (thus ignore by the stack).
     */

    return true;
}

esp_err_t esp_ble_audio_gmap_register(esp_ble_audio_gmap_role_t role,
                                      esp_ble_audio_gmap_feat_t features)
{
    int err;

    if (valid_gmap_role(role) == false ||
            valid_gmap_features(role, features) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_gmap_register_safe(role, features);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_gmap_set_role(esp_ble_audio_gmap_role_t role,
                                      esp_ble_audio_gmap_feat_t features)
{
    int err;

    if (valid_gmap_role(role) == false ||
            valid_gmap_features(role, features) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_gmap_set_role_safe(role, features);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_GMAP */
