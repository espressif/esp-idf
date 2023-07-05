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
