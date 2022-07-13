/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef FREERTOS_ADDITITIONS_INC_H_
#define FREERTOS_ADDITITIONS_INC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"

#if ( INCLUDE_vTaskPrioritySet == 1 )

typedef struct {
    UBaseType_t uxPriority;
#if ( configUSE_MUTEXES == 1 )
    UBaseType_t uxBasePriority;
#endif
} prvTaskSavedPriority_t;

#endif // ( INCLUDE_vTaskPrioritySet == 1)

#ifdef __cplusplus
}
#endif

#endif //FREERTOS_ADDITITIONS_INC_H_
