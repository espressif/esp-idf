/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

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

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*******************************************************************************
// Copyright (c) 2003-2015 Cadence Design Systems, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------
*/
#include <stdlib.h>
#include <string.h>
#include <xtensa/config/core.h>

#include "xtensa_rtos.h"

#include "soc/cpu.h"

#include "FreeRTOS.h"
#include "task.h"

#include "esp_debug_helpers.h"
#include "esp_heap_caps.h"
#include "esp_private/crosscore_int.h"

#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_compiler.h"

/* Defined in portasm.h */
extern void _frxt_tick_timer_init(void);

/* Defined in xtensa_context.S */
extern void _xt_coproc_init(void);


#if CONFIG_FREERTOS_CORETIMER_0
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER0_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif
#if CONFIG_FREERTOS_CORETIMER_1
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER1_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif

_Static_assert(tskNO_AFFINITY == CONFIG_FREERTOS_NO_AFFINITY, "incorrect tskNO_AFFINITY value");

/*-----------------------------------------------------------*/
unsigned port_xSchedulerRunning[portNUM_PROCESSORS] = {0}; // Duplicate of inaccessible xSchedulerRunning; needed at startup to avoid counting nesting
unsigned port_interruptNesting[portNUM_PROCESSORS] = {0};  // Interrupt nesting level. Increased/decreased in portasm.c, _frxt_int_enter/_frxt_int_exit
BaseType_t port_uxCriticalNesting[portNUM_PROCESSORS] = {0};  
BaseType_t port_uxOldInterruptState[portNUM_PROCESSORS] = {0};  
/*-----------------------------------------------------------*/

// User exception dispatcher when exiting
void _xt_user_exit(void);

#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
// Wrapper to allow task functions to return (increases stack overhead by 16 bytes)
static void vPortTaskWrapper(TaskFunction_t pxCode, void *pvParameters)
{
	pxCode(pvParameters);
	//FreeRTOS tasks should not return. Log the task name and abort.
	char * pcTaskName = pcTaskGetTaskName(NULL);
	ESP_LOGE("FreeRTOS", "FreeRTOS Task \"%s\" should not return, Aborting now!", pcTaskName);
	abort();
}
#endif

/*
 * Stack initialization
 */
