// xtensa_overlay_os_hook.c -- Overlay manager OS hooks for FreeRTOS.

// Copyright (c) 2015-2015 Cadence Design Systems Inc.
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


#include "FreeRTOS.h"
#include "semphr.h"

#if configUSE_MUTEX

/* Mutex object that controls access to the overlay. Currently only one
 * overlay region is supported so one mutex suffices.
 */
static SemaphoreHandle_t xt_overlay_mutex;


/* This function should be overridden to provide OS specific init such
 * as the creation of a mutex lock that can be used for overlay locking.
 * Typically this mutex would be set up with priority inheritance. See
 * overlay manager documentation for more details.
 */
void xt_overlay_init_os(void)
{
    /* Create the mutex for overlay access. Priority inheritance is
     * required.
     */
    xt_overlay_mutex = xSemaphoreCreateMutex();
}


/* This function locks access to shared overlay resources, typically
 * by acquiring a mutex.
 */
void xt_overlay_lock(void)
{
    xSemaphoreTake(xt_overlay_mutex, 0);
}


/* This function releases access to shared overlay resources, typically
 * by unlocking a mutex.
 */
void xt_overlay_unlock(void)
{
    xSemaphoreGive(xt_overlay_mutex);
}

#endif
