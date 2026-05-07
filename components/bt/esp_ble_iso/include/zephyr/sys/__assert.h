/*
 * SPDX-FileCopyrightText: 2011-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_SYS_ASSERT_H_
#define ZEPHYR_INCLUDE_SYS_ASSERT_H_

#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __ASSERT_NO_MSG(test)       assert(test)

#define __ASSERT(test, fmt, ...)    assert(test)

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_SYS_ASSERT_H_ */
