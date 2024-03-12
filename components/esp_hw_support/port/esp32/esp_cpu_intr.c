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
    uint32_t flags[SOC_CPU_CORES_NUM];
} intr_desc_t;


/**
 * @brief Reserve interrupt 5 or 25 for Bluetooth BR/EDR and BLE controller.
 */
#if CONFIG_BTDM_CTRL_HLI
    #define STATE_INTERRUPT_5    0
    #define STATE_INTERRUPT_25   ESP_CPU_INTR_DESC_FLAG_RESVD
#else // !CONFIG_BTDM_CTRL_HLI
    #define STATE_INTERRUPT_5    ESP_CPU_INTR_DESC_FLAG_RESVD
    #define STATE_INTERRUPT_25   0
#endif // CONFIG_BTDM_CTRL_HLI


/**
 * @brief Interrupt 1 is used by UART HCI, regardless of High-Level Interrupt (HLI) configuration
 */
#if CONFIG_BTDM_CTRL_HCI_MODE_UART_H4
    #define STATE_INTERRUPT_1   ESP_CPU_INTR_DESC_FLAG_RESVD
    /* Interrupt 7 being a software interrupt, it is marked as "special" if not used */
    #define STATE_INTERRUPT_7   ESP_CPU_INTR_DESC_FLAG_SPECIAL
#else // !BTDM_CTRL_HCI_MODE_UART_H4
    #define STATE_INTERRUPT_1   0
    #define STATE_INTERRUPT_7   ESP_CPU_INTR_DESC_FLAG_RESVD
#endif // BTDM_CTRL_HCI_MODE_UART_H4


/**
 * @brief Reserve the interrupts on the core where Bluetooth will run.
 * The macro CONFIG_BTDM_CTRL_PINNED_TO_CORE is only defined if Bluetooth controller is enabled.
 * It is set to the core where it will run.
 */
#ifdef CONFIG_BTDM_CTRL_PINNED_TO_CORE
    #if CONFIG_BTDM_CTRL_PINNED_TO_CORE == 0
        /* Interrupt 1 is used by Bluetooth UART HCI, check code above */
        #define CORE_0_INTERRUPT_1  STATE_INTERRUPT_1
        #define CORE_1_INTERRUPT_1  0
        /* Interrupt 5 may be used by Bluetooth BR/EDR and BLE controller */
        #define CORE_0_INTERRUPT_5  STATE_INTERRUPT_5
        #define CORE_1_INTERRUPT_5  0
        /* Interrupt 7 is used by Bluetooth VHCI software interrupt */
        #define CORE_0_INTERRUPT_7  STATE_INTERRUPT_7
        #define CORE_1_INTERRUPT_7  ESP_CPU_INTR_DESC_FLAG_SPECIAL
        /* Interrupt 8 is used by Bluetooth BB */
        #define CORE_0_INTERRUPT_8  ESP_CPU_INTR_DESC_FLAG_RESVD
        #define CORE_1_INTERRUPT_8  0
        /* Interrupt 25 may be used by Bluetooth BR/EDR and BLE controller */
        #define CORE_0_INTERRUPT_25 STATE_INTERRUPT_25
        #define CORE_1_INTERRUPT_25 0
    #elif CONFIG_BTDM_CTRL_PINNED_TO_CORE == 1
        /* Interrupt 1 is used by Bluetooth UART HCI, check code above */
        #define CORE_0_INTERRUPT_1  0
        #define CORE_1_INTERRUPT_1  STATE_INTERRUPT_1
        /* Interrupt 5 may be used by Bluetooth BR/EDR and BLE controller */
        #define CORE_0_INTERRUPT_5  0
        #define CORE_1_INTERRUPT_5  STATE_INTERRUPT_5
        /* Interrupt 7 is used by Bluetooth VHCI software interrupt */
        #define CORE_0_INTERRUPT_7  ESP_CPU_INTR_DESC_FLAG_SPECIAL
        #define CORE_1_INTERRUPT_7  STATE_INTERRUPT_7
        /* Interrupt 8 is used by Bluetooth BB */
        #define CORE_0_INTERRUPT_8  0
        #define CORE_1_INTERRUPT_8  ESP_CPU_INTR_DESC_FLAG_RESVD
        /* Interrupt 25 may be used by Bluetooth BR/EDR and BLE controller */
        #define CORE_0_INTERRUPT_25 0
        #define CORE_1_INTERRUPT_25 STATE_INTERRUPT_25
    #endif
