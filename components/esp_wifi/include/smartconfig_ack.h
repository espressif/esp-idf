// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SMARTCONFIG_ACK_H
#define SMARTCONFIG_ACK_H

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
