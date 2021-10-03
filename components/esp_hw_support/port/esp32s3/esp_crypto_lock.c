/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>

#include "esp_crypto_lock.h"

/* Lock overview:
SHA: peripheral independent, but DMA is shared with AES
AES: peripheral independent, but DMA is shared with SHA
MPI/RSA: independent
HMAC: needs SHA
DS: needs HMAC (which needs SHA), AES and MPI
*/

/*
 * Single lock for SHA, HMAC, DS and AES peripherals.
 * SHA and AES share a reserved GDMA channel.
 * DS uses HMAC, HMAC uses SHA, so they may also not be used simulaneously.
 */
static _lock_t s_crypto_sha_aes_hmac_ds_lock;

/* Lock for the MPI/RSA peripheral, also used by the DS peripheral */
static _lock_t s_crypto_mpi_lock;

void esp_crypto_ds_lock_acquire(void)
{
    _lock_acquire_recursive(&s_crypto_sha_aes_hmac_ds_lock);
}

void esp_crypto_ds_lock_release(void)
{
    _lock_release_recursive(&s_crypto_sha_aes_hmac_ds_lock);
}

void esp_crypto_hmac_lock_acquire(void)
{
    _lock_acquire_recursive(&s_crypto_sha_aes_hmac_ds_lock);
}

void esp_crypto_hmac_lock_release(void)
{
    _lock_release_recursive(&s_crypto_sha_aes_hmac_ds_lock);
}

void esp_crypto_sha_aes_lock_acquire(void)
{
    _lock_acquire_recursive(&s_crypto_sha_aes_hmac_ds_lock);
}

void esp_crypto_sha_aes_lock_release(void)
{
    _lock_release_recursive(&s_crypto_sha_aes_hmac_ds_lock);
}

void esp_crypto_mpi_lock_acquire(void)
{
    _lock_acquire(&s_crypto_mpi_lock);
}

void esp_crypto_mpi_lock_release(void)
{
    _lock_release(&s_crypto_mpi_lock);
}
