/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief APM Region PMS authority
 */
typedef enum {
    APM_REGION_PMS_X = 0x1, /*!< Region executive authority. */
    APM_REGION_PMS_W = 0x2, /*!< Region write authority. */
    APM_REGION_PMS_R = 0x4, /*!< Region read authority. */
} apm_region_pms_t;

#ifdef __cplusplus
}
#endif
