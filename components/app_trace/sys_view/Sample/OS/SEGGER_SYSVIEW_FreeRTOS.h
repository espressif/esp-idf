/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2015 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the RTT protocol and J-Link.                       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* conditions are met:                                                *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this list of conditions and the following disclaimer.    *
*                                                                    *
* o Redistributions in binary form must reproduce the above          *
*   copyright notice, this list of conditions and the following      *
*   disclaimer in the documentation and/or other materials provided  *
*   with the distribution.                                           *
*                                                                    *
* o Neither the name of SEGGER Microcontroller GmbH & Co. KG         *
*   nor the names of its contributors may be used to endorse or      *
*   promote products derived from this software without specific     *
*   prior written permission.                                        *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: V2.42                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_FreeRTOS.h
Purpose : Interface between FreeRTOS and SystemView.
Revision: $Rev: 3734 $

Notes:
  (1) Include this file at the end of FreeRTOSConfig.h
*/

#ifndef SYSVIEW_FREERTOS_H
#define SYSVIEW_FREERTOS_H

#include "SEGGER_SYSVIEW.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#ifndef portSTACK_GROWTH
  #define portSTACK_GROWTH              ( -1 )
#endif

#define SYSVIEW_FREERTOS_MAX_NOF_TASKS  16

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

// for dual-core targets we use event ID to keep core ID bit (0 or 1)
// use the highest - 1 bit of event ID to indicate core ID
// the highest bit can not be used due to event ID encoding method
// this reduces supported ID range to [0..63] (for 1 byte IDs) plus [128..16383] (for 2 bytes IDs)
// so original continuous event IDs range is split into two sub-ranges for 1-bytes IDs and 2-bytes ones

// events which use apiFastID_OFFSET will have 1 byte ID,
// so for the sake of bandwidth economy events which are generated more frequently should use this ID offset
// currently all used events fall into this range
#define apiFastID_OFFSET                          (32u)

#define apiID_VTASKDELETE                         (1u)
#define apiID_VTASKDELAY                          (2u)
#define apiID_VTASKDELAYUNTIL                     (3u)
#define apiID_VTASKSUSPEND                        (4u)
#define apiID_ULTASKNOTIFYTAKE                    (5u)
#define apiID_VTASKNOTIFYGIVEFROMISR              (6u)
#define apiID_VTASKPRIORITYINHERIT                (7u)
#define apiID_VTASKRESUME                         (8u)
#define apiID_VTASKSTEPTICK                       (9u)
#define apiID_XTASKPRIORITYDISINHERIT             (10u)
#define apiID_XTASKRESUMEFROMISR                  (11u)
#define apiID_XTASKGENERICNOTIFY                  (12u)
#define apiID_XTASKGENERICNOTIFYFROMISR           (13u)
#define apiID_XTASKNOTIFYWAIT                     (14u)
#define apiID_XQUEUEGENERICCREATE                 (15u)
#define apiID_VQUEUEDELETE                        (16u)
#define apiID_XQUEUEGENERICRECEIVE                (17u)
#define apiID_XQUEUEPEEKFROMISR                   (18u)
#define apiID_XQUEUERECEIVEFROMISR                (19u)
#define apiID_VQUEUEADDTOREGISTRY                 (20u)
#define apiID_XQUEUEGENERICSEND                   (21u)
#define apiID_XQUEUEGENERICSENDFROMISR            (22u)
#define apiID_VTASKPRIORITYSET                    (23u)
#define apiID_UXTASKPRIORITYGETFROMISR            (24u)
#define apiID_XTASKGETTICKCOUNTFROMISR            (25u)
#define apiID_XEVENTGROUPCLEARBITSFROMISR         (26u)
#define apiID_XEVENTGROUPSETBITSFROMISR           (27u)
#define apiID_XEVENTGROUPGETBITSFROMISR           (28u)
#define apiID_XQUEUEGIVEFROMISR                   (29u)
#define apiID_XQUEUEISQUEUEEMPTYFROMISR           (30u)
#define apiID_XQUEUEISQUEUEFULLFROMISR            (31u) // the maximum allowed apiID for the first ID range

// events which use apiSlowID_OFFSET will have 2-bytes ID
#define apiSlowID_OFFSET                          (127u)

