/*
 * SPDX-FileCopyrightText: 2017 Linaro Limited
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_TYPES_H_
#define _BLE_MESH_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int         bt_mesh_atomic_t;

#ifndef PRIu64
#define PRIu64      "llu"
#endif

#ifndef PRIx64
#define PRIx64      "llx"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_TYPES_H_ */
