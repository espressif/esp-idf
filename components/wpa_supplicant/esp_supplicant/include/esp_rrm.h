/**
 * Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ESP_RRM_H
#define _ESP_RRM_H

#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Callback function type to get neighbor report
  *
  * @param  ctx: neighbor report context
  * @param  report: neighbor report
  * @param  report_len: neighbor report length
  *
  * @return
  *    - void
  */
typedef void (*neighbor_rep_request_cb)(void *ctx, const uint8_t *report, size_t report_len);

/**
  * @brief  Send Radio measurement neighbor report request to connected AP
  *
  * @param  cb: callback function for neighbor report
  * @param  cb_ctx: callback context
  *
  * @return
  *    - 0: success else failure
  */
int esp_rrm_send_neighbor_rep_request(neighbor_rep_request_cb cb,
				      void *cb_ctx);

#ifdef __cplusplus
}
#endif
#endif
