/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_bluedroid_hci.h"
#include "common/bt_target.h"
#include "hci/hci_trans_int.h"
#if (BT_CONTROLLER_INCLUDED == TRUE)
#include "esp_bt.h"
#endif
#include "hci_log/bt_hci_log.h"

#define LOG_TAG "HCI_API"

static esp_bluedroid_hci_driver_operations_t s_hci_driver_ops = { 0 };

esp_err_t esp_bluedroid_attach_hci_driver(const esp_bluedroid_hci_driver_operations_t *p_ops)
{
    if (!p_ops) {
        ESP_LOGE(LOG_TAG, "%s invalid function parameter", __func__);
        return ESP_FAIL;
    }

    s_hci_driver_ops.send                   = p_ops->send;
    s_hci_driver_ops.check_send_available   = p_ops->check_send_available;
    s_hci_driver_ops.register_host_callback = p_ops->register_host_callback;

    return ESP_OK;
}

esp_err_t esp_bluedroid_detach_hci_driver(void)
{
    s_hci_driver_ops.send                   = NULL;
    s_hci_driver_ops.check_send_available   = NULL;
    s_hci_driver_ops.register_host_callback = NULL;

    return ESP_OK;
}

/****************************************************************
 *                         INTERNAL USE                         *
 ****************************************************************/

bool hci_host_check_send_available(void)
{
    bool can_send = false;
#if (BT_CONTROLLER_INCLUDED == TRUE)
    can_send = esp_vhci_host_check_send_available();
#else /* BT_CONTROLLER_INCLUDED == TRUE */
    if (s_hci_driver_ops.check_send_available) {
        can_send = s_hci_driver_ops.check_send_available();
    }
#endif /* BT_CONTROLLER_INCLUDED == TRUE */
    return can_send;
}

void hci_host_send_packet(uint8_t *data, uint16_t len)
{
#if (BT_HCI_LOG_INCLUDED == TRUE)
    bt_hci_log_record_hci_data(data[0], data, len);
#endif
#if (BT_CONTROLLER_INCLUDED == TRUE)
    esp_vhci_host_send_packet(data, len);
#else /* BT_CONTROLLER_INCLUDED == TRUE */
    if (s_hci_driver_ops.send) {
        s_hci_driver_ops.send(data, len);
    }
#endif /* BT_CONTROLLER_INCLUDED == TRUE */
}

esp_err_t hci_host_register_callback(const esp_bluedroid_hci_driver_callbacks_t *callback)
{
    esp_err_t ret = ESP_FAIL;

    if (!callback) {
        ESP_LOGE(LOG_TAG, "%s invalid function parameter", __func__);
        return ESP_FAIL;
    }

#if (BT_CONTROLLER_INCLUDED == TRUE)
    ret = esp_vhci_host_register_callback((esp_vhci_host_callback_t *)callback);
#else /* BT_CONTROLLER_INCLUDED == TRUE */
    if (s_hci_driver_ops.register_host_callback) {
        ret = s_hci_driver_ops.register_host_callback(callback);
    }
#endif /* BT_CONTROLLER_INCLUDED == TRUE */

    return ret;
}
