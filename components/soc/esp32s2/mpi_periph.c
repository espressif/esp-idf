/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/hwcrypto_reg.h"
#include "soc/mpi_periph.h"

const uint32_t MPI_LL_BLOCK_BASES[4] = {
    RSA_MEM_X_BLOCK_BASE,
    RSA_MEM_Y_BLOCK_BASE,
    RSA_MEM_Z_BLOCK_BASE,
    RSA_MEM_M_BLOCK_BASE,
};

const uint32_t MPI_LL_OPERATIONS[3] = {
    RSA_MULT_START_REG,
    RSA_MOD_MULT_START_REG,
    RSA_MODEXP_START_REG,
};
