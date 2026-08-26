/*
 * SPDX-FileCopyrightText: 2018 Vikrant More
 * SPDX-FileContributor: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MODEL_COMMON_H_
#define _MODEL_COMMON_H_

#include "mesh/types.h"

#ifdef __cplusplus
extern "C" {
#endif

float bt_mesh_sqrt(float square);

int32_t bt_mesh_ceil(float num);

float bt_mesh_log2(float num);

#ifdef __cplusplus
}
#endif

#endif /* _MODEL_COMMON_H_ */
