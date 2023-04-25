/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>

#include "esp_crypto_lock.h"

/* Lock overview:
MPI/RSA: independent
*/

/* Lock for the MPI/RSA peripheral */

static _lock_t s_crypto_mpi_lock;

void esp_crypto_mpi_lock_acquire(void)
{
    _lock_acquire(&s_crypto_mpi_lock);
}

void esp_crypto_mpi_lock_release(void)
{
    _lock_release(&s_crypto_mpi_lock);
}
