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

/* Lock for ECC peripheral */
static _lock_t s_crypto_ecc_lock;

/* Lock for ECDSA peripheral */
static _lock_t s_crypto_ecdsa_lock;

void esp_crypto_ecc_lock_acquire(void)
{
    _lock_acquire(&s_crypto_ecc_lock);
}

void esp_crypto_ecc_lock_release(void)
{
    _lock_release(&s_crypto_ecc_lock);
}

void esp_crypto_ecdsa_lock_acquire(void)
{
    _lock_acquire(&s_crypto_ecdsa_lock);
    esp_crypto_ecc_lock_acquire();
}

void esp_crypto_ecdsa_lock_release(void)
{
    esp_crypto_ecc_lock_release();
    _lock_release(&s_crypto_ecdsa_lock);
}
