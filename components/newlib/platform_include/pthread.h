// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef __ESP_PLATFORM_PTHREAD_H__
#define __ESP_PLATFORM_PTHREAD_H__

#include <sys/types.h>
#include <sys/time.h>
#include <sys/features.h>

#include_next <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

int pthread_condattr_getclock(const pthread_condattr_t * attr, clockid_t * clock_id);

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);

#ifdef __cplusplus
}
#endif

#endif // __ESP_PLATFORM_PTHREAD_H__
