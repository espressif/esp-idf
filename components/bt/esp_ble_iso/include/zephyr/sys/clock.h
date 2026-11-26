/*
 * SPDX-FileCopyrightText: 2014-2015 Wind River Systems, Inc.
 * SPDX-FileCopyrightText: 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief System clock time units
 */

#ifndef ZEPHYR_INCLUDE_SYS_CLOCK_H_
#define ZEPHYR_INCLUDE_SYS_CLOCK_H_

/** number of microseconds per millisecond */
#define USEC_PER_MSEC 1000U

/* Signed, unlike upstream: K_SECONDS() and its users rely on int arithmetic */
/** number of milliseconds per second */
#define MSEC_PER_SEC 1000

/** number of microseconds per second */
#define USEC_PER_SEC ((USEC_PER_MSEC) * (MSEC_PER_SEC))

#endif /* ZEPHYR_INCLUDE_SYS_CLOCK_H_ */
