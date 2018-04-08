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

#include "osi/mutex.h"


/* static section */
static osi_mutex_t gl_mutex; /* Recursive Type */


/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
int osi_mutex_new(osi_mutex_t *mutex)
{
    int xReturn = -1;

    *mutex = xSemaphoreCreateMutex();

    if (*mutex != NULL) {
        xReturn = 0;
    }

    return xReturn;
}

/** Lock a mutex
 * @param mutex the mutex to lock */
int osi_mutex_lock(osi_mutex_t *mutex, uint32_t timeout)
{
    int ret = 0;

    if (timeout == OSI_MUTEX_MAX_TIMEOUT) {
        if (xSemaphoreTake(*mutex, portMAX_DELAY) != pdTRUE) {
            ret = -1;
        }
    } else {
        if (xSemaphoreTake(*mutex, timeout / portTICK_PERIOD_MS) != pdTRUE)  {
            ret = -2;
        }
    }

    return ret;
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void osi_mutex_unlock(osi_mutex_t *mutex)
{
    xSemaphoreGive(*mutex);
}

/** Delete a semaphore
 * @param mutex the mutex to delete */
void osi_mutex_free(osi_mutex_t *mutex)
{
    vSemaphoreDelete(*mutex);
    *mutex = NULL;
}

int osi_mutex_global_init(void)
{
    gl_mutex = xSemaphoreCreateRecursiveMutex();
    if (gl_mutex == NULL) {
        return -1;
    }

    return 0;
}

void osi_mutex_global_deinit(void)
{
    vSemaphoreDelete(gl_mutex);
}

void osi_mutex_global_lock(void)
{
    xSemaphoreTakeRecursive(gl_mutex, portMAX_DELAY);
}

void osi_mutex_global_unlock(void)
{
    xSemaphoreGiveRecursive(gl_mutex);
}