#else // Bluetooth not enabled
    #define CORE_0_INTERRUPT_1  0
    #define CORE_1_INTERRUPT_1  0
    #define CORE_0_INTERRUPT_5  0
    #define CORE_1_INTERRUPT_5  0
    #define CORE_0_INTERRUPT_7  ESP_CPU_INTR_DESC_FLAG_SPECIAL
    #define CORE_1_INTERRUPT_7  ESP_CPU_INTR_DESC_FLAG_SPECIAL
    #define CORE_0_INTERRUPT_8  0
    #define CORE_1_INTERRUPT_8  0
    #define CORE_0_INTERRUPT_25 0
    #define CORE_1_INTERRUPT_25 0
#endif


/**
 * @brief The system interrupts (memory access, cache, watchdog, ...) can be allocated on either level 4 or level 5 interrupts.
 * Check the configuration.
 */
#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
    #define CORE_0_INTERRUPT_24 0
    #define CORE_1_INTERRUPT_24 0
    /* If CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 is enabled, Bluetooth macros above take care of interrupt 25 */
    /* Interrupt 26 reserved for T1 Watchdog, cache and memory access errors  */
    #define CORE_0_INTERRUPT_26 ESP_CPU_INTR_DESC_FLAG_RESVD
    #define CORE_1_INTERRUPT_26 ESP_CPU_INTR_DESC_FLAG_RESVD
    #define CORE_0_INTERRUPT_28 0
    #define CORE_1_INTERRUPT_28 0
    /* Interrupt 31 reserved for IPC ISRs */
    #define CORE_0_INTERRUPT_31 ESP_CPU_INTR_DESC_FLAG_RESVD
    #define CORE_1_INTERRUPT_31 ESP_CPU_INTR_DESC_FLAG_RESVD
#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4
    /* Interrupt reserved for T1 Watchdog, make sure it is enabled */
    #if CONFIG_ESP_INT_WDT
        #define CORE_0_INTERRUPT_24 ESP_CPU_INTR_DESC_FLAG_RESVD
        #define CORE_1_INTERRUPT_24 ESP_CPU_INTR_DESC_FLAG_RESVD
    #else // !CONFIG_ESP_INT_WDT
        #define CORE_0_INTERRUPT_24 0
        #define CORE_1_INTERRUPT_24 0
    #endif
    /* If CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4 is enabled, Bluetooth HLI is not enabled for sure (guaranteed by Kconfig),
     * so we can discard the macro previously defined for interrupt 25 */
    #undef CORE_0_INTERRUPT_25
    #undef CORE_1_INTERRUPT_25
    /* Interrupt reserved for memory access and cache errors */
    #define CORE_0_INTERRUPT_25 ESP_CPU_INTR_DESC_FLAG_RESVD
    #define CORE_1_INTERRUPT_25 ESP_CPU_INTR_DESC_FLAG_RESVD
    #define CORE_0_INTERRUPT_26 0
    #define CORE_1_INTERRUPT_26 0
    /* Interrupt reserved for IPC ISRs */
    #define CORE_0_INTERRUPT_28 ESP_CPU_INTR_DESC_FLAG_RESVD
    #define CORE_1_INTERRUPT_28 ESP_CPU_INTR_DESC_FLAG_RESVD
    #define CORE_0_INTERRUPT_31 0
    #define CORE_1_INTERRUPT_31 0
#endif // CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5