#if portUSING_MPU_WRAPPERS
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters, BaseType_t xRunPrivileged )
#else
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
#endif
{
	StackType_t *sp, *tp;
	XtExcFrame  *frame;
	#if XCHAL_CP_NUM > 0
	uint32_t *p;
	#endif
	uint32_t *threadptr;
	void *task_thread_local_start;
	extern int _thread_local_start, _thread_local_end, _flash_rodata_start, _flash_rodata_align;
	// TODO: check that TLS area fits the stack
	uint32_t thread_local_sz = (uint8_t *)&_thread_local_end - (uint8_t *)&_thread_local_start;

	thread_local_sz = ALIGNUP(0x10, thread_local_sz);

	/* Initialize task's stack so that we have the following structure at the top:

		----LOW ADDRESSES ----------------------------------------HIGH ADDRESSES----------
		 task stack | interrupt stack frame | thread local vars | co-processor save area |
		----------------------------------------------------------------------------------
					|																	 |
					SP 																pxTopOfStack

		All parts are aligned to 16 byte boundary. */
	sp = (StackType_t *) (((UBaseType_t)(pxTopOfStack + 1) - XT_CP_SIZE - thread_local_sz - XT_STK_FRMSZ) & ~0xf);

	/* Clear the entire frame (do not use memset() because we don't depend on C library) */
	for (tp = sp; tp <= pxTopOfStack; ++tp)
		*tp = 0;

	frame = (XtExcFrame *) sp;

	/* Explicitly initialize certain saved registers */
	#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
	frame->pc	= (UBaseType_t) vPortTaskWrapper;	/* task wrapper						*/
	#else
	frame->pc   = (UBaseType_t) pxCode;				/* task entrypoint					*/
	#endif
	frame->a0	= 0;								/* to terminate GDB backtrace		*/
	frame->a1	= (UBaseType_t) sp + XT_STK_FRMSZ;	/* physical top of stack frame		*/
	frame->exit = (UBaseType_t) _xt_user_exit;		/* user exception exit dispatcher	*/

	/* Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode. */
	/* Also set entry point argument parameter. */
	#ifdef __XTENSA_CALL0_ABI__
		#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
		frame->a2 = (UBaseType_t) pxCode;
		frame->a3 = (UBaseType_t) pvParameters;
		#else
		frame->a2 = (UBaseType_t) pvParameters;
		#endif
	frame->ps = PS_UM | PS_EXCM;
	#else
	/* + for windowed ABI also set WOE and CALLINC (pretend task was 'call4'd). */
		#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
		frame->a6 = (UBaseType_t) pxCode;
		frame->a7 = (UBaseType_t) pvParameters;
		#else
		frame->a6 = (UBaseType_t) pvParameters;
		#endif
	frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1);
	#endif

	#ifdef XT_USE_SWPRI
	/* Set the initial virtual priority mask value to all 1's. */
	frame->vpri = 0xFFFFFFFF;
	#endif

	/* Init threadptr register and set up TLS run-time area.
	 * The following diagram illustrates the layout of link-time and run-time
	 * TLS sections.
	 *
	 *          +-------------+
	 *          |Section:     |      Linker symbols:
	 *          |.flash.rodata|      ---------------
	 *       0x0+-------------+ <-- _flash_rodata_start
	 *        ^ |             |
	 *        | | Other data  |
	 *        | |     ...     |
	 *        | +-------------+ <-- _thread_local_start
	 *        | |.tbss        | ^
	 *        v |             | |
	 *    0xNNNN|int example; | | (thread_local_size)
	 *          |.tdata       | v
	 *          +-------------+ <-- _thread_local_end
	 *          | Other data  |
	 *          |     ...     |
	 *          |             |
	 *          +-------------+
	 *
	 *                                Local variables of
	 *                              pxPortInitialiseStack
	 *                             -----------------------
	 *          +-------------+ <-- pxTopOfStack
	 *          |.tdata (*)   |  ^
	 *        ^ |int example; |  |(thread_local_size
	 *        | |             |  |
	 *        | |.tbss (*)    |  v
	 *        | +-------------+ <-- task_thread_local_start
	 * 0xNNNN | |             |  ^
	 *        | |             |  |
	 *        | |             |  |_thread_local_start - _rodata_start
	 *        | |             |  |
	 *        | |             |  v
	 *        v +-------------+ <-- threadptr
	 *
	 *   (*) The stack grows downward!
	 */
	task_thread_local_start = (void *)(((uint32_t)pxTopOfStack - XT_CP_SIZE - thread_local_sz) & ~0xf);
	memcpy(task_thread_local_start, &_thread_local_start, thread_local_sz);
	threadptr = (uint32_t *)(sp + XT_STK_EXTRA);
	/* Calculate THREADPTR value.
	 * The generated code will add THREADPTR value to a constant value determined at link time,
	 * to get the address of the TLS variable.
	 * The constant value is calculated by the linker as follows
	 * (search for 'tpoff' in elf32-xtensa.c in BFD):
	 *    offset = address - tls_section_vma + align_up(TCB_SIZE, tls_section_alignment)
	 * where TCB_SIZE is hardcoded to 8.
	 * Note this is slightly different compared to the RISC-V port, where offset = address - tls_section_vma.
	 */
	const uint32_t tls_section_alignment = (uint32_t) &_flash_rodata_align;  /* ALIGN value of .flash.rodata section */
	const uint32_t tcb_size = 8; /* Unrelated to FreeRTOS, this is the constant from BFD */
	const uint32_t base = (tcb_size + tls_section_alignment - 1) & (~(tls_section_alignment - 1));
	*threadptr = (uint32_t)task_thread_local_start - ((uint32_t)&_thread_local_start - (uint32_t)&_flash_rodata_start) - base;

	#if XCHAL_CP_NUM > 0
	/* Init the coprocessor save area (see xtensa_context.h) */
	/* No access to TCB here, so derive indirectly. Stack growth is top to bottom.
         * //p = (uint32_t *) xMPUSettings->coproc_area;
	 */
	p = (uint32_t *)(((uint32_t) pxTopOfStack - XT_CP_SIZE) & ~0xf);
	p[0] = 0;
	p[1] = 0;
	p[2] = (((uint32_t) p) + 12 + XCHAL_TOTAL_SA_ALIGN - 1) & -XCHAL_TOTAL_SA_ALIGN;
	#endif

	return sp;
}

/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the Xtensa port will get stopped.  If required simply
	disable the tick interrupt here. */
}

/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
	// Interrupts are disabled at this point and stack contains PS with enabled interrupts when task context is restored

	#if XCHAL_CP_NUM > 0
	/* Initialize co-processor management for tasks. Leave CPENABLE alone. */
	_xt_coproc_init();
	#endif

	/* Init the tick divisor value */
	_xt_tick_divisor_init();

	/* Setup the hardware to generate the tick. */
	_frxt_tick_timer_init();

	port_xSchedulerRunning[xPortGetCoreID()] = 1;

	// Cannot be directly called from C; never returns
	__asm__ volatile ("call0    _frxt_dispatch\n");

	/* Should not get here. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

BaseType_t xPortSysTickHandler( void )
{
	BaseType_t ret;

	portbenchmarkIntLatency();
	traceISR_ENTER(SYSTICK_INTR_ID);
	ret = xTaskIncrementTick();
	if( ret != pdFALSE )
	{
		portYIELD_FROM_ISR();
	} else {
		traceISR_EXIT();
	}
	return ret;
}


void vPortYieldOtherCore( BaseType_t coreid ) {
	esp_crosscore_int_send_yield( coreid );
}

/*-----------------------------------------------------------*/

