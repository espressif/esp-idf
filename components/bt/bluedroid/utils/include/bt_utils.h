/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef BT_UTILS_H
#define BT_UTILS_H

// static const char BT_UTILS_MODULE[] = "bt_utils_module";

/*******************************************************************************
**  Type definitions
********************************************************************************/

typedef enum {
    TASK_HIGH_MEDIA = 0,
    TASK_HIGH_GKI_TIMER,
    TASK_HIGH_BTU,
    TASK_HIGH_HCI_WORKER,
    TASK_HIGH_USERIAL_READ,
    TASK_UIPC_READ,
    TASK_JAVA_ALARM,
    TASK_HIGH_MAX
} tHIGH_PRIORITY_TASK;

/*******************************************************************************
**  Functions
********************************************************************************/

void raise_priority_a2dp(tHIGH_PRIORITY_TASK high_task);
void adjust_priority_a2dp(int start);
#define UNUSED(x) (void)(x)
#endif /* BT_UTILS_H */
