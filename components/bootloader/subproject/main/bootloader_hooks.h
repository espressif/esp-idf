// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
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

#ifndef BOOTLOADER_HOOKS_H
#define BOOTLOADER_HOOKS_H

/**
 * @file The 2nd stage bootloader can be overriden or completed by an application.
 * The functions declared here are weak, and thus, are meant to be defined by a user
 * project, if required.
 * Please check `custom_bootloader` ESP-IDF examples for more details about this feature.
 */


/**
 * @brief Function executed *before* the second stage bootloader initialization,
 * if provided.
 */
void __attribute__((weak)) bootloader_before_init(void);

/**
 * @brief Function executed *after* the second stage bootloader initialization,
 * if provided.
 */
void __attribute__((weak)) bootloader_after_init(void);

#endif // BOOTLOADER_HOOKS_H
