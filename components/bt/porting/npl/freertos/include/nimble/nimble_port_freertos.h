/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2019-2022 Espressif Systems (Shanghai) CO LTD
 */

#ifndef _NIMBLE_PORT_FREERTOS_H
#define _NIMBLE_PORT_FREERTOS_H

#include "nimble/nimble_npl.h"

#ifdef __cplusplus
extern "C" {
#endif

void nimble_port_freertos_init(TaskFunction_t host_task_fn);
void nimble_port_freertos_deinit(void);
void npl_freertos_funcs_init(void);
void npl_freertos_funcs_deinit(void);
int npl_freertos_mempool_init(void);
struct npl_funcs_t * npl_freertos_funcs_get(void);
#ifdef __cplusplus
}
#endif

#endif /* _NIMBLE_PORT_FREERTOS_H */
