// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdbool.h>

#include "eri.h"
#include "xtensa-debug-module.h"

// Low-level Xtensa TRAX utils

/**
 * @brief  Start a Trax trace on the current CPU with instructions as unit of delay.
 * Memory blocks to be used as Trax memory must be enabled before
 * calling this function, if needed.
 */
void xt_trax_start_trace_instructions(void);

/**
 * @brief  Start a Trax trace on the current CPU with words as unit of delay.
 * Memory blocks to be used as Trax memory must be enabled before
 * calling this function, if needed.
 */
void xt_trax_start_trace_words(void);

/**
 * @brief Check if Trax trace is active on current CPU.
 *
 * @return bool. Return true if trace is active.
 */
bool xt_trax_trace_is_active(void);

/**
 * @brief  Trigger a Trax trace stop after the indicated delay. If this is called
 *         before and the previous delay hasn't ended yet, this will overwrite
 *         that delay with the new value. The delay will always start at the time
 *         the function is called.
 *
 * @param  delay : The delay to stop the trace in, in the unit indicated to
 *              trax_start_trace. Note: the trace memory has 4K words available.
 */
void xt_trax_trigger_traceend_after_delay(int delay);
