/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_check.h"
#include "soc/clic_reg.h"

#include "rvsleep-frames.h"
#include "sleep_cpu_retention.h"


extern RvCoreCriticalSleepFrame *rv_core_critical_regs_frame[portNUM_PROCESSORS];

#define R_CONCAT(s1, s2)    _R_CONCAT(s1, s2)
#define _R_CONCAT(s1, s2)   s1 ## s2

#define CPU_DOMAIN_DEV_SZ0 (CPU_DOMAIN_DEV_END_ADDR0 - CPU_DOMAIN_DEV_START_ADDR0)
#define CPU_DOMAIN_DEV_SZ1 (CPU_DOMAIN_DEV_END_ADDR1 - CPU_DOMAIN_DEV_START_ADDR1)
#define CPU_DOMAIN_DEV_SZ2 (CPU_DOMAIN_DEV_END_ADDR2 - CPU_DOMAIN_DEV_START_ADDR2)
#define CPU_DOMAIN_DEV_SZ3 (CPU_DOMAIN_DEV_END_ADDR3 - CPU_DOMAIN_DEV_START_ADDR3)
#define CPU_DOMAIN_DEV_SZ4 (CPU_DOMAIN_DEV_END_ADDR4 - CPU_DOMAIN_DEV_START_ADDR4)
#define CPU_DOMAIN_DEV_SZ5 (CPU_DOMAIN_DEV_END_ADDR5 - CPU_DOMAIN_DEV_START_ADDR5)
#define CPU_DOMAIN_DEV_SZ6 (CPU_DOMAIN_DEV_END_ADDR6 - CPU_DOMAIN_DEV_START_ADDR6)
#define CPU_DOMAIN_DEV_SZ7 (CPU_DOMAIN_DEV_END_ADDR7 - CPU_DOMAIN_DEV_START_ADDR7)

#define CPU_DOMAIN_DEV_TOTAL_SZ(n)    (R_CONCAT(__TOTAL_SZ, n))

#define __TOTAL_SZ0 (sizeof(cpu_domain_dev_sleep_frame_t))
#define __TOTAL_SZ1 ((__TOTAL_SZ0) + (sizeof(cpu_domain_dev_regs_region_t)) + CPU_DOMAIN_DEV_SZ0)
#define __TOTAL_SZ2 ((__TOTAL_SZ1) + (sizeof(cpu_domain_dev_regs_region_t)) + CPU_DOMAIN_DEV_SZ1)
#define __TOTAL_SZ3 ((__TOTAL_SZ2) + (sizeof(cpu_domain_dev_regs_region_t)) + CPU_DOMAIN_DEV_SZ2)
#define __TOTAL_SZ4 ((__TOTAL_SZ3) + (sizeof(cpu_domain_dev_regs_region_t)) + CPU_DOMAIN_DEV_SZ3)
#define __TOTAL_SZ5 ((__TOTAL_SZ4) + (sizeof(cpu_domain_dev_regs_region_t)) + CPU_DOMAIN_DEV_SZ4)
#define __TOTAL_SZ6 ((__TOTAL_SZ5) + (sizeof(cpu_domain_dev_regs_region_t)) + CPU_DOMAIN_DEV_SZ5)
#define __TOTAL_SZ7 ((__TOTAL_SZ6) + (sizeof(cpu_domain_dev_regs_region_t)) + CPU_DOMAIN_DEV_SZ6)
#define __TOTAL_SZ8 ((__TOTAL_SZ7) + (sizeof(cpu_domain_dev_regs_region_t)) + CPU_DOMAIN_DEV_SZ7)


static void * cpu_domain_dev_sleep_frame_alloc_and_init(const cpu_domain_dev_regs_region_t *regions, const int region_num, void * frame)
{
    const int region_sz = sizeof(cpu_domain_dev_regs_region_t) * region_num;
    int regs_frame_sz = 0;
    for (int num = 0; num < region_num; num++) {
        regs_frame_sz += regions[num].end - regions[num].start;
    }
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

#undef CPU_DOMAIN_DEV_START_ADDR0
#undef CPU_DOMAIN_DEV_END_ADDR0
#undef CPU_DOMAIN_DEV_START_ADDR1
#undef CPU_DOMAIN_DEV_END_ADDR1

#define CPU_DOMAIN_DEV_START_ADDR0 (CLIC_INT_CONFIG_REG)
#define CPU_DOMAIN_DEV_END_ADDR0 (CLIC_INT_THRESH_REG + 4)
#define CPU_DOMAIN_DEV_START_ADDR1 (CLIC_INT_CTRL_REG(0))
#define CPU_DOMAIN_DEV_END_ADDR1 (CLIC_INT_CTRL_REG(47) + 4)

static inline void * cpu_domain_clic_sleep_frame_alloc_and_init(uint8_t core_id)
{
    static DRAM_ATTR cpu_domain_dev_regs_region_t regions[portNUM_PROCESSORS][2] = {
       [0 ... portNUM_PROCESSORS - 1] = {
            { .start = CPU_DOMAIN_DEV_START_ADDR0, .end = CPU_DOMAIN_DEV_END_ADDR0 },
            { .start = CPU_DOMAIN_DEV_START_ADDR1, .end = CPU_DOMAIN_DEV_END_ADDR1 },
        }
    };
    static DRAM_ATTR uint8_t sleep_frame[portNUM_PROCESSORS][CPU_DOMAIN_DEV_TOTAL_SZ(2)] __attribute__((aligned(4)));
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions[core_id], sizeof(regions[core_id]) / sizeof(regions[core_id][0]), sleep_frame[core_id]);
}