const static intr_desc_t intr_desc_table [SOC_CPU_INTR_NUM] = {
    /* Interrupt 0 reserved for WMAC (Wifi) */
    [0] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } },
    /* Interrupt 1 reserved for BT/BLE Host HCI DMA when CONFIG_BTDM_CTRL_HCI_MODE_UART_H4 is enabled */
    [1] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { CORE_0_INTERRUPT_1,             CORE_1_INTERRUPT_1              } },
    [2] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    [3] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    /* Interrupt 4 reserved for WBB */
    [4] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { ESP_CPU_INTR_DESC_FLAG_RESVD,   0                               } },
    /* Interrupt 5 reserved for BT/BLE Controller when Bluetooth HLI is enabled */
    [5] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { CORE_0_INTERRUPT_5,             CORE_1_INTERRUPT_5              } },
#if CONFIG_FREERTOS_CORETIMER_0
    [6] = { 1, ESP_CPU_INTR_TYPE_NA,     { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } },
#else
    [6] = { 1, ESP_CPU_INTR_TYPE_NA,     { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
#endif
    /* Interrupt 7 reserved for Bluetooth VHCI (software interrupt) */
    [7] = { 1, ESP_CPU_INTR_TYPE_NA,     { CORE_0_INTERRUPT_7,             CORE_1_INTERRUPT_7              } },
    /* Interrupt 8 reserved for BT/BLE BB(RX/TX) */
    [8] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { CORE_0_INTERRUPT_8,             CORE_1_INTERRUPT_8              } },
    [9] = { 1, ESP_CPU_INTR_TYPE_LEVEL,  { 0,                              0                               } },
    [10] = { 1, ESP_CPU_INTR_TYPE_EDGE,  { 0,                              0                               } },
    [11] = { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [12] = { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [13] = { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [14] = { 7, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, // NMI
#if CONFIG_FREERTOS_CORETIMER_1
    [15] = { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } },
#else
    [15] = { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
#endif
    [16] = { 5, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [17] = { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [18] = { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [19] = { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [20] = { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [21] = { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    [22] = { 3, ESP_CPU_INTR_TYPE_EDGE,  { 0,                              0                               } },
    [23] = { 3, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    /* Interrupt 24 reserved for T1 WDT when CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4 is enabled */
    [24] = { 4, ESP_CPU_INTR_TYPE_LEVEL, { CORE_0_INTERRUPT_24,            CORE_1_INTERRUPT_24             } },
    /* Interrupt 25 reserved for Memory access and cache errors when CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4 is enabled
     * Reserved for BT/BLE Controller when CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 is enabled */
    [25] = { 4, ESP_CPU_INTR_TYPE_LEVEL, { CORE_0_INTERRUPT_25,            CORE_1_INTERRUPT_25             } },
    /* Interrupt 26 reserved for T1 WDT, Memory access and cache errors when CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 is enabled */
    [26] = { 5, ESP_CPU_INTR_TYPE_LEVEL, { CORE_0_INTERRUPT_26,            CORE_1_INTERRUPT_26             } },
    [27] = { 3, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } },
    /* Interrupt 28 reserved for IPC when CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4 is enabled */
    [28] = { 4, ESP_CPU_INTR_TYPE_EDGE,  { CORE_0_INTERRUPT_28,            CORE_1_INTERRUPT_28             } },
    [29] = { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } },
    [30] = { 4, ESP_CPU_INTR_TYPE_EDGE,  { 0,                              0                               } },
    /* Interrupt 31 reserved for IPC when CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 is enabled */
    [31] = { 5, ESP_CPU_INTR_TYPE_LEVEL, { CORE_0_INTERRUPT_31,            CORE_1_INTERRUPT_31             } },
};


void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret)
{
    assert(core_id >= 0 && core_id < SOC_CPU_CORES_NUM && intr_desc_ret != NULL);
    intr_desc_ret->priority = intr_desc_table[intr_num].priority;
    intr_desc_ret->type = intr_desc_table[intr_num].type;
    intr_desc_ret->flags = intr_desc_table[intr_num].flags[core_id];
}
