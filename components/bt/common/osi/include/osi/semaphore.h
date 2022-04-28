/******************************************************************************
 *
 *  Copyright (C) 2015 Google, Inc.
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

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define OSI_SEM_MAX_TIMEOUT 0xffffffffUL

typedef SemaphoreHandle_t osi_sem_t;

#define osi_sem_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define osi_sem_set_invalid( x ) ( ( *x ) = NULL )

int osi_sem_new(osi_sem_t *sem, uint32_t max_count, uint32_t init_count);

void osi_sem_free(osi_sem_t *sem);

int osi_sem_take(osi_sem_t *sem, uint32_t timeout);

void osi_sem_give(osi_sem_t *sem);


#endif /* __SEMAPHORE_H__ */
