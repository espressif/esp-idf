/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/hwcrypto_reg.h"
#include "soc/mpi_periph.h"

const uint32_t MPI_LL_BLOCK_BASES[SOC_MPI_MEM_BLOCKS_NUM] = {
    RSA_MEM_X_BLOCK_BASE,
    RSA_MEM_Y_BLOCK_BASE,
    RSA_MEM_Z_BLOCK_BASE,
    RSA_MEM_M_BLOCK_BASE,
};

const uint32_t MPI_LL_OPERATIONS[SOC_MPI_OPERATIONS_NUM] = {
    RSA_MULT_START_REG,
};
