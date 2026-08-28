/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
ECDSA: needs ECC and MPI, and its reset pulse holds SHA (and thus the SHA/AES DMA) in reset
Key Manager: shared key-usage selectors (ECDSA/HMAC/DS/XTS-AES flash);
             esp_crypto_key_mgr_enable_periph_clk(true) resets it
*/

#if !NON_OS_BUILD
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

#if defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED)
/* Single lock for SHA and AES, sharing a reserved GDMA channel */
static _lock_t s_crypto_sha_aes_lock;
#endif /* defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED) */

#ifdef SOC_ECC_SUPPORTED
/* Lock for ECC peripheral */
static _lock_t s_crypto_ecc_lock;
#endif /* SOC_ECC_SUPPORTED */

#ifdef SOC_ECDSA_SUPPORTED
/* Lock for ECDSA peripheral */
static _lock_t s_crypto_ecdsa_lock;
#if SOC_ECDSA_USES_MPI
#include "hal/ecdsa_ll.h"
#endif /* SOC_ECDSA_USES_MPI */
#endif /* SOC_ECDSA_SUPPORTED */

#if SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT
/* Lock for Key Manager peripheral */
static _lock_t s_crypto_key_manager_lock;
#endif /* SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT */

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

#if defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED)
void esp_crypto_sha_aes_lock_acquire(void)
{
    _lock_acquire(&s_crypto_sha_aes_lock);
}

void esp_crypto_sha_aes_lock_release(void)
{
    _lock_release(&s_crypto_sha_aes_lock);
}
#endif /* defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED) */

#if defined(SOC_SHA_CRYPTO_DMA) || defined(SOC_AES_CRYPTO_DMA)
void esp_crypto_dma_lock_acquire(void)
{
    _lock_acquire(&s_crypto_sha_aes_lock);
}

void esp_crypto_dma_lock_release(void)
{
    _lock_release(&s_crypto_sha_aes_lock);
}
#endif /* defined(SOC_SHA_CRYPTO_DMA) || defined(SOC_AES_CRYPTO_DMA) */

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
#if defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED)
    /* Enabling the ECDSA peripheral pulses the ECDSA reset
       (esp_crypto_ecdsa_enable_periph_clk() -> ecdsa_ll_reset_register()), and on every
       target that has an ECDSA peripheral that reset also holds SHA in reset: see the
       "otherwise SHA is held in reset" note in sha_ll_reset_register(). SHA shares its
       (G)DMA channel with AES, and the SHA/AES lock is what serializes both of them, so
       it has to be held across the pulse. Without it, a hardware ECDSA operation on one
       core lands in the middle of an unrelated SHA or AES transfer on the other core,
       which completes without an error but yields wrong output.
       Taken before the MPI lock to keep the acquisition order of
       esp_crypto_ds_lock_acquire() (SHA/AES before MPI) and avoid a lock cycle. */
    esp_crypto_sha_aes_lock_acquire();
#endif /* defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED) */
#ifdef SOC_ECDSA_USES_MPI
    if (ecdsa_ll_is_mpi_required()) {
        esp_crypto_mpi_lock_acquire();
    }
#endif /* SOC_ECDSA_USES_MPI */
}

void esp_crypto_ecdsa_lock_release(void)
{
#ifdef SOC_ECDSA_USES_MPI
    if (ecdsa_ll_is_mpi_required()) {
        esp_crypto_mpi_lock_release();
    }
#endif /* SOC_ECDSA_USES_MPI */
#if defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED)
    esp_crypto_sha_aes_lock_release();
#endif /* defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED) */
    esp_crypto_ecc_lock_release();
    _lock_release(&s_crypto_ecdsa_lock);
}
#endif /* SOC_ECDSA_SUPPORTED */

#if SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT
void esp_crypto_key_manager_lock_acquire(void)
{
    _lock_acquire(&s_crypto_key_manager_lock);
}

void esp_crypto_key_manager_lock_release(void)
{
    _lock_release(&s_crypto_key_manager_lock);
}
#endif /* SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT */
#else /* NON_OS_BUILD */
#ifdef SOC_HMAC_SUPPORTED
void esp_crypto_hmac_lock_acquire(void) {}

void esp_crypto_hmac_lock_release(void) {}
#endif /* SOC_HMAC_SUPPORTED */

#ifdef SOC_DIG_SIGN_SUPPORTED
void esp_crypto_ds_lock_acquire(void) {}

void esp_crypto_ds_lock_release(void) {}
#endif /* SOC_DIG_SIGN_SUPPORTED */

#if defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED)
void esp_crypto_sha_aes_lock_acquire(void) {}

void esp_crypto_sha_aes_lock_release(void) {}
#endif /* defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED) */

#if defined(SOC_SHA_CRYPTO_DMA) || defined(SOC_AES_CRYPTO_DMA)
void esp_crypto_dma_lock_acquire(void) {}

void esp_crypto_dma_lock_release(void) {}
#endif /* defined(SOC_SHA_CRYPTO_DMA) || defined(SOC_AES_CRYPTO_DMA) */

#ifdef SOC_MPI_SUPPORTED
void esp_crypto_mpi_lock_acquire(void) {}

void esp_crypto_mpi_lock_release(void) {}
#endif /* SOC_MPI_SUPPORTED */

#ifdef SOC_ECC_SUPPORTED
void esp_crypto_ecc_lock_acquire(void) {}

void esp_crypto_ecc_lock_release(void) {}
#endif /* SOC_ECC_SUPPORTED */

#ifdef SOC_ECDSA_SUPPORTED
void esp_crypto_ecdsa_lock_acquire(void) {}

void esp_crypto_ecdsa_lock_release(void) {}
#endif /* SOC_ECDSA_SUPPORTED */

#if SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT
void esp_crypto_key_manager_lock_acquire(void) {}

void esp_crypto_key_manager_lock_release(void) {}
#endif /* SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT */
#endif /* !NON_OS_BUILD */
