/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _NIMBLE_PORT_H
#define _NIMBLE_PORT_H

#include "esp_err.h"
#include "nimble/nimble_npl.h"

#define NIMBLE_CORE (CONFIG_BT_NIMBLE_PINNED_TO_CORE < portNUM_PROCESSORS ? CONFIG_BT_NIMBLE_PINNED_TO_CORE : tskNO_AFFINITY)

#define NIMBLE_HS_STACK_SIZE CONFIG_BT_NIMBLE_HOST_TASK_STACK_SIZE

#if SOC_ESP_NIMBLE_CONTROLLER && CONFIG_BT_CONTROLLER_ENABLED
#define NIMBLE_LL_STACK_SIZE CONFIG_BT_LE_CONTROLLER_TASK_STACK_SIZE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief nimble_port_init - Initialize controller and NimBLE host stack
*
* @return esp_err_t   - ESP_OK ( if success)
*                       Error code in case of failure
*/
esp_err_t nimble_port_init(void);

/**
* @brief nimble_port_deinit - Deinitialize controller and NimBLE host stack
*
* @return esp_err_t   - ESP_OK ( if success)
*                       Error code in case of failure
*/
esp_err_t nimble_port_deinit(void);

void nimble_port_run(void);
int nimble_port_stop(void);

/**
 * @brief esp_nimble_init - Initialize the NimBLE host stack
 *
 * @return esp_err_t
 */
esp_err_t esp_nimble_init(void);

/**
 * @brief esp_nimble_deinit - Deinitialize the NimBLE host stack
 *
 * @return esp_err_t
 */
esp_err_t esp_nimble_deinit(void);

struct ble_npl_eventq *nimble_port_get_dflt_eventq(void);


#ifdef __cplusplus
}
#endif

#endif /* _NIMBLE_PORT_H */
