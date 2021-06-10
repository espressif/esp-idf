// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sys/lock.h>

#include "esp_crypto_lock.h"

/* Lock overview:
SHA: peripheral independent, but DMA is shared with AES
AES: peripheral independent, but DMA is shared with SHA
MPI/RSA: independent
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
