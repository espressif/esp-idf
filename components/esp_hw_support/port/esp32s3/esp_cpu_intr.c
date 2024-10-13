/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_cpu.h"

/* The ESP32-S3 uses the SysTimer for the FreeRTOS system tick, there is no need to use Xtensa core timer interrupts,
 * marked as ESP_CPU_INTR_DESC_FLAG_SPECIAL in the table below */

/**
 * @brief Type defined for the table below
 */
typedef struct {
    int priority;
    esp_cpu_intr_type_t type;
    uint32_t flags[SOC_CPU_CORES_NUM];
} intr_desc_t;


const static intr_desc_t intr_desc_table [SOC_CPU_INTR_NUM] = {
    /* Interrupt 0 reserved for WMAC (Wifi) */
#if CONFIG_ESP_WIFI_TASK_PINNED_TO_CORE_0
    [0] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { ESP_CPU_INTR_DESC_FLAG_RESVD,   0                               } },
#else
    [0] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              ESP_CPU_INTR_DESC_FLAG_RESVD    } },
#endif
    [1] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    [2] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    [3] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    /* Interrupt 4 reserved for WBB */
    [4] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { ESP_CPU_INTR_DESC_FLAG_RESVD,   0                               } },
    [5] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    [6] = { 1, ESP_CPU_INTR_TYPE_NA,     { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [7] = { 1, ESP_CPU_INTR_TYPE_NA,     { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [8] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    [9] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    [10] = { 1, ESP_CPU_INTR_TYPE_EDGE,  { 0,                              0                               } },
    [11] = { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [12] = { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [13] = { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    /* Interrupt 14 reserved for NMI (Non-Maskable Interrupts) */
    [14] = { 7, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, // NMI
    [15] = { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [16] = { 5, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [17] = { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [18] = { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [19] = { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [20] = { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [21] = { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [22] = { 3, ESP_CPU_INTR_TYPE_EDGE,  { 0,                              0                               } },
    [23] = { 3, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    /* Interrupt 24 reserved for T1 WDT */
    [24] = { 4, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } },
    /* Interrupt 25 reserved for memory access and cache errors */
    [25] = { 4, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } },
    [26] = { 5, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [27] = { 3, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    /* Interrupt 28 reserved for IPC */
    [28] = { 4, ESP_CPU_INTR_TYPE_EDGE,  { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } },
    [29] = { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [30] = { 4, ESP_CPU_INTR_TYPE_EDGE,  { 0,                              0                               } },
    [31] = { 5, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
};


void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret)
{
    assert(core_id >= 0 && core_id < SOC_CPU_CORES_NUM && intr_desc_ret != NULL);
    intr_desc_ret->priority = intr_desc_table[intr_num].priority;
    intr_desc_ret->type = intr_desc_table[intr_num].type;
    intr_desc_ret->flags = intr_desc_table[intr_num].flags[core_id];
}
