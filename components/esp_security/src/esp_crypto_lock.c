/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>

#include "esp_crypto_lock.h"

/* Lock overview:
SHA: peripheral independent, but DMA is shared with AES
AES: peripheral independent, but DMA is shared with SHA
MPI/RSA: independent
ECC: independent
HMAC: needs SHA
DS: needs HMAC (which needs SHA), AES and MPI
ECDSA: needs ECC and MPI
*/

#ifdef SOC_DIG_SIGN_SUPPORTED
/* Lock for DS peripheral */
static _lock_t s_crypto_ds_lock;
#endif /* SOC_DIG_SIGN_SUPPORTED */

#ifdef SOC_HMAC_SUPPORTED
/* Lock for HMAC peripheral */
static _lock_t s_crypto_hmac_lock;
#endif /* SOC_HMAC_SUPPORTED */

#ifdef SOC_MPI_SUPPORTED
/* Lock for the MPI/RSA peripheral, also used by the DS peripheral */
static _lock_t s_crypto_mpi_lock;
#endif /* SOC_MPI_SUPPORTED */

#if defined(SOC_SHA_SUPPORTED) && defined(SOC_AES_SUPPORTED)
/* Single lock for SHA and AES, sharing a reserved GDMA channel */
static _lock_t s_crypto_sha_aes_lock;
#endif /* defined(SOC_SHA_SUPPORTED) && defined(SOC_AES_SUPPORTED) */

#ifdef SOC_ECC_SUPPORTED
/* Lock for ECC peripheral */
static _lock_t s_crypto_ecc_lock;
#endif /* SOC_ECC_SUPPORTED */

#ifdef SOC_ECDSA_SUPPORTED
/* Lock for ECDSA peripheral */
static _lock_t s_crypto_ecdsa_lock;
#endif /* SOC_ECDSA_SUPPORTED */

#ifdef SOC_KEY_MANAGER_SUPPORTED
/* Lock for Key Manager peripheral */
static _lock_t s_crypto_key_manager_lock;
#endif /* SOC_KEY_MANAGER_SUPPORTED */

#ifdef SOC_HMAC_SUPPORTED
void esp_crypto_hmac_lock_acquire(void)
{
    _lock_acquire(&s_crypto_hmac_lock);
    esp_crypto_sha_aes_lock_acquire();
}

void esp_crypto_hmac_lock_release(void)
{
    esp_crypto_sha_aes_lock_release();
    _lock_release(&s_crypto_hmac_lock);
}
#endif /* SOC_HMAC_SUPPORTED */

#ifdef SOC_DIG_SIGN_SUPPORTED
void esp_crypto_ds_lock_acquire(void)
{
    _lock_acquire(&s_crypto_ds_lock);
    esp_crypto_hmac_lock_acquire();
    esp_crypto_mpi_lock_acquire();
}

void esp_crypto_ds_lock_release(void)
{
    esp_crypto_mpi_lock_release();
    esp_crypto_hmac_lock_release();
    _lock_release(&s_crypto_ds_lock);
}
#endif /* SOC_DIG_SIGN_SUPPORTED */

#if defined(SOC_SHA_SUPPORTED) && defined(SOC_AES_SUPPORTED)
void esp_crypto_sha_aes_lock_acquire(void)
{
    _lock_acquire(&s_crypto_sha_aes_lock);
}

void esp_crypto_sha_aes_lock_release(void)
{
    _lock_release(&s_crypto_sha_aes_lock);
}
#endif /* defined(SOC_SHA_SUPPORTED) && defined(SOC_AES_SUPPORTED) */

#if defined(SOC_SHA_CRYPTO_DMA) && defined(SOC_AES_CRYPTO_DMA)
void esp_crypto_dma_lock_acquire(void)
{
    _lock_acquire(&s_crypto_sha_aes_lock);
}

void esp_crypto_dma_lock_release(void)
{
    _lock_release(&s_crypto_sha_aes_lock);
}
#endif /* defined(SOC_SHA_CRYPTO_DMA) && defined(SOC_AES_CRYPTO_DMA) */

#ifdef SOC_MPI_SUPPORTED
void esp_crypto_mpi_lock_acquire(void)
{
    _lock_acquire(&s_crypto_mpi_lock);
}

void esp_crypto_mpi_lock_release(void)
{
    _lock_release(&s_crypto_mpi_lock);
}
#endif /* SOC_MPI_SUPPORTED */

#ifdef SOC_ECC_SUPPORTED
void esp_crypto_ecc_lock_acquire(void)
{
    _lock_acquire(&s_crypto_ecc_lock);
}

void esp_crypto_ecc_lock_release(void)
{
    _lock_release(&s_crypto_ecc_lock);
}
#endif /* SOC_ECC_SUPPORTED */

#ifdef SOC_ECDSA_SUPPORTED
void esp_crypto_ecdsa_lock_acquire(void)
{
    _lock_acquire(&s_crypto_ecdsa_lock);
    esp_crypto_ecc_lock_acquire();
#ifdef SOC_ECDSA_USES_MPI
    esp_crypto_mpi_lock_acquire();
#endif /* SOC_ECDSA_USES_MPI */
}

void esp_crypto_ecdsa_lock_release(void)
{
#ifdef SOC_ECDSA_USES_MPI
    esp_crypto_mpi_lock_release();
#endif /* SOC_ECDSA_USES_MPI */
    esp_crypto_ecc_lock_release();
    _lock_release(&s_crypto_ecdsa_lock);
}
#endif /* SOC_ECDSA_SUPPORTED */

#ifdef SOC_KEY_MANAGER_SUPPORTED
void esp_crypto_key_manager_lock_acquire(void)
{
    _lock_acquire(&s_crypto_key_manager_lock);
}

void esp_crypto_key_manager_lock_release(void)
{
    _lock_release(&s_crypto_key_manager_lock);
}
#endif /* SOC_KEY_MANAGER_SUPPORTED */
