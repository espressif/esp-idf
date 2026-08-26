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

#include <stdio.h>
#include <stdbool.h>

#include "esp_err.h"

#include "xtensa-debug-module.h"
#include "eri.h"

bool xt_trax_trace_is_active(void)
{
    return eri_read(ERI_TRAX_TRAXSTAT)&TRAXSTAT_TRACT;
}

static void _xt_trax_start_trace(bool instructions)
{
    uint32_t v;
    eri_write(ERI_TRAX_PCMATCHCTRL, 31); //do not stop at any pc match
    v=TRAXCTRL_TREN | TRAXCTRL_TMEN | TRAXCTRL_PTOWS | (1<<TRAXCTRL_SMPER_SHIFT);
    if (instructions) {
        v|=TRAXCTRL_CNTU;
    }
    //Enable trace. This trace has no stop condition and will just keep on running.
    eri_write(ERI_TRAX_TRAXCTRL, v);
}

void xt_trax_start_trace_instructions(void)
{
    _xt_trax_start_trace(true);
}

void xt_trax_start_trace_words(void)
{
    _xt_trax_start_trace(false);
}

void xt_trax_trigger_traceend_after_delay(int delay)
{
    eri_write(ERI_TRAX_DELAYCNT, delay);
    eri_write(ERI_TRAX_TRAXCTRL, eri_read(ERI_TRAX_TRAXCTRL)|TRAXCTRL_TRSTP);
    //ToDo: This will probably trigger a trace done interrupt. ToDo: Fix, but how? -JD
    eri_write(ERI_TRAX_TRAXCTRL, 0);
}
