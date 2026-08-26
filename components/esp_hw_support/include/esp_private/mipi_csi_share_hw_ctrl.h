/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MIPI_CSI_BRG_USER_CSI,      ///< Used by CSI Host
    MIPI_CSI_BRG_USER_ISP_DVP,  ///< Used by ISP DVP
    MIPI_CSI_BRG_USER_SHARE,    ///< Don't care user, share the usage with other users
} mipi_csi_brg_user_t;

/**
 * @brief Claim MIPI CSI Bridge peripheral
 *
 * @param[in]  user    CSI Bridge user
 * @param[out] out_id  ID of the CSI Bridge
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_NOT_FOUND  No free CSI Bridge
 */
esp_err_t mipi_csi_brg_claim(mipi_csi_brg_user_t user, int *out_id);

/**
 * @brief Declaim MIPI CSI Bridge peripheral
 *
 * @param[in] id  ID of the CSI Bridge
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_STATE  CSI Bridge isn't claimed yet
 */
esp_err_t mipi_csi_brg_declaim(int id);

#ifdef __cplusplus
}
#endif
