// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __BT_USER_CONFIG_H__
#define __BT_USER_CONFIG_H__
#include "sdkconfig.h"

#define UC_TRACE_LEVEL_NONE                 0          /* No trace messages to be generated    */
#define UC_TRACE_LEVEL_ERROR                1          /* Error condition trace messages       */
#define UC_TRACE_LEVEL_WARNING              2          /* Warning condition trace messages     */
#define UC_TRACE_LEVEL_API                  3          /* API traces                           */
#define UC_TRACE_LEVEL_EVENT                4          /* Debug messages for events            */
#define UC_TRACE_LEVEL_DEBUG                5          /* Full debug messages                  */
#define UC_TRACE_LEVEL_VERBOSE              6          /* Verbose debug messages               */

//DYNAMIC ENV ALLOCATOR
#ifdef CONFIG_BT_BLE_DYNAMIC_ENV_MEMORY
#define UC_BT_BLE_DYNAMIC_ENV_MEMORY            CONFIG_BT_BLE_DYNAMIC_ENV_MEMORY
#else
#define UC_BT_BLE_DYNAMIC_ENV_MEMORY            FALSE
#endif

#ifdef CONFIG_BT_STACK_NO_LOG
#define UC_BT_STACK_NO_LOG               CONFIG_BT_STACK_NO_LOG
#else
#define UC_BT_STACK_NO_LOG               FALSE
#endif

/**********************************************************
 * Thread/Task reference
 **********************************************************/
#ifdef CONFIG_BT_BLUEDROID_PINNED_TO_CORE
#define UC_TASK_PINNED_TO_CORE              (CONFIG_BT_BLUEDROID_PINNED_TO_CORE < portNUM_PROCESSORS ? CONFIG_BT_BLUEDROID_PINNED_TO_CORE : tskNO_AFFINITY)
#else
#define UC_TASK_PINNED_TO_CORE              (0)
#endif

#ifdef CONFIG_BT_BTC_TASK_STACK_SIZE
#define UC_BTC_TASK_STACK_SIZE              CONFIG_BT_BTC_TASK_STACK_SIZE
#else
#define UC_BTC_TASK_STACK_SIZE              4096
#endif

/**********************************************************
 * Trace reference
 **********************************************************/

#ifdef CONFIG_LOG_DEFAULT_LEVEL
#define UC_LOG_DEFAULT_LEVEL                CONFIG_LOG_DEFAULT_LEVEL
#else
#define UC_LOG_DEFAULT_LEVEL                3
#endif

#ifdef CONFIG_BOOTLOADER_LOG_LEVEL
#define UC_BOOTLOADER_LOG_LEVEL             CONFIG_BOOTLOADER_LOG_LEVEL
#else
#define UC_BOOTLOADER_LOG_LEVEL             3
#endif

#ifdef CONFIG_BT_LOG_BTC_TRACE_LEVEL
#define UC_BT_LOG_BTC_TRACE_LEVEL           CONFIG_BT_LOG_BTC_TRACE_LEVEL
#else
#define UC_BT_LOG_BTC_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_OSI_TRACE_LEVEL
#define UC_BT_LOG_OSI_TRACE_LEVEL           CONFIG_BT_LOG_OSI_TRACE_LEVEL
#else
#define UC_BT_LOG_OSI_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

//MEMORY DEBUG
#ifdef CONFIG_BT_BLUEDROID_MEM_DEBUG
#define UC_BT_BLUEDROID_MEM_DEBUG TRUE
#else
#define UC_BT_BLUEDROID_MEM_DEBUG FALSE
#endif

#endif /* __BT_USER_CONFIG_H__ */
