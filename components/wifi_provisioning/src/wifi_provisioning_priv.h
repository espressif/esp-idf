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

#include <esp_event_loop.h>

#include <protocomm.h>
#include <protocomm_security.h>

#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/wifi_config.h"

/**
 * @brief   Notify manager that provisioning is done
 *
 * Stops the provisioning. This is called by the get_status_handler()
 * when the status is connected. This has no effect if main application
 * has disabled auto stop on completion by calling
 * wifi_prov_mgr_disable_auto_stop()
 *
 * @return
 *  - ESP_OK      : Provisioning will be stopped
 *  - ESP_FAIL    : Failed to stop provisioning
 */
esp_err_t wifi_prov_mgr_done(void);

/**
 * @brief   Get protocomm handlers for wifi_config provisioning endpoint
 *
 * @return  wifi_prov_config_handlers_t structure
 */
wifi_prov_config_handlers_t get_wifi_prov_handlers(void);
