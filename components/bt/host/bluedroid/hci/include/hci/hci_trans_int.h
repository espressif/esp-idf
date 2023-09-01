/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __HCI_TRANS_INT_H__
#define __HCI_TRANS_INT_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_bluedroid_hci.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief host checks whether it can send data to controller
 *
 * @return true if host can send data, false otherwise
 */
bool hci_host_check_send_available(void);

/**
 * @brief host sends packet to controller
 *
 * @param[in] data pointer to data buffer
 * @param[in] len  length of data in byte
 */
void hci_host_send_packet(uint8_t *data, uint16_t len);

/**
 * @brief register the HCI function interface
 *
 * @param[in] callback HCI function interface
 *
 * @return ESP_OK register successfully, ESP_FAIL otherwise
 */
esp_err_t hci_host_register_callback(const esp_bluedroid_hci_driver_callbacks_t *callback);

#ifdef __cplusplus
}
#endif

#endif /* __HCI_TRANS_INT_H__ */
