/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <assert.h>
#include "soc/soc_caps.h"
#include "esp_cpu.h"

/* Xtensa core has 3 interrupts dedicated to timers, we can use either timer0 or timer1 depending on the Kconfig,
 * timer2 is always set to SPECIAL in our configuration array */

/**
 * @brief Type defined for the table below
 */
typedef struct {
    int priority;
    esp_cpu_intr_type_t type;
    uint32_t flags;
} intr_desc_t;


const static intr_desc_t intr_desc_table [SOC_CPU_INTR_NUM] = {
    /* Interrupt 0 reserved for WMAC (Wifi) */
    [0] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  ESP_CPU_INTR_DESC_FLAG_RESVD   },
    [1] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  0                              },
    [2] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  0                              },
    [3] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  0                              },
    /* Interrupt 4 reserved for WBB */
    [4] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  ESP_CPU_INTR_DESC_FLAG_RESVD   },
    [5] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  0                              },
#if CONFIG_FREERTOS_CORETIMER_0
    [6] = { 1, ESP_CPU_INTR_TYPE_NA,     ESP_CPU_INTR_DESC_FLAG_RESVD   },
#else
    [6] = { 1, ESP_CPU_INTR_TYPE_NA,     ESP_CPU_INTR_DESC_FLAG_SPECIAL },
#endif
    [7] = { 1, ESP_CPU_INTR_TYPE_NA,     ESP_CPU_INTR_DESC_FLAG_SPECIAL },
    [8] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  0                              },
    [9] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  0                              },
    [10] = { 1, ESP_CPU_INTR_TYPE_EDGE,  0                              },
    [11] = { 3, ESP_CPU_INTR_TYPE_NA,    ESP_CPU_INTR_DESC_FLAG_SPECIAL },
    [12] = { 1, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    [13] = { 1, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    /* Interrupt 14 reserved for NMI (Non-Maskable Interrupts) */
    [14] = { 7, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
#if CONFIG_FREERTOS_CORETIMER_1
    [15] = { 3, ESP_CPU_INTR_TYPE_NA,    ESP_CPU_INTR_DESC_FLAG_RESVD   },
#else
    [15] = { 3, ESP_CPU_INTR_TYPE_NA,    ESP_CPU_INTR_DESC_FLAG_SPECIAL },
#endif
    [16] = { 5, ESP_CPU_INTR_TYPE_NA,    ESP_CPU_INTR_DESC_FLAG_SPECIAL },
    [17] = { 1, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    [18] = { 1, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    [19] = { 2, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    [20] = { 2, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    [21] = { 2, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    [22] = { 3, ESP_CPU_INTR_TYPE_EDGE,  0                              },
    [23] = { 3, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    /* Interrupt 24 reserved for T1 WDT */
    [24] = { 4, ESP_CPU_INTR_TYPE_LEVEL, ESP_CPU_INTR_DESC_FLAG_RESVD   },
    /* Interrupt 25 reserved for memory access errors */
    [25] = { 4, ESP_CPU_INTR_TYPE_LEVEL, ESP_CPU_INTR_DESC_FLAG_RESVD   },
    [26] = { 5, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    [27] = { 3, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
    [28] = { 4, ESP_CPU_INTR_TYPE_EDGE,  0                              },
    [29] = { 3, ESP_CPU_INTR_TYPE_NA,    ESP_CPU_INTR_DESC_FLAG_SPECIAL },
    [30] = { 4, ESP_CPU_INTR_TYPE_EDGE,  0                              },
    [31] = { 5, ESP_CPU_INTR_TYPE_LEVEL, 0                              },
};


void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret)
{
    assert(core_id == 0 && intr_num < SOC_CPU_INTR_NUM && intr_desc_ret != NULL);
    intr_desc_ret->priority = intr_desc_table[intr_num].priority;
    intr_desc_ret->type = intr_desc_table[intr_num].type;
    intr_desc_ret->flags = intr_desc_table[intr_num].flags;
}
