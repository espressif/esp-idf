// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates handle to esp_modem used as an esp-netif driver
 *
 * @param dte ESP Modem DTE object
 *
 * @return opaque pointer to esp-modem IO driver used to attach to esp-netif
 */
void *esp_modem_netif_setup(modem_dte_t *dte);

/**
 * @brief Destroys the esp-netif driver handle
 *
 * @param h pointer to the esp-netif adapter for esp-modem
 */
void esp_modem_netif_teardown(void *h);

/**
 * @brief Clears default handlers for esp-modem lifecycle
 *
 * @param h pointer to the esp-netif adapter for esp-modem
 */
esp_err_t esp_modem_netif_clear_default_handlers(void *h);

/**
 * @brief Setups default handlers for esp-modem lifecycle
 *
 * @param h pointer to the esp-netif adapter for esp-modem
 * @param esp_netif pointer corresponding esp-netif instance
 */
esp_err_t esp_modem_netif_set_default_handlers(void *h, esp_netif_t * esp_netif);

#ifdef __cplusplus
}
#endif
