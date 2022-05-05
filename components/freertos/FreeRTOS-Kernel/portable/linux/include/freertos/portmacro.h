/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_attr.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define portBYTE_ALIGNMENT			16

/* Type definitions. */
#define portCHAR		uint8_t
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		int32_t
#define portSHORT		int16_t
#define portSTACK_TYPE	uint8_t
#define portBASE_TYPE	int
// interrupt module will mask interrupt with priority less than threshold
#define RVHAL_EXCM_LEVEL    4

typedef portSTACK_TYPE			StackType_t;
typedef portBASE_TYPE			BaseType_t;
typedef unsigned portBASE_TYPE	UBaseType_t;
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

typedef int portMUX_TYPE;

#define portTICK_PERIOD_MS			( ( TickType_t ) 1 )

#ifdef __cplusplus
}
#endif
