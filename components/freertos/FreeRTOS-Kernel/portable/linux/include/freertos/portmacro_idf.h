/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This is the "IDF-part" of the POSIX portmacro.
 * We need additional definitions for code in ESP-IDF which is kept here to separate the original
 * FreeRTOS POSIX port code from the additional IDF POSIX port code.
 */

#pragma once

#include <stdint.h>
#include <stdlib.h> // This is for malloc(), used by portmacro.h
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_task.h"
#include "spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif

// interrupt module will mask interrupt with priority less than threshold
#define RVHAL_EXCM_LEVEL    4

typedef spinlock_t portMUX_TYPE;

/**< Spinlock initializer */
#define portMUX_INITIALIZER_UNLOCKED {                      \
            .owner = portMUX_FREE_VAL,                      \
            .count = 0,                                     \
        }
#define portMUX_FREE_VAL                    SPINLOCK_FREE           /**< Spinlock is free. [refactor-todo] check if this is still required */

void vPortYieldFromISR(void);
void vPortYieldOtherCore(BaseType_t coreid);

#define portMUX_INITIALIZE(mux)             spinlock_initialize(mux)    /*< Initialize a spinlock to its unlocked state */

/**
 * @brief Get the current core's ID
 *
 * @note dummy function for freertos simulator, always returns 0.
 @ return BaseType_t 0
 */
static inline BaseType_t IRAM_ATTR xPortGetCoreID(void)
{
    return (BaseType_t) 0;
}

static inline bool portVALID_TCB_MEM(const void *ptr)
{
    return true;
}

static inline bool portVALID_STACK_MEM(const void *ptr)
{
    return true;
}

#define pvPortMallocTcbMem(size)        pvPortMalloc(size)
#define pvPortMallocStackMem(size)      pvPortMalloc(size)

BaseType_t xPortCheckIfInISR(void);

/**
 * @brief Checks if the current core is in an ISR context
 *
 * - ISR context consist of Low/Mid priority ISR, or time tick ISR
 * - High priority ISRs aren't detected here, but they normally cannot call C code, so that should not be an issue anyway.
 *
 * @note [refactor-todo] Check if this should be inlined
 * @return
 *  - pdTRUE if in ISR
 *  - pdFALSE otherwise
 */
static inline BaseType_t xPortInIsrContext(void)
{
    //Just call the FreeRTOS port interface version
    return xPortCheckIfInISR();
}

#ifdef __cplusplus
}
#endif
