/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>

#include "esp_crypto_lock.h"

/* Single lock for SHA and AES engine which both use the crypto DMA */

static _lock_t s_crypto_dma_lock;

/* Lock for the MPI/RSA peripheral, also used by the DS peripheral */
static _lock_t s_crypto_mpi_lock;

void esp_crypto_dma_lock_acquire(void)
{
    _lock_acquire(&s_crypto_dma_lock);
}

void esp_crypto_dma_lock_release(void)
{
    _lock_release(&s_crypto_dma_lock);
}

void esp_crypto_mpi_lock_acquire(void)
{
    _lock_acquire(&s_crypto_mpi_lock);
}

void esp_crypto_mpi_lock_release(void)
{
    _lock_release(&s_crypto_mpi_lock);
}
