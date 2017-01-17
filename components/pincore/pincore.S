#define NOERROR #
NOERROR: .error "C preprocessor needed for this file: make sure its filename\
 ends in uppercase .S, or use xt-xcc's -x assembler-with-cpp option."

#include "xtensa_rtos.h"
#include "sdkconfig.h"

#define PINCORE_SELECT_CURRENT_CORE 0x00000100

#define PINCORE_MASKED_INT_LEVEL 15

#define PINCORE_TASKTCB_XCOREID_OFFSET ((0x38+configMAX_TASK_NAME_LEN+3)&~3)

.extern pxCurrentTCB

/*
Obeys ABI conventions per prototype:
    void pincore_alter_current_freertos_task(int * coreaff)
*/

    .section	.iram1
    .global		pincore_alter_current_freertos_task
    .type		pincore_alter_current_freertos_task,@function
    .align		4

pincore_alter_current_freertos_task:
    ENTRY0

    rsil        a7, PINCORE_MASKED_INT_LEVEL

    getcoreid	a3									/* a3 = current core */
    l32i		a4, a2, 0							/* a4 = *coreaff */
    bnei		a4, PINCORE_SELECT_CURRENT_CORE, core_provided		/* if a4 == PINCORE_SELECT_CURRENT_CORE {					*/
    mov			a4, a3								/* 		a4 = a3 // new core = current core	*/
    												/* }										*/
core_provided:
    movi		a5, pxCurrentTCB
    addx4     	a5, a3, a5					
    l32i		a5, a5, 0                   		/* a5 = start of pxCurrentTCB[cpuid] */
    addi		a5, a5, PINCORE_TASKTCB_XCOREID_OFFSET  	/* a5 += offset to xCoreID in tcb struct */
    l32i    	a6, a5, 0                       	/* a6 = current task core assignment */
    s32i		a6, a2, 0							/* *coreaff = a6 */
    s32i		a4, a5, 0							/* current_task_core_assg = a4 // store new core in the TCB struct */
	
	wsr.ps		a7
    rsync

    RET0