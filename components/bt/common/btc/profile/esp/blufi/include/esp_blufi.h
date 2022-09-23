/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BLUFI_H__
#define __ESP_BLUFI_H__

#include "esp_blufi_api.h"
#include "esp_err.h"

#ifdef CONFIG_BT_NIMBLE_ENABLED
#include "nimble/ble.h"
#include "host/ble_gap.h"
#include "modlog/modlog.h"
#endif

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "esp_gap_ble_api.h"
#endif

#define BLUFI_APP_UUID      0xFFFF
#define BLUFI_DEVICE_NAME            "BLUFI_DEVICE"

#ifdef CONFIG_BT_NIMBLE_ENABLED
struct ble_hs_cfg;
struct ble_gatt_register_ctxt;
struct gatt_value {
    struct os_mbuf *buf;
    uint16_t val_handle;
    uint8_t type;
    void *ptr;
};
#define SERVER_MAX_VALUES       3
#define MAX_VAL_SIZE            512
extern struct gatt_value gatt_values[SERVER_MAX_VALUES];
/* GATT server callback */
void esp_blufi_gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

/* Initialise gatt server */
int esp_blufi_gatt_svr_init(void);
void esp_blufi_btc_init(void);
void esp_blufi_btc_deinit(void);
#endif

#ifdef CONFIG_BT_BLUEDROID_ENABLED
/**
 * @brief           Close a connection  a remote device.
 *
 * @param[in]       gatts_if: GATT server access interface
 * @param[in]       conn_id: connection ID to be closed.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_blufi_close(uint8_t gatts_if, uint16_t conn_id);
void esp_blufi_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
#endif

/* Initialise blufi profile */
uint8_t esp_blufi_init(void);

/* start advertising */
void bleprph_advertise(void);

/* send notifications */
void esp_blufi_send_notify(void *arg);

/* Deinitialise blufi */
void esp_blufi_deinit(void);
/* disconnect */
void esp_blufi_disconnect(void);

/* Stop advertisement */
void esp_blufi_adv_stop(void);

/* Start advertisement */
void esp_blufi_adv_start(void);

void esp_blufi_send_encap(void *arg);

#ifdef CONFIG_BT_NIMBLE_ENABLED
/**
 * @brief Handle gap event for BluFi.
 *        This function can be called inside custom use gap event handler.
 *        It provide minimal event management for BluFi purpose.
 *
 * @param[in] event The type of event being signalled.
 * @param[in] arg Application-specified argument. Currently unused
 * @return int 0 in case of success.
 *             Other in case of failure.
 */
int esp_blufi_handle_gap_events(struct ble_gap_event *event, void *arg);
#endif

#endif/* _ESP_BLUFI_ */
