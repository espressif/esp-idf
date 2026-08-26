/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Test fixture: multiple log levels in one function */

void multi_level_func(int status, int handle) {
    APPL_TRACE_ERROR("critical error %d", status);
    APPL_TRACE_WARNING("warning condition %d", status);
    APPL_TRACE_API("API call with handle %d", handle);
    APPL_TRACE_DEBUG("debug info %d %d", status, handle);
    APPL_TRACE_EVENT("event occurred");
    APPL_TRACE_VERBOSE("verbose detail %d", handle);
}
