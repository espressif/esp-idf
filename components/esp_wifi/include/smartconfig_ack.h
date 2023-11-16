/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SMARTCONFIG_ACK_H
#define SMARTCONFIG_ACK_H

#include "esp_smartconfig.h"
#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Send smartconfig ACK to cellphone.
  *
  * @attention The API can only be used when receiving SC_EVENT_GOT_SSID_PSWD event.
  *
  * @param  type: smartconfig type(ESPTouch or AirKiss);
  *         token: token from the cellphone;
  *         cellphone_ip: IP address of the cellphone;
  *
  * @return ESP_OK: succeed
  *         others: fail
  */
esp_err_t sc_send_ack_start(smartconfig_type_t type, uint8_t token, uint8_t *cellphone_ip);

/**
  * @brief  Stop sending smartconfig ACK to cellphone.
  */
void sc_send_ack_stop(void);

#ifdef __cplusplus
}
#endif
#endif
