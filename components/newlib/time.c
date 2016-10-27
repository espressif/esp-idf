// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/reent.h>
#include <sys/time.h>
#include <sys/times.h>
#include "esp_attr.h"


clock_t _times_r(struct _reent *r, struct tms *ptms)
{
    __errno_r(r) = ENOSYS;
    return (clock_t) -1;
}

// TODO: read time from RTC
int _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz)
{
    __errno_r(r) = ENOSYS;
    return -1;
}
