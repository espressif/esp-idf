/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

void pthread_internal_local_storage_destructor_callback(TaskHandle_t handle);

extern portMUX_TYPE pthread_lazy_init_lock;
