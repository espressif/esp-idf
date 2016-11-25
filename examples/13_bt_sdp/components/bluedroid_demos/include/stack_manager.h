/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
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

#ifndef __STACK_MANAGER_H__
#define __STACK_MANAGER_H__

#include <stdbool.h>
#include "future.h"

bool stack_manager_is_stack_running(void);

future_t *stack_manager_get_hack_future(void);

#endif /* __STACK_MANAGER_H__*/
