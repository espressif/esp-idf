/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LDO Unit
 *
 * @note See datasheet to know LDO, alias includes but not least to `VFB/VOn`
 */
typedef enum {
    LDO_UNIT_1 = 1,    ///< LDO 1
    LDO_UNIT_2,        ///< LDO 2
    LDO_UNIT_3,        ///< LDO 3
    LDO_UNIT_4,        ///< LDO 4
} ldo_unit_t;

#ifdef __cplusplus
}
#endif
