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

#ifndef __ESP_NEWLIB_H__
#define __ESP_NEWLIB_H__

#include <sys/reent.h>

/*
 * Initialize newlib time functions
 */
void esp_newlib_time_init(void);

/**
 * Replacement for newlib's _REENT_INIT_PTR and __sinit.
 *
 * Called from startup code and FreeRTOS, not intended to be called from
 * application code.
 */
void esp_reent_init(struct _reent* r);

/** 
 * Clean up some of lazily allocated buffers in REENT structures.
 */
void esp_reent_cleanup(void);

/**
 * Function which sets up syscall table used by newlib functions in ROM.
 *
 * Called from the startup code, not intended to be called from application
 * code.
 */
void esp_setup_syscall_table(void);

/**
 * Update current microsecond time from RTC
 */
void esp_set_time_from_rtc(void);

/*
 * Sync counters RTC and FRC. Update boot_time.
 */
void esp_sync_counters_rtc_and_frc(void);

#endif //__ESP_NEWLIB_H__
