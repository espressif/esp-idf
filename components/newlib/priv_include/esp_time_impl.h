/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

void esp_time_impl_init(void);

uint64_t esp_time_impl_get_time(void);

uint64_t esp_time_impl_get_time_since_boot(void);

uint32_t esp_time_impl_get_time_resolution(void);

void esp_time_impl_set_boot_time(uint64_t t);

uint64_t esp_time_impl_get_boot_time(void);