#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
esp_err_t esp_sleep_cpu_retention_init_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr, smp_retention_state_t *s_smp_retention_state)
{
    static DRAM_ATTR uint8_t rv_core_critical_regs[RV_SLEEP_CTX_FRMSZ * portNUM_PROCESSORS] __attribute__((aligned(4)));
    static DRAM_ATTR uint8_t rv_core_non_critical_regs[sizeof(RvCoreNonCriticalSleepFrame)* portNUM_PROCESSORS] __attribute__((aligned(4)));
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.critical_frame[core_id] == NULL) {
            void * regs = rv_core_critical_regs + core_id * RV_SLEEP_CTX_FRMSZ;
            sleep_cpu_retention_ptr->retent.critical_frame[core_id] = (RvCoreCriticalSleepFrame *) regs;
            rv_core_critical_regs_frame[core_id] = (RvCoreCriticalSleepFrame *) regs;
        }
        if (sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] == NULL) {
            sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] = (RvCoreNonCriticalSleepFrame *)(rv_core_non_critical_regs+core_id * sizeof(RvCoreNonCriticalSleepFrame));
        }
    }

    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.clic_frame[core_id] == NULL) {
            void *frame = cpu_domain_clic_sleep_frame_alloc_and_init(core_id);
            sleep_cpu_retention_ptr->retent.clic_frame[core_id] = (cpu_domain_dev_sleep_frame_t *)frame;
        }
    }
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        atomic_init(&s_smp_retention_state[core_id], SMP_IDLE);
    }
    return ESP_OK;
}
#else
esp_err_t esp_sleep_cpu_retention_init_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr)
{
    static DRAM_ATTR uint8_t rv_core_critical_regs[RV_SLEEP_CTX_FRMSZ * portNUM_PROCESSORS] __attribute__((aligned(4)));
    static DRAM_ATTR uint8_t rv_core_non_critical_regs[sizeof(RvCoreNonCriticalSleepFrame)* portNUM_PROCESSORS] __attribute__((aligned(4)));
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.critical_frame[core_id] == NULL) {
            void * regs = rv_core_critical_regs + core_id * RV_SLEEP_CTX_FRMSZ;
            sleep_cpu_retention_ptr->retent.critical_frame[core_id] = (RvCoreCriticalSleepFrame *) regs;
            rv_core_critical_regs_frame[core_id] = (RvCoreCriticalSleepFrame *) regs;
        }
        if (sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] == NULL) {
            sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] = (RvCoreNonCriticalSleepFrame *)(rv_core_non_critical_regs+core_id * sizeof(RvCoreNonCriticalSleepFrame));
        }
    }

    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.clic_frame[core_id] == NULL) {
            void *frame = cpu_domain_clic_sleep_frame_alloc_and_init(core_id);
            sleep_cpu_retention_ptr->retent.clic_frame[core_id] = (cpu_domain_dev_sleep_frame_t *)frame;
        }
    }
    return ESP_OK;
}
#endif

esp_err_t esp_sleep_cpu_retention_deinit_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr)
{
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.critical_frame[core_id]) {
            sleep_cpu_retention_ptr->retent.critical_frame[core_id] = NULL;
            rv_core_critical_regs_frame[core_id] = NULL;
        }
        if (sleep_cpu_retention_ptr->retent.non_critical_frame[core_id]) {
            sleep_cpu_retention_ptr->retent.non_critical_frame[core_id] = NULL;
        }
    }
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (sleep_cpu_retention_ptr->retent.clic_frame[core_id]) {
            sleep_cpu_retention_ptr->retent.clic_frame[core_id] = NULL;
        }
    }
    return ESP_OK;
}
