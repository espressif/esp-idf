// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <protocomm.h>
#include <protocomm_httpd.h>

#include "wifi_provisioning/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Scheme that can be used by manager for provisioning
 *          over SoftAP transport with HTTP server
 */
extern const wifi_prov_scheme_t wifi_prov_scheme_softap;

/**
 *
 * @brief Provide HTTPD Server handle externally.
 *
 * Useful in cases wherein applications need the webserver for some
 * different operations, and do not want the wifi provisioning component
 * to start/stop a new instance.
 *
 * @note This API should be called before wifi_prov_mgr_start_provisioning()
 *
 * @param[in] handle Handle to HTTPD server instance
 */
void wifi_prov_scheme_softap_set_httpd_handle(void *handle);
#ifdef __cplusplus
}
#endif
