// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ESP_DPP_H
#define ESP_DPP_H

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_DPP_FAILURE         (ESP_ERR_WIFI_BASE + 151)  /*!< Generic failure during DPP Operation */
#define ESP_ERR_DPP_NO_MEM          (ESP_ERR_WIFI_BASE + 152)  /*!< Failure to allocate memory in DPP Operation */
#define ESP_ERR_DPP_TIMEOUT         (ESP_ERR_WIFI_BASE + 153)  /*!< DPP Operation timed out */
#define ESP_ERR_DPP_TX_FAILURE      (ESP_ERR_WIFI_BASE + 154)  /*!< DPP Frame Tx failed OR not Acked */
#define ESP_ERR_DPP_INVALID_ATTR    (ESP_ERR_WIFI_BASE + 155)  /*!< Encountered invalid DPP Attribute */
#define ESP_ERR_DPP_NOT_SUPPORTED   (ESP_ERR_WIFI_BASE + 156)  /*!< Encountered invalid DPP Attribute */

enum dpp_bootstrap_type {
	DPP_BOOTSTRAP_QR_CODE,
	DPP_BOOTSTRAP_PKEX,
	DPP_BOOTSTRAP_NFC_URI,
};

typedef enum {
    WIFI_DPP_URI_READY,
    WIFI_DPP_CFG_RECVD,
    WIFI_DPP_FAIL,
} wifi_dpp_event_t;

typedef void (*wifi_dpp_event_cb_t)(wifi_dpp_event_t evt, void *data);

esp_err_t esp_supp_dpp_init(wifi_dpp_event_cb_t evt_cb);
void esp_supp_dpp_deinit(void);

esp_err_t esp_dpp_bootstrap_gen(uint8_t channel, enum dpp_bootstrap_type type,
                                const char *key, const char *info);

void esp_dpp_start_listen(uint8_t channel);
void esp_dpp_stop_listen(void);

#ifdef __cplusplus
}
#endif
#endif /* ESP_DPP_H */
