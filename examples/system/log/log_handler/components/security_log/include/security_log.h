/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/* Example module: install once above the application handler, then stop first. */
void security_log_start(const char *path);
void security_log_emit_samples(void);
/* Call only after log producers using this handler have stopped and finished. */
void security_log_stop(void);
