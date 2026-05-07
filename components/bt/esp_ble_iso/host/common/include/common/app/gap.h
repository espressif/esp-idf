/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_APP_GAP_H_
#define HOST_COMMON_APP_GAP_H_

#include <stdint.h>

#include "sdkconfig.h"

#include "nimble/gap.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_le_addr {
    uint8_t type;
    uint8_t val[6];
};

struct bt_le_gap_app_ext_scan_recv_param {
    uint16_t event_type;
    struct bt_le_addr addr;
    int8_t rssi;
    int8_t tx_power;
    uint8_t sid;
    uint8_t pri_phy;
    uint8_t sec_phy;
    uint16_t per_adv_itvl;
    uint8_t data_len;
    uint8_t *data;
};

struct bt_le_gap_app_pa_sync_param {
    uint8_t status;
    uint16_t sync_handle;
    struct bt_le_addr addr;
    uint8_t sid;
    uint8_t adv_phy;
    uint16_t per_adv_itvl;
    uint8_t adv_ca;
};

struct bt_le_gap_app_pa_sync_lost_param {
    uint16_t sync_handle;
    uint8_t reason;
};

struct bt_le_gap_app_pa_sync_recv_param {
    uint16_t sync_handle;
    int8_t tx_power;
    int8_t rssi;
    uint8_t data_status;
    uint8_t data_len;
    uint8_t *data;
};

struct bt_le_gap_app_acl_connect_param {
    uint8_t status;
    uint16_t conn_handle;
    uint8_t role;
    struct bt_le_addr dst;
};

struct bt_le_gap_app_acl_disconnect_param {
    uint16_t conn_handle;
    uint8_t reason;
};

struct bt_le_gap_app_security_change_param {
    uint8_t status;
    uint16_t conn_handle;
    uint8_t role;
    uint8_t sec_level;
    uint8_t bonded : 1;
    struct bt_le_addr dst;
};

struct bt_le_gap_app_identity_resolve_param {
    uint16_t conn_handle;
    struct bt_le_addr rpa;
    struct bt_le_addr identity;
};

struct bt_le_gap_app_bond_delete_param {
    uint8_t id;
    struct bt_le_addr peer;
};

struct bt_le_gap_app_biginfo_recv_param {
    uint16_t sync_handle;
    uint8_t  num_bis;
    uint8_t  nse;
    uint16_t iso_interval;
    uint8_t  bn;
    uint8_t  pto;
    uint8_t  irc;
    uint16_t max_pdu;
    uint32_t sdu_interval;
    uint16_t max_sdu;
    uint8_t  phy;
    uint8_t  framing;
    uint8_t  encryption;
};

struct bt_le_gap_app_param {
    uint8_t type;

    union {
        struct bt_le_gap_app_ext_scan_recv_param    ext_scan_recv;
        struct bt_le_gap_app_pa_sync_param          pa_sync;
        struct bt_le_gap_app_pa_sync_lost_param     pa_sync_lost;
        struct bt_le_gap_app_pa_sync_recv_param     pa_sync_recv;
        struct bt_le_gap_app_acl_connect_param      acl_connect;
        struct bt_le_gap_app_acl_disconnect_param   acl_disconnect;
        struct bt_le_gap_app_security_change_param  security_change;
        struct bt_le_gap_app_identity_resolve_param identity_resolve;
        struct bt_le_gap_app_bond_delete_param      bond_delete;
    };
};

enum bt_le_gap_app_param_type {
    BT_LE_GAP_APP_PARAM_EXT_SCAN_RECV,
    BT_LE_GAP_APP_PARAM_PA_SYNC,
    BT_LE_GAP_APP_PARAM_PA_SYNC_LOST,
    BT_LE_GAP_APP_PARAM_PA_SYNC_RECV,
    BT_LE_GAP_APP_PARAM_ACL_CONNECT,
    BT_LE_GAP_APP_PARAM_ACL_DISCONNECT,
    BT_LE_GAP_APP_PARAM_SECURITY_CHANGE,
    BT_LE_GAP_APP_PARAM_IDENTITY_RESOLVE,
    BT_LE_GAP_APP_PARAM_BOND_DELETE,

    BT_LE_GAP_APP_PARAM_MAX,
};

