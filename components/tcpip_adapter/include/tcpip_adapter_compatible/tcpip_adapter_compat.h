// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _TCPIP_ADAPTER_COMPAT_H_
#define _TCPIP_ADAPTER_COMPAT_H_

/**
 * @brief This function is called from wifi_init to assure backward compatibility mode
 * of tcpip_adapter. In case of legacy use, default instances of ap and sta
 * are created and handlers are registered
 *
 * @note This api is given in a separate header, which is included internally (from wifi driver)
 * rather then user initialization code.
 *
 * @return ESP_OK on success
 */
esp_err_t tcpip_adapter_set_default_wifi_handlers(void);

/**
 * @brief This function is called from ethernet driver init code to facilitate
 * autostart fo the driver in backward compatible tcpip_adapter way
 *
 * @param[in] h Handle to the ethernet driver
 *
 * @return ESP_OK on success
 */
esp_err_t tcpip_adapter_start_eth(void* h);

#endif //_TCPIP_ADAPTER_COMPAT_H_
