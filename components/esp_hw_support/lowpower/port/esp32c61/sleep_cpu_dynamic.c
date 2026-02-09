/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_check.h"
#include "esp_heap_caps.h"

#include "soc/intpri_reg.h"
#include "soc/cache_reg.h"
#include "soc/clic_reg.h"
#include "soc/clint_reg.h"
#include "esp_sleep.h"

#include "rvsleep-frames.h"
#include "sleep_cpu_retention.h"


extern RvCoreCriticalSleepFrame *rv_core_critical_regs_frame;

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

static inline void * cpu_domain_cache_config_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = CACHE_L1_CACHE_AUTOLOAD_CTRL_REG, .end = CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_REG + 4 },
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static inline void * cpu_domain_clint_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = CLINT_MINT_SIP_REG,        .end = CLINT_MINT_SIP_REG + 4        },
        { .start = CLINT_MINT_MTIMECMP_L_REG, .end = CLINT_MINT_MTIMECMP_H_REG + 4 },
        { .start = CLINT_MINT_TIMECTL_REG,    .end = CLINT_MINT_TIMECTL_REG + 4    },
        { .start = CLINT_MINT_MTIME_L_REG,    .end = CLINT_MINT_MTIME_H_REG + 4    }
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static inline void * cpu_domain_clic_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = CLIC_INT_CONFIG_REG, .end = CLIC_INT_THRESH_REG + 4 },
        { .start = CLIC_INT_CTRL_REG(0), .end = CLIC_INT_CTRL_REG(47) + 4 },
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

esp_err_t esp_sleep_cpu_retention_init_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr)
{
    if (sleep_cpu_retention_ptr->retent.critical_frame == NULL) {
        void *frame = heap_caps_calloc(1, RV_SLEEP_CTX_FRMSZ, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
        if (frame == NULL) {
            goto err;
        }
        sleep_cpu_retention_ptr->retent.critical_frame = (RvCoreCriticalSleepFrame *)frame;
        rv_core_critical_regs_frame = (RvCoreCriticalSleepFrame *)frame;
    }
    if (sleep_cpu_retention_ptr->retent.non_critical_frame == NULL) {
        void *frame = heap_caps_calloc(1, sizeof(RvCoreNonCriticalSleepFrame), MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
        if (frame == NULL) {
            goto err;
        }
        sleep_cpu_retention_ptr->retent.non_critical_frame = (RvCoreNonCriticalSleepFrame *)frame;
    }
    if (sleep_cpu_retention_ptr->retent.cache_config_frame == NULL) {
        void *frame = cpu_domain_cache_config_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        sleep_cpu_retention_ptr->retent.cache_config_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    if (sleep_cpu_retention_ptr->retent.clic_frame == NULL) {
        void *frame = cpu_domain_clic_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        sleep_cpu_retention_ptr->retent.clic_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    if (sleep_cpu_retention_ptr->retent.clint_frame == NULL) {
        void *frame = cpu_domain_clint_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        sleep_cpu_retention_ptr->retent.clint_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    return ESP_OK;
err:
    esp_sleep_cpu_retention_deinit();
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_sleep_cpu_retention_deinit_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr)
{
    if (sleep_cpu_retention_ptr->retent.critical_frame) {
        heap_caps_free((void *)sleep_cpu_retention_ptr->retent.critical_frame);
        sleep_cpu_retention_ptr->retent.critical_frame = NULL;
        rv_core_critical_regs_frame = NULL;
    }
    if (sleep_cpu_retention_ptr->retent.non_critical_frame) {
        heap_caps_free((void *)sleep_cpu_retention_ptr->retent.non_critical_frame);
        sleep_cpu_retention_ptr->retent.non_critical_frame = NULL;
    }
    if (sleep_cpu_retention_ptr->retent.cache_config_frame) {
        heap_caps_free((void *)sleep_cpu_retention_ptr->retent.cache_config_frame);
        sleep_cpu_retention_ptr->retent.cache_config_frame = NULL;
    }
    if (sleep_cpu_retention_ptr->retent.clic_frame) {
        heap_caps_free((void *)sleep_cpu_retention_ptr->retent.clic_frame);
        sleep_cpu_retention_ptr->retent.clic_frame = NULL;
    }
    if (sleep_cpu_retention_ptr->retent.clint_frame) {
        heap_caps_free((void *)sleep_cpu_retention_ptr->retent.clint_frame);
        sleep_cpu_retention_ptr->retent.clint_frame = NULL;
    }
    return ESP_OK;
}
