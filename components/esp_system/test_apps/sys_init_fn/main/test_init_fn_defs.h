/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Maximum number of init function invocations we track for ordering tests.
 */
#define INIT_FN_TRACE_MAX  16

/**
 * Global trace log filled by init functions to record their execution order.
 * Each init function appends its own tag (an arbitrary integer) to
 * trace_log[trace_count] and increments trace_count.
 */
extern int trace_log[INIT_FN_TRACE_MAX];
extern int trace_count;

/* Flags set by individual init functions so tests can verify they executed */
extern bool core_prio_200_executed;
extern bool core_prio_250_executed;
extern bool secondary_prio_200_executed;
extern bool secondary_prio_250_executed;
