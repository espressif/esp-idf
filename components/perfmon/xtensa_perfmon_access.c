// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "xtensa_perfmon_access.h"
#include "xtensa-debug-module.h"
#include "eri.h"


esp_err_t xtensa_perfmon_init(int id, uint16_t select, uint16_t mask, int kernelcnt, int tracelevel)
{
    if (id >= ERI_PERFMON_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t pmc =
        ((tracelevel & PMCTRL_TRACELEVEL_MASK) << PMCTRL_TRACELEVEL_SHIFT) |
        ((select & PMCTRL_SELECT_MASK) << PMCTRL_SELECT_SHIFT) |
        ((mask & PMCTRL_MASK_MASK) << PMCTRL_MASK_SHIFT) |
        ((kernelcnt & 1) << PMCTRL_KRNLCNT_SHIFT);

    eri_write(ERI_PERFMON_PM0 + id * sizeof(int32_t), 0);
    eri_write(ERI_PERFMON_PMCTRL0 + id * sizeof(int32_t), pmc);
    return ESP_OK;
}

void xtensa_perfmon_dump(void)
{
    uint32_t pgm = eri_read(ERI_PERFMON_PGM);
    uint32_t intpc = eri_read(ERI_PERFMON_INTPC);
    printf("perfmon: PGM=%08x, INTPC=%08x\n", pgm, intpc);

    for (int i = 0 ; i < ERI_PERFMON_MAX ; i++) {
        uint32_t pm = eri_read(ERI_PERFMON_PM0 + i * sizeof(int32_t));
        uint32_t pmctrl = eri_read(ERI_PERFMON_PMCTRL0 + i * sizeof(int32_t));
        uint32_t pmstat = eri_read(ERI_PERFMON_PMSTAT0 + i * sizeof(int32_t));
        printf("perfmon: pm%i=%08x, pmctrl%i=%08x, pmstat%i=%08x\n", i, pm, i, pmctrl, i, pmstat);
    }
}

esp_err_t xtensa_perfmon_reset(int id)
{
    if (id >= ERI_PERFMON_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    eri_write(ERI_PERFMON_PM0 + id * sizeof(int32_t), 0);
    return ESP_OK;
}

void xtensa_perfmon_start(void)
{
    eri_write(ERI_PERFMON_PGM, PGM_PMEN);
}

void xtensa_perfmon_stop(void)
{
    eri_write(ERI_PERFMON_PGM, 0);
}

uint32_t xtensa_perfmon_value(int id)
{
    if (id >= ERI_PERFMON_MAX) {
        return 0;
    }
    uint32_t result = eri_read(ERI_PERFMON_PM0 + id * sizeof(int32_t));
    return result;
}

esp_err_t xtensa_perfmon_overflow(int id)
{
    if (id >= ERI_PERFMON_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t result = eri_read(ERI_PERFMON_PMSTAT0 + id * sizeof(int32_t));
    if (result & 1) {
        return ESP_FAIL;
    }
    return ESP_OK;
}
