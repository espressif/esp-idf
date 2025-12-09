/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/timer_types.h"
#include "hal/timg_ll.h"
#include "soc/timer_group_struct.h"

#define TIMER_LL_GPTIMERS_TOTAL     (TIMG_LL_INST_NUM * TIMG_LL_GPTIMERS_PER_INST)

#define TIMER_LL_GET_HW(group_id) ((group_id == 0) ? (&TIMERG0) : (&TIMERG1))

// Bit width of GPTIMER counter
#define TIMER_LL_COUNTER_BIT_WIDTH   54

// Get alarm interrupt mask with the given timer ID
#define TIMER_LL_EVENT_ALARM(timer_id) (1 << (timer_id))

// Support RC_FAST as function clock
#define TIMER_LL_FUNC_CLOCK_SUPPORT_RC_FAST 1
