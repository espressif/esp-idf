/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_BLE_H_
#define _BTC_BLE_MESH_BLE_H_

#include <stdint.h>
#include "btc/btc_manage.h"
#include "mesh/adapter.h"
#include "esp_ble_mesh_ble_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t   addr[6];                                /*!< Device address */
    uint8_t   addr_type;                              /*!< Device address type */
#if CONFIG_BLE_MESH_USE_BLE_50
    uint8_t   adv_type __attribute__((deprecated("`event_type` should be used to determine the advertising type")));   /*!< advertising type  */
#else
    uint8_t   adv_type;                               /*!< Advertising type  */
#endif
    uint8_t   *data;                                  /*!< Advertising data */
    uint16_t  length;                                 /*!< Advertising data length */
    int8_t    rssi;                                   /*!< RSSI of the advertising packet */
#if CONFIG_BLE_MESH_USE_BLE_50
    uint8_t   event_type;                             /*!< Extended advertising event type */
    uint8_t   primary_phy;                            /*!< Extended advertising primary PHY */
    uint8_t   secondary_phy;                          /*!< Extended advertising secondary PHY */
    uint8_t   sid;                                    /*!< Extended advertising set ID */
    uint8_t   tx_power;                               /*!< Extended advertising TX power */
    uint8_t   dir_addr_type;                          /*!< Direct address type */
    uint8_t   dir_addr[6];                            /*!< Direct address */
    uint8_t   data_status;                            /*!< Data type */
    uint16_t  per_adv_interval;                       /*!< Periodic advertising interval */
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
} bt_mesh_ble_adv_report_t;

typedef union {
    struct {
        esp_ble_mesh_ble_adv_param_t param;
        esp_ble_mesh_ble_adv_data_t data;
    } start_ble_adv;
    struct {
        uint8_t index;
    } stop_ble_adv;
    struct {
        esp_ble_mesh_ble_scan_param_t param;
    } start_ble_scan;
    struct {
        /* RFU */
    } stop_ble_scan;
    struct ble_mesh_scan_params {
        uint16_t scan_interval;
        uint16_t uncoded_scan_window;
    } scan_params;
} btc_ble_mesh_ble_args_t;

typedef enum {
    BTC_BLE_MESH_ACT_START_BLE_ADV,
    BTC_BLE_MESH_ACT_STOP_BLE_ADV,
    BTC_BLE_MESH_ACT_START_BLE_SCAN,
    BTC_BLE_MESH_ACT_STOP_BLE_SCAN,
    BTC_BLE_MESH_ACT_UPDATE_SCAN_PARAMS,
} btc_ble_mesh_ble_act_t;

void bt_mesh_ble_scan_cb_evt_to_btc(bt_mesh_ble_adv_report_t *adv_report);

#if CONFIG_BT_NIMBLE_ENABLED && CONFIG_BLE_MESH_USE_BLE_50
void bt_mesh_ble_nimble_evt_to_btc(struct ble_gap_event *event, void *arg);
#endif /* CONFIG_BT_NIMBLE_ENABLED && CONFIG_BLE_MESH_USE_BLE_50 */

void btc_ble_mesh_ble_call_handler(btc_msg_t *msg);

void btc_ble_mesh_ble_cb_handler(btc_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_BLE_H_ */
