/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include_next "mbedtls/ecp.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_ECP_MUL_ALT) || defined(MBEDTLS_ECP_MUL_ALT_SOFT_FALLBACK)
int ecp_mul_restartable_internal( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
             mbedtls_ecp_restart_ctx *rs_ctx );
#endif

#if defined(MBEDTLS_ECP_MUL_ALT_SOFT_FALLBACK)
int ecp_mul_restartable_internal_soft( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
             mbedtls_ecp_restart_ctx *rs_ctx );
#endif

#if defined(MBEDTLS_ECP_VERIFY_ALT_SOFT_FALLBACK)

int mbedtls_ecp_check_pubkey_soft(const mbedtls_ecp_group *grp,
                              const mbedtls_ecp_point *pt );
#endif

#ifdef __cplusplus
}
#endif