#define apiID_VTASKALLOCATEMPUREGIONS             (1u)
#define apiID_UXTASKPRIORITYGET                   (2u)
#define apiID_ETASKGETSTATE                       (3u)
#define apiID_VTASKSTARTSCHEDULER                 (4u)
#define apiID_VTASKENDSCHEDULER                   (5u)
#define apiID_VTASKSUSPENDALL                     (6u)
#define apiID_XTASKRESUMEALL                      (7u)
#define apiID_XTASKGETTICKCOUNT                   (8u)
#define apiID_UXTASKGETNUMBEROFTASKS              (9u)
#define apiID_PCTASKGETTASKNAME                   (10u)
#define apiID_UXTASKGETSTACKHIGHWATERMARK         (11u)
#define apiID_VTASKSETAPPLICATIONTASKTAG          (12u)
#define apiID_XTASKGETAPPLICATIONTASKTAG          (13u)
#define apiID_VTASKSETTHREADLOCALSTORAGEPOINTER   (14u)
#define apiID_PVTASKGETTHREADLOCALSTORAGEPOINTER  (15u)
#define apiID_XTASKCALLAPPLICATIONTASKHOOK        (16u)
#define apiID_XTASKGETIDLETASKHANDLE              (17u)
#define apiID_UXTASKGETSYSTEMSTATE                (18u)
#define apiID_VTASKLIST                           (19u)
#define apiID_VTASKGETRUNTIMESTATS                (20u)
#define apiID_XTASKNOTIFYSTATECLEAR               (21u)
#define apiID_XTASKGETCURRENTTASKHANDLE           (22u)
#define apiID_VTASKSETTIMEOUTSTATE                (23u)
#define apiID_XTASKCHECKFORTIMEOUT                (24u)
#define apiID_VTASKMISSEDYIELD                    (25u)
#define apiID_XTASKGETSCHEDULERSTATE              (26u)
#define apiID_XTASKGENERICCREATE                  (27u)
#define apiID_UXTASKGETTASKNUMBER                 (28u)
#define apiID_VTASKSETTASKNUMBER                  (29u)
#define apiID_ETASKCONFIRMSLEEPMODESTATUS         (30u)
#define apiID_XTIMERCREATE                        (31u)
#define apiID_PVTIMERGETTIMERID                   (32u)
#define apiID_VTIMERSETTIMERID                    (33u)
#define apiID_XTIMERISTIMERACTIVE                 (34u)
#define apiID_XTIMERGETTIMERDAEMONTASKHANDLE      (35u)
#define apiID_XTIMERPENDFUNCTIONCALLFROMISR       (36u)
#define apiID_XTIMERPENDFUNCTIONCALL              (37u)
#define apiID_PCTIMERGETTIMERNAME                 (38u)
#define apiID_XTIMERCREATETIMERTASK               (39u)
#define apiID_XTIMERGENERICCOMMAND                (40u)
#define apiID_UXQUEUEMESSAGESWAITING              (41u)
#define apiID_UXQUEUESPACESAVAILABLE              (42u)
#define apiID_UXQUEUEMESSAGESWAITINGFROMISR       (43u)
#define apiID_XQUEUEALTGENERICSEND                (44u)
#define apiID_XQUEUEALTGENERICRECEIVE             (45u)
#define apiID_XQUEUECRSENDFROMISR                 (46u)
#define apiID_XQUEUECRRECEIVEFROMISR              (47u)
#define apiID_XQUEUECRSEND                        (48u)
#define apiID_XQUEUECRRECEIVE                     (49u)
#define apiID_XQUEUECREATEMUTEX                   (50u)
#define apiID_XQUEUECREATECOUNTINGSEMAPHORE       (51u)
#define apiID_XQUEUEGETMUTEXHOLDER                (52u)
#define apiID_XQUEUETAKEMUTEXRECURSIVE            (53u)
#define apiID_XQUEUEGIVEMUTEXRECURSIVE            (54u)
#define apiID_VQUEUEUNREGISTERQUEUE               (55u)
#define apiID_XQUEUECREATESET                     (56u)
#define apiID_XQUEUEADDTOSET                      (57u)
#define apiID_XQUEUEREMOVEFROMSET                 (58u)
#define apiID_XQUEUESELECTFROMSET                 (59u)
#define apiID_XQUEUESELECTFROMSETFROMISR          (60u)
#define apiID_XQUEUEGENERICRESET                  (61u)
#define apiID_VLISTINITIALISE                     (62u)
#define apiID_VLISTINITIALISEITEM                 (63u)
#define apiID_VLISTINSERT                         (64u)
#define apiID_VLISTINSERTEND                      (65u)
#define apiID_UXLISTREMOVE                        (66u)
#define apiID_XEVENTGROUPCREATE                   (67u)
#define apiID_XEVENTGROUPWAITBITS                 (68u)
#define apiID_XEVENTGROUPCLEARBITS                (69u)
#define apiID_XEVENTGROUPSETBITS                  (70u)
#define apiID_XEVENTGROUPSYNC                     (71u)
#define apiID_VEVENTGROUPDELETE                   (72u)
#define apiID_UXEVENTGROUPGETNUMBER               (73u)

