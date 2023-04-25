/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

typedef enum {
    MPI_MULT = 0x0,             // (X * Y)
    MPI_MODMULT,                // (X * Y) Mod M
    MPI_MODEXP,                 // (X ^ Y) Mod M
} mpi_op_t;

typedef enum {
    MPI_PARAM_X = 0x0,
    MPI_PARAM_Y,
    MPI_PARAM_Z,
    MPI_PARAM_M,
} mpi_param_t;
