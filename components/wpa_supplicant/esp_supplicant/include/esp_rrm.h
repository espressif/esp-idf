/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_RRM_H
#define _ESP_RRM_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "esp_bit_defs.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RRM feature disable mask bits
 *
 * Polarity: 0 means the feature is enabled (default). Setting a bit disables
 * that feature at runtime. Bits not listed here are reserved; setting them
 * returns ESP_ERR_INVALID_ARG.
 */
#define ESP_RRM_FEATURE_DISABLE_BEACON_MEASUREMENT  BIT(0) /**< Disable AP Beacon Measurement Request handling (no beacon-report scan/report) */
#define ESP_RRM_FEATURE_DISABLE_LINK_MEASUREMENT    BIT(1) /**< Disable AP Link Measurement Request/Report handling */
#define ESP_RRM_FEATURE_DISABLE_NEIGHBOR_REPORT     BIT(2) /**< Disable Neighbor Report request TX and response RX */

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
  * @return
  *    - 0: success
  *    - -1: AP does not support RRM, or neighbor report has been disabled via
  *          esp_rrm_set_feature_disable_mask(ESP_RRM_FEATURE_DISABLE_NEIGHBOR_REPORT).
  *          The disabled check is evaluated before the not-connected check.
  *    - -2: station not connected to AP
  */
int esp_rrm_send_neighbor_report_request(void);

/**
  * @brief  Check RRM capability of connected AP
  *
  * @return
  *    - true: AP supports RRM
  *    - false: AP does not support RRM or station not connected to AP
  */
bool esp_rrm_is_rrm_supported_connection(void);

/**
  * @brief  Set runtime RRM feature disable mask
  *
  * @param  disable_mask  Bitmask of ESP_RRM_FEATURE_DISABLE_* flags.
  *                       0 enables all supported RRM features (default).
  *
  * @return
  *    - ESP_OK: success
  *    - ESP_ERR_INVALID_ARG: reserved/unknown bits set in disable_mask
  */
esp_err_t esp_rrm_set_feature_disable_mask(uint32_t disable_mask);

/**
  * @brief  Get runtime RRM feature disable mask
  *
  * @param[out] disable_mask  Current disable mask
  *
  * @return
  *    - ESP_OK: success
  *    - ESP_ERR_INVALID_ARG: disable_mask is NULL
  */
esp_err_t esp_rrm_get_feature_disable_mask(uint32_t *disable_mask);

#ifdef __cplusplus
}
#endif
#endif
