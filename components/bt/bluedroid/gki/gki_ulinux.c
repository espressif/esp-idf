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
#include <string.h>
#include "bt_trace.h"
#include "gki_int.h"
#include "osi.h"
#include "osi_arch.h"
#include "alarm.h"
#include "bt_defs.h"

tGKI_CB gki_cb;

int gki_init(void)
{
    memset(&gki_cb, 0, sizeof(gki_cb));

    //pthread_mutexattr_t attr;
    //pthread_mutexattr_init(&attr);
    //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&gki_cb.lock, NULL);//&attr);

    gki_buffer_init();
    return 0;
}

void gki_clean_up(void)
{
    gki_buffer_cleanup();

    pthread_mutex_destroy(&gki_cb.lock);
}

UINT32 GKI_get_os_tick_count(void)
{
    return osi_alarm_now();
}

// Sleep the calling thread unconditionally for |timeout_ms| milliseconds.
void GKI_delay(UINT32 timeout_ms)
{
    osi_delay_ms(timeout_ms);
    /*TODO:*/
}

void GKI_enable(void)
{
    pthread_mutex_unlock(&gki_cb.lock);
}

void GKI_disable(void)
{
    pthread_mutex_lock(&gki_cb.lock);
}
