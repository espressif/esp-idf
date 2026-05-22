/*
 * Cooperative syscalls subsystem for the Linux FreeRTOS simulator.
 *
 * This header exposes the public initialization API needed by the
 * FreeRTOS Linux port. The subsystem provides blocking read()/write()
 * for FreeRTOS tasks without stalling the cooperative scheduler by
 * forwarding operations to a dedicated I/O worker thread.
 *
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void linux_port_coop_syscalls_init(void);

#ifdef __cplusplus
}
#endif