struct bt_le_gap_app_event_ext_scan_recv {
    uint16_t event_type;
    struct bt_le_addr addr;
    int8_t rssi;
    int8_t tx_power;
    uint8_t sid;
    uint8_t pri_phy;
    uint8_t sec_phy;
    uint16_t per_adv_itvl;
    uint8_t data_len;
    uint8_t *data;  /* Will be freed after the event is processed */
};

struct bt_le_gap_app_event_pa_sync {
    uint8_t status;
    uint16_t sync_handle;
    struct bt_le_addr addr;
    uint8_t sid;
    uint8_t adv_phy;
    uint16_t per_adv_itvl;
    uint8_t adv_ca;
};

struct bt_le_gap_app_event_pa_sync_lost {
    uint16_t sync_handle;
    uint8_t reason;
};

struct bt_le_gap_app_event_pa_sync_recv {
    uint16_t sync_handle;
    int8_t tx_power;
    int8_t rssi;
    uint8_t data_status;
    uint8_t data_len;
    uint8_t *data;  /* Will be freed after the event is processed */
};

struct bt_le_gap_app_event_acl_connect {
    uint8_t status;
    uint16_t conn_handle;
    uint8_t role;
    struct bt_le_addr dst;
};

struct bt_le_gap_app_event_acl_disconnect {
    uint16_t conn_handle;
    uint8_t reason;
};

struct bt_le_gap_app_event_security_change {
    uint8_t status;
    uint16_t conn_handle;
    uint8_t role;
    uint8_t sec_level;
    uint8_t bonded : 1;
    struct bt_le_addr dst;
};

struct bt_le_gap_app_event_identity_resolve {
    uint16_t conn_handle;
    struct bt_le_addr rpa;
    struct bt_le_addr identity;
};

struct bt_le_gap_app_event_bond_delete {
    uint8_t id;
    struct bt_le_addr peer;
};

struct bt_le_gap_app_event_biginfo_recv {
    uint16_t sync_handle;
    uint8_t  num_bis;
    uint8_t  nse;
    uint16_t iso_interval;
    uint8_t  bn;
    uint8_t  pto;
    uint8_t  irc;
    uint16_t max_pdu;
    uint32_t sdu_interval;
    uint16_t max_sdu;
    uint8_t  phy;
    uint8_t  framing;
    uint8_t  encryption;
};

struct bt_le_gap_app_event {
    uint8_t type;

    union {
        struct bt_le_gap_app_event_ext_scan_recv    ext_scan_recv;
        struct bt_le_gap_app_event_pa_sync          pa_sync;
        struct bt_le_gap_app_event_pa_sync_lost     pa_sync_lost;
        struct bt_le_gap_app_event_pa_sync_recv     pa_sync_recv;
        struct bt_le_gap_app_event_acl_connect      acl_connect;
        struct bt_le_gap_app_event_acl_disconnect   acl_disconnect;
        struct bt_le_gap_app_event_security_change  security_change;
        struct bt_le_gap_app_event_identity_resolve identity_resolve;
        struct bt_le_gap_app_event_bond_delete      bond_delete;
        struct bt_le_gap_app_event_biginfo_recv     biginfo_recv;
    };
};

enum bt_le_gap_app_event_type {
    BT_LE_GAP_APP_EVENT_EXT_SCAN_RECV,
    BT_LE_GAP_APP_EVENT_PA_SYNC,
    BT_LE_GAP_APP_EVENT_PA_SYNC_LOST,
    BT_LE_GAP_APP_EVENT_PA_SYNC_RECV,
    BT_LE_GAP_APP_EVENT_ACL_CONNECT,
    BT_LE_GAP_APP_EVENT_ACL_DISCONNECT,
    BT_LE_GAP_APP_EVENT_SECURITY_CHANGE,
    BT_LE_GAP_APP_EVENT_IDENTITY_RESOLVE,
    BT_LE_GAP_APP_EVENT_BOND_DELETE,
    BT_LE_GAP_APP_EVENT_BIGINFO_RECV,

    BT_LE_GAP_APP_EVENT_MAX,
};

typedef void (*bt_le_gap_app_cb)(struct bt_le_gap_app_event *event);

int bt_le_gap_app_cb_register(bt_le_gap_app_cb cb);

void bt_le_gap_app_cb_unregister(void);

void bt_le_gap_app_biginfo_event(uint8_t *param);

void bt_le_gap_handle_event(uint8_t *data, size_t data_len);

void bt_le_gap_app_post_event(uint8_t type, void *param);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_APP_GAP_H_ */
