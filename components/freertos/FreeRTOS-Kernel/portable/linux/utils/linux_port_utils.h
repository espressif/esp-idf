/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct thread *thread_hdl;

void linux_port_setup_backtrace_signal(void);
void linux_port_print_backtrace(void);
pthread_t linux_port_get_scheduled_task_pthread(void);

#ifdef __cplusplus
}
#endif
