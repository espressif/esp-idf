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

File    : SEGGER_SYSVIEW_FreeRTOS.c
Purpose : Interface between FreeRTOS and SystemView.
Revision: $Rev: 3734 $
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#include "string.h" // Required for memset



typedef struct SYSVIEW_FREERTOS_TASK_STATUS SYSVIEW_FREERTOS_TASK_STATUS;

struct SYSVIEW_FREERTOS_TASK_STATUS {
  U32         xHandle;
  const char* pcTaskName;
  unsigned    uxCurrentPriority;
  U32         pxStack;
  unsigned    uStackHighWaterMark;
};

static SYSVIEW_FREERTOS_TASK_STATUS _aTasks[SYSVIEW_FREERTOS_MAX_NOF_TASKS];

/*********************************************************************
*
*       _cbSendTaskList()
*
*  Function description
*    This function is part of the link between FreeRTOS and SYSVIEW.
*    Called from SystemView when asked by the host, it uses SYSVIEW
*    functions to send the entire task list to the host.
*/
static void _cbSendTaskList(void) {
  unsigned n;

  for (n = 0; n < SYSVIEW_FREERTOS_MAX_NOF_TASKS; n++) {
    if (_aTasks[n].xHandle) {
#if INCLUDE_uxTaskGetStackHighWaterMark // Report Task Stack High Watermark
      _aTasks[n].uStackHighWaterMark = uxTaskGetStackHighWaterMark((TaskHandle_t)_aTasks[n].xHandle);
#endif
      SYSVIEW_SendTaskInfo((U32)_aTasks[n].xHandle, _aTasks[n].pcTaskName, (unsigned)_aTasks[n].uxCurrentPriority, (U32)_aTasks[n].pxStack, (unsigned)_aTasks[n].uStackHighWaterMark);
    }
  }
}

/*********************************************************************
*
*       _cbGetTime()
*
*  Function description
*    This function is part of the link between FreeRTOS and SYSVIEW.
*    Called from SystemView when asked by the host, returns the
*    current system time in micro seconds.
*/
static U64 _cbGetTime(void) {
  U64 Time;

  Time = xTaskGetTickCountFromISR();
  Time *= portTICK_PERIOD_MS;
  Time *= 1000;
  return Time;
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       SYSVIEW_AddTask()
*
*  Function description
*    Add a task to the internal list and record its information.
*/
void SYSVIEW_AddTask(U32 xHandle, const char* pcTaskName, unsigned uxCurrentPriority, U32  pxStack, unsigned uStackHighWaterMark) {
  unsigned n;
  
  if (memcmp(pcTaskName, "IDLE", 5) == 0) {
    return;
  }
  
  for (n = 0; n < SYSVIEW_FREERTOS_MAX_NOF_TASKS; n++) {
    if (_aTasks[n].xHandle == 0) {
      break;
    }
  }
  if (n == SYSVIEW_FREERTOS_MAX_NOF_TASKS) {
    SEGGER_SYSVIEW_Warn("SYSTEMVIEW: Could not record task information. Maximum number of tasks reached.");
    return;
  }

  _aTasks[n].xHandle = xHandle;
  _aTasks[n].pcTaskName = pcTaskName;
  _aTasks[n].uxCurrentPriority = uxCurrentPriority;
  _aTasks[n].pxStack = pxStack;
  _aTasks[n].uStackHighWaterMark = uStackHighWaterMark;

  SYSVIEW_SendTaskInfo(xHandle, pcTaskName,uxCurrentPriority, pxStack, uStackHighWaterMark);

}

/*********************************************************************
*
*       SYSVIEW_UpdateTask()
*
*  Function description
*    Update a task in the internal list and record its information.
*/
void SYSVIEW_UpdateTask(U32 xHandle, const char* pcTaskName, unsigned uxCurrentPriority, U32 pxStack, unsigned uStackHighWaterMark) {
  unsigned n;
  
  if (memcmp(pcTaskName, "IDLE", 5) == 0) {
    return;
  }

  for (n = 0; n < SYSVIEW_FREERTOS_MAX_NOF_TASKS; n++) {
    if (_aTasks[n].xHandle == xHandle) {
      break;
    }
  }
  if (n < SYSVIEW_FREERTOS_MAX_NOF_TASKS) {
    _aTasks[n].pcTaskName = pcTaskName;
    _aTasks[n].uxCurrentPriority = uxCurrentPriority;
    _aTasks[n].pxStack = pxStack;
    _aTasks[n].uStackHighWaterMark = uStackHighWaterMark;

    SYSVIEW_SendTaskInfo(xHandle, pcTaskName, uxCurrentPriority, pxStack, uStackHighWaterMark);
  } else {
    SYSVIEW_AddTask(xHandle, pcTaskName, uxCurrentPriority, pxStack, uStackHighWaterMark);
  }
}

/*********************************************************************
*
*       SYSVIEW_DeleteTask()
*
*  Function description
*    Delete a task from the internal list.
*/
void SYSVIEW_DeleteTask(U32 xHandle) {
  unsigned n;

  for (n = 0; n < SYSVIEW_FREERTOS_MAX_NOF_TASKS; n++) {
    if (_aTasks[n].xHandle == xHandle) {
      break;
    }
  }
  if (n == SYSVIEW_FREERTOS_MAX_NOF_TASKS) {
    SEGGER_SYSVIEW_Warn("SYSTEMVIEW: Could not find task information. Cannot delete task.");
    return;
  }

  _aTasks[n].xHandle = 0;
}

/*********************************************************************
*
*       SYSVIEW_SendTaskInfo()
*
*  Function description
*    Record task information.
*/
void SYSVIEW_SendTaskInfo(U32 TaskID, const char* sName, unsigned Prio, U32 StackBase, unsigned StackSize) {
  SEGGER_SYSVIEW_TASKINFO TaskInfo;

  memset(&TaskInfo, 0, sizeof(TaskInfo)); // Fill all elements with 0 to allow extending the structure in future version without breaking the code
  TaskInfo.TaskID     = TaskID;
  TaskInfo.sName      = sName;
  TaskInfo.Prio       = Prio;
  TaskInfo.StackBase  = StackBase;
  TaskInfo.StackSize  = StackSize;
  SEGGER_SYSVIEW_SendTaskInfo(&TaskInfo);
}

/*********************************************************************
*
*       SYSVIEW_RecordU32x4()
*
*  Function description
*    Record an event with 4 parameters
*/
void SYSVIEW_RecordU32x4(unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3) {
      U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32];
      U8* pPayload;
      //
      pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);                // Prepare the packet for SystemView
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para0);             // Add the first parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para1);             // Add the second parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para2);             // Add the third parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para3);             // Add the fourth parameter to the packet
      //
      SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, Id);             // Send the packet
}

/*********************************************************************
*
*       SYSVIEW_RecordU32x5()
*
*  Function description
*    Record an event with 5 parameters
*/
void SYSVIEW_RecordU32x5(unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4) {
      U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + 5 * SEGGER_SYSVIEW_QUANTA_U32];
      U8* pPayload;
      //
      pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);                // Prepare the packet for SystemView
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para0);             // Add the first parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para1);             // Add the second parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para2);             // Add the third parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para3);             // Add the fourth parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para4);             // Add the fifth parameter to the packet
      //
      SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, Id);             // Send the packet
}

/*********************************************************************
*
*       Public API structures
*
**********************************************************************
*/
// Callbacks provided to SYSTEMVIEW by FreeRTOS
const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI = {
  _cbGetTime,
  _cbSendTaskList,
};

/*************************** End of file ****************************/
