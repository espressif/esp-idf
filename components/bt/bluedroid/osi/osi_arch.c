/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the bluedroid stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "osi_arch.h"

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
int
osi_mutex_new(osi_mutex_t *pxMutex)
{
    int xReturn = -1;

    *pxMutex = xSemaphoreCreateMutex();

    if (*pxMutex != NULL) {
        xReturn = 0;
    }

    //LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("osi_mutex_new: m=%p\n", *pxMutex));

    return xReturn;
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void
osi_mutex_lock(osi_mutex_t *pxMutex)
{
    while (xSemaphoreTake(*pxMutex, portMAX_DELAY) != pdPASS);
}

int
osi_mutex_trylock(osi_mutex_t *pxMutex)
{
    if (xSemaphoreTake(*pxMutex, 0) == pdPASS) {
        return 0;
    } else {
        return -1;
    }
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void
osi_mutex_unlock(osi_mutex_t *pxMutex)
{
    xSemaphoreGive(*pxMutex);
}

/** Delete a semaphore
 * @param mutex the mutex to delete */
void
osi_mutex_free(osi_mutex_t *pxMutex)
{
    //LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("osi_mutex_free: m=%p\n", *pxMutex));
    vQueueDelete(*pxMutex);
}

/*-----------------------------------------------------------------------------------*/
//  Creates and returns a new semaphore. The "init_count" argument specifies
//  the initial state of the semaphore, "max_count" specifies the maximum value
//  that can be reached.
int
osi_sem_new(osi_sem_t *sem, uint32_t max_count, uint32_t init_count)
{
    int xReturn = -1;
    if (sem) {
        *sem = xSemaphoreCreateCounting(max_count, init_count);
        if ((*sem) != NULL) {
            xReturn = 0;
        }
    }

    return xReturn;
}

/*-----------------------------------------------------------------------------------*/
// Signals a semaphore
void
osi_sem_signal(osi_sem_t *sem)
{
    xSemaphoreGive(*sem);
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  OSI_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  osi_sem_wait(), that uses the osi_arch_sem_wait() function.
*/
uint32_t
osi_sem_wait(osi_sem_t *sem, uint32_t timeout)
{
    portTickType StartTime, EndTime, Elapsed;
    unsigned long ulReturn;

    StartTime = xTaskGetTickCount();

    if (timeout != 0) {
        if (xSemaphoreTake(*sem, timeout / portTICK_PERIOD_MS) == pdTRUE) {
            EndTime = xTaskGetTickCount();
            Elapsed = (EndTime - StartTime) * portTICK_PERIOD_MS;

            if (Elapsed == 0) {
                Elapsed = 1;
            }

            ulReturn = Elapsed;
        } else {
            ulReturn = OSI_ARCH_TIMEOUT;
        }
    } else { // must block without a timeout
        while (xSemaphoreTake(*sem, portMAX_DELAY) != pdTRUE);

        EndTime = xTaskGetTickCount();
        Elapsed = (EndTime - StartTime) * portTICK_PERIOD_MS;

        if (Elapsed == 0) {
            Elapsed = 1;
        }

        ulReturn = Elapsed;
    }

    return ulReturn ; // return time blocked
}

/*-----------------------------------------------------------------------------------*/
// Deallocates a semaphore
void
osi_sem_free(osi_sem_t *sem)
{
    vSemaphoreDelete(*sem);
}

/*-----------------------------------------------------------------------------------*/
// Initialize osi arch
void
osi_arch_init(void)
{
}

/*-----------------------------------------------------------------------------------*/
uint32_t
osi_now(void)
{
    return xTaskGetTickCount();
}


void osi_delay_ms(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}


