// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#include <sys/lock.h>
#include <sys/reent.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Global variables used by newlib in ROM

   Note that any of these symbols which are used by both ROM & IDF will have duplicate copies
   in each "side" of the memory. However they're all pointers, and the pointers will be to the same
   thing, so it's not a big memory waste and functionality is the same.

   Some variables which look like they should be here, but aren't:

   - __sf_fake_stdin, __sf_fake_stdout, __sf_fake_stderr - These are defined in ROM because ROM includes findfp.c,
   but only used if _REENT_INIT or _REENT_INIT_PTR are ever called and ROM doesn't use these macros anywhere unless
   printf() or similar is called without initializing reent first. ESP-IDF sets up its own minimal reent structures.

   - __lock___sinit_recursive_mutex, etc. - these are combined into common_recursive_mutex & common_mutex to save space
*/
typedef struct {
    _LOCK_T common_recursive_mutex;
    _LOCK_T common_mutex;
    struct _reent *global_reent;
} esp_rom_newlib_global_data_t;

/* Called from IDF newlib component setup
   to initialize common data shared between ROM and IDF
*/
void esp_rom_newlib_init_global_data(const esp_rom_newlib_global_data_t *data);

#ifdef __cplusplus
}
#endif
