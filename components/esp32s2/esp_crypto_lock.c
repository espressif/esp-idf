// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#include <sys/lock.h>

#include "esp_crypto_lock.h"

/* Single lock for SHA engine
*/
static _lock_t s_crypto_lock;

void esp_crypto_lock_acquire(void)
{
    _lock_acquire(&s_crypto_lock);
}

void esp_crypto_lock_release(void)
{
    _lock_release(&s_crypto_lock);
}

