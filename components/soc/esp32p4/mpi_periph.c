/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/rsa_reg.h"
#include "soc/mpi_periph.h"

const uint32_t MPI_LL_BLOCK_BASES[4] = {
    RSA_X_MEM,
    RSA_Y_MEM,
    RSA_Z_MEM,
    RSA_M_MEM,
};

const uint32_t MPI_LL_OPERATIONS[3] = {
    RSA_SET_START_MULT_REG,
    RSA_SET_START_MODMULT_REG,
    RSA_SET_START_MODEXP_REG,
};