#define traceTASK_NOTIFY_TAKE()                                       SEGGER_SYSVIEW_RecordU32x2(apiFastID_OFFSET + apiID_ULTASKNOTIFYTAKE, xClearCountOnExit, xTicksToWait)
#define traceTASK_DELAY()                                             SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_VTASKDELAY, xTicksToDelay)
#define traceTASK_DELAY_UNTIL()                                       SEGGER_SYSVIEW_RecordVoid(apiFastID_OFFSET + apiID_VTASKDELAYUNTIL)
#define traceTASK_DELETE( pxTCB )                                     if (pxTCB != NULL) {                                              \
						                                                SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_VTASKDELETE,  \
						                                                                      SEGGER_SYSVIEW_ShrinkId((U32)pxTCB)); 	\
					                                                    SYSVIEW_DeleteTask((U32)pxTCB);                                 \
					                                                  }
#define traceTASK_NOTIFY_GIVE_FROM_ISR()                              SEGGER_SYSVIEW_RecordU32x2(apiFastID_OFFSET + apiID_VTASKNOTIFYGIVEFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB), (U32)pxHigherPriorityTaskWoken)
#define traceTASK_PRIORITY_INHERIT( pxTCB, uxPriority )               SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_VTASKPRIORITYINHERIT, (U32)pxMutexHolder)
#define traceTASK_RESUME( pxTCB )                                     SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_VTASKRESUME, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB))
#define traceINCREASE_TICK_COUNT( xTicksToJump )                      SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_VTASKSTEPTICK, xTicksToJump)
#define traceTASK_SUSPEND( pxTCB )                                    SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_VTASKSUSPEND, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB))
#define traceTASK_PRIORITY_DISINHERIT( pxTCB, uxBasePriority )        SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_XTASKPRIORITYDISINHERIT, (U32)pxMutexHolder)
#define traceTASK_RESUME_FROM_ISR( pxTCB )                            SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_XTASKRESUMEFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB))
#define traceTASK_NOTIFY()                                            SYSVIEW_RecordU32x4(apiFastID_OFFSET + apiID_XTASKGENERICNOTIFY, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB), ulValue, eAction, (U32)pulPreviousNotificationValue)
#define traceTASK_NOTIFY_FROM_ISR()                                   SYSVIEW_RecordU32x5(apiFastID_OFFSET + apiID_XTASKGENERICNOTIFYFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB), ulValue, eAction, (U32)pulPreviousNotificationValue, (U32)pxHigherPriorityTaskWoken)
#define traceTASK_NOTIFY_WAIT()                                       SYSVIEW_RecordU32x4(apiFastID_OFFSET + apiID_XTASKNOTIFYWAIT, ulBitsToClearOnEntry, ulBitsToClearOnExit, (U32)pulNotificationValue, xTicksToWait)

