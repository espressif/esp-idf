/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_check.h"
#include "esp_heap_caps.h"

#include "soc/clic_reg.h"
#include "esp_sleep.h"

#include "rvsleep-frames.h"
#include "sleep_cpu_retention.h"


extern RvCoreCriticalSleepFrame *rv_core_critical_regs_frame[portNUM_PROCESSORS];

static void * cpu_domain_dev_sleep_frame_alloc_and_init(const cpu_domain_dev_regs_region_t *regions, const int region_num)
{
    const int region_sz = sizeof(cpu_domain_dev_regs_region_t) * region_num;
    int regs_frame_sz = 0;
    for (int num = 0; num < region_num; num++) {
        regs_frame_sz += regions[num].end - regions[num].start;
    }
    void *frame = heap_caps_malloc(sizeof(cpu_domain_dev_sleep_frame_t) + region_sz + regs_frame_sz, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
    if (frame) {
        cpu_domain_dev_regs_region_t *region = (cpu_domain_dev_regs_region_t *)(frame + sizeof(cpu_domain_dev_sleep_frame_t));
        memcpy(region, regions, region_num * sizeof(cpu_domain_dev_regs_region_t));
        void *regs_frame = frame + sizeof(cpu_domain_dev_sleep_frame_t) + region_sz;
        memset(regs_frame, 0, regs_frame_sz);
        *(cpu_domain_dev_sleep_frame_t *)frame = (cpu_domain_dev_sleep_frame_t) {
            .region = region,
            .region_num = region_num,
            .regs_frame = (uint32_t *)regs_frame
        };
    }
    return frame;
}

static inline void * cpu_domain_clic_sleep_frame_alloc_and_init(uint8_t core_id)
{
    const static cpu_domain_dev_regs_region_t regions[portNUM_PROCESSORS][2] = {
       [0 ... portNUM_PROCESSORS - 1] = {
            { .start = CLIC_INT_CONFIG_REG, .end = CLIC_INT_THRESH_REG + 4 },
            { .start = CLIC_INT_CTRL_REG(0), .end = CLIC_INT_CTRL_REG(47) + 4 },
        }
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions[core_id], sizeof(regions[core_id]) / sizeof(cpu_domain_dev_regs_region_t));
}

#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
esp_err_t esp_sleep_cpu_retention_init_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr, smp_retention_state_t *s_smp_retention_state)
{
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.critical_frame[core_id] == NULL) {
            void *frame = heap_caps_calloc(1, RV_SLEEP_CTX_FRMSZ, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
            if (frame == NULL) {
                goto err;
            }
            sleep_cpu_retention_ptr->retent.critical_frame[core_id] = (RvCoreCriticalSleepFrame *)frame;
            rv_core_critical_regs_frame[core_id] = (RvCoreCriticalSleepFrame *)frame;
        }
        if (sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] == NULL) {
            void *frame = heap_caps_calloc(1, sizeof(RvCoreNonCriticalSleepFrame), MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
            if (frame == NULL) {
                goto err;
            }
            sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] = (RvCoreNonCriticalSleepFrame *)frame;
        }
    }
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.clic_frame[core_id] == NULL) {
            void *frame = cpu_domain_clic_sleep_frame_alloc_and_init(core_id);
            if (frame == NULL) {
                goto err;
            }
            sleep_cpu_retention_ptr->retent.clic_frame[core_id] = (cpu_domain_dev_sleep_frame_t *)frame;
        }
    }
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        atomic_init(&s_smp_retention_state[core_id], SMP_IDLE);
    }
    return ESP_OK;
err:
    esp_sleep_cpu_retention_deinit();
    return ESP_ERR_NO_MEM;
}
#else
esp_err_t esp_sleep_cpu_retention_init_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr)
{
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.critical_frame[core_id] == NULL) {
            void *frame = heap_caps_calloc(1, RV_SLEEP_CTX_FRMSZ, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
            if (frame == NULL) {
                goto err;
            }
            sleep_cpu_retention_ptr->retent.critical_frame[core_id] = (RvCoreCriticalSleepFrame *)frame;
            rv_core_critical_regs_frame[core_id] = (RvCoreCriticalSleepFrame *)frame;
        }
        if (sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] == NULL) {
            void *frame = heap_caps_calloc(1, sizeof(RvCoreNonCriticalSleepFrame), MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
            if (frame == NULL) {
                goto err;
            }
            sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] = (RvCoreNonCriticalSleepFrame *)frame;
        }
    }
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.clic_frame[core_id] == NULL) {
            void *frame = cpu_domain_clic_sleep_frame_alloc_and_init(core_id);
            if (frame == NULL) {
                goto err;
            }
            sleep_cpu_retention_ptr->retent.clic_frame[core_id] = (cpu_domain_dev_sleep_frame_t *)frame;
        }
    }
    return ESP_OK;
err:
    esp_sleep_cpu_retention_deinit();
    return ESP_ERR_NO_MEM;
}
#endif

esp_err_t esp_sleep_cpu_retention_deinit_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr)
{
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.critical_frame[core_id]) {
            heap_caps_free((void *)sleep_cpu_retention_ptr->retent.critical_frame[core_id]);
            sleep_cpu_retention_ptr->retent.critical_frame[core_id] = NULL;
            rv_core_critical_regs_frame[core_id] = NULL;
        }
        if (sleep_cpu_retention_ptr->retent.non_critical_frame[core_id]) {
            heap_caps_free((void *)sleep_cpu_retention_ptr->retent.non_critical_frame[core_id]);
            sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] = NULL;
        }
    }
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.clic_frame[core_id]) {
            heap_caps_free((void *)sleep_cpu_retention_ptr->retent.clic_frame[core_id]);
            sleep_cpu_retention_ptr->retent.clic_frame[core_id] = NULL;
        }
    }
    return ESP_OK;
}
