/*
 * SPDX-FileCopyrightText: 2011-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_SYS_ASSERT_H_
#define ZEPHYR_INCLUDE_SYS_ASSERT_H_

#include <stdint.h>

#include "utils/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __ASSERT_NO_MSG(test)       BT_LE_ASSERT(test)

/* fmt is dropped as before; BT_LE_ASSERT logs the stringified test instead. */
#define __ASSERT(test, fmt, ...)    BT_LE_ASSERT(test)

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_SYS_ASSERT_H_ */
