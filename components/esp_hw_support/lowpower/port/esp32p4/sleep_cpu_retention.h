/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SLEEP_CPU_RETENTION_H__
#define __SLEEP_CPU_RETENTION_H__

#include "rvsleep-frames.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
#include <stdatomic.h>
#include "soc/hp_system_reg.h"
typedef enum {
    SMP_IDLE,
    SMP_BACKUP_START,
    SMP_BACKUP_DONE,
    SMP_RESTORE_START,
    SMP_RESTORE_DONE,
    SMP_SKIP_RETENTION,
} smp_retention_state_t;
#endif

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
        RvCoreCriticalSleepFrame *critical_frame[portNUM_PROCESSORS];
        RvCoreNonCriticalSleepFrame *non_critical_frame[portNUM_PROCESSORS];
        cpu_domain_dev_sleep_frame_t *clic_frame[portNUM_PROCESSORS];
    } retent;
} sleep_cpu_retention_t;

#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
    esp_err_t esp_sleep_cpu_retention_init_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr, smp_retention_state_t *s_smp_retention_state);
#else
    esp_err_t esp_sleep_cpu_retention_init_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr);
#endif

esp_err_t esp_sleep_cpu_retention_deinit_impl(sleep_cpu_retention_t *sleep_cpu_retention_ptr);

#endif /* #ifndef __SLEEP_CPU_RETENTION_H__ */
