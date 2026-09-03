/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>

#include "esp_crypto_lock.h"

/* Lock overview.

   Two separate relations decide what a lock must cover:

   1. Functional dependency - which peripherals an operation drives:
        SHA: independent, but DMA is shared with AES
        AES: independent, but DMA is shared with SHA
        MPI/RSA: independent
        ECC: independent
        HMAC: needs SHA
        DS: needs HMAC (which needs SHA), AES and MPI
        ECDSA: needs ECC, SHA where the K value is derived deterministically or
               the Z value is taken from SHA rather than supplied, and MPI on
               some targets

   2. Reset coupling - which peripherals are also reset when this one's RST_EN is
      pulsed, because the hardware reset tree is shared:
        AES/SHA/MPI/ECC: itself only
        HMAC:  HMAC, SHA
        DS:    DS, AES, SHA, MPI
        ECDSA: ECDSA, SHA, ECC, and MPI where SOC_ECDSA_USES_MPI
        KM:    KM, AES, ECC

   A lock must cover the union of both. The reset coupling is why the ECDSA lock
   takes the SHA/AES and MPI locks even though an ECDSA operation does not
   necessarily use those engines.

   The Key Manager holds key usage selectors shared by ECDSA, HMAC, DS and the
   XTS-AES engines. The accelerator paths take the Key Manager lock around the
   clock enable that lets those selectors be written; only the Key Manager's own
   driver resets the peripheral, because that reset is one of the couplings above.


   Acquisition order, which every path must follow to stay deadlock-free:
        DS -> ECDSA -> HMAC -> ECC -> SHA/AES -> MPI -> Key Manager
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
    /* The ECDSA reset holds SHA, which shares its DMA with AES. Taken before MPI
       to keep esp_crypto_ds_lock_acquire()'s order. */
    esp_crypto_sha_aes_lock_acquire();
#endif /* defined(SOC_SHA_SUPPORTED) || defined(SOC_AES_SUPPORTED) */
    /* Unconditional under the cap: the reset coupling is present whether or not
       this revision needs the MPI engine. */
#if (SOC_MPI_SUPPORTED && SOC_ECDSA_USES_MPI)
    esp_crypto_mpi_lock_acquire();
#endif /* (SOC_MPI_SUPPORTED && SOC_ECDSA_USES_MPI) */
}

void esp_crypto_ecdsa_lock_release(void)
{
#if (SOC_MPI_SUPPORTED && SOC_ECDSA_USES_MPI)
    esp_crypto_mpi_lock_release();
#endif /* (SOC_MPI_SUPPORTED && SOC_ECDSA_USES_MPI) */
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
