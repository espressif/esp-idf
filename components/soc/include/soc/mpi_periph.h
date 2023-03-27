/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern const uint32_t MPI_LL_BLOCK_BASES[SOC_MPI_MEM_BLOCKS_NUM];
extern const uint32_t MPI_LL_OPERATIONS[SOC_MPI_OPERATIONS_NUM];

#ifdef __cplusplus
}
#endif
