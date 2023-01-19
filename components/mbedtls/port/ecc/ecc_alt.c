/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "soc/hwcrypto_periph.h"
#include "ecc_impl.h"

#include "mbedtls/ecp.h"
#include "mbedtls/platform_util.h"

#define ECP_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA )
#define ECP_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )

#if defined(MBEDTLS_ECP_MUL_ALT) || defined(MBEDTLS_ECP_MUL_ALT_SOFT_FALLBACK)

#define MAX_SIZE            32     // 256 bits

static int esp_mbedtls_ecp_point_multiply(const mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
        const mbedtls_mpi *m, const mbedtls_ecp_point *P)
{
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    uint8_t x_tmp[MAX_SIZE] = {0};
    uint8_t y_tmp[MAX_SIZE] = {0};

    uint8_t m_le[MAX_SIZE] = {0};
    ecc_point_t p_pt = {0};
    ecc_point_t r_pt = {0};

    p_pt.len = grp->pbits / 8;

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(&P->MBEDTLS_PRIVATE(X), p_pt.x, MAX_SIZE));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(&P->MBEDTLS_PRIVATE(Y), p_pt.y, MAX_SIZE));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(m, m_le, MAX_SIZE));

    ret = esp_ecc_point_multiply(&p_pt, m_le, &r_pt, false);

    for (int i = 0; i < MAX_SIZE; i++) {
        x_tmp[MAX_SIZE - i - 1] = r_pt.x[i];
        y_tmp[MAX_SIZE - i - 1] = r_pt.y[i];
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&R->MBEDTLS_PRIVATE(X), x_tmp, MAX_SIZE));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&R->MBEDTLS_PRIVATE(Y), y_tmp, MAX_SIZE));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&R->MBEDTLS_PRIVATE(Z), 1));
    return ret;

cleanup:
    return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
}

int ecp_mul_restartable_internal( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
             mbedtls_ecp_restart_ctx *rs_ctx )
{
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    if (grp->id != MBEDTLS_ECP_DP_SECP192R1 && grp->id != MBEDTLS_ECP_DP_SECP256R1) {
#if defined(MBEDTLS_ECP_MUL_ALT_SOFT_FALLBACK)
        return ecp_mul_restartable_internal_soft(grp, R, m, P, f_rng, p_rng, rs_ctx);
#else
        return ret;
#endif
    }

    /* Common sanity checks to conform with mbedTLS return values */
    MBEDTLS_MPI_CHK( mbedtls_ecp_check_privkey(grp, m) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_check_pubkey(grp, P) );

    MBEDTLS_MPI_CHK( esp_mbedtls_ecp_point_multiply(grp, R, m, P) );
cleanup:
    return( ret );
}

#endif /* defined(MBEDTLS_ECP_MUL_ALT) || defined(MBEDTLS_ECP_MUL_ALT_SOFT_FALLBACK) */

#if defined(MBEDTLS_ECP_VERIFY_ALT) || defined(MBEDTLS_ECP_VERIFY_ALT_SOFT_FALLBACK)

int mbedtls_ecp_check_pubkey( const mbedtls_ecp_group *grp,
                              const mbedtls_ecp_point *pt )
{
    int res;
    ecc_point_t point;

    if (grp->id != MBEDTLS_ECP_DP_SECP192R1 && grp->id != MBEDTLS_ECP_DP_SECP256R1) {
#if defined(MBEDTLS_ECP_VERIFY_ALT_SOFT_FALLBACK)
        return mbedtls_ecp_check_pubkey_soft(grp, pt);
#else
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
#endif
    }

    ECP_VALIDATE_RET( grp != NULL );
    ECP_VALIDATE_RET( pt  != NULL );

    /* Must use affine coordinates */
    if( mbedtls_mpi_cmp_int( &pt->MBEDTLS_PRIVATE(Z), 1 ) != 0 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

    mbedtls_platform_zeroize((void *)&point, sizeof(ecc_point_t));

    memcpy(&point.x, pt->MBEDTLS_PRIVATE(X).MBEDTLS_PRIVATE(p), mbedtls_mpi_size(&pt->MBEDTLS_PRIVATE(X)));
    memcpy(&point.y, pt->MBEDTLS_PRIVATE(Y).MBEDTLS_PRIVATE(p), mbedtls_mpi_size(&pt->MBEDTLS_PRIVATE(Y)));

    point.len = grp->pbits / 8;

    res = esp_ecc_point_verify(&point);
    if (res == 1) {
        return 0;
    } else {
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }
}
#endif /* defined(MBEDTLS_ECP_VERIFY_ALT) || defined(MBEDTLS_ECP_VERIFY_ALT_SOFT_FALLBACK) */
