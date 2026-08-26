/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SLEEP_CPU_RETENTION_H__
#define __SLEEP_CPU_RETENTION_H__

#include "rvsleep-frames.h"
#include "esp_err.h"


typedef struct {
    uint32_t start;
    uint32_t end;
} cpu_domain_dev_regs_region_t;

typedef struct {
    cpu_domain_dev_regs_region_t *region;
    int region_num;
    uint32_t *regs_frame;
} cpu_domain_dev_sleep_frame_t;

/**
 * Internal structure which holds all requested light sleep cpu retention parameters
 */
typedef struct {
    struct {
        RvCoreCriticalSleepFrame *critical_frame;
        RvCoreNonCriticalSleepFrame *non_critical_frame;
        cpu_domain_dev_sleep_frame_t *intpri_frame;
        cpu_domain_dev_sleep_frame_t *cache_config_frame;
        cpu_domain_dev_sleep_frame_t *plic_frame;
        cpu_domain_dev_sleep_frame_t *clint_frame;
    } retent;
} sleep_cpu_retention_t;

esp_err_t esp_sleep_cpu_retention_init_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr);
esp_err_t esp_sleep_cpu_retention_deinit_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr);

#endif /* #ifndef __SLEEP_CPU_RETENTION_H__ */