#define traceQUEUE_CREATE( pxNewQueue )                               SEGGER_SYSVIEW_RecordU32x3(apiFastID_OFFSET + apiID_XQUEUEGENERICCREATE, uxQueueLength, uxItemSize, ucQueueType)
#define traceQUEUE_DELETE( pxQueue )                                  SEGGER_SYSVIEW_RecordU32(apiFastID_OFFSET + apiID_VQUEUEDELETE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue))
#define traceQUEUE_PEEK( pxQueue )                                    SYSVIEW_RecordU32x4(apiFastID_OFFSET + apiID_XQUEUEGENERICRECEIVE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer), xTicksToWait, xJustPeeking)
#define traceQUEUE_PEEK_FROM_ISR( pxQueue )                           SEGGER_SYSVIEW_RecordU32x2(apiFastID_OFFSET + apiID_XQUEUEPEEKFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer))
#define traceQUEUE_PEEK_FROM_ISR_FAILED( pxQueue )                    SEGGER_SYSVIEW_RecordU32x2(apiFastID_OFFSET + apiID_XQUEUEPEEKFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer))
#define traceQUEUE_RECEIVE( pxQueue )                                 SYSVIEW_RecordU32x4(apiFastID_OFFSET + apiID_XQUEUEGENERICRECEIVE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer), xTicksToWait, xJustPeeking)
#define traceQUEUE_RECEIVE_FAILED( pxQueue )                          SYSVIEW_RecordU32x4(apiFastID_OFFSET + apiID_XQUEUEGENERICRECEIVE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer), xTicksToWait, xJustPeeking)
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue )                        SEGGER_SYSVIEW_RecordU32x3(apiFastID_OFFSET + apiID_XQUEUERECEIVEFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer), (U32)pxHigherPriorityTaskWoken)
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue )                 SEGGER_SYSVIEW_RecordU32x3(apiFastID_OFFSET + apiID_XQUEUERECEIVEFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer), (U32)pxHigherPriorityTaskWoken)
#define traceQUEUE_REGISTRY_ADD( xQueue, pcQueueName )                SEGGER_SYSVIEW_RecordU32x2(apiFastID_OFFSET + apiID_VQUEUEADDTOREGISTRY, SEGGER_SYSVIEW_ShrinkId((U32)xQueue), (U32)pcQueueName)
#if ( configUSE_QUEUE_SETS != 1 )
  #define traceQUEUE_SEND( pxQueue )                                    SYSVIEW_RecordU32x4(apiFastID_OFFSET + apiID_XQUEUEGENERICSEND, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), (U32)pvItemToQueue, xTicksToWait, xCopyPosition)
#else
  #define traceQUEUE_SEND( pxQueue )                                    SYSVIEW_RecordU32x4(apiFastID_OFFSET + apiID_XQUEUEGENERICSEND, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), 0, 0, xCopyPosition)
#endif
#define traceQUEUE_SEND_FAILED( pxQueue )                             SYSVIEW_RecordU32x4(apiFastID_OFFSET + apiID_XQUEUEGENERICSEND, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), (U32)pvItemToQueue, xTicksToWait, xCopyPosition)
#define traceQUEUE_SEND_FROM_ISR( pxQueue )                           SEGGER_SYSVIEW_RecordU32x2(apiFastID_OFFSET + apiID_XQUEUEGENERICSENDFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), (U32)pxHigherPriorityTaskWoken)
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue )                    SEGGER_SYSVIEW_RecordU32x2(apiFastID_OFFSET + apiID_XQUEUEGENERICSENDFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), (U32)pxHigherPriorityTaskWoken)

#if( portSTACK_GROWTH < 0 )
#define traceTASK_CREATE(pxNewTCB)                  if (pxNewTCB != NULL) {                                             \
                                                      SEGGER_SYSVIEW_OnTaskCreate((U32)pxNewTCB);                       \
                                                      SYSVIEW_AddTask((U32)pxNewTCB,                                    \
                                                                      &(pxNewTCB->pcTaskName[0]),                       \
                                                                      pxNewTCB->uxPriority,                             \
                                                                      (U32)pxNewTCB->pxStack,                           \
                                                                      ((U32)pxNewTCB->pxTopOfStack - (U32)pxNewTCB->pxStack) \
                                                                      );                                                \
                                                    }
#else
#define traceTASK_CREATE(pxNewTCB)                  if (pxNewTCB != NULL) {                                             \
                                                      SEGGER_SYSVIEW_OnTaskCreate((U32)pxNewTCB);                       \
                                                      SYSVIEW_AddTask((U32)pxNewTCB,                                    \
                                                                      &(pxNewTCB->pcTaskName[0]),                       \
                                                                      pxNewTCB->uxPriority,                             \
                                                                      (U32)pxNewTCB->pxStack,                           \
                                                                      (U32)(pxNewTCB->pxStack-pxNewTCB->pxTopOfStack)   \
                                                                      );                                                \
                                                    }
