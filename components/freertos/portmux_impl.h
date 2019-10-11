/*
    Copyright (C) 2016-2017 Espressif Shanghai PTE LTD
    Copyright (C) 2015 Real Time Engineers Ltd.

    All rights reserved

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html
*/

/* This header exists for performance reasons, in order to inline the
   implementation of vPortCPUAcquireMutexIntsDisabled and
   vPortCPUReleaseMutexIntsDisabled into the
   vTaskEnterCritical/vTaskExitCritical functions in task.c as well as the
   vPortCPUAcquireMutex/vPortCPUReleaseMutex implementations.

   Normally this kind of performance hack is over the top, but
   vTaskEnterCritical/vTaskExitCritical is called a great
   deal by FreeRTOS internals.

   It should be #included by freertos port.c or tasks.c, in esp-idf.

   The way it works is that it essentially uses portmux_impl.inc.h as a
   generator template of sorts. When no external memory is used, this
   template is only used to generate the vPortCPUAcquireMutexIntsDisabledInternal
   and vPortCPUReleaseMutexIntsDisabledInternal functions, which use S32C1 to
   do an atomic compare & swap. When external memory is used the functions
   vPortCPUAcquireMutexIntsDisabledExtram and vPortCPUReleaseMutexIntsDisabledExtram
   are also generated, which use uxPortCompareSetExtram to fake the S32C1 instruction.
   The wrapper functions vPortCPUAcquireMutexIntsDisabled and
   vPortCPUReleaseMutexIntsDisabled will then use the appropriate function to do the
   actual lock/unlock.
*/
#include "soc/cpu.h"
#include "portable.h"
#include "soc/soc_memory_layout.h"

/* XOR one core ID with this value to get the other core ID */
#define CORE_ID_REGVAL_XOR_SWAP (CORE_ID_REGVAL_PRO ^ CORE_ID_REGVAL_APP)




//Define the mux routines for use with muxes in internal RAM
#define PORTMUX_AQUIRE_MUX_FN_NAME vPortCPUAcquireMutexIntsDisabledInternal
#define PORTMUX_RELEASE_MUX_FN_NAME vPortCPUReleaseMutexIntsDisabledInternal
#define PORTMUX_COMPARE_SET_FN_NAME uxPortCompareSet
#include "portmux_impl.inc.h"
#undef PORTMUX_AQUIRE_MUX_FN_NAME
#undef PORTMUX_RELEASE_MUX_FN_NAME
#undef PORTMUX_COMPARE_SET_FN_NAME


#if defined(CONFIG_ESP32_SPIRAM_SUPPORT)

#define PORTMUX_AQUIRE_MUX_FN_NAME vPortCPUAcquireMutexIntsDisabledExtram
#define PORTMUX_RELEASE_MUX_FN_NAME vPortCPUReleaseMutexIntsDisabledExtram
#define PORTMUX_COMPARE_SET_FN_NAME uxPortCompareSetExtram
#include "portmux_impl.inc.h"
#undef PORTMUX_AQUIRE_MUX_FN_NAME
#undef PORTMUX_RELEASE_MUX_FN_NAME
#undef PORTMUX_COMPARE_SET_FN_NAME

#endif


#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
#define PORTMUX_AQUIRE_MUX_FN_ARGS portMUX_TYPE *mux, int timeout_cycles, const char *fnName, int line
#define PORTMUX_RELEASE_MUX_FN_ARGS portMUX_TYPE *mux, const char *fnName, int line
#define PORTMUX_AQUIRE_MUX_FN_CALL_ARGS(x) x, timeout_cycles, fnName, line
#define PORTMUX_RELEASE_MUX_FN_CALL_ARGS(x) x, fnName, line
#else
#define PORTMUX_AQUIRE_MUX_FN_ARGS portMUX_TYPE *mux, int timeout_cycles
#define PORTMUX_RELEASE_MUX_FN_ARGS portMUX_TYPE *mux
#define PORTMUX_AQUIRE_MUX_FN_CALL_ARGS(x) x, timeout_cycles
#define PORTMUX_RELEASE_MUX_FN_CALL_ARGS(x) x
#endif


static inline bool __attribute__((always_inline)) vPortCPUAcquireMutexIntsDisabled(PORTMUX_AQUIRE_MUX_FN_ARGS) {
#if defined(CONFIG_ESP32_SPIRAM_SUPPORT)
	if (esp_ptr_external_ram(mux)) {
		return vPortCPUAcquireMutexIntsDisabledExtram(PORTMUX_AQUIRE_MUX_FN_CALL_ARGS(mux));
	}
#endif
	return vPortCPUAcquireMutexIntsDisabledInternal(PORTMUX_AQUIRE_MUX_FN_CALL_ARGS(mux));
}


static inline void vPortCPUReleaseMutexIntsDisabled(PORTMUX_RELEASE_MUX_FN_ARGS) {
#if defined(CONFIG_ESP32_SPIRAM_SUPPORT)
	if (esp_ptr_external_ram(mux)) {
		vPortCPUReleaseMutexIntsDisabledExtram(PORTMUX_RELEASE_MUX_FN_CALL_ARGS(mux));
		return;
	}
#endif
	vPortCPUReleaseMutexIntsDisabledInternal(PORTMUX_RELEASE_MUX_FN_CALL_ARGS(mux));
}

