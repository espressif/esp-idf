/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The possible efuse keys for the HMAC peripheral
 */
typedef enum {
    HMAC_KEY0 = 0,
    HMAC_KEY1,
    HMAC_KEY2,
    HMAC_KEY3,
    HMAC_KEY4,
    HMAC_KEY5,
#if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
    HMAC_KEY_KM = 7,
#endif
    HMAC_KEY_MAX = 8,
} hmac_key_id_t;

/**
 * The HMAC peripheral can be configured to deliver its output to the user directly, or to deliver
 * the output directly to another peripheral instead, e.g. the Digital Signature peripheral.
 */
typedef enum {
    HMAC_OUTPUT_USER = 0,           /**< Let user provide a message and read the HMAC result */
    HMAC_OUTPUT_DS = 1,             /**< HMAC is provided to the DS peripheral to decrypt DS private key parameters */
    HMAC_OUTPUT_JTAG_ENABLE = 2,    /**< HMAC is used to enable JTAG after soft-disabling it */
    HMAC_OUTPUT_ALL = 3             /**< HMAC is used for both as DS input for or enabling JTAG */
} hmac_hal_output_t;

#ifdef __cplusplus
}
#endif
