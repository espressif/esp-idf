/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_platform.hpp"

using namespace nvs;

#ifdef LINUX_TARGET
Lock::Lock() {}
Lock::~Lock() {}
esp_err_t nvs::Lock::init() {return ESP_OK;}
void Lock::uninit() {}
#else

#include "sys/lock.h"

Lock::Lock()
{
    // Newlib implementation ensures that even if mSemaphore was 0, it gets initialized.
    // Locks mSemaphore
    _lock_acquire(&mSemaphore);
}

Lock::~Lock()
{
    // Unlocks mSemaphore
    _lock_release(&mSemaphore);
}

esp_err_t Lock::init()
{
    // Let postpone initialization to the Lock::Lock.
    // It is designed to lazy initialize the semaphore in a properly guarded critical section
    return ESP_OK;
}

void Lock::uninit()
{
    // Uninitializes mSemaphore. Please be aware that uninitialization of semaphore shared and held by another thread
    // can cause undefined behavior
    if (mSemaphore) {
        _lock_close(&mSemaphore);
    }
}

_lock_t Lock::mSemaphore = 0;

#endif
