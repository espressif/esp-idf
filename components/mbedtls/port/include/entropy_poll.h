/**
 * Mbedtls entropy_poll.h file
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef MBEDTLS_ENTROPY_POLL_H
#define MBEDTLS_ENTROPY_POLL_H
#include "mbedtls/build_info.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Entropy poll callback for a hardware source
 *
 *
 * \note            This must accept NULL as its first argument.
 */
int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen );

#ifdef __cplusplus
}
#endif

#endif /* entropy_poll.h */