#endif
#define traceTASK_PRIORITY_SET(pxTask, uxNewPriority) {                                                                 \
                                                        SEGGER_SYSVIEW_RecordU32x2(apiFastID_OFFSET+apiID_VTASKPRIORITYSET, \
                                                                                   SEGGER_SYSVIEW_ShrinkId((U32)pxTCB), \
                                                                                   uxNewPriority                        \
                                                                                  );                                    \
                                                        SYSVIEW_UpdateTask((U32)pxTask,                                 \
                                                                           &(pxTask->pcTaskName[0]),                    \
                                                                           uxNewPriority,                               \
                                                                           (U32)pxTask->pxStack,                        \
                                                                           0                                            \
                                                                          );                                            \
                                                      }
//
// Define INCLUDE_xTaskGetIdleTaskHandle as 1 in FreeRTOSConfig.h to allow identification of Idle state.
//
#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
  #define traceTASK_SWITCHED_IN()                   if(prvGetTCBFromHandle(NULL) == xTaskGetIdleTaskHandle()) {           \
                                                      SEGGER_SYSVIEW_OnIdle();                                          \
                                                    } else {                                                            \
                                                      SEGGER_SYSVIEW_OnTaskStartExec((U32)pxCurrentTCB[xPortGetCoreID()]); \
                                                    }
#else
  #define traceTASK_SWITCHED_IN()                   {                                                                   \
                                                      if (memcmp(pxCurrentTCB[xPortGetCoreID()]->pcTaskName, "IDLE", 5) != 0) { \
                                                        SEGGER_SYSVIEW_OnTaskStartExec((U32)pxCurrentTCB[xPortGetCoreID()]);    \
                                                      } else {                                                          \
                                                        SEGGER_SYSVIEW_OnIdle();                                        \
                                                      }                                                                 \
                                                    }
#endif

#define traceMOVED_TASK_TO_READY_STATE(pxTCB)       SEGGER_SYSVIEW_OnTaskStartReady((U32)pxTCB)
#define traceREADDED_TASK_TO_READY_STATE(pxTCB)

#define traceMOVED_TASK_TO_DELAYED_LIST()           SEGGER_SYSVIEW_OnTaskStopReady((U32)pxCurrentTCB[xPortGetCoreID()],  (1u << 2))
#define traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST()  SEGGER_SYSVIEW_OnTaskStopReady((U32)pxCurrentTCB[xPortGetCoreID()],  (1u << 2))
#define traceMOVED_TASK_TO_SUSPENDED_LIST(pxTCB)    SEGGER_SYSVIEW_OnTaskStopReady((U32)pxTCB,         ((3u << 3) | 3))

#define traceISR_EXIT_TO_SCHEDULER()                SEGGER_SYSVIEW_RecordExitISRToScheduler()
#define traceISR_EXIT()                             SEGGER_SYSVIEW_RecordExitISR()
#define traceISR_ENTER(_n_)                         SEGGER_SYSVIEW_RecordEnterISR(_n_)

/*********************************************************************
*
*       API functions
*
**********************************************************************
*/
#ifdef __cplusplus
extern "C" {
#endif
void SYSVIEW_AddTask      (U32 xHandle, const char* pcTaskName, unsigned uxCurrentPriority, U32  pxStack, unsigned uStackHighWaterMark);
void SYSVIEW_UpdateTask   (U32 xHandle, const char* pcTaskName, unsigned uxCurrentPriority, U32 pxStack, unsigned uStackHighWaterMark);
void SYSVIEW_DeleteTask   (U32 xHandle);
void SYSVIEW_SendTaskInfo (U32 TaskID, const char* sName, unsigned Prio, U32 StackBase, unsigned StackSize);
void SYSVIEW_RecordU32x4  (unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3);
void SYSVIEW_RecordU32x5  (unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4);

#ifdef __cplusplus
}
#endif

#endif

/*************************** End of file ****************************/
