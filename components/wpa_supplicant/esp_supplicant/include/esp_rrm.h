/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
  *    - 0: success
  *    - -1: AP does not support RRM
  *    - -2: station not connected to AP
  */
int esp_rrm_send_neighbor_rep_request(neighbor_rep_request_cb cb,
				      void *cb_ctx);

/**
  * @brief  Check RRM capability of connected AP
  *
  * @return
  *    - true: AP supports RRM
  *    - false: AP does not support RRM or station not connected to AP
  */
bool esp_rrm_is_rrm_supported_connection(void);

#ifdef __cplusplus
}
#endif
#endif
