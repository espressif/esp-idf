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
*/

/* Lock for DS peripheral */
static _lock_t s_crypto_ds_lock;

/* Lock for HMAC peripheral */
static _lock_t s_crypto_hmac_lock;

/* Lock for the MPI/RSA peripheral, also used by the DS peripheral */
static _lock_t s_crypto_mpi_lock;

/* Single lock for SHA and AES, sharing a reserved GDMA channel */
static _lock_t s_crypto_sha_aes_lock;

/* Lock for ECC peripheral */
static _lock_t s_crypto_ecc_lock;

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

void esp_crypto_sha_aes_lock_acquire(void)
{
    _lock_acquire(&s_crypto_sha_aes_lock);
}

void esp_crypto_sha_aes_lock_release(void)
{
    _lock_release(&s_crypto_sha_aes_lock);
}

void esp_crypto_mpi_lock_acquire(void)
{
    _lock_acquire(&s_crypto_mpi_lock);
}

void esp_crypto_mpi_lock_release(void)
{
    _lock_release(&s_crypto_mpi_lock);
}

void esp_crypto_ecc_lock_acquire(void)
{
    _lock_acquire(&s_crypto_ecc_lock);
}

void esp_crypto_ecc_lock_release(void)
{
    _lock_release(&s_crypto_ecc_lock);
}