/*
 * Used to set coprocessor area in stack. Current hack is to reuse MPU pointer for coprocessor area.
 */
#if portUSING_MPU_WRAPPERS
void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions, StackType_t *pxBottomOfStack, uint32_t usStackDepth )
{
	#if XCHAL_CP_NUM > 0
	xMPUSettings->coproc_area = (StackType_t*)((((uint32_t)(pxBottomOfStack + usStackDepth - 1)) - XT_CP_SIZE ) & ~0xf);


	/* NOTE: we cannot initialize the coprocessor save area here because FreeRTOS is going to
         * clear the stack area after we return. This is done in pxPortInitialiseStack().
	 */
	#endif
}

void vPortReleaseTaskMPUSettings( xMPU_SETTINGS *xMPUSettings )
{
	/* If task has live floating point registers somewhere, release them */
	_xt_coproc_release( xMPUSettings->coproc_area );
}

#endif

/*
 * Returns true if the current core is in ISR context; low prio ISR, med prio ISR or timer tick ISR. High prio ISRs
 * aren't detected here, but they normally cannot call C code, so that should not be an issue anyway.
 */
BaseType_t xPortInIsrContext(void)
{
	unsigned int irqStatus;
	BaseType_t ret;
	irqStatus=portENTER_CRITICAL_NESTED();
	ret=(port_interruptNesting[xPortGetCoreID()] != 0);
	portEXIT_CRITICAL_NESTED(irqStatus);
	return ret;
}

/*
 * This function will be called in High prio ISRs. Returns true if the current core was in ISR context
 * before calling into high prio ISR context.
 */
BaseType_t IRAM_ATTR xPortInterruptedFromISRContext(void)
{
	return (port_interruptNesting[xPortGetCoreID()] != 0);
}

void vPortAssertIfInISR(void)
{
	configASSERT(xPortInIsrContext());
}

void vPortSetStackWatchpoint( void* pxStackStart ) {
	//Set watchpoint 1 to watch the last 32 bytes of the stack.
	//Unfortunately, the Xtensa watchpoints can't set a watchpoint on a random [base - base+n] region because
	//the size works by masking off the lowest address bits. For that reason, we futz a bit and watch the lowest 32
	//bytes of the stack we can actually watch. In general, this can cause the watchpoint to be triggered at most
	//28 bytes early. The value 32 is chosen because it's larger than the stack canary, which in FreeRTOS is 20 bytes.
	//This way, we make sure we trigger before/when the stack canary is corrupted, not after.
	int addr=(int)pxStackStart;
	addr=(addr+31)&(~31);
	esp_set_watchpoint(1, (char*)addr, 32, ESP_WATCHPOINT_STORE);
}

uint32_t xPortGetTickRateHz(void) {
	return (uint32_t)configTICK_RATE_HZ;
}

void __attribute__((optimize("-O3"))) vPortEnterCritical(portMUX_TYPE *mux)
{
	BaseType_t oldInterruptLevel = portENTER_CRITICAL_NESTED();
	/* Interrupts may already be disabled (because we're doing this recursively)
	* but we can't get the interrupt level after
	* vPortCPUAquireMutex, because it also may mess with interrupts.
	* Get it here first, then later figure out if we're nesting
	* and save for real there.
	*/ 
	vPortCPUAcquireMutex( mux );
	BaseType_t coreID = xPortGetCoreID();
	BaseType_t newNesting = port_uxCriticalNesting[coreID] + 1;
	port_uxCriticalNesting[coreID] = newNesting;

	if( newNesting == 1 )
	{
		//This is the first time we get called. Save original interrupt level.
		port_uxOldInterruptState[coreID] = oldInterruptLevel;
	}
}

void __attribute__((optimize("-O3"))) vPortExitCritical(portMUX_TYPE *mux)
{
	vPortCPUReleaseMutex( mux );
	BaseType_t coreID = xPortGetCoreID();
	BaseType_t nesting =  port_uxCriticalNesting[coreID];
	
	if(nesting > 0U)
	{
		nesting--;
		port_uxCriticalNesting[coreID] = nesting;

		if( nesting == 0U )
		{
			portEXIT_CRITICAL_NESTED(port_uxOldInterruptState[coreID]);
		}
	}
}

void  __attribute__((weak)) vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	#define ERR_STR1 "***ERROR*** A stack overflow in task "
	#define ERR_STR2 " has been detected."
	const char *str[] = {ERR_STR1, pcTaskName, ERR_STR2};

	char buf[sizeof(ERR_STR1) + CONFIG_FREERTOS_MAX_TASK_NAME_LEN + sizeof(ERR_STR2) + 1 /* null char */] = { 0 };

	char *dest = buf;
	for (int i = 0 ; i < sizeof(str)/ sizeof(str[0]); i++) {
		dest = strcat(dest, str[i]);
	}
	esp_system_abort(buf);
}
